#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qs

content_length = int(os.environ.get("CONTENT_LENGTH", 0) or 0)
body = ""
if content_length > 0:
    body = sys.stdin.read(content_length)

params = parse_qs(body)
name = params.get("name", [""])[0]
message = params.get("message", [""])[0]

output = f"""name:    {name}
message: {message}

method:  {os.environ.get('REQUEST_METHOD', '')}
host:    {os.environ.get('HTTP_HOST', '')}"""

print("Content-Type: text/plain")
print(f"Content-Length: {len(output)}")
print()
print(output)
