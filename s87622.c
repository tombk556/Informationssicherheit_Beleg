#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

int main() {
    // Der zu verschlüsselnde Text
    unsigned char plaintext[] = "Kîndêr wêînên.\nNârrên wârtên.\nDûmmê wîssên.\nKlêînê mêînên.\nWêîsê gêhên în dên Gârtên.";

    // Buffer für den Schlüssel und den IV
    unsigned char key[32]; // AES-128-XTS benötigt 2x128 Bit Schlüssel
    unsigned char iv[16];  // XTS benötigt 128 Bit IV

    // Lese den Schlüssel und den IV aus der Datei
    FILE *keyfile = fopen("./bin/s87622-key2.bin", "rb");
    if(!keyfile) {
        perror("Fehler beim Öffnen der Schlüsseldatei");
        return 1;
    }
    if(fread(key, 1, 32, keyfile) != 32 || fread(iv, 1, 16, keyfile) != 16) {
        perror("Fehler beim Lesen des Schlüssels/IV");
        return 1;
    }
    fclose(keyfile);

    // Chiffrat-Buffer
    unsigned char ciphertext[1024];

    // Kontext für die Verschlüsselung initialisieren
    EVP_CIPHER_CTX *ctx;
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Verschlüsselung initialisieren
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_xts(), NULL, key, iv)) handleErrors();

    int len;
    int ciphertext_len;

    // Verschlüsselung durchführen
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, strlen((char *)plaintext))) handleErrors();
    ciphertext_len = len;

    // Verschlüsselung abschließen
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    // Kontext freigeben
    EVP_CIPHER_CTX_free(ctx);

    // Verschlüsselten Text in Datei schreiben
    FILE *outfile = fopen("./bin/s87622-result2.bin", "wb");
    if(!outfile) {
        perror("Fehler beim Öffnen der Ausgabedatei");
        return 1;
    }
    fwrite(ciphertext, 1, ciphertext_len, outfile);
    fclose(outfile);

    printf("Verschlüsselung erfolgreich!\n");

    return 0;
}
