PROGNAME=s87622
SSLDIR=./openssl-3.0.12
SSLLIBFLAGS=-lcrypto
SSLFLAGS=-L $(SSLDIR) -isystem $(SSLDIR)/include

all:
	cc -g -Wall $(SSLFLAGS) $(PROGNAME).c $(SSLLIBFLAGS) -o $(PROGNAME)

macos:
	cc -g -Wall $(SSLFLAGS) $(PROGNAME).c $(SSLLIBFLAGS) -o $(PROGNAME)
	install_name_tool -change /usr/local/lib/libcrypto.3.dylib $(SSLDIR)/libcrypto.3.dylib $(PROGNAME)

run:
	@LD_LIBRARY_PATH=$(SSLDIR) ./$(PROGNAME)
