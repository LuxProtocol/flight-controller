#pragma once
#include <Arduino.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"

#include "mon_struts.h"

#define NUM_CORES            portNUM_PROCESSORS
#define MIN_PERIOD_MS        100
#define GET_TIMEOUT_MS       100
#define REFRESH_MS           5000

static_assert(NUM_CORES <= SYSMON_MAX_CORES, "Increase SYSMON_MAX_CORES");

// ---- Sampler state: touched only by the monitor task, so no lock needed ----
static TaskStatus_t s_status[SYSMON_MAX_TASKS];
static uint32_t     s_delta[SYSMON_MAX_TASKS];
static struct {
    TaskHandle_t handle;
    uint32_t     runtime;
} s_prev[SYSMON_MAX_TASKS];
static int               s_prev_n = 0;
static int64_t           s_t_prev = 0;
static sysmon_snapshot_t s_work;            // being built

// ---- Published snapshot: guarded by s_mutex ----
static sysmon_snapshot_t s_snap;
static bool              s_have_snap = false;
static SemaphoreHandle_t s_mutex     = NULL;
static bool              s_started   = false;

const char *system_monitor_state_str(uint8_t state)
{
    switch ((eTaskState)state) {
    case eRunning:   return "RUN";
    case eReady:     return "RDY";
    case eBlocked:   return "BLK";
    case eSuspended: return "SUS";
    case eDeleted:   return "DEL";
    default:         return "?";
    }
}

static int clamp_x10(int v)
{
    if (v < 0)    return 0;
    if (v > 1000) return 1000;
    return v;
}

static void fill_heap(sysmon_heap_info_t *h, uint32_t caps)
{
    memset(h, 0, sizeof(*h));
    h->total = (uint32_t)heap_caps_get_total_size(caps);
    if (h->total == 0) {
        return;   // memory type not present
    }
    h->free          = (uint32_t)heap_caps_get_free_size(caps);
    h->min_free      = (uint32_t)heap_caps_get_minimum_free_size(caps);
    h->largest_block = (uint32_t)heap_caps_get_largest_free_block(caps);
}

static void save_baseline(UBaseType_t n, int64_t now)
{
    for (UBaseType_t i = 0; i < n; i++) {
        s_prev[i].handle  = s_status[i].xHandle;
        s_prev[i].runtime = s_status[i].ulRunTimeCounter;
    }
    s_prev_n = (int)n;
    s_t_prev = now;
}

static void fill_tasks(UBaseType_t n, int64_t elapsed)
{
    uint32_t idle_delta[NUM_CORES] = { 0 };

    for (UBaseType_t i = 0; i < n; i++) {
        // Run-time delta since the previous sample (new tasks start from 0).
        uint32_t last = 0;
        for (int j = 0; j < s_prev_n; j++) {
            if (s_prev[j].handle == s_status[i].xHandle) {
                last = s_prev[j].runtime;
                break;
            }
        }
        s_delta[i] = s_status[i].ulRunTimeCounter - last;   // wrap-safe

        if (strncmp(s_status[i].pcTaskName, "IDLE", 4) == 0) {
            int c = (int)s_status[i].xCoreID;
            if (c < 0 || c >= NUM_CORES) {
                c = 0;
            }
            idle_delta[c] += s_delta[i];
        }

        sysmon_task_info_t *t = &s_work.tasks[i];
        strncpy(t->name, s_status[i].pcTaskName, SYSMON_NAME_LEN - 1);
        t->name[SYSMON_NAME_LEN - 1] = '\0';
        t->state      = (uint8_t)s_status[i].eCurrentState;
        t->priority   = (uint8_t)s_status[i].uxCurrentPriority;
        t->core       = (s_status[i].xCoreID == tskNO_AFFINITY) ? -1 : (int8_t)s_status[i].xCoreID;
        t->stack_free = (uint32_t)s_status[i].usStackHighWaterMark;
        t->cpu_x10    = (uint16_t)clamp_x10((int)(((int64_t)s_delta[i] * 1000) / elapsed));
    }
    s_work.task_count = (uint8_t)n;

    // Per-core and total load from idle time.
    uint32_t idle_sum = 0;
    for (int c = 0; c < NUM_CORES; c++) {
        s_work.core_load_x10[c] =
            (uint16_t)clamp_x10(1000 - (int)(((int64_t)idle_delta[c] * 1000) / elapsed));
        idle_sum += idle_delta[c];
    }
    s_work.total_load_x10 =
        (uint16_t)clamp_x10(1000 - (int)(((int64_t)idle_sum * 1000) / (elapsed * NUM_CORES)));
}

