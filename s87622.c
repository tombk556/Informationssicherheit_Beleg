#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

int main(void) {
    FILE *bin_file_cipher, *bin_file_key, *bin_file_decrypted;
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    const EVP_CIPHER *cipher_type;
    EVP_CIPHER_CTX *ctx;
    int key_length, iv_length;
    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;

    // Lade das Chiffrat aus ./bin/s87622-cipher.bin
    bin_file_cipher = fopen("./bin/s87622-cipher.bin", "rb");
    if (!bin_file_cipher) {
        perror("Fehler beim Öffnen der Chiffrat-Datei");
        return 1;
    }

    // Lade den Schlüssel und Intitaliserungs Vektor (IV) aus ./bin/s87622-key1.bin
    bin_file_key = fopen("./bin/s87622-key1.bin", "rb");
    if (!bin_file_key) {
        perror("Fehler beim Öffnen der Schlüsseldatei");
        fclose(bin_file_cipher);
        return 1;
    }

    cipher_type = EVP_aes_256_xts();
    key_length = EVP_CIPHER_key_length(cipher_type);
    iv_length = EVP_CIPHER_iv_length(cipher_type);

    // Lese Schlüssel und IV aus der Datei
    fread(key, 1, key_length, bin_file_key);
    fread(iv, 1, iv_length, bin_file_key);
    fclose(bin_file_key);

    // Initialisiere die Entschlüsselung
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, cipher_type, NULL, key, iv);

    // Öffne die Ausgabedatei
    bin_file_decrypted = fopen("decrypted.bin", "wb");
    if (!bin_file_decrypted) {
        perror("Fehler beim Öffnen der Ausgabedatei");
        fclose(bin_file_cipher);
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    // Entschlüsselungsprozess
    while ((inlen = fread(inbuf, 1, 1024, bin_file_cipher)) > 0) {
        if (!EVP_DecryptUpdate(ctx, outbuf, &outlen, inbuf, inlen)) {
            // Fehlerbehandlung
            fprintf(stderr, "Fehler bei der Entschlüsselung.\n");
            fclose(bin_file_cipher);
            fclose(bin_file_decrypted);
            EVP_CIPHER_CTX_free(ctx);
            return 1;
        }
        fwrite(outbuf, 1, outlen, bin_file_decrypted);
    }

    if (!EVP_DecryptFinal_ex(ctx, outbuf, &outlen)) {
        // Fehlerbehandlung für den finalen Block
        fprintf(stderr, "Fehler bei der finalen Entschlüsselung.\n");
        fclose(bin_file_cipher);
        fclose(bin_file_decrypted);
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    fwrite(outbuf, 1, outlen, bin_file_decrypted);

    // Aufräumarbeiten
    fclose(bin_file_cipher);
    fclose(bin_file_decrypted);
    EVP_CIPHER_CTX_free(ctx);

    printf("Entschlüsselung erfolgreich abgeschlossen.\n");
    return 0;
}
