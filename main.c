#include <stdio.h>

long file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Fehler beim Öffnen der Datei");
        return -1;
    }

    fseek(file, 0, SEEK_END);  // Bewegen Sie den Dateizeiger an das Ende der Datei
    long size = ftell(file);   // Holt die aktuelle Position des Dateizeigers (Dateigröße)
    fclose(file);

    return size;
}

int main() {
    const char *filename = "./s87622-result.bin";
    long size = file_size(filename);

    if (size != -1) {
        printf("Die Größe der Datei '%s' ist %ld Bytes.\n", filename, size);
    }

    return 0;
}