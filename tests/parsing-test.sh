#!/bin/bash

HOST="http://localhost:8080"
PASS=0
FAIL=0

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

print_result() {
    local label="$1"
    local expected="$2"
    local actual="$3"

    echo -e "${CYAN}[TEST] $label${NC}"
    if echo "$actual" | grep -q "$expected"; then
        echo -e "${GREEN}  PASS${NC} (matched: '$expected')"
        ((PASS++))
    else
        echo -e "${RED}  FAIL${NC} (expected: '$expected', got: '$actual')"
        ((FAIL++))
    fi
    echo ""
}

echo "==============================="
echo "   webserv curl test suite"
echo "==============================="
echo ""

# --- GET ---

echo -e "${CYAN}--- GET Requests ---${NC}"

# Basic GET
result=$(curl -s -o /dev/null -w "%{http_code}" "$HOST/")
print_result "GET /" "200\|301\|302\|404" "$result"

# GET non-existent resource
result=$(curl -s -o /dev/null -w "%{http_code}" "$HOST/does_not_exist")
print_result "GET /does_not_exist (expect 404)" "404" "$result"

# GET with query string
result=$(curl -s -o /dev/null -w "%{http_code}" "$HOST/?foo=bar&baz=qux")
print_result "GET with query string" "200\|404" "$result"

# GET with custom header
result=$(curl -s -o /dev/null -w "%{http_code}" -H "X-Custom-Header: hello" "$HOST/")
print_result "GET with custom header" "200\|404" "$result"

# GET with Accept header
result=$(curl -s -o /dev/null -w "%{http_code}" -H "Accept: text/html" "$HOST/")
print_result "GET with Accept: text/html" "200\|404" "$result"

# --- POST ---

echo -e "${CYAN}--- POST Requests ---${NC}"

# POST with form data
result=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -d "username=foo&password=bar" "$HOST/")
print_result "POST with form data" "200\|201\|400\|404\|405" "$result"

# POST with JSON body
result=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -H "Content-Type: application/json" \
    -d '{"key":"value"}' "$HOST/")
print_result "POST with JSON body" "200\|201\|400\|404\|405" "$result"

# POST with no body
result=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -H "Content-Length: 0" "$HOST/")
print_result "POST with empty body" "200\|201\|400\|404\|405" "$result"

# POST with large body
BIG_BODY=$(python3 -c "print('A' * 10000)")
result=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -H "Content-Type: text/plain" \
    -d "$BIG_BODY" "$HOST/")
print_result "POST with large body (10000 bytes)" "200\|201\|400\|404\|405\|413" "$result"

# --- DELETE ---

echo -e "${CYAN}--- DELETE Requests ---${NC}"

# DELETE request
result=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE "$HOST/some_resource")
print_result "DELETE /some_resource" "200\|204\|404\|405" "$result"

# --- Headers ---

echo -e "${CYAN}--- Header Edge Cases ---${NC}"

# Missing Host header (HTTP/1.0 style, no Host)
result=$(curl -s -o /dev/null -w "%{http_code}" --http1.0 "$HOST/")
print_result "Request with HTTP/1.0 (no Host header)" "200\|400\|404\|501" "$result"

# Sending Connection: close
result=$(curl -s -o /dev/null -w "%{http_code}" -H "Connection: close" "$HOST/")
print_result "GET with Connection: close" "200\|404" "$result"

# Sending Connection: keep-alive
result=$(curl -s -o /dev/null -w "%{http_code}" -H "Connection: keep-alive" "$HOST/")
print_result "GET with Connection: keep-alive" "200\|404" "$result"

# Very long header value
LONG_VAL=$(python3 -c "print('A' * 8000)")
result=$(curl -s -o /dev/null -w "%{http_code}" -H "X-Long-Header: $LONG_VAL" "$HOST/")
print_result "GET with very long header value" "200\|400\|404\|431" "$result"

# --- Methods ---

echo -e "${CYAN}--- Uncommon Methods ---${NC}"

# HEAD request (should return headers only, no body)
result=$(curl -s -o /dev/null -w "%{http_code}" -I "$HOST/")
print_result "HEAD /" "200\|404\|405" "$result"

# PUT request
result=$(curl -s -o /dev/null -w "%{http_code}" -X PUT \
    -d "some data" "$HOST/upload")
print_result "PUT /upload" "200\|201\|404\|405" "$result"

# Unknown/custom method
result=$(curl -s -o /dev/null -w "%{http_code}" -X FOOBAR "$HOST/")
print_result "Unknown method FOOBAR" "400\|405\|501" "$result"

# --- Chunked & Transfer Encoding ---

echo -e "${CYAN}--- Chunked Transfer ---${NC}"

# POST with chunked transfer encoding
result=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
    -H "Transfer-Encoding: chunked" \
    --data-binary "hello chunked world" "$HOST/")
print_result "POST with chunked transfer encoding" "200\|201\|400\|404\|405\|411" "$result"

# --- Summary ---

echo "==============================="
echo -e "Results: ${GREEN}$PASS passed${NC}, ${RED}$FAIL failed${NC}"
echo "==============================="