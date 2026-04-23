#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <arpa/inet.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

static const std::string HOST = "18.202.148.130";

static const std::vector<int> PORTS = { 3336, 8083, 8446, 49155, 3481, 65532 };

// ===== Replace these placeholders with your details =====
static const std::string FULL_NAME = "Sai Krishna Varanasi";

static const std::vector<std::string> EMAILS
    = { "vsaikrishna.1512@email.com", "vsaikrishna926@email.com" };

static const std::string SKYPE = "N/A";
static const std::string BIRTHDATE = "25.03.1994"; // dd.mm.yyyy
static const std::string COUNTRY = "India";

static const std::vector<std::string> ADDRESS_LINES
    = { "Dasari Srinivasa Nagar", "Rajahmundry, Andhra Pradesh 533105" };
// =============================================================

std::string
sha1Hex (const std::string &input)
{
  unsigned char hash[SHA_DIGEST_LENGTH];

  SHA1 (reinterpret_cast<const unsigned char *> (input.c_str ()), input.size (),
        hash);

  std::ostringstream oss;

  for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
    {
      oss << std::hex << std::setw (2) << std::setfill ('0') << (int)hash[i];
    }

  return oss.str ();
}

std::string
counterSuffix (uint64_t counter)
{
  static const std::string chars = "abcdefghijklmnopqrstuvwxyz"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "0123456789";

  std::string out;

  do
    {
      out += chars[counter % chars.size ()];
      counter /= chars.size ();
    }
  while (counter > 0);

  return out;
}

bool
startsWithZeros (const std::string &hash, int difficulty)
{
  for (int i = 0; i < difficulty; ++i)
    {
      if (i >= (int)hash.size () || hash[i] != '0')
        {
          return false;
        }
    }

  return true;
}

std::string
solvePOW (const std::string &authdata, int difficulty)
{
  uint64_t counter = 0;

  while (true)
    {
      std::string suffix = counterSuffix (counter++);
      std::string hash = sha1Hex (authdata + suffix);

      if (startsWithZeros (hash, difficulty))
        {
          return suffix;
        }

      if (counter % 1000000 == 0)
        {
          std::cout << "Tried: " << counter << std::endl;
        }
    }
}

void
writeLine (SSL *ssl, const std::string &line)
{
  std::string out = line + "\n";

  int ret = SSL_write (ssl, out.c_str (), out.size ());

  if (ret <= 0)
    {
      std::cerr << "SSL_write failed\n";
      ERR_print_errors_fp (stderr);
      throw std::runtime_error ("SSL_write failed");
    }

  std::cout << ">> " << line << std::endl;
}

std::string
readLine (SSL *ssl)
{
  std::string line;
  char ch;

  while (true)
    {
      int bytes = SSL_read (ssl, &ch, 1);

      if (bytes <= 0)
        {
          int err = SSL_get_error (ssl, bytes);

          if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
            {
              continue;
            }

          std::cerr << "SSL_read timeout / disconnect / failure\n";
          ERR_print_errors_fp (stderr);
          break;
        }

      if (ch == '\n')
        {
          break;
        }

      if (ch != '\r')
        {
          line += ch;
        }
    }

  std::cout << "<< " << line << std::endl;
  return line;
}

std::vector<std::string>
split (const std::string &s)
{
  std::stringstream ss (s);
  std::vector<std::string> parts;
  std::string item;

  while (ss >> item)
    {
      parts.push_back (item);
    }

  return parts;
}

void
setSocketTimeout (int sock, int seconds)
{
  timeval timeout{};
  timeout.tv_sec = seconds;
  timeout.tv_usec = 0;

  if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout))
      < 0)
    {
      perror ("setsockopt SO_RCVTIMEO failed");
    }

  if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (timeout))
      < 0)
    {
      perror ("setsockopt SO_SNDTIMEO failed");
    }
}

