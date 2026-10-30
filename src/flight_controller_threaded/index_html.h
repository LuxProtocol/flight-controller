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
.flight{grid-column:1/-1}.flight-body{display:grid;grid-template-columns:1.35fr 1fr;gap:20px;margin:22px 0 14px}.drone-view{position:relative;min-height:300px;border:1px solid var(--line);border-radius:12px;background:var(--surface);overflow:hidden}.drone-view canvas{position:absolute;inset:0;width:100%;height:100%;display:block}
.motor-map{display:flex;flex-direction:column;gap:8px}.front-label{text-align:center;font-size:10px;letter-spacing:.18em;color:var(--muted)}
.motor-grid{position:relative;flex:1;display:grid;grid-template-columns:1fr 1fr;gap:14px;padding:4px;background:linear-gradient(to top right,transparent calc(50% - 1px),var(--edge) 50%,transparent calc(50% + 1px)),linear-gradient(to bottom right,transparent calc(50% - 1px),var(--edge) 50%,transparent calc(50% + 1px))}.motor-grid::after{content:"";position:absolute;left:50%;top:50%;width:12px;height:12px;margin:-6px;border-radius:50%;background:var(--panel);border:2px solid var(--edge)}
.motor{--p:0;position:relative;z-index:1;display:grid;justify-items:center;gap:1px;padding:12px 8px;background:var(--panel);border:1px solid var(--line);border-top:3px solid var(--c);border-radius:12px;text-align:center}
.ring{position:relative;display:grid;place-items:center;width:72px;height:72px;margin-bottom:6px;border-radius:50%;background:conic-gradient(var(--c) calc(var(--p)*1%),var(--track) 0)}.ring::before{content:"";position:absolute;inset:7px;border-radius:50%;background:var(--panel)}.ring b{position:relative;font-size:16px;font-weight:600;font-variant-numeric:tabular-nums}
.m-name{font-size:12px;font-weight:650}.m-pos{font-size:10px;color:var(--muted);text-transform:uppercase;letter-spacing:.1em}.m-val{font-size:13px;font-variant-numeric:tabular-nums}.mono{font-family:Consolas,Monaco,monospace;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
:root[data-layout="phone"] .flight-body{grid-template-columns:1fr}@media(max-width:680px){:root:not([data-layout="laptop"]) .flight-body{grid-template-columns:1fr}}
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
<section class="panel flight" aria-labelledby="flight-title">
<div class="panel-head"><div><p class="eyebrow">Flight data</p><h2 id="flight-title">Attitude &amp; motor output</h2></div><span id="flight-tag" class="live-tag">Awaiting stream</span></div>
<div class="flight-body">
<div class="drone-view"><canvas id="drone" role="img" aria-label="Live 3D view of the drone’s attitude"></canvas></div>
<div class="motor-map" role="group" aria-label="Motor outputs, top view, front at the top"><div class="front-label">▲ FRONT</div><div id="motor-grid" class="motor-grid"></div></div>
</div>
<p id="flight-raw" class="hint mono">Last message: —</p>
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

// ---- Flight data: live attitude and motor outputs over server-sent events ----
const FLIGHT_STREAM_URL = "/events";  // AsyncEventSource path on the ESP32
const FLIGHT_EVENT = "flightData";    // event name passed to events.send()
const FLIGHT_STALE_MS = 2000;         // no message for this long = stream stalled
const MOTOR_PWM_MIN = 1000, MOTOR_PWM_MAX = 2000;                // ESC pulse range, µs
const MOTOR_VECTOR_SCALE = 5, ARROW_MIN = 0.4, ARROW_MAX = 6;    // thrust arrow length, model units
// Body frame: +Y is up and the front is -Z. Order must match the firmware's motor order.
const MOTORS = [
  {name: "M1", pos: "Front Right", color: "#cc3333", at: [ 5, 0, -5], spin:  1},
  {name: "M2", pos: "Front Left",  color: "#33aa33", at: [-5, 0, -5], spin: -1},
  {name: "M3", pos: "Rear Left",   color: "#3366cc", at: [-5, 0,  5], spin:  1},
  {name: "M4", pos: "Rear Right",  color: "#ccaa00", at: [ 5, 0,  5], spin: -1},
];
const flight = {
  q: [0, 0, 0, 1], gotQuat: false, calibrated: false, hasMotors: false, dirty: false, last: 0, raw: "",
  motors: MOTORS.map(() => ({pwm: null, vec: [0, 0, 0]})),
};

// Quaternions are [x, y, z, w].
const qMul = (a, b) => [
  a[3]*b[0] + a[0]*b[3] + a[1]*b[2] - a[2]*b[1],
  a[3]*b[1] - a[0]*b[2] + a[1]*b[3] + a[2]*b[0],
  a[3]*b[2] + a[0]*b[1] - a[1]*b[0] + a[2]*b[3],
  a[3]*b[3] - a[0]*b[0] - a[1]*b[1] - a[2]*b[2]];
const qAxis = (axis, deg) => {
  const h = deg * Math.PI / 360, s = Math.sin(h);
  return [axis === "x" ? s : 0, axis === "y" ? s : 0, axis === "z" ? s : 0, Math.cos(h)];
};
function qRot(q, v) {
  const [x, y, z, w] = q, tx = 2*(y*v[2] - z*v[1]), ty = 2*(z*v[0] - x*v[2]), tz = 2*(x*v[1] - y*v[0]);
  return [v[0] + w*tx + y*tz - z*ty, v[1] + w*ty + z*tx - x*tz, v[2] + w*tz + x*ty - y*tx];
}
// Euler angles (degrees, YZX order) to a quaternion, matching the serial viewer.
function eulerToQuat(o, calibrated) {
  const [x, y, z] = calibrated ? [360 - o[2], o[0], o[1]] : [o[2], o[0] - 180, -o[1]];
  return qMul(qMul(qAxis("y", y), qAxis("z", z)), qAxis("x", x));
}

const clamp01 = x => Math.min(1, Math.max(0, x));
const thrustOf = pwm => clamp01((pwm - MOTOR_PWM_MIN) / (MOTOR_PWM_MAX - MOTOR_PWM_MIN));
const motorLevel = m => m.pwm != null ? thrustOf(m.pwm) : clamp01(Math.hypot(...m.vec));

// Accepts the same lines the serial viewer reads:
//   "Quaternion: w,x,y,z"   "Orientation: heading,roll,pitch"   "Calibration: ..."
//   "Motors: p1,p2,p3,p4" (PWM µs)  or  "Motors: x1,y1,z1, ... x4,y4,z4" (vectors)
function handleFlightLine(line) {
  const m = /^(orientation|quaternion|calibration|motors?)\s*:\s*(.*)$/i.exec(line);
  if (!m) return;
  const key = m[1].toLowerCase(), v = m[2].split(",").map(Number);
  if (v.some(Number.isNaN)) return;
  if (key === "quaternion" && v.length >= 4) { flight.q = [v[1], v[3], -v[2], v[0]]; flight.gotQuat = true; }
  else if (key === "orientation" && v.length >= 3) { if (!flight.gotQuat) flight.q = eulerToQuat(v, flight.calibrated); }
  else if (key === "calibration") flight.calibrated = true;
  else if (key.startsWith("motor")) {
    if (v.length >= 12) MOTORS.forEach((_, i) => { flight.motors[i] = {pwm: null, vec: v.slice(i*3, i*3 + 3)}; });
    else if (v.length >= 4) MOTORS.forEach((_, i) => { flight.motors[i] = {pwm: v[i], vec: [0, thrustOf(v[i]), 0]}; });
    else return;
    flight.hasMotors = true;
  }
}
const flightSource = new EventSource(FLIGHT_STREAM_URL);
flightSource.addEventListener(FLIGHT_EVENT, e => {
  flight.last = performance.now();
  for (const line of String(e.data).split("\n")) { flight.raw = line.trim(); handleFlightLine(flight.raw); }
  flight.dirty = true;
});

// Motor map: front motors on the top row, rear on the bottom.
const motorCards = [1, 0, 2, 3].map(i => {
  const m = MOTORS[i], el = document.createElement("div");
  el.className = "motor"; el.style.setProperty("--c", m.color);
  el.innerHTML = `<div class="ring"><b>--</b></div><div class="m-name">${m.name}</div><div class="m-pos">${m.pos}</div><div class="m-val">— µs</div>`;
  $("motor-grid").appendChild(el);
  return {i, el, pct: el.querySelector("b"), val: el.querySelector(".m-val")};
});
function updateMotorMap() {
  if (!flight.hasMotors) return;
  for (const c of motorCards) {
    const m = flight.motors[c.i], lvl = motorLevel(m);
    c.el.style.setProperty("--p", (lvl * 100).toFixed(1));
    c.pct.textContent = Math.round(lvl * 100) + "%";
    c.val.textContent = m.pwm != null ? Math.round(m.pwm) + " µs" : "|F| " + Math.hypot(...m.vec).toFixed(2);
  }
}

// Drone view: a small 3D projection drawn on a 2D canvas, so no external libraries are needed
// (the ESP32's own Wi-Fi network usually has no internet access to load one from).
const calm = window.matchMedia("(prefers-reduced-motion: reduce)").matches;
const phase = MOTORS.map(() => 0);
const camera = qMul(qAxis("x", 24), qAxis("y", -35));   // fixed viewing angle: from above and behind
let palette = null, paletteAt = -1e9;
function readPalette(now) {
  if (palette && now - paletteAt < 500) return palette;   // re-read so theme changes show up
  const cs = getComputedStyle(document.documentElement), g = n => cs.getPropertyValue(n).trim();
  paletteAt = now;
  return palette = {ink: g("--ink"), muted: g("--muted"), line: g("--line"), edge: g("--edge"), accent: g("--accent")};
}
function drawDrone(now, dt) {
  const cv = $("drone"), W = cv.clientWidth, H = cv.clientHeight;
  if (!W || !H) return;
  const dpr = window.devicePixelRatio || 1;
  if (cv.width !== Math.round(W * dpr) || cv.height !== Math.round(H * dpr)) {
    cv.width = Math.round(W * dpr); cv.height = Math.round(H * dpr);
  }
  const ctx = cv.getContext("2d"), P = readPalette(now), f = Math.min(W, H) * 1.5;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  ctx.clearRect(0, 0, W, H);
  ctx.lineCap = "round"; ctx.lineJoin = "round";

  // Returns [screenX, screenY, depth, pixelsPerUnit]. The camera sits at z = 30.
  const toScreen = p => { const v = qRot(camera, p), s = f / (30 - v[2]); return [W/2 + v[0]*s, H/2 - v[1]*s, v[2], s]; };
  const body = p => toScreen(qRot(flight.q, p));   // also applies the drone's attitude
  const path = pts => { ctx.beginPath(); pts.forEach((p, i) => i ? ctx.lineTo(p[0], p[1]) : ctx.moveTo(p[0], p[1])); };
  const seg = (a, b) => path([a, b]);
  const ring = (c, r) => Array.from({length: 20}, (_, k) => [c[0] + r*Math.cos(k*Math.PI/10), c[1], c[2] + r*Math.sin(k*Math.PI/10)]);

  // Floor grid and shadow stay fixed in the world, so the drone's tilt reads against them.
  ctx.strokeStyle = P.line; ctx.lineWidth = 1;
  for (let i = -12; i <= 12; i += 4) {
    seg(toScreen([i, -7, -12]), toScreen([i, -7, 12])); ctx.stroke();
    seg(toScreen([-12, -7, i]), toScreen([12, -7, i])); ctx.stroke();
  }
  path(ring([0, -7, 0], 6).map(toScreen)); ctx.closePath();
  ctx.globalAlpha = .25; ctx.fillStyle = "#000"; ctx.fill(); ctx.globalAlpha = 1;

  // Collect the parts, then paint them far to near.
  const hub = body([0, 0, 0]), items = [];
  MOTORS.forEach((m, i) => {
    const top = [m.at[0], .35, m.at[2]], mp = body(m.at), mo = flight.motors[i], lvl = motorLevel(mo);
    if (!calm) phase[i] += m.spin * lvl * 14 * dt;   // propellers spin faster with more thrust
    items.push({z: Math.min(hub[2], mp[2]) - .001, draw() {
      ctx.strokeStyle = P.muted; ctx.lineWidth = .4 * hub[3]; seg(hub, mp); ctx.stroke();
    }});
    items.push({z: mp[2], draw() {
      ctx.fillStyle = m.color;
      ctx.beginPath(); ctx.arc(mp[0], mp[1], .55 * mp[3], 0, 7); ctx.fill();
      path(ring(top, 1.6).map(body)); ctx.closePath();
      ctx.globalAlpha = .12 + .25 * lvl; ctx.fill();
      ctx.globalAlpha = .8; ctx.strokeStyle = m.color; ctx.lineWidth = 1.5; ctx.stroke(); ctx.globalAlpha = 1;
      const tb = body(top), dx = 1.6 * Math.cos(phase[i]), dz = 1.6 * Math.sin(phase[i]);
      ctx.lineWidth = .2 * tb[3];
      seg(body([top[0] + dx, top[1], top[2] + dz]), body([top[0] - dx, top[1], top[2] - dz])); ctx.stroke();
      const mag = Math.hypot(...mo.vec);   // thrust arrow, same scaling as the serial viewer
      if (mag > 1e-6) {
        const len = Math.min(ARROW_MAX, Math.max(ARROW_MIN, mag * MOTOR_VECTOR_SCALE));
        const tip = body(top.map((c, k) => c + mo.vec[k] / mag * len));
        const ax = tip[0] - tb[0], ay = tip[1] - tb[1], d = Math.hypot(ax, ay) || 1;
        const ux = ax / d, uy = ay / d, px = -uy, py = ux, hs = .7 * tip[3];
        ctx.strokeStyle = ctx.fillStyle = m.color; ctx.lineWidth = Math.max(2, .22 * tb[3]);
        seg(tb, tip); ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(tip[0] + ux*hs, tip[1] + uy*hs);
        ctx.lineTo(tip[0] - ux*hs*.6 + px*hs*.6, tip[1] - uy*hs*.6 + py*hs*.6);
        ctx.lineTo(tip[0] - ux*hs*.6 - px*hs*.6, tip[1] - uy*hs*.6 - py*hs*.6);
        ctx.closePath(); ctx.fill();
      }
    }});
  });
  items.push({z: hub[2], draw() {
    path(ring([0, .4, 0], 1.4).map(body)); ctx.closePath();
    ctx.fillStyle = P.ink; ctx.globalAlpha = .9; ctx.fill(); ctx.globalAlpha = 1;
    ctx.strokeStyle = P.edge; ctx.lineWidth = 1.5; ctx.stroke();
    ctx.strokeStyle = P.accent; ctx.lineWidth = .35 * hub[3];
    seg(body([0, .4, -.6]), body([0, .4, -2.4])); ctx.stroke();   // nose marker points to the front
  }});
  items.sort((a, b) => a.z - b.z).forEach(it => it.draw());

  if (!flight.last || now - flight.last > FLIGHT_STALE_MS) {
    ctx.fillStyle = P.muted; ctx.font = "12px system-ui, sans-serif"; ctx.textAlign = "left";
    ctx.fillText("Waiting for flight data…", 14, H - 14);
  }
}
let lastFrame = performance.now(), lastUi = 0, flightTag = "";
(function frame(now) {
  const dt = Math.min(.05, (now - lastFrame) / 1000); lastFrame = now;
  drawDrone(now, dt);
  if (now - lastUi > 66) {   // text updates at about 15 Hz, the drawing runs every frame
    lastUi = now;
    if (flight.dirty) { flight.dirty = false; updateMotorMap(); $("flight-raw").textContent = "Last message: " + (flight.raw || "—"); }
    const tag = !flight.last ? "Awaiting stream" : now - flight.last > FLIGHT_STALE_MS ? "Stream stalled" : "Streaming";
    if (tag !== flightTag) $("flight-tag").textContent = flightTag = tag;
  }
  requestAnimationFrame(frame);
})(performance.now());
</script>
</body>
</html>
)rawliteral";