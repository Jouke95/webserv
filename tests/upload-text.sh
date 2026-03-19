#!/bin/bash

HOST="http://localhost:8080"

echo "=== Response from $HOST ==="
echo ""
curl -v -X POST -d "Test bericht 5891" "$HOST/text"
echo ""
echo "==========================="

echo "=== POST ==="
curl -v -X POST -d "username=foo&password=bar" "$HOST/"
echo ""