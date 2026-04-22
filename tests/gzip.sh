#!/bin/bash

HOST="${HOST:-http://localhost:8080}"
UPLOAD_PATH="${UPLOAD_PATH:-/uploads/gzip-test.txt}"
UPLOAD_FILE="${UPLOAD_FILE:-var/www/uploads/gzip-test.txt}"

PASS=0
FAIL=0

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

pass() {
	echo -e "${GREEN}  PASS${NC} $1"
	((PASS++))
}

fail() {
	echo -e "${RED}  FAIL${NC} $1"
	((FAIL++))
}

cleanup() {
	rm -f /tmp/webserv-gzip-body.out /tmp/webserv-gzip-upload.txt /tmp/webserv-gzip-upload.txt.gz
	rm -f "$UPLOAD_FILE"
}

trap cleanup EXIT

if ! command -v curl >/dev/null 2>&1; then
	echo "curl is required for this test"
	exit 1
fi

if ! command -v gzip >/dev/null 2>&1; then
	echo "gzip is required for this test"
	exit 1
fi

echo "==============================="
echo "   webserv gzip test suite"
echo "==============================="
echo ""

cleanup

echo -e "${CYAN}[TEST] GET / with Accept-Encoding: gzip${NC}"
headers=$(curl -s -D - -H "Accept-Encoding: gzip" "$HOST/" -o /tmp/webserv-gzip-body.out)
status=$(echo "$headers" | awk 'NR == 1 {print $2}')
encoding=$(echo "$headers" | tr -d '\r' | awk -F': ' 'tolower($1) == "content-encoding" {print tolower($2)}')
vary=$(echo "$headers" | tr -d '\r' | awk -F': ' 'tolower($1) == "vary" {print tolower($2)}')

if [ "$status" = "200" ]; then
	pass "status is 200"
else
	fail "expected status 200, got '$status'"
fi

if [ "$encoding" = "gzip" ]; then
	pass "response has Content-Encoding: gzip"
else
	fail "expected Content-Encoding: gzip, got '$encoding'"
fi

if echo "$vary" | grep -q "accept-encoding"; then
	pass "response has Vary: Accept-Encoding"
else
	fail "expected Vary to include Accept-Encoding, got '$vary'"
fi

if gzip -t /tmp/webserv-gzip-body.out 2>/dev/null; then
	pass "response body is valid gzip data"
else
	fail "response body is not valid gzip data"
fi

echo ""
echo -e "${CYAN}[TEST] POST $UPLOAD_PATH with Content-Encoding: gzip${NC}"
printf "decompressed gzip upload\nsecond line\n" > /tmp/webserv-gzip-upload.txt
gzip -c /tmp/webserv-gzip-upload.txt > /tmp/webserv-gzip-upload.txt.gz

post_status=$(curl -s -o /dev/null -w "%{http_code}" -X POST \
	-H "Content-Encoding: gzip" \
	-H "Content-Type: text/plain" \
	--data-binary @/tmp/webserv-gzip-upload.txt.gz \
	"$HOST$UPLOAD_PATH")

if [ "$post_status" = "201" ]; then
	pass "gzipped upload returns 201"
else
	fail "expected upload status 201, got '$post_status'"
fi

if [ -f "$UPLOAD_FILE" ] && cmp -s /tmp/webserv-gzip-upload.txt "$UPLOAD_FILE"; then
	pass "uploaded file contains decompressed body"
else
	fail "uploaded file does not match decompressed body"
fi

echo ""
echo "==============================="
echo -e "Results: ${GREEN}$PASS passed${NC}, ${RED}$FAIL failed${NC}"
echo "==============================="

if [ "$FAIL" -ne 0 ]; then
	exit 1
fi
