#pragma once
#ifndef FIN_ETI_PASSWORD_ENCRYPTION_H
#define FIN_ETI_PASSWORD_ENCRYPTION_H

/*
* Sample Code taken from MCX ETI Document File.
* As it is for Linux environment so, I have modified it to use with windows.
* It is using OPEN SSL(BIO) to encryp and decrypt the string
*/


//#ifdef __cplusplus
//extern "C" {
//#endif
//#include <openssl/applink.c>
//#ifdef __cplusplus
//};
//#endif

#include <openssl/pem.h> 
#include <openssl/ssl.h> 
#include <openssl/rsa.h> 
#include <openssl/evp.h> 
#include <openssl/bio.h> 
#include <openssl/err.h> 
#include <math.h>
//#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>

namespace FIN {
	namespace ETI {

		class Password_Encryption
		{
		public:
			Password_Encryption(){}
			std::string Base64Encode(const char* message, int len);
			int public_encrypt(unsigned char* data, int data_len, unsigned char* encrypted, std::string fileName);
			RSA* createRSAWithFilename(const char* filename, int spublic);
			std::string encryptPassword(std::string password,std::string);
			void printLastError(char* msg);
		private:
			int padding = RSA_PKCS1_PADDING;
		};

	}
}
#endif