# Architecture Explanation

---

# High-Level System Flow


Client Application
        ↓
TCP Socket Connection
        ↓
Port Fallback Logic
        ↓
TLS Handshake (OpenSSL)
        ↓
Server Certificate Verification
        ↓
Hostname Verification
        ↓
Mutual TLS Authentication
        ↓
Proof-of-Work (POW) Solver
        ↓
Command Processing Engine
        ↓
SHA1 Challenge Response
        ↓
Final Submission


---

# Detailed Component Explanation

---

## 1. Client Application

This is the main C++ program (`main.cpp`) responsible for:

* establishing secure communication
* solving Proof-of-Work challenge
* exchanging protocol commands
* handling certificate validation
* ensuring proper cleanup

This acts as the secure TLS client.

---

## 2. TCP Socket Connection

Before TLS starts, a normal TCP connection is created using:


socket()
connect()


TLS always runs on top of TCP.

TCP provides:

* reliable delivery
* ordered packets
* retransmission support

Without TCP, TLS cannot function.

---

## 3. Port Fallback Logic

The server exposes multiple ports:


3336
8083
8446
49155
3481
65532


Instead of depending on only one port, the client tries all ports sequentially.

### Why?

This improves:

* reliability
* fault tolerance
* production readiness

This is a senior-level engineering decision.

---

## 4. TLS Handshake (OpenSSL)

After TCP connection, OpenSSL performs:


SSL_connect()


This starts:

* cipher negotiation
* certificate exchange
* session key generation
* secure encrypted channel creation

After handshake:

communication becomes encrypted.

---

## 5. Server Certificate Verification

The client verifies server identity using:


SSL_CTX_load_verify_locations()
SSL_CTX_set_verify()


The provided:

text
ca.crt


is loaded as trusted Root CA.

This ensures:

the client only trusts certificates signed by Exasol’s CA.

This prevents fake servers.

---

## 6. Hostname Verification

Even if certificate is valid, hostname must also match.

Implemented using:


SSL_set1_host()


This prevents:

Man-In-The-Middle (MITM) attacks

where attackers use valid but wrong certificates.

This is extremely important.

---

## 7. Mutual TLS Authentication (mTLS)

Unlike normal HTTPS:

both sides verify each other.

Client sends:

text
client.crt
client.key


Server verifies:

client identity

This creates strong two-way trust.

This is called:

Mutual TLS (mTLS)

---

## 8. Proof-of-Work (POW) Solver

Server sends:

text
POW <authdata> <difficulty>


Client must find a suffix such that:

text
SHA1(authdata + suffix)


starts with required leading zeroes.

This prevents:

* spam
* bots
* abuse

This is computational verification.

---

## 9. Command Processing Engine

Server sends commands like:

* NAME
* MAILNUM
* MAIL1
* COUNTRY
* BIRTHDATE
* ADDRLINE1
* END

Client parses and responds correctly.

This forms the protocol engine.

---

## 10. SHA1 Challenge Response

Every response must include:

text
SHA1(authdata + challenge)


This proves:

* request authenticity
* response integrity

This prevents replay attacks.

---

## 11. Final Submission

When server sends:

text
END


Client responds:

text
OK


This marks successful submission.

Application completes securely.

---

# Design Decisions

---

## Why use explicit `ca.crt`?

Using system trust store is risky.

Exasol provided dedicated Root CA.

Explicit trust improves:

* strict validation
* controlled security
* predictable trust model

---

## Why custom `cleanup()`?

OpenSSL requires manual memory management.

Without cleanup:

* memory leaks
* dangling sockets
* SSL instability

Proper cleanup ensures production-grade code quality.

---

## Why hostname verification?

Certificate validation alone is not enough.

Hostname validation prevents:

certificate misuse attacks.

This is critical security practice.

---

## Why multi-port retry?

Real-world systems require fault tolerance.

Port retry improves:

availability + resilience

This reflects production engineering thinking.
