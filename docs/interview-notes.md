# Common Interview Questions

## Why is private key never shared?

Private key is used only for cryptographic signing and proof of identity.

It must remain secret forever.

Only certificates are exchanged during TLS handshake.

If private key is leaked, security is completely broken.

---

## Difference between encryption and signing?

### Encryption

Encrypt with Public Key
Decrypt with Private Key

Used for:

confidentiality

---

### Signing

Sign with Private Key
Verify with Public Key

Used for:

identity + integrity

---

## What is certificate chain?

Certificate chain is the trust path from:

Leaf Certificate
→ Intermediate CA
→ Root CA

Trust is established through this chain.

---

## Why trust Root CA instead of server certificate?

We do not trust the server certificate directly.

We trust the Root CA because it signs the certificate.

Trust is anchored at Root CA.
