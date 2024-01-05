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

void speichere_mit_muetzen(const unsigned char *input, int input_len, char **output) {
    int max_output_len = input_len * 3 + 1; 
    *output = (char*)malloc(max_output_len);
    if (!*output) {
        perror("Speicherzuweisungsfehler");
        return;
    }

    (*output)[0] = '\0'; 
    // Vokale durch Dächer ersetzen
    for (int i = 0; i < input_len; i++) {
        switch (input[i]) {

            case 'a': strcat(*output, "â"); break;
            case 'e': strcat(*output, "ê"); break;
            case 'i': strcat(*output, "î"); break;
            case 'o': strcat(*output, "ô"); break;
            case 'u': strcat(*output, "û"); break;
            case 'A': strcat(*output, "Â"); break;
            case 'E': strcat(*output, "Ê"); break;
            case 'I': strcat(*output, "Î"); break;
            case 'O': strcat(*output, "Ô"); break;
            case 'U': strcat(*output, "Û"); break;
            default:
                {
                    size_t len = strlen(*output);
                    (*output)[len] = input[i];
                    (*output)[len + 1] = '\0'; 
                }
                break;
        }
    }
}

void verschluesseln_und_speichern(const unsigned char *input, int input_len, const char *key_path, const char *output_path) {
    FILE *key_file = fopen(key_path, "rb");
    FILE *output_datei = fopen(output_path, "wb");
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    const EVP_CIPHER *cipher_verfahren;
    EVP_CIPHER_CTX *ctx;
    unsigned char output_raw[1024 + EVP_MAX_BLOCK_LENGTH];
    int outlen;

    if (!key_file || !output_datei) {
        perror("Fehler beim Öffnen der Schlüssel- oder Ausgabedatei");
        return;
    }

    // Lese Schlüssel und IV
    fread(key, 1, EVP_CIPHER_key_length(cipher_verfahren), key_file);
    fread(iv, 1, EVP_CIPHER_iv_length(cipher_verfahren), key_file);
    fclose(key_file);

    // Verschlüsselungsverfahren initialisieren
    cipher_verfahren = EVP_aes_128_xts();
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, cipher_verfahren, NULL, key, iv);

    // Verschlüsselungsprozess
    if (!EVP_EncryptUpdate(ctx, output_raw, &outlen, input, input_len)) {
        fprintf(stderr, "Fehler bei der Verschlüsselung.\n");
        fclose(output_datei);
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    // Schreibe die verschlüsselten Daten in die Ausgabedatei
    fwrite(output_raw, 1, outlen, output_datei);
    fclose(output_datei);
    EVP_CIPHER_CTX_free(ctx);
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

    printf("Verschlüsselte Nachricht aus s87622-cipher.bin ORIGINAL: \n");
    printf("\n");
    printf("%s", output_raw); 

    printf("Verschlüsselte Nachricht aus s87622-cipher.bin mit 'winterlichen Mützen': \n");
    printf("\n");
    char *muetzen_output = NULL;
    speichere_mit_muetzen(output_raw, outlen, &muetzen_output);
    printf("%s", muetzen_output);
    
    // Verschlüsselung
    verschluesseln_und_speichern((unsigned char*)muetzen_output, strlen(muetzen_output), "./bin/s87622-key2.bin", "./s87622-result.bin");



    // Aufräumarbeiten
    fclose(s87622_bin);
    EVP_CIPHER_CTX_free(ctx);

    return 0;
}
