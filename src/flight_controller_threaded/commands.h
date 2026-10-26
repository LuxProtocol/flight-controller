#pragma once
#include <Arduino.h>
#include <strings.h>
#include <stdlib.h>

constexpr int MAX_ARGS = 8;


static void cmdHelp(int argc, char **argv);

static void cmdStatus(int, char **) {
  Serial.printf("uptime   : %lu s\n", millis()/1000);
  Serial.printf("free heap: %u bytes\n", ESP.getFreeHeap());
}

static void cmdReboot(int, char **) {
  Serial.print("rebooting...\n");
  Serial.flush();               // wait for the UART to finish sending
  ESP.restart();
}

static void cmdEcho(int argc, char **argv) {
  for (int i = 1; i < argc; i++) Serial.printf("%s ", argv[i]);
  Serial.print("\n");
}

// ---------- Command table ----------
struct Command {
  const char *name;
  const char *help;
  void (*fn)(int argc, char **argv);
};

static const Command commands[] = {
  {"help",     "list commands",         cmdHelp},
  {"status",   "show system status",    cmdStatus},
  {"reboot",   "restart the chip",      cmdReboot},
  {"echo", "echo text back", cmdEcho},
};

static void cmdHelp(int, char **) {
  Serial.printf("\nusage: [option] ...\n");
  for (const auto &c : commands) Serial.printf("  %-10s %s\n", c.name, c.help);
}

// ---------- Tokenize + dispatch (modifies `line` in place) ----------
static void dispatchCommand(char *line) {
  char *argv[MAX_ARGS];
  int argc = 0;
  char *save = nullptr;

  for (char *tok = strtok_r(line, " \t", &save);
       tok && argc < MAX_ARGS;
       tok = strtok_r(nullptr, " \t", &save)) {
    argv[argc++] = tok;
  }
  if (argc == 0) return;

  for (const auto &c : commands) {
    if (!strcasecmp(argv[0], c.name)) { c.fn(argc, argv); return; }
  }
  Serial.printf("unknown command '%s' (try 'help')\n", argv[0]);
}
