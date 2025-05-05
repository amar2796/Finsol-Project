#include "Password_Encryption.h"
namespace FIN {
	namespace ETI {
		std::string Password_Encryption::Base64Encode(const char* message, int len) { //Encodes a string to base64
			BIO* bio, * b64;
			BUF_MEM* bptr;

			b64 = BIO_new(BIO_f_base64());
			bio = BIO_new(BIO_s_mem());
			b64 = BIO_push(b64, bio);
			BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

			BIO_write(b64, message, len);
			BIO_flush(b64);

			// Convert the encoded data to a string
			BIO_get_mem_ptr(b64, &bptr);
			std::string data_str(bptr->data, bptr->length);
			BIO_free_all(b64);
			return data_str;
		}
		RSA* Password_Encryption::createRSAWithFilename(const char* filename, int spublic)
		{
			FILE* fp = fopen(filename, "rb");
			if (fp == NULL)
			{
				printf("Unable to open file %s \n", filename);
				return 0;
			}
			RSA* rsa = RSA_new();
			if (spublic)
			{
				rsa = PEM_read_RSA_PUBKEY(fp, &rsa, NULL, NULL);
			}
			else
			{
				rsa = PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL);
			}
			return rsa;
		}
		//Function encrypts the value returns length of encrypted text
		int Password_Encryption::public_encrypt(unsigned char* data, int data_len, unsigned char* encrypted, std::string fileName)
		{
			RSA* rsa = createRSAWithFilename(fileName.c_str(), 1);//Path of public key	
			int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
			return result;
		}
		std::string Password_Encryption::encryptPassword(std::string password,std::string publicKeyPath)
		{
			char encrypted[4098] = {};			
			int encrypted_length = public_encrypt((unsigned char*)password.c_str(), password.length(), (unsigned char*)encrypted, publicKeyPath);
			if (encrypted_length == -1) {
				printLastError((char*)("Public Encrypt failed "));
				exit(0);
			}
			//printf("Encrypted length =%d\n", encrypted_length);

			//Copy char array to pointer	
			char* enc = (char*)encrypted;

			//Base64 Encoding 			
			std::string encodedString = Base64Encode(enc, encrypted_length);
			return encodedString;
		}
		void Password_Encryption::printLastError(char* msg)
		{
			char* err = {};
			ERR_load_crypto_strings();
			ERR_error_string(ERR_get_error(), err);
			printf("%s ERROR: %s\n", msg, err);
			free(err);
		}
	}
}