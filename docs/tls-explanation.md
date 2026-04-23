# TLS / SSL Explanation for Exasol Assessment

## Overview

This project uses OpenSSL in C++ to establish a secure TLS connection with the Exasol assessment server using Mutual TLS (mTLS).

The client authenticates the server, and the server authenticates the client using certificates.

---

# Certificate Files Used

## client.key

Private key of the client.

Used for:

* proving ownership of client certificate
* digital signatures during TLS handshake

This file must never be shared publicly.

---

## client.crt

Client certificate.

Used by:

* server to verify client identity

This is the leaf certificate.

---

## ca.crt

Root CA certificate.

Used by:

* client to verify server certificate
* server to verify client certificate

This is a self-signed Root CA certificate.

---

# TLS Verification Flow

## Client verifies Server

1. Server sends server certificate
2. Client checks whether it is signed by trusted CA (`ca.crt`)
3. Server proves ownership of private key by signing handshake data
4. Client verifies signature using public key from server certificate

---

## Server verifies Client

1. Client sends client certificate (`client.crt`)
2. Server checks whether it is signed by trusted CA (`ca.crt`)
3. Client proves ownership of `client.key`
4. Server verifies the proof

This creates Mutual TLS (mTLS).

---

# Why Private Key is Never Shared

Private key is never transmitted.

It is only used internally for signing.

Only certificates are exchanged.

This is one of the most important TLS security principles.

---

# Certificate Chain

In this assessment:

client.crt → signed by → ca.crt

There is no Intermediate CA.

The provided `ca.crt` acts as a self-signed Root CA.

---

# Security Best Practices

* never upload `client.key`
* never expose certificates publicly
* always verify hostname
* use proper cleanup for SSL resources
* validate certificate chain before trusting server

---

## Key Learnings

During this project, I gained strong practical understanding of:

* OpenSSL-based TLS client implementation in C++
* Mutual TLS (mTLS) authentication
* Client certificate vs Server certificate handling
* Root CA vs Intermediate CA concepts
* Self-signed certificates and trust chains
* Certificate verification using `SSL_CTX_load_verify_locations()`
* Hostname verification using `SSL_set1_host()`
* Secure Proof-of-Work (POW) solving logic
* Multi-port fallback connection strategies
* Secure cleanup of OpenSSL resources
* Importance of private key protection
* Real-world certificate chain validation

One major takeaway:

**We do not trust the server certificate directly — we trust the Root CA.**