static void sample(void)
{
    uint32_t total_rt;
    UBaseType_t n = uxTaskGetSystemState(s_status, SYSMON_MAX_TASKS, &total_rt);

    int64_t now     = esp_timer_get_time();
    int64_t elapsed = now - s_t_prev;
    if (elapsed <= 0) {
        elapsed = 1;
    }

    memset(&s_work, 0, sizeof(s_work));
    s_work.timestamp_us = now;
    s_work.interval_us  = (uint32_t)elapsed;
    s_work.core_count   = NUM_CORES;
    fill_heap(&s_work.internal_heap, MALLOC_CAP_INTERNAL);
    fill_heap(&s_work.psram_heap,    MALLOC_CAP_SPIRAM);

    if (n == 0) {
        s_work.overflow = true;      // more tasks than SYSMON_MAX_TASKS
        s_prev_n = 0;
        s_t_prev = now;
    } else {
        fill_tasks(n, elapsed);
        save_baseline(n, now);
    }

    // Publish: the copy is the only thing done while holding the lock.
    if (xSemaphoreTake(s_mutex, portMAX_DELAY) == pdTRUE) {
        memcpy(&s_snap, &s_work, sizeof(s_snap));
        s_have_snap = true;
        xSemaphoreGive(s_mutex);
    }
}

static void monitor_task(void *pvParameters)
{
    const TickType_t period = pdMS_TO_TICKS(REFRESH_MS);

    s_mutex = xSemaphoreCreateMutex();

    // Baseline so the first snapshot covers a real interval.
    UBaseType_t n = uxTaskGetSystemState(s_status, SYSMON_MAX_TASKS, NULL);
    save_baseline(n, esp_timer_get_time());

    while (1) {
        vTaskDelay(period);
        sample();
    }
}

bool system_monitor_get(sysmon_snapshot_t *out)
{
    if (out == NULL || s_mutex == NULL) {
        return false;
    }

    bool ok = false;
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(GET_TIMEOUT_MS)) == pdTRUE) {
        if (s_have_snap) {
            memcpy(out, &s_snap, sizeof(*out));
            ok = true;
        }
        xSemaphoreGive(s_mutex);
    }
    return ok;
}

// ---------------------------------------------------------------------------
// Text output
// ---------------------------------------------------------------------------

typedef struct {
    char  *buf;
    size_t size;
    size_t used;
    char **lines;
    int    max;
    int    count;
} line_out_t;

// Append one NUL-terminated line to the buffer. Returns false if it did not fit
// (nothing is added in that case).
static bool add_line(line_out_t *o, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
static bool add_line(line_out_t *o, const char *fmt, ...)
{
    if (o->count >= o->max || o->used >= o->size) {
        return false;
    }
    char  *dst  = o->buf + o->used;
    size_t room = o->size - o->used;

    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(dst, room, fmt, ap);
    va_end(ap);

    if (n < 0 || (size_t)n >= room) {
        return false;   // would have been truncated
    }
    o->lines[o->count++] = dst;
    o->used += (size_t)n + 1;
    return true;
}

int system_monitor_get_lines(char *buf, size_t buf_size, char **lines, int max_lines)
{
    if (buf == NULL || lines == NULL || buf_size == 0 || max_lines <= 0 || s_mutex == NULL) {
        return 0;
    }
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(GET_TIMEOUT_MS)) != pdTRUE) {
        return 0;
    }
    if (!s_have_snap) {
        xSemaphoreGive(s_mutex);
        return 0;
    }

    // Format straight from the published snapshot while holding the lock: no copy
    // of the 1.2 KB struct needed. The lock is only held for the formatting.
    line_out_t o = { buf, buf_size, 0, lines, max_lines, 0 };
    const sysmon_snapshot_t *s = &s_snap;

    bool ok = add_line(&o, "S,%u,%u,%u,%u,%u,%u",
                       (unsigned)(s->timestamp_us / 1000),
                       (unsigned)s->interval_us,
                       (unsigned)s->core_count,
                       (unsigned)s->total_load_x10,
                       (unsigned)s->task_count,
                       s->overflow ? 1u : 0u);

    for (int c = 0; ok && c < s->core_count; c++) {
        ok = add_line(&o, "C,%d,%u", c, (unsigned)s->core_load_x10[c]);
    }

    const sysmon_heap_info_t *hp[2]   = { &s->internal_heap, &s->psram_heap };
    const char               *hname[2] = { "internal", "psram" };
    for (int i = 0; ok && i < 2; i++) {
        ok = add_line(&o, "H,%s,%u,%u,%u,%u", hname[i],
                      (unsigned)hp[i]->total,
                      (unsigned)hp[i]->free,
                      (unsigned)hp[i]->min_free,
                      (unsigned)hp[i]->largest_block);
    }

    for (int i = 0; ok && i < s->task_count; i++) {
        const sysmon_task_info_t *t = &s->tasks[i];

        char name[SYSMON_NAME_LEN];
        for (int k = 0; k < SYSMON_NAME_LEN; k++) {
            name[k] = (t->name[k] == ',') ? '_' : t->name[k];
        }
        name[SYSMON_NAME_LEN - 1] = '\0';

        ok = add_line(&o, "T,%s,%s,%u,%d,%u,%u",
                      name,
                      system_monitor_state_str(t->state),
                      (unsigned)t->priority,
                      (int)t->core,
                      (unsigned)t->stack_free,
                      (unsigned)t->cpu_x10);
    }

    xSemaphoreGive(s_mutex);
    return o.count;
}