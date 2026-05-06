#!/usr/bin/env python3

import os
import sys
import json
import urllib.parse

SCORES_FILE = os.path.join(os.path.dirname(__file__), "scores.json")

def load_scores():
    try:
        with open(SCORES_FILE) as f:
            return json.load(f)
    except:
        return []

def save_scores(scores):
    with open(SCORES_FILE, "w") as f:
        json.dump(scores, f)

def handle_get():
    scores = load_scores()
    print("Content-Type: application/json\r")
    print("\r")
    print(json.dumps(scores))

def handle_post():
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body   = sys.stdin.read(length)
    params = urllib.parse.parse_qs(body)

    name  = params.get("name",  ["anonymous"])[0][:20]
    try:
        score = int(params.get("score", [0])[0])
    except:
        score = 0

    # Sanitize name
    name = name.strip() or "anonymous"

    scores = load_scores()
    scores.append({"name": name, "score": score})
    scores.sort(key=lambda x: x["score"], reverse=True)
    scores = scores[:10]  # top 10 bijhouden
    save_scores(scores)

    print("Content-Type: application/json\r")
    print("\r")
    print(json.dumps({"ok": True}))

method = os.environ.get("REQUEST_METHOD", "GET").upper()

if method == "POST":
    handle_post()
else:
    handle_get()