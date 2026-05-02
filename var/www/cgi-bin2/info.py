#!/usr/bin/env python3
import os
import sys
import json

content_length = int(os.environ.get("CONTENT_LENGTH", 0) or 0)
body = ""
if content_length > 0:
    body = sys.stdin.read(content_length)

env_keys = [
    "REQUEST_METHOD", "QUERY_STRING", "SCRIPT_FILENAME",
    "SCRIPT_NAME", "PATH_INFO", "SERVER_NAME", "SERVER_PORT",
    "SERVER_PROTOCOL", "HTTP_HOST", "CONTENT_TYPE", "CONTENT_LENGTH",
    "GATEWAY_INTERFACE", "REDIRECT_STATUS"
]

data = {k: os.environ.get(k, "") for k in env_keys}

rows = ""
for k, v in data.items():
    val = v if v else "<em style='color:#444'>empty</em>"
    rows += f"<tr><td>{k}</td><td>{val}</td></tr>"

output = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>CGI env — webserv</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600&display=swap');
  body {{ background: #0a0a0f; color: #e0e0f0; font-family: 'JetBrains Mono', monospace; padding: 40px; }}
  a {{ color: #7b8cff; font-size: 12px; text-decoration: none; display: block; margin-bottom: 32px; }}
  h1 {{ font-size: 20px; color: #7b8cff; margin-bottom: 24px; }}
  table {{ border-collapse: collapse; width: 100%; max-width: 700px; }}
  td {{ padding: 8px 16px; border-bottom: 1px solid #1e1e2e; font-size: 13px; }}
  td:first-child {{ color: #7b8cff; width: 220px; }}
</style>
</head>
<body>
<a href="/">← back to index</a>
<h1>CGI environment</h1>
<table>{rows}</table>
</body>
</html>"""

print("Content-Type: text/html")
print(f"Content-Length: {len(output)}")
print()
print(output)
