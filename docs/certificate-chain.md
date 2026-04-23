┌────────────────────┐
│   Root CA (ca.crt) │
│   Self Signed      │
└─────────┬──────────┘
          ↓ signs
┌────────────────────┐
│ Client Cert        │
│ (client.crt)       │
└─────────┬──────────┘
          ↓ owned by
┌────────────────────┐
│ Client Private Key │
│ (client.key)       │
└────────────────────┘