#pragma once
#include <Arduino.h>

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#110c1c">
<title>Aerial Matrix | Device console</title>
<style>
:root{color-scheme:dark;--bg:#110c1c;--panel:#1c142b;--ink:#f4efff;--muted:#b3a5cc;--line:#3b2d51;--accent:#c19aff;--err:#ffaaa1;--surface:#2b1f40;--edge:#58416f;--primary-ink:#241036;--lamp-bg:#2a1e3d;--lamp-ink:#b8a1d4;--lamp-on-bg:#422765;--lamp-on-ink:#e2caff;--lamp-on-edge:#9970cb;--track:#46345f;--knob:#e9daff}

:root[data-theme="light"]{color-scheme:light;--bg:#edf2f6;--panel:#ffffff;--ink:#172b3c;--muted:#51667b;--line:#cbd6e0;--accent:#087568;--err:#b33227;--surface:#f2f6f9;--edge:#bdcbd7;--primary-ink:#ffffff;--lamp-bg:#edf2f6;--lamp-ink:#5c7185;--lamp-on-bg:#e0f5ee;--lamp-on-ink:#086b53;--lamp-on-edge:#3f9680;--track:#c0cedb;--knob:#ffffff}
:root[data-theme="dark"]{color-scheme:dark;--bg:#101113;--panel:#1c1e22;--ink:#f1f3f5;--muted:#a7adb7;--line:#363a42;--accent:#c8ced8;--err:#ffaaa1;--surface:#272a30;--edge:#4a505c;--primary-ink:#171a20;--lamp-bg:#292d34;--lamp-ink:#adb6c5;--lamp-on-bg:#35404b;--lamp-on-ink:#e0ecf9;--lamp-on-edge:#8295ab;--track:#414751;--knob:#e1e6ed}
.preferences{display:flex;align-items:center;justify-content:space-between;gap:18px;flex-wrap:wrap;margin:0 0 24px}.theme-picker{display:flex;flex-wrap:wrap;max-width:100%;align-items:center;gap:4px;padding:4px;background:var(--panel);border:1px solid var(--line);border-radius:10px}.theme-picker button,.layout-toggle{font-size:12px;min-height:40px;border-radius:7px;padding:8px 14px;background:transparent;color:var(--muted);border:1px solid transparent;font-weight:600}.theme-picker button[aria-pressed="true"]{background:var(--surface);color:var(--ink);border-color:var(--edge)}.theme-picker button::before{content:"";display:inline-block;width:8px;height:8px;margin-right:7px;border-radius:50%;background:#c19aff}.theme-picker button[data-theme="light"]::before{background:#087568}.theme-picker button[data-theme="dark"]::before{background:#c8ced8}.layout-toggle{color:var(--ink);background:var(--panel);border-color:var(--line)}.layout-toggle:hover{border-color:var(--accent)}
:root[data-layout="laptop"] .shell{min-width:960px}
:root[data-layout="laptop"] .preferences{max-width:calc(100vw - 80px)}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--ink);font:15px/1.6 "Segoe UI",system-ui,sans-serif}button{font:inherit}button:focus-visible,a:focus-visible{outline:3px solid var(--accent);outline-offset:5px}button{cursor:pointer}button:disabled{opacity:.45;cursor:wait}button:hover:not(:disabled){filter:brightness(1.13)}
.shell{max-width:1160px;margin:auto;padding:0 40px}.topbar{display:flex;justify-content:space-between;align-items:center;gap:20px;min-height:92px;border-bottom:1px solid var(--line)}.brand{display:flex;align-items:center;gap:13px;font-size:16px;font-weight:650;letter-spacing:.02em}.mark{display:grid;place-items:center;width:36px;height:36px;border:1px solid var(--edge);border-radius:10px;color:var(--accent);font-size:23px}.brand small{display:block;font-size:10px;letter-spacing:.18em;color:var(--muted);font-weight:500}.connection{display:flex;align-items:center;gap:9px;font-size:12px;color:var(--muted)}.dot{width:7px;height:7px;border-radius:50%;background:#8390a1}.connection[data-state="online"] .dot{background:var(--accent)}.connection[data-state="offline"] .dot{background:var(--err)}
.intro{padding:48px 0 30px;display:flex;justify-content:space-between;gap:24px;align-items:end}.eyebrow{font-size:10px;text-transform:uppercase;letter-spacing:.18em;color:var(--accent);font-weight:650;margin:0 0 9px}h1{font-size:clamp(28px,4vw,38px);line-height:1.2;font-weight:550;letter-spacing:-.035em;margin:0 0 12px}.subtitle{color:var(--muted);margin:0}.device{font-size:11px;text-align:right;color:var(--muted);line-height:1.8;white-space:nowrap}.device strong{color:var(--ink);font-weight:500}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:20px}.panel{background:var(--panel);border:1px solid var(--line);border-radius:14px;padding:26px}.panel-head{display:flex;justify-content:space-between;gap:16px;align-items:start}.panel h2{font-size:18px;font-weight:550;margin:1px 0 0;letter-spacing:-.02em}.number{font:11px monospace;color:var(--muted)}.hint{color:var(--muted);font-size:12px;margin:0}.visual{min-height:195px;display:flex;align-items:center;justify-content:center;gap:32px}.lamp{width:92px;height:92px;border:1px solid var(--edge);border-radius:50%;display:grid;place-items:center;color:var(--lamp-ink);background:var(--lamp-bg);transition:background .2s,color .2s,box-shadow .2s}.lamp svg{width:36px;height:36px}.lamp.on{color:var(--lamp-on-ink);background:var(--lamp-on-bg);border-color:var(--lamp-on-edge);box-shadow:0 0 50px #55d8be12}.state-label{font-size:10px;color:var(--muted);text-transform:uppercase;letter-spacing:.12em}.state-value{font-size:25px;font-weight:500;line-height:1.5}.state-note{font-size:11px;color:var(--muted)}
.primary,.secondary{width:100%;padding:12px 18px;border-radius:8px;font-size:13px;font-weight:650;border:1px solid transparent}.primary{background:var(--accent);color:var(--primary-ink)}.secondary{background:var(--surface);color:var(--ink);border-color:var(--edge)}.msg{font-size:12px;color:var(--err);min-height:20px;margin:9px 0 0}.msg.ok{color:var(--accent)}
.secondary[aria-busy="true"]::before{content:"";display:inline-block;width:.9em;height:.9em;margin-right:.6em;vertical-align:-.1em;border:2px solid currentColor;border-right-color:transparent;border-radius:50%;animation:spin .8s linear infinite}@keyframes spin{to{transform:rotate(360deg)}}
.esc-visual{min-height:195px;display:flex;align-items:center;justify-content:space-between;gap:20px;padding:20px 8px}.esc-symbol{color:var(--muted);display:block;font-size:26px;margin-bottom:10px;line-height:1}.switch{position:relative;flex:none;width:64px;height:36px;padding:0;border:1px solid var(--edge);border-radius:30px;background:var(--track);transition:background .25s,border-color .25s,box-shadow .25s}.switch::after{content:"";position:absolute;top:4px;left:4px;width:26px;height:26px;border-radius:50%;background:var(--knob);box-shadow:0 1px 4px #0006;transition:transform .35s cubic-bezier(.3,1.5,.5,1),width .15s,background .25s}.switch:active::after{width:32px}.switch[aria-checked="true"]{background:#4d2728;border-color:#bd6e63;box-shadow:0 0 22px -6px #ff7a6e}.switch[aria-checked="true"]::after{transform:translateX(28px);background:#ffb1a4}.switch[aria-checked="true"]:active::after{transform:translateX(22px)}.esc-note{font-size:12px;margin:0;padding:11px 14px;border:1px solid var(--edge);border-radius:8px;color:var(--muted);background:var(--surface)}
.telemetry{grid-column:1/-1;padding-bottom:22px}.stats{display:grid;grid-template-columns:repeat(3,1fr);margin:25px 0 23px;gap:24px}.metric{border-right:1px solid var(--line);padding:0 20px 0 0}.metric:last-child{border:0}.stats dt{font-size:12px;color:var(--muted);margin-bottom:5px}.stats dd{margin:0;font-size:27px;letter-spacing:-.025em;font-variant-numeric:tabular-nums;line-height:1.5}.stats.stale dd{color:var(--muted)}.system-footer{display:flex;align-items:center;justify-content:space-between;gap:20px;border-top:1px solid var(--line);padding-top:18px}.system-footer .secondary{width:auto;white-space:nowrap;font-size:12px;padding:9px 16px}.system-footer .msg{margin:0}.live-tag{color:var(--muted);font-size:11px}.footer{display:flex;justify-content:space-between;gap:16px;color:var(--muted);font-size:11px;padding:24px 0 32px}.footer span:last-child{color:var(--muted)}
:root[data-layout="phone"] .shell{padding:0 20px}:root[data-layout="phone"] .topbar{min-height:80px}:root[data-layout="phone"] .brand{font-size:14px}:root[data-layout="phone"] .connection{max-width:132px;font-size:11px}:root[data-layout="phone"] .intro{padding:32px 0 24px}:root[data-layout="phone"] .device{display:none}:root[data-layout="phone"] .grid{grid-template-columns:1fr;gap:16px}:root[data-layout="phone"] .panel{padding:22px}:root[data-layout="phone"] .telemetry{grid-column:auto}:root[data-layout="phone"] .visual,:root[data-layout="phone"] .esc-visual{min-height:170px}:root[data-layout="phone"] .stats{grid-template-columns:1fr;gap:14px;margin:20px 0}:root[data-layout="phone"] .metric{border-right:0;border-bottom:1px solid var(--line);padding:0 0 14px;display:flex;justify-content:space-between;align-items:center}:root[data-layout="phone"] .metric:last-child{padding:0}:root[data-layout="phone"] .stats dd{font-size:22px}:root[data-layout="phone"] .stats dt{margin:0}:root[data-layout="phone"] .system-footer{align-items:start;flex-direction:column;gap:12px}:root[data-layout="phone"] .footer{flex-direction:column;gap:3px}
:root[data-layout="phone"] .shell{max-width:480px}
@media(max-width:680px){:root:not([data-layout="laptop"]) .shell{padding:0 20px}:root:not([data-layout="laptop"]) .topbar{min-height:80px}:root:not([data-layout="laptop"]) .brand{font-size:14px}:root:not([data-layout="laptop"]) .connection{max-width:132px;font-size:11px}:root:not([data-layout="laptop"]) .intro{padding:32px 0 24px}:root:not([data-layout="laptop"]) .device{display:none}:root:not([data-layout="laptop"]) .grid{grid-template-columns:1fr;gap:16px}:root:not([data-layout="laptop"]) .panel{padding:22px}:root:not([data-layout="laptop"]) .telemetry{grid-column:auto}:root:not([data-layout="laptop"]) .visual,:root:not([data-layout="laptop"]) .esc-visual{min-height:170px}:root:not([data-layout="laptop"]) .stats{grid-template-columns:1fr;gap:14px;margin:20px 0}:root:not([data-layout="laptop"]) .metric{border-right:0;border-bottom:1px solid var(--line);padding:0 0 14px;display:flex;justify-content:space-between;align-items:center}:root:not([data-layout="laptop"]) .metric:last-child{padding:0}:root:not([data-layout="laptop"]) .stats dd{font-size:22px}:root:not([data-layout="laptop"]) .stats dt{margin:0}:root:not([data-layout="laptop"]) .system-footer{align-items:start;flex-direction:column;gap:12px}:root:not([data-layout="laptop"]) .footer{flex-direction:column;gap:3px}}
@media(prefers-reduced-motion:reduce){*{transition:none!important;animation:none!important}}
</style>
</head>
<body>
<div class="shell">
<header class="topbar"><div class="brand"><span class="mark" aria-hidden="true">⌁</span><div>AERIAL MATRIX<small>CONTROL SYSTEMS</small></div></div><div id="connection" class="connection" data-state="waiting" role="status"><span class="dot" aria-hidden="true"></span><span id="connection-text">Connecting…</span></div></header>
<main>
<div class="intro"><div><p class="eyebrow">Device console</p><h1>Controller overview</h1><p class="subtitle">Flight systems and lighting, in one place.</p></div><div class="device">CONTROL INTERFACE<br><strong>ESP32 · Local network</strong></div></div>
<div class="preferences" aria-label="Display preferences">
<div class="theme-picker" role="group" aria-label="Color theme">
<button id="theme-purple" type="button" data-theme="purple" aria-pressed="true">Purple (LP natural)</button>
<button id="theme-light" type="button" data-theme="light" aria-pressed="false">Light</button>
<button id="theme-dark" type="button" data-theme="dark" aria-pressed="false">Dark</button>
</div>
<button id="layout-toggle" class="layout-toggle" type="button">Use phone layout</button>
</div>
<div class="grid">
<section class="panel" aria-labelledby="light-title">
<div class="panel-head"><div><p class="eyebrow">Attachment</p><h2 id="light-title">Lighting control</h2></div><span class="number">01</span></div>
<div class="visual"><div id="lamp" class="lamp" aria-hidden="true"><svg viewBox="0 0 32 32" fill="none" stroke="currentColor" stroke-width="1.6"><path d="M11 22c0-5-5-5-5-12a10 10 0 0 1 20 0c0 7-5 7-5 12zM12 26h8M14 30h4M12 22v-8m8 8v-8m-8 0 4 3 4-3" transform="translate(0 1) scale(1 .9)"/></svg></div><div><div class="state-label">Light state</div><div id="state" class="state-value" aria-live="polite">Unknown</div><div class="state-note">Synced with the controller</div></div></div>
<button id="btn" class="primary" disabled>Toggle light</button><p id="msg" class="msg" role="status"></p>
</section>
<section class="panel" aria-labelledby="esc-title">
<div class="panel-head"><div><p class="eyebrow">Flight system</p><h2 id="esc-title">ESC control</h2></div><span class="number">02</span></div>
<div class="esc-visual"><div><span class="esc-symbol" aria-hidden="true">✣</span><div class="state-label">Controller arm state</div><div id="esc-state" class="state-value" aria-live="polite">Disarmed</div></div><button id="esc" class="switch" role="switch" aria-checked="false" aria-labelledby="esc-title" disabled></button></div>
<p class="esc-note">Arming enables motor output.</p><p id="esc-msg" class="msg" role="status"></p>
</section>
<section class="panel telemetry" aria-labelledby="system-title">
<div class="panel-head"><div><p class="eyebrow">System telemetry</p><h2 id="system-title">Controller health</h2></div><span id="updated" class="live-tag">Awaiting device</span></div>
<dl id="stats" class="stats stale"><div class="metric"><dt>Uptime</dt><dd id="uptime">—</dd></div><div class="metric"><dt>Available memory</dt><dd id="heap">—</dd></div><div class="metric"><dt>Chip temperature</dt><dd id="temp">—</dd></div></dl>
<div class="system-footer"><div><p class="hint">Restart controller</p><p class="hint">Briefly disconnects this console.</p><p id="reboot-msg" class="msg" role="status"></p></div><button id="reboot" class="secondary" disabled>Reboot ESP32</button></div>
</section>
</div>
</main>
<footer class="footer"><span>Aerial Matrix / Device console</span><span id="rate">Local control · Updates every second</span></footer>
</div>
<script>
const $ = id => document.getElementById(id);
// Preferences belong to this browser, and never send controller commands.
const themeNames = ["purple", "light", "dark"];
const compactScreen = window.matchMedia("(max-width: 680px)");
let layoutChoice = "auto";
function storedPreference(key) {
  try { return localStorage.getItem("aerial-matrix-" + key); } catch (_) { return null; }
}
function savePreference(key, value) {
  try { localStorage.setItem("aerial-matrix-" + key, value); } catch (_) { /* Storage may be unavailable. */ }
}
function applyTheme(theme) {
  // Preserve preferences saved by the previous theme set.
  if (theme === "navy") theme = "purple";
  if (theme === "graphite") theme = "dark";
  if (!themeNames.includes(theme)) theme = "purple";
  document.documentElement.dataset.theme = theme;
  for (const name of themeNames) $("theme-" + name).setAttribute("aria-pressed", String(name === theme));
  document.querySelector('meta[name="theme-color"]').setAttribute("content", {purple:"#110c1c",light:"#edf2f6",dark:"#101113"}[theme]);
}
function currentLayout() {
  return layoutChoice === "auto" ? (compactScreen.matches ? "phone" : "laptop") : layoutChoice;
}
function applyLayout() {
  document.documentElement.dataset.layout = layoutChoice;
  const current = currentLayout();
  $("layout-toggle").textContent = current === "phone" ? "Use laptop layout" : "Use phone layout";
  $("layout-toggle").setAttribute("aria-label", current === "phone" ? "Phone layout active. Switch to laptop layout" : "Laptop layout active. Switch to phone layout");
}
for (const name of themeNames) $("theme-" + name).addEventListener("click", () => {
  applyTheme(name); savePreference("theme", name);
});
$("layout-toggle").addEventListener("click", () => {
  layoutChoice = currentLayout() === "phone" ? "laptop" : "phone";
  applyLayout(); savePreference("layout", layoutChoice);
});
const savedLayout = storedPreference("layout");
if (savedLayout === "phone" || savedLayout === "laptop") layoutChoice = savedLayout;
applyTheme(storedPreference("theme"));
applyLayout();
compactScreen.addEventListener("change", () => { if (layoutChoice === "auto") applyLayout(); });

// How often telemetry and the light state refresh, in milliseconds.
const STATUS_INTERVAL_MS = 1000;
$("rate").textContent = "Local control · Updates every " + STATUS_INTERVAL_MS / 1000 + " s";

let escBusy = false, lightBusy = false, rebooting = false, connected = false, hasArmReadback = false;
// Starts disarmed. If /systemstatus ever includes "armed: 0/1", the switch follows it.
let armState = false;
const sleep = ms => new Promise(resolve => setTimeout(resolve, ms));
async function request(path, timeout = 2500) {
  const ctl = new AbortController();
  const timer = setTimeout(() => ctl.abort(), timeout);
  try {
    const r = await fetch(path, {cache:"no-store", signal:ctl.signal});
    if (!r.ok) throw new Error("HTTP " + r.status);
    return await r.text();
  } finally { clearTimeout(timer); }
}
async function alive() {
  try { await request("/state", 1500); return true; } catch (_) { return false; }
}
function controls() {
  $("btn").disabled = !connected || lightBusy || rebooting;
  $("esc").disabled = !connected || rebooting;
  $("reboot").disabled = !connected || rebooting;
}
function connection(ok) {
  connected = ok;
  $("connection").dataset.state = ok ? "online" : "offline";
  $("connection-text").textContent = ok ? "Device connected" : rebooting ? "Reconnecting…" : "Device unavailable";
  controls();
}
function setEsc(on) {
  armState = on;
  $("esc").setAttribute("aria-checked", String(on));
  $("esc-state").textContent = on ? "Armed" : "Disarmed";
  controls();
}
function showLight(text) {
  const on = text.trim().toUpperCase() === "ON";
  $("lamp").classList.toggle("on", on);
  $("state").textContent = on ? "On" : "Off";
}
$("btn").addEventListener("click", async () => {
  if (lightBusy || rebooting) return;
  lightBusy = true; controls(); $("msg").textContent = "";
  $("btn").setAttribute("aria-busy", "true");
  try { showLight(await request("/toggle")); }
  catch (_) { $("state").textContent = "Unknown"; $("lamp").classList.remove("on"); $("msg").textContent = "Light change unconfirmed. Check the light before retrying."; }
  finally { lightBusy = false; $("btn").removeAttribute("aria-busy"); controls(); }
});
$("esc").addEventListener("click", async () => {
  if (escBusy || rebooting) return;
  const want = !armState;
  if (want && !window.confirm("Arm ESCs and enable motor output?")) return;
  escBusy = true; $("esc").setAttribute("aria-busy", "true"); $("esc-msg").textContent = "";
  setEsc(want); // flip right away so the animation feels instant
  try { await request(want ? "/esc/arm" : "/esc/disarm"); }
  catch (_) { setEsc(!want); $("esc-msg").textContent = "Command unconfirmed. Switch left unchanged."; }
  finally { escBusy = false; $("esc").removeAttribute("aria-busy"); controls(); }
});
function fmtUptime(s) {
  const d = Math.floor(s / 86400), h = Math.floor(s % 86400 / 3600), m = Math.floor(s % 3600 / 60);
  return [d && d + "d", (d || h) && h + "h", (d || h || m) && m + "m", s % 60 + "s"].filter(Boolean).join(" ");
}
async function pollStatus() {
  try {
    const text = await request("/systemstatus");
    const up = /uptime:\s*(\d+)/.exec(text), heap = /free heap:\s*(\d+)/.exec(text), temp = /temp:\s*(-?\d+)/.exec(text), arm = /armed:\s*([01])/.exec(text);
    if (!up || !heap) throw new Error("Unexpected status response");
    $("uptime").textContent = fmtUptime(Number(up[1]));
    $("heap").textContent = (Number(heap[1]) / 1024).toFixed(1) + " KiB";
    $("temp").textContent = temp ? temp[1] + " °C" : "—";
    $("stats").classList.remove("stale"); $("updated").textContent = "Updated just now";
    if (arm) { hasArmReadback = true; if (!escBusy) setEsc(arm[1] === "1"); }
    connection(true);
    return true;
  } catch (_) {
    $("uptime").textContent = $("heap").textContent = $("temp").textContent = "—";
    $("stats").classList.add("stale"); $("updated").textContent = "Telemetry unavailable";
    $("state").textContent = "Unknown"; $("lamp").classList.remove("on");
    connection(false); return false;
  }
}
$("reboot").addEventListener("click", async () => {
  if (rebooting) return;
  if (!window.confirm("Restart the controller? The console will briefly disconnect.")) return;
  rebooting = true; controls(); $("reboot").setAttribute("aria-busy", "true");
  $("reboot").textContent = "Rebooting…"; $("reboot-msg").textContent = "Waiting for the controller to restart…";
  $("reboot-msg").classList.add("ok");
  connection(false);
  try { await request("/reboot", 3000); } catch (_) { /* A restart may interrupt the reply. */ }
  const start = Date.now(); let back = false;
  // Wait for it to drop off the network first (up to 5 s) so a slow shutdown isn't mistaken for a finished reboot.
  while (Date.now() - start < 5000 && await alive()) await sleep(300);
  while (Date.now() - start < 60000) {
    if (await pollStatus()) { back = true; break; }
    await sleep(1000);
  }
  if (back && !hasArmReadback) { setEsc(false); $("esc-msg").textContent = ""; } // ESC boots disarmed
  $("reboot-msg").textContent = back ? "Controller reconnected." : "Reconnect to the controller Wi-Fi and check power.";
  $("reboot-msg").classList.toggle("ok", back);
  rebooting = false; $("reboot").removeAttribute("aria-busy"); $("reboot").textContent = "Reboot ESP32"; controls();
});
(async function statusLoop() {
  for (;;) {
    if (!document.hidden && !rebooting) {
      await pollStatus();
      if (connected && !lightBusy && !rebooting) {
        try { showLight(await request("/state")); }
        catch (_) { $("state").textContent = "Unknown"; $("lamp").classList.remove("on"); }
      }
    }
    await sleep(STATUS_INTERVAL_MS);
  }
})();
</script>
</body>
</html>
)rawliteral";