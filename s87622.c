#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

FILE *bin_datei(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Fehler beim Öffnen der Datei: %s", path);
        perror(err_msg);
    }
    return file;
}

void winterliche_muetzen(char c, FILE *output) {
    switch (c) {
        case 'a': fprintf(output, "â"); break;
        case 'e': fprintf(output, "ê"); break;
        case 'i': fprintf(output, "î"); break;
        case 'o': fprintf(output, "ô"); break;
        case 'u': fprintf(output, "û"); break;
        default:  fputc(c, output); break;
    }
}

int main(void) {
    // Variablen definieren und instanziieren
    FILE *s87622_bin, *s87622_key1_bin;
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    const EVP_CIPHER *cipher_verfahren;
    EVP_CIPHER_CTX *ctx;
    int key_length, iv_length;
    unsigned char input[1024], output_raw[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;

    // Lade das Chiffrat und den Schlüssel/IV
    s87622_bin = bin_datei("./bin/s87622-cipher.bin");
    s87622_key1_bin = bin_datei("./bin/s87622-key1.bin");

    cipher_verfahren = EVP_aes_256_xts();
    key_length = EVP_CIPHER_key_length(cipher_verfahren);
    iv_length = EVP_CIPHER_iv_length(cipher_verfahren);

    // Lese Schlüssel und IV
    fread(key, 1, key_length, s87622_key1_bin);
    fread(iv, 1, iv_length, s87622_key1_bin);
    fclose(s87622_key1_bin);

    // Initialisiere Entschlüsselung
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, cipher_verfahren, NULL, key, iv);

    // Entschlüsselungsprozess
    while ((inlen = fread(input, 1, 1024, s87622_bin)) > 0) {
        if (!EVP_DecryptUpdate(ctx, output_raw, &outlen, input, inlen)) {
            // Fehlerbehandlung
            fprintf(stderr, "Fehler bei der Entschlüsselung.\n");
            fclose(s87622_bin);
            EVP_CIPHER_CTX_free(ctx);
            return 1;
        }
    }

    // Ausgabe der Nachricht
    printf("Verschlüsselte Nachricht aus s87622-cipher.bin ORIGINAL: \n");
    printf("\n");
    printf("%s", output_raw); 

    printf("Verschlüsselte Nachricht aus s87622-cipher.bin mit 'winterlichen Mützen': \n");
    printf("\n");
    for (int i = 0; i < outlen; i++) {
        winterliche_muetzen(output_raw[i], stdout); 
    }



    // Aufräumarbeiten
    fclose(s87622_bin);
    EVP_CIPHER_CTX_free(ctx);

    return 0;
}
