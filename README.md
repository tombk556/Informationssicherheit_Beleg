# Informationssicherheit_Beleg

## Openssl installieren
```sh
tar xfz ~westfeld/30-isprak/openssl-3.0.12.tar.gz

cd openssl-3.0.12

./Configure

make

make test
```

## Makefile konfigurieren
Je nachdem wo das openssl-3.0.12 als Ordner abgelegt wurde das Makefile angpassen:

SSLDIR=./openssl-3.0.12 


## Code ausführen

```sh
make macos && ./s87622
```

## Entschlüsselte Datei anschauen:
```sh
cat decrypted.bin
```
Output:

```text
Kinder weinen.
Narren warten.
Dumme wissen.
Kleine meinen.
Weise gehen in den Garten.


1059847875%  
```


