// OpenSSL 3 compatibility symbols for old static libmysqlclient builds.
#include <openssl/ssl.h>
#include <openssl/evp.h>

extern "C" {

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#ifdef SSL_get_peer_certificate
#undef SSL_get_peer_certificate
#endif
X509 *SSL_get_peer_certificate(const SSL *ssl)
{
	return SSL_get1_peer_certificate(ssl);
}

#ifdef EVP_CIPHER_iv_length
#undef EVP_CIPHER_iv_length
#endif
int EVP_CIPHER_iv_length(const EVP_CIPHER *cipher)
{
	return EVP_CIPHER_get_iv_length(cipher);
}

#ifdef EVP_CIPHER_block_size
#undef EVP_CIPHER_block_size
#endif
int EVP_CIPHER_block_size(const EVP_CIPHER *cipher)
{
	return EVP_CIPHER_get_block_size(cipher);
}
#endif

}
