makehistos : makehistos.cxx
	g++ -o $@ -I.. $(shell root-config --cflags) $^ $(shell root-config --libs)

clean:
	$(RM) makehistos

.PHONY: clean

.DELETE_ON_ERROR:
#EOF
