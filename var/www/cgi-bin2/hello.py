#!/usr/bin/env python3
import os
import urllib.parse

method = os.environ.get("REQUEST_METHOD", "")
query  = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query)

name = params.get("name", [""])[0]
age  = params.get("age",  [""])[0]

def row(label, value):
    val = value if value else "<em>empty</em>"
    return f"<div class='row'><span class='label'>{label}</span><span class='value'>{val}</span></div>"

output = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>hello.py — webserv</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600&display=swap');
  :root {{
    --bg:#0a0a0f;--surface:#111118;--border:#1e1e2e;
    --accent:#00ff9d;--text:#e0e0f0;--muted:#555570;
    --mono:'JetBrains Mono',monospace;
  }}
  *{{margin:0;padding:0;box-sizing:border-box;}}
  body{{background:var(--bg);color:var(--text);font-family:var(--mono);min-height:100vh;padding:48px 24px;}}
  body::before{{content:'';position:fixed;inset:0;
    background-image:linear-gradient(rgba(0,255,157,0.03) 1px,transparent 1px),linear-gradient(90deg,rgba(0,255,157,0.03) 1px,transparent 1px);
    background-size:40px 40px;pointer-events:none;}}
  .wrap{{max-width:600px;margin:0 auto;position:relative;z-index:1;}}
  .back{{font-size:11px;color:var(--muted);text-decoration:none;display:block;margin-bottom:32px;}}
  .back:hover{{color:var(--accent);}}
  .badge{{font-size:11px;color:var(--accent);letter-spacing:3px;text-transform:uppercase;margin-bottom:12px;display:flex;align-items:center;gap:8px;}}
  .badge::before{{content:'';width:6px;height:6px;background:var(--accent);border-radius:50%;box-shadow:0 0 8px var(--accent);}}
  h1{{font-size:22px;font-weight:600;margin-bottom:32px;}}
  h1 span{{color:var(--accent);}}
  .row{{display:flex;gap:16px;padding:12px 16px;border-bottom:1px solid var(--border);font-size:13px;}}
  .row:first-child{{border-top:1px solid var(--border);}}
  .label{{color:var(--muted);width:140px;flex-shrink:0;}}
  .value{{color:var(--text);}}
  em{{color:#333;font-style:normal;}}
  .hint{{font-size:11px;color:var(--muted);margin-top:24px;}}
  .hint a{{color:var(--accent);text-decoration:none;}}
</style>
</head>
<body>
<div class="wrap">
  <a class="back" href="/">← back to index</a>
  <div class="badge">🐍 python CGI</div>
  <h1>hello <span>world</span></h1>
  {row("method", method)}
  {row("query_string", query)}
  {row("name", name)}
  {row("age", age)}
  <p class="hint">try: <a href="/cgi/hello.py?name=jouke&age=21">/cgi/hello.py?name=jouke&age=21</a></p>
</div>
</body>
</html>"""

print("Content-Type: text/html")
print(f"Content-Length: {len(output)}")
print()
print(output)