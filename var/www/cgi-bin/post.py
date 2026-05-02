#!/usr/bin/env python3
import os
import sys
import urllib.parse

print("Content-Type: text/html")
print()

method = os.environ.get("REQUEST_METHOD", "")
query  = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query)
length = int(os.environ.get("CONTENT_LENGTH", 0) or 0)

body = sys.stdin.read(length) if length > 0 else ""
params = urllib.parse.parse_qs(body)

name = params.get("name", [""])[0]
age  = params.get("age", [""])[0]

print("<html><body>")
print(f"<p>Method: {method}</p>")
print(f"<p>Query string: {query}</p>")
print(f"<p>Name: {name}</p>")
print(f"<p>Age: {age}</p>")
print("</body></html>")