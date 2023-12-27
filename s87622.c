/*
 * compile and run with:
 *	make
 *	make run
 */
#include <stdio.h>
#include <openssl/evp.h>
#include <stdlib.h>


int main()
{
	/*
	 * If the following line causes the error message
	 * 	undefined reference to 'EVP_idea_ecb',
	 * please check the SSLDIR that is set in the Makefile.
	 */
	// EVP_idea_ecb();
	
	// Dateien für Schlüssel, IV und Chiffrat öffnen
    FILE *keyFile = fopen("s87622-key1.bin", "rb");
    FILE *cipherFile = fopen("s87622-cipher.bin", "rb");

	printf("hello, world\n");
	return 0;
}
