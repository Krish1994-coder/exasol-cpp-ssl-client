# Troubleshooting Guide

## openssl/ssl.h not found

### Cause

OpenSSL development libraries are missing.

### Fix

Ubuntu:

sudo apt install libssl-dev

---

## TLS connect failed

### Check

* ca.crt
* client.crt
* client.key
* hostname verification
* server certificate trust

---

## Private key mismatch

### Cause

client.key does not match client.crt

### Fix

Ensure both were generated as a valid pair.

Use:

SSL_CTX_check_private_key()

to verify matching.

---

## Certificate verification failed

### Cause

Wrong CA certificate or hostname mismatch

### Fix

Use correct:

* ca.crt
* SSL_set1_host()

for proper verification
