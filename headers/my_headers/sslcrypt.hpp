#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <iostream>
#include <exception>
#pragma once
//#pragma warning(disable : 4996) //migrate as soon as possible...

#ifndef SSLCRYPT
#define SSLCRYPT


inline SSL_CTX* create_SSL_context()
{
	const SSL_METHOD* method;
	SSL_CTX* ctx = NULL;
	method = TLS_server_method();
	ctx = SSL_CTX_new(method);
	if (!ctx)
	{
		perror("not able to create context");
		
	};
	return ctx;
}


inline void set_key_cert(SSL_CTX* ctx)
{
	int h = SSL_CTX_use_certificate_file(ctx, "./cert.pem", SSL_FILETYPE_PEM);
	if (h <= 0){std::cout << "error issuing cert..." << std::endl; }
	int i = SSL_CTX_use_PrivateKey_file(ctx, "./key.pem", SSL_FILETYPE_PEM);
	if (i <= 0) { std::cout << "error issuing key..." << std::endl; }

}

inline int invoke_server(SSL_CTX* ctx, int fd, const char* reply)
{
	//fd is obtained from the file descriptor of the accept function
	int n = 0;
	//fd is obtained from the file descriptor of the accept function
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, fd);
	n = SSL_do_handshake(ssl);
	std::cout << "handshake value: " << n << std::endl;
	if (SSL_accept(ssl) >= 0)
	{
		perror("ssl does not accept it...");
		
	}
	else
	{
		SSL_write(ssl, reply, strlen(reply));
	}
	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	return n;
}

inline int invoke_(int fd, const char* reply)
{
std::cout << "invoking..." << std::endl;
	auto f = create_SSL_context();
	set_key_cert(f);
	return invoke_server(f, fd, reply);
}
	
#endif//SSL