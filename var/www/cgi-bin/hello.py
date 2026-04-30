#!/usr/bin/env python3
import os

# ── Stap 1: headers ──────────────────────────────────────────
print("Content-Type: text/html")
print()  # lege regel = einde van headers

# ── Stap 2: lees wat de server ons geeft ─────────────────────
method = os.environ.get("REQUEST_METHOD", "onbekend")
query  = os.environ.get("QUERY_STRING", "")   # bv: name=Jouke

# ── Stap 3: schrijf de HTML pagina ───────────────────────────
print("<html>")
print("<body>")

print("<h1>Hallo van CGI!</h1>")
print(f"<p>Method: {method}</p>")
print(f"<p>Query string: {query}</p>")

print("</body>")
print("</html>")