int
main ()
{
  SSL_library_init ();
  SSL_load_error_strings ();
  OpenSSL_add_all_algorithms ();

  SSL_CTX *ctx = nullptr;
  SSL *ssl = nullptr;
  int sock = -1;
  SSL_CTX *ctx = SSL_CTX_new (TLS_client_method ());

  if (!ctx)
    {
      std::cerr << "Failed to create SSL context\n";
      return 1;
    }

  // =========================================================
  // USE ca.crt (recommended)
  // =========================================================

  if (SSL_CTX_load_verify_locations (ctx, "ca.crt", nullptr) != 1)
    {

      std::cerr << "Failed loading CA certificate (ca.crt)\n";
      ERR_print_errors_fp (stderr);
      cleanup (nullptr, -1, ctx);
      return 1;
    }

  // verify server certificate using ca.crt
  SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, nullptr);

  // =========================================================

  // Load client certificate + private key

  if (SSL_CTX_use_certificate_file (ctx, "client.crt", SSL_FILETYPE_PEM) <= 0
      || SSL_CTX_use_PrivateKey_file (ctx, "client.key", SSL_FILETYPE_PEM) <= 0)
    {

      std::cerr << "Failed loading client certificate/private key\n";
      ERR_print_errors_fp (stderr);
      cleanup (nullptr, -1, ctx);
      return 1;
    }

  if (!SSL_CTX_check_private_key (ctx))
    {
      std::cerr << "Private key does not match certificate\n";
      ERR_print_errors_fp (stderr);
      cleanup (nullptr, -1, ctx);
      return 1;
    }

  // =========================================================
  // Multi-port fallback connection logic
  // =========================================================

  bool connected = false;

  for (int port : PORTS)
    {
      sock = socket (AF_INET, SOCK_STREAM, 0);

      if (sock < 0)
        {
          continue;
        }

      sockaddr_in server{};
      server.sin_family = AF_INET;
      server.sin_port = htons (port);

      if (inet_pton (AF_INET, HOST.c_str (), &server.sin_addr) <= 0)
        {

          std::cerr << "Invalid host address\n";
          cleanup (nullptr, sock, ctx);
          return 1;
        }

      std::cout << "Trying port: " << port << std::endl;

      if (connect (sock, (sockaddr *)&server, sizeof (server)) == 0)
        {

          std::cout << "Connected on port: " << port << std::endl;

          connected = true;
          break;
        }

      close (sock);
      sock = -1;
    }

  if (!connected)
    {
      std::cerr << "Failed to connect to any server port\n";
      cleanup (nullptr, -1, ctx);
      return 1;
    }

  setSocketTimeout (sock, 6);

  // =========================================================
  // TLS handshake
  // =========================================================

  SSL *ssl = SSL_new (ctx);

  if (!ssl)
    {
      std::cerr << "SSL_new failed\n";
      cleanup (nullptr, sock, ctx);
      return 1;
    }

  // IMPORTANT:
  SSL_set_tlsext_host_name (ssl, HOST.c_str ());

  SSL_set1_host (ssl, VERIFY_HOSTNAME.c_str ());
  SSL_set_fd (ssl, sock);

  if (SSL_connect (ssl) <= 0)
    {
      std::cerr << "TLS connect failed\n";
      ERR_print_errors_fp (stderr);
      return 1;
    }

  std::string authdata;

  try
    {
      while (true)
        {
          std::string line = readLine (ssl);

          if (line.empty ())
            {
              break;
            }

          auto args = split (line);

          if (args.empty ())
            {
              continue;
            }

          const std::string &cmd = args[0];

          if (cmd == "HELO")
            {
              writeLine (ssl, "TOAKUEI");
            }

          else if (cmd == "POW")
            {
              if (args.size () < 3)
                {
                  std::cerr << "Invalid POW command\n";
                  break;
                }

              authdata = args[1];
              int difficulty = std::stoi (args[2]);

              std::cout << "Solving POW difficulty: " << difficulty
                        << std::endl;

              // POW timeout = 2 hours
              setSocketTimeout (sock, 7200);

              std::string suffix = solvePOW (authdata, difficulty);

              writeLine (ssl, suffix);

              // Restore normal timeout
              setSocketTimeout (sock, 6);
            }

          else if (cmd == "NAME")
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid NAME command\n";
                  break;
                }

              writeLine (ssl, sha1Hex (authdata + args[1]) + " " + FULL_NAME);
            }

          else if (cmd == "MAILNUM")
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid MAILNUM command\n";
                  break;
                }

              writeLine (ssl, sha1Hex (authdata + args[1]) + " "
                                  + std::to_string (EMAILS.size ()));
            }

          else if (cmd.rfind ("MAIL", 0) == 0)
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid MAIL command\n";
                  break;
                }

              int idx = std::stoi (cmd.substr (4)) - 1;

              if (idx >= 0 && idx < (int)EMAILS.size ())
                {

                  writeLine (ssl,
                             sha1Hex (authdata + args[1]) + " " + EMAILS[idx]);
                }
              else
                {
                  std::cerr << "Invalid MAIL index received\n";
                  break;
                }
            }

          else if (cmd == "SKYPE")
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid SKYPE command\n";
                  break;
                }

              writeLine (ssl, sha1Hex (authdata + args[1]) + " " + SKYPE);
            }

          else if (cmd == "BIRTHDATE")
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid BIRTHDATE command\n";
                  break;
                }

              writeLine (ssl, sha1Hex (authdata + args[1]) + " " + BIRTHDATE);
            }

          else if (cmd == "COUNTRY")
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid COUNTRY command\n";
                  break;
                }

              writeLine (ssl, sha1Hex (authdata + args[1]) + " " + COUNTRY);
            }

          else if (cmd == "ADDRNUM")
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid ADDRNUM command\n";
                  break;
                }

              writeLine (ssl, sha1Hex (authdata + args[1]) + " "
                                  + std::to_string (ADDRESS_LINES.size ()));
            }

          else if (cmd.rfind ("ADDRLINE", 0) == 0)
            {
              if (args.size () < 2)
                {
                  std::cerr << "Invalid ADDRLINE command\n";
                  break;
                }

              int idx = std::stoi (cmd.substr (8)) - 1;

              if (idx >= 0 && idx < (int)ADDRESS_LINES.size ())
                {

                  writeLine (ssl, sha1Hex (authdata + args[1]) + " "
                                      + ADDRESS_LINES[idx]);
                }
              else
                {
                  std::cerr << "Invalid ADDRLINE index received\n";
                  break;
                }
            }

          else if (cmd == "END")
            {
              writeLine (ssl, "OK");

              std::cout << "\n====================================\n"
                        << "APPLICATION SUBMITTED SUCCESSFULLY\n"
                        << "====================================\n";

              break;
            }

          else if (cmd == "ERROR")
            {
              std::cerr << "Server ERROR: ";

              for (size_t i = 1; i < args.size (); ++i)
                {
                  std::cerr << args[i] << " ";
                }

              std::cerr << std::endl;
              break;
            }
        }
    }
  catch (const std::exception &e)
    {
      std::cerr << "Exception: " << e.what () << std::endl;
    }

  cleanup (ssl, sock, ctx);

  return 0;
}