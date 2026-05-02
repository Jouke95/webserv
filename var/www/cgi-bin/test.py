#!/usr/bin/env python3
import os
import sys

method = os.environ.get('REQUEST_METHOD', '')
body = sys.stdin.read() if method == 'POST' else ''

print("Content-Type: text/plain\r")
print("\r")
print("Method: " + method + "\r")
print("Body: " + body + "\r")