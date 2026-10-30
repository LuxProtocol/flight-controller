#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef SYSMON_MAX_TASKS
#define SYSMON_MAX_TASKS 40     // size of the per-task table
#endif
#define SYSMON_MAX_CORES 2      // ESP32-S3 is dual core
#define SYSMON_NAME_LEN  16     // matches configMAX_TASK_NAME_LEN

typedef struct {
    char     name[SYSMON_NAME_LEN];
    uint8_t  state;             // eTaskState value; see system_monitor_state_str()
    uint8_t  priority;
    int8_t   core;              // 0, 1, or -1 if the task is not pinned
    uint32_t stack_free;        // lowest free stack ever seen, in bytes
    uint16_t cpu_x10;           // CPU share of ONE core over the interval, tenths of a percent
} sysmon_task_info_t;

typedef struct {
    uint32_t total;             // bytes; 0 if this memory type is not present (e.g. no PSRAM)
    uint32_t free;
    uint32_t min_free;          // lowest free ever
    uint32_t largest_block;
} sysmon_heap_info_t;

typedef struct {
    int64_t  timestamp_us;      // esp_timer time when the sample was taken
    uint32_t interval_us;       // length of the window the CPU figures cover

    uint8_t  core_count;
    uint16_t core_load_x10[SYSMON_MAX_CORES];   // per-core load, tenths of a percent
    uint16_t total_load_x10;                    // average across cores, tenths of a percent

    sysmon_heap_info_t internal_heap;
    sysmon_heap_info_t psram_heap;

    bool     overflow;          // true if there are more than SYSMON_MAX_TASKS tasks:
                                // then the task table and CPU loads are not valid (heap still is)
    uint8_t  task_count;
    sysmon_task_info_t tasks[SYSMON_MAX_TASKS];
} sysmon_snapshot_t;            // roughly 1.2 KB with the defaults

/**
 * Start the background sampling task. Call once, e.g. from setup().
 * period_ms is how often a new snapshot is taken (minimum 100).
 * Returns false if it was already started or could not be created.
 *
 * Requires CONFIG_FREERTOS_USE_TRACE_FACILITY, CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
 * and CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID.
 */
bool system_monitor_start(uint32_t period_ms);

/**
 * Copy the most recent snapshot into *out. Safe to call from any task, any core.
 * NOT safe from an ISR.
 *
 * Returns false if the monitor is not started, no snapshot exists yet (the first one
 * appears after one period), or the lock could not be taken within 100 ms.
 *
 * Each calling thread needs its own sysmon_snapshot_t. The struct is large, so make it
 * static/global or heap-allocated rather than a local on a small task stack.
 */
bool system_monitor_get(sysmon_snapshot_t *out);

/** Sizing for system_monitor_get_lines(): 1 summary + cores + 2 heap + one line per task. */
#define SYSMON_MAX_LINES       (1 + SYSMON_MAX_CORES + 2 + SYSMON_MAX_TASKS)
#define SYSMON_LINES_BUF_BYTES (320 + 48 * SYSMON_MAX_TASKS)

/**
 * Text version of system_monitor_get(), for parsing elsewhere (another module, a
 * serial/network link, a different device).
 *
 * Formats the latest snapshot into the caller-supplied buf as separate NUL-terminated
 * strings and fills lines[] with pointers into buf. Nothing is allocated, so there is
 * nothing to free. The pointers stay valid as long as buf does.
 *
 * Returns the number of lines written, or 0 if there is no snapshot yet, the monitor is
 * not started, or the arguments are bad. If buf or lines[] is too small, whole lines
 * are dropped from the end (never cut in half); compare the T-line count with
 * task_count from the S line to detect that.
 *
 * Line format: comma separated, integers only, no trailing newline.
 *   S,<uptime_ms>,<interval_us>,<core_count>,<total_load_x10>,<task_count>,<overflow 0|1>
 *   C,<core>,<load_x10>                                   one per core
 *   H,<internal|psram>,<total>,<free>,<min_free>,<largest_block>
 *   T,<name>,<state>,<priority>,<core>,<stack_free>,<cpu_x10>   one per task
 *
 * Notes: *_x10 values are tenths of a percent. core is -1 for unpinned tasks. State is
 * RUN/RDY/BLK/SUS/DEL. Commas in task names are replaced with '_'. uptime_ms wraps
 * after about 49.7 days. H lines for a missing memory type (no PSRAM) are all zeros.
 *
 * Safe from any task, not from an ISR. Needs roughly 1 KB of stack headroom in the caller.
 */
int system_monitor_get_lines(char *buf, size_t buf_size, char **lines, int max_lines);

/** "RUN", "RDY", "BLK", "SUS", "DEL" for a sysmon_task_info_t.state value. */
const char *system_monitor_state_str(uint8_t state);