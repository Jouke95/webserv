#!/usr/bin/env python3

import os
import time
import subprocess

def read_file(path):
    try:
        with open(path) as f:
            return f.read().strip()
    except:
        return "N/A"

def get_uptime():
    try:
        seconds = float(read_file("/proc/uptime").split()[0])
        h = int(seconds // 3600)
        m = int((seconds % 3600) // 60)
        s = int(seconds % 60)
        return f"{h}h {m}m {s}s"
    except:
        return "N/A"

def get_load():
    try:
        parts = read_file("/proc/loadavg").split()
        return parts[0], parts[1], parts[2]
    except:
        return "N/A", "N/A", "N/A"

def get_memory():
    try:
        info = {}
        for line in open("/proc/meminfo"):
            parts = line.split()
            if parts[0] in ("MemTotal:", "MemAvailable:"):
                info[parts[0]] = int(parts[1])
        total = info["MemTotal:"] // 1024
        avail = info["MemAvailable:"] // 1024
        used  = total - avail
        pct   = round(used / total * 100)
        return total, used, avail, pct
    except:
        return 0, 0, 0, 0

def get_disk():
    try:
        out = subprocess.check_output(["df", "-h", "/"], text=True).splitlines()[1].split()
        return out[1], out[2], out[3], out[4]  # total, used, avail, pct
    except:
        return "N/A", "N/A", "N/A", "N/A"

def get_cpu_cores():
    try:
        count = 0
        for line in open("/proc/cpuinfo"):
            if line.startswith("processor"):
                count += 1
        return count
    except:
        return "N/A"

load1, load5, load15 = get_load()
mem_total, mem_used, mem_avail, mem_pct = get_memory()
disk_total, disk_used, disk_avail, disk_pct = get_disk()
now = time.strftime("%Y-%m-%d %H:%M:%S")

# CGI header — verplicht
print("Content-Type: text/html\r")
print("\r")

print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="5">
    <title>Server Stats</title>
    <style>
        * {{ box-sizing: border-box; margin: 0; padding: 0; }}
        body {{
            font-family: 'Courier New', monospace;
            background: #0d0d0d;
            color: #e0e0e0;
            padding: 40px;
        }}
        h1 {{
            font-size: 1.4rem;
            color: #00ff99;
            margin-bottom: 8px;
            letter-spacing: 2px;
            text-transform: uppercase;
        }}
        .subtitle {{
            font-size: 0.75rem;
            color: #555;
            margin-bottom: 40px;
        }}
        .grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
            gap: 20px;
        }}
        .card {{
            background: #1a1a1a;
            border: 1px solid #2a2a2a;
            border-radius: 8px;
            padding: 20px;
        }}
        .card-title {{
            font-size: 0.7rem;
            color: #555;
            text-transform: uppercase;
            letter-spacing: 1px;
            margin-bottom: 12px;
        }}
        .card-value {{
            font-size: 1.8rem;
            color: #00ff99;
            font-weight: bold;
        }}
        .card-sub {{
            font-size: 0.75rem;
            color: #666;
            margin-top: 6px;
        }}
        .bar-bg {{
            background: #2a2a2a;
            border-radius: 4px;
            height: 6px;
            margin-top: 10px;
            overflow: hidden;
        }}
        .bar-fill {{
            height: 100%;
            background: #00ff99;
            border-radius: 4px;
            transition: width 0.3s;
        }}
        .bar-fill.warn  {{ background: #ffaa00; }}
        .bar-fill.alert {{ background: #ff4444; }}
        .load-row {{
            display: flex;
            justify-content: space-between;
            margin-top: 4px;
        }}
        .load-label {{ color: #555; font-size: 0.75rem; }}
        .load-val   {{ color: #e0e0e0; font-size: 0.75rem; }}
    </style>
</head>
<body>
    <h1>&#9608; webserv / stats</h1>
    <p class="subtitle">refreshes every 5 seconds &mdash; {now}</p>

    <div class="grid">

        <div class="card">
            <div class="card-title">Uptime</div>
            <div class="card-value">{get_uptime()}</div>
            <div class="card-sub">{get_cpu_cores()} CPU core(s)</div>
        </div>

        <div class="card">
            <div class="card-title">Load Average</div>
            <div class="load-row">
                <span class="load-label">1 min</span>
                <span class="load-val">{load1}</span>
            </div>
            <div class="load-row">
                <span class="load-label">5 min</span>
                <span class="load-val">{load5}</span>
            </div>
            <div class="load-row">
                <span class="load-label">15 min</span>
                <span class="load-val">{load15}</span>
            </div>
        </div>

        <div class="card">
            <div class="card-title">Memory</div>
            <div class="card-value">{mem_pct}%</div>
            <div class="card-sub">{mem_used} MB used / {mem_total} MB total</div>
            <div class="bar-bg">
                <div class="bar-fill {'alert' if mem_pct > 85 else 'warn' if mem_pct > 60 else ''}"
                     style="width: {mem_pct}%"></div>
            </div>
        </div>

        <div class="card">
            <div class="card-title">Disk (/)</div>
            <div class="card-value">{disk_pct}</div>
            <div class="card-sub">{disk_used} used / {disk_total} total &mdash; {disk_avail} free</div>
            <div class="bar-bg">
                <div class="bar-fill {'alert' if int(disk_pct.strip('%') or 0) > 85 else 'warn' if int(disk_pct.strip('%') or 0) > 60 else ''}"
                     style="width: {disk_pct}"></div>
            </div>
        </div>

    </div>
</body>
</html>
""")