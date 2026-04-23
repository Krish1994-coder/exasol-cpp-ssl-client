# Exasol C++ SSL/TLS Client Assessment

A production-grade secure C++ client implementation for the Exasol technical assessment using OpenSSL, Mutual TLS (mTLS), certificate validation, multi-port fallback logic, and Proof-of-Work challenge solving.

This project demonstrates strong understanding of:

* TLS / SSL internals
* Mutual TLS Authentication (mTLS)
* Certificate chain validation
* OpenSSL secure client implementation
* Linux socket programming
* Proof-of-Work (POW) solving
* Secure SHA1 challenge-response protocols
* Production-grade resource cleanup
* Hostname verification and MITM prevention

---

## Features

### Secure TLS Connection using OpenSSL

Implements full SSL/TLS client communication using:

* `SSL_connect()`
* `SSL_CTX_new()`
* `SSL_new()`
* `SSL_read()`
* `SSL_write()`

---

### Mutual TLS Authentication (mTLS)

Both client and server verify each other using:

* `client.crt`
* `client.key`
* `ca.crt`

This ensures strong two-way authentication.

---

### Server Certificate Validation

Strict verification using:

* Root CA trust validation
* Certificate chain verification
* Hostname verification using `SSL_set1_host()`

This prevents:

* impersonation attacks
* fake servers
* MITM attacks

---

### Multi-Port Fallback Logic

The client automatically attempts connection across multiple ports:

text id="yyf91x"
3336
8083
8446
49155
3481
65532


This improves:

* reliability
* fault tolerance
* production readiness

---

### Proof-of-Work (POW) Solver

Implements computational challenge solving:

text id="mcbwwh"
SHA1(authdata + suffix)


until required leading zero difficulty is satisfied.

This prevents:

* abuse
* spam
* automated attacks

---

### Secure Challenge-Response Protocol

Each server challenge is answered using:

text id="mtr0c7"
SHA1(authdata + challenge)


This ensures:

* integrity
* authenticity
* replay protection

---

### Safe Cleanup + Exception Handling

Custom cleanup mechanism ensures:

* SSL shutdown
* socket cleanup
* OpenSSL memory deallocation

This prevents:

* memory leaks
* dangling sockets
* unstable TLS sessions

---

## Technologies Used

* C++
* OpenSSL
* Linux Socket Programming
* SSL / TLS
* Mutual TLS (mTLS)
* X.509 Certificates
* Certificate Chain Validation
* SHA1 Cryptographic Hashing
* Secure Client Authentication

---

## Project Structure

text id="l3jnd1"
exasol-assessment/
│
├── main.cpp
├── README.md
│
├── docs/
│   ├── architecture.md
│   ├── tls-explanation.md
│   ├── certificate-chain.md
│   ├── interview-notes.md
│   └── troubleshooting.md
│
└── sample_output.txt


---

## Security Note

Private certificates and keys are intentionally excluded from this repository:

* `client.key`
* `client.crt`
* `ca.crt`

These files must never be committed to GitHub.

They are sensitive security assets and should remain private.

---

## Key Security Learnings

### Private keys must never be shared

Private keys are used for:

* identity proof
* digital signing

They must always remain secret.

---

### Trust is placed on Root CA

We trust:

text id="iqj4ng"
Root CA → Intermediate CA → Server Certificate


not directly on the server certificate.

This is the foundation of certificate trust.

---

### Hostname verification is mandatory

Even valid certificates must match:

text id="t62vba"
Certificate CN / SAN


Otherwise attackers can misuse valid certificates.

---

### Signing ≠ Encryption

Encryption provides:

confidentiality

Signing provides:

identity + integrity

This is a critical interview topic.

---

## Sample Success Output

text id="fj2tcl"
Trying port: 3336
Connected on port: 3336

<< HELO
>> TOAKUEI

<< POW authdata 5
Solving POW difficulty: 5

<< NAME challenge
>> sha1hash Sai Krishna Varanasi

...

====================================
APPLICATION SUBMITTED SUCCESSFULLY
====================================


---

## Interview Focus Areas

This project prepares for deep technical discussions on:

* TLS Handshake internals
* Certificate validation flow
* Private/Public key usage
* Root CA trust model
* Mutual TLS
* Proof-of-Work systems
* OpenSSL internals
* Linux socket lifecycle
* Secure production-grade C++ systems

---

## Author

### Sai Krishna Varanasi

C++ | Security | Backend | Distributed Systems | TLS Engineering

---

