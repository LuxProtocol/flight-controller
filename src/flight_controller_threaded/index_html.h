#pragma once
#include <Arduino.h>

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 light</title>
<style>
  :root { --bg:#f2f4f3; --panel:#fff; --ink:#1b2421; --line:#cdd5d1; --on:#1f8a5b; --off:#8a949f; --err:#b3392f; }
  @media (prefers-color-scheme: dark) {
    :root { --bg:#141a18; --panel:#1d2522; --ink:#e8eeeb; --line:#324039; --on:#3fcf8e; --off:#66727c; --err:#ff7a6e; }
  }
  * { box-sizing: border-box; }
  body { margin:0; min-height:100vh; display:grid; place-items:center; background:var(--bg);
         color:var(--ink); font:16px/1.5 "Segoe UI", system-ui, sans-serif; padding:24px; }
  main { width:100%; max-width:380px; background:var(--panel); border:1px solid var(--line);
         border-radius:14px; padding:28px; text-align:center; }
  section + section { margin-top:28px; padding-top:28px; border-top:1px solid var(--line); }
  h2 { font-size:1.25rem; margin:0 0 8px; }
  .lamp { width:120px; height:120px; border-radius:50%; margin:24px auto 16px; background:var(--off);
          transition:background .2s, box-shadow .2s; }
  .lamp.on { background:var(--on); box-shadow:0 0 40px var(--on); }
  #state { font-size:1.1rem; font-weight:600; margin-bottom:20px; }
  button { width:100%; padding:12px; font:inherit; font-weight:600; cursor:pointer; color:#fff;
           background:var(--on); border:0; border-radius:8px; }
  @media (prefers-color-scheme: dark) { button { color:#0b1411; } }
  button:focus-visible { outline:3px solid var(--on); outline-offset:2px; }
  button:disabled { opacity:.6; }
  .msg { margin:14px 0 0; min-height:1.5em; font-size:.875rem; color:var(--err); }
  .msg.ok { color:var(--ink); }

  /* Secondary (outline) button + busy spinner */
  button.secondary { background:transparent; color:var(--ink); border:1px solid var(--line); }
  button.secondary:hover:not(:disabled) { border-color:var(--ink); }
  button.secondary[aria-busy="true"]::before { content:""; display:inline-block; width:.9em; height:.9em;
      margin-right:.6em; vertical-align:-.1em; border:2px solid currentColor; border-right-color:transparent;
      border-radius:50%; animation:spin .8s linear infinite; }
  @keyframes spin { to { transform:rotate(360deg); } }

  /* System stats */
  dl.stats { display:grid; grid-template-columns:1fr auto; gap:6px 16px; margin:16px 0 20px; text-align:left; font-size:.95rem; }
  dl.stats dt { opacity:.7; }
  dl.stats dd { margin:0; font-weight:600; text-align:right; font-variant-numeric:tabular-nums; transition:opacity .2s; }
  dl.stats.stale dd { opacity:.45; }

  /* ESC switch */
  button.switch { position:relative; display:block; width:84px; height:44px; padding:0; margin:24px auto 14px;
                  border-radius:999px; background:var(--off); box-shadow:0 0 0 0 transparent;
                  transition:background .25s ease, box-shadow .25s ease; }
  button.switch::after { content:""; position:absolute; top:4px; left:4px; width:36px; height:36px;
                         border-radius:50%; background:#fff; box-shadow:0 1px 4px rgba(0,0,0,.35);
                         transition:transform .35s cubic-bezier(.3,1.5,.5,1), width .15s ease; }
  button.switch[aria-checked="true"] { background:var(--on); box-shadow:0 0 22px -4px var(--on); }
  button.switch[aria-checked="true"]::after { transform:translateX(40px); }
  button.switch:active::after { width:44px; }
  button.switch[aria-checked="true"]:active::after { transform:translateX(32px); }
  button.switch[aria-busy="true"] { cursor:progress; }
  #esc-state { font-size:1.1rem; font-weight:600; }
  @media (prefers-reduced-motion: reduce) {
    button.switch, button.switch::after { transition:none; }
    button.secondary[aria-busy="true"]::before { animation:none; }
  }
</style>
</head>
<body>
<main>
  <section>
    <h2>Attachments</h2>
    <div id="lamp" class="lamp"></div>
    <div id="state" aria-live="polite">Light Attachment</div>
    <button id="btn">Toggle light</button>
    <p id="msg" class="msg" role="status"></p>
  </section>

  <section>
    <h2 id="esc-title">ESC Status</h2>
    <button id="esc" class="switch" role="switch" aria-checked="false" aria-labelledby="esc-title"></button>
    <div id="esc-state" aria-live="polite">Off</div>
    <p id="esc-msg" class="msg" role="status"></p>
  </section>

  <section>
    <h2>Controller</h2>
    <dl id="stats" class="stats stale">
      <dt>Uptime</dt><dd id="uptime">—</dd>
      <dt>Free heap</dt><dd id="heap">—</dd>
      <dt>Temperature</dt><dd id="temp">—</dd>
    </dl>
    <button id="reboot" class="secondary">Reboot ESP32</button>
    <p id="reboot-msg" class="msg" role="status"></p>
  </section>
</main>
<script>
  // How often to refresh the system status, in milliseconds.
  const STATUS_INTERVAL_MS = 1000;

  const lamp = document.getElementById("lamp");
  const stateEl = document.getElementById("state");
  const btn = document.getElementById("btn");
  const msg = document.getElementById("msg");

  function show(text) {
    const on = text.trim() === "ON";
    lamp.classList.toggle("on", on);
    stateEl.textContent = on ? "On" : "Off";
    msg.textContent = "";
  }
  async function call(path) {
    btn.disabled = true;
    try {
      const r = await fetch(path);
      show(await r.text());
    } catch (e) {
      msg.textContent = "Lost connection to the ESP32.";
    } finally {
      btn.disabled = false;
    }
  }
  btn.addEventListener("click", () => call("/toggle"));
  call("/state");

  // ESC switch
  const esc = document.getElementById("esc");
  const escState = document.getElementById("esc-state");
  const escMsg = document.getElementById("esc-msg");
  let escBusy = false;

  function setEsc(on) {
    esc.setAttribute("aria-checked", String(on));
    escState.textContent = on ? "On" : "Off";
  }
  esc.addEventListener("click", async () => {
    if (escBusy) return;
    const want = esc.getAttribute("aria-checked") !== "true";
    escBusy = true;
    esc.setAttribute("aria-busy", "true");
    escMsg.textContent = "";
    setEsc(want); // flip right away so the animation feels instant
    try {
      const r = await fetch(want ? "/esc/arm" : "/esc/disarm");
      if (!r.ok) throw new Error("HTTP " + r.status);
    } catch (e) {
      setEsc(!want); // slide back if the ESP32 didn't confirm
      escMsg.textContent = "Couldn't reach the ESC. Switch not changed.";
    } finally {
      escBusy = false;
      esc.removeAttribute("aria-busy");
    }
  });

  // Reboot
  const rebootBtn = document.getElementById("reboot");
  const rebootMsg = document.getElementById("reboot-msg");
  const sleep = (ms) => new Promise((res) => setTimeout(res, ms));

  // Resolves true if the path answers OK within `ms`, false on error or timeout.
  async function probe(path, ms) {
    const ctl = new AbortController();
    const timer = setTimeout(() => ctl.abort(), ms);
    try {
      const r = await fetch(path, { cache: "no-store", signal: ctl.signal });
      return r.ok;
    } catch (e) {
      return false;
    } finally {
      clearTimeout(timer);
    }
  }
  function say(text, ok) {
    rebootMsg.textContent = text;
    rebootMsg.classList.toggle("ok", ok);
  }

  rebootBtn.addEventListener("click", async () => {
    rebootBtn.disabled = true;
    rebootBtn.setAttribute("aria-busy", "true");
    rebootBtn.textContent = "Rebooting…";
    say("Waiting for the ESP32 to restart…", true);

    const start = Date.now();
    await probe("/reboot", 3000); // the connection may drop as it restarts, so ignore the result

    // Wait for it to go offline (up to 5 s), then for it to come back (up to 60 s).
    while (Date.now() - start < 5000 && (await probe("/state", 1500))) await sleep(300);
    let back = false;
    while (Date.now() - start < 60000) {
      if (await probe("/state", 1500)) { back = true; break; }
      await sleep(1000);
    }

    if (back) {
      say("Back online.", true);
      setEsc(false);            // the ESC starts disarmed after a reboot
      escMsg.textContent = "";
      call("/state");           // refresh the light
    } else {
      say("The ESP32 didn’t come back within 60 seconds. Check power and Wi-Fi.", false);
    }
    rebootBtn.disabled = false;
    rebootBtn.removeAttribute("aria-busy");
    rebootBtn.textContent = "Reboot ESP32";
  });

  // System status
  const statsEl = document.getElementById("stats");
  const uptimeEl = document.getElementById("uptime");
  const heapEl = document.getElementById("heap");
  const tempEl = document.getElementById("temp");

  function fmtUptime(s) {
    const d = Math.floor(s / 86400), h = Math.floor((s % 86400) / 3600),
          m = Math.floor((s % 3600) / 60), sec = s % 60;
    return [d && d + "d", (d || h) && h + "h", (d || h || m) && m + "m", sec + "s"]
      .filter(Boolean).join(" ");
  }
  async function pollStatus() {
    const ctl = new AbortController();
    const timer = setTimeout(() => ctl.abort(), 2000);
    try {
      const r = await fetch("/systemstatus", { cache: "no-store", signal: ctl.signal });
      if (!r.ok) throw new Error("HTTP " + r.status);
      const text = await r.text();
      const up = /uptime:\s*(\d+)/.exec(text);
      const heap = /free heap:\s*(\d+)/.exec(text);
      const temp = /temp:\s*(-?\d+)/.exec(text);
      if (!up || !heap) throw new Error("Unexpected response");
      uptimeEl.textContent = fmtUptime(Number(up[1]));
      heapEl.textContent = Number(heap[1]).toLocaleString() + " bytes";
      tempEl.textContent = temp ? temp[1] + " °C" : "—";
      statsEl.classList.remove("stale");
    } catch (e) {
      uptimeEl.textContent = heapEl.textContent = tempEl.textContent = "—";
      statsEl.classList.add("stale");
    } finally {
      clearTimeout(timer);
    }
  }
  // Waits for each response before scheduling the next, so slow replies never pile up.
  // Pauses while the tab is hidden.
  (async function statusLoop() {
    for (;;) {
      if (!document.hidden) await pollStatus();
      await sleep(STATUS_INTERVAL_MS);
    }
  })();
</script>
</body>
</html>
)rawliteral";