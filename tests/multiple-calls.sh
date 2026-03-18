#!/bin/bash

HOST="http://localhost:8080"

echo "=== GET ==="
curl -v "$HOST/"
echo ""

echo "=== POST ==="
curl -v -X POST -d "username=foo&password=bar" "$HOST/"
echo ""

echo "=== PUT ==="
curl -v -X PUT -d "some data" "$HOST/upload"
echo ""

echo "=== DELETE ==="
curl -v -X DELETE "$HOST/some_resource"
echo ""