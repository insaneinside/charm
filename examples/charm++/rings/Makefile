CHARMC=../../../bin/charmc $(OPTS)

all: pgm

pgm:  rings.o
	$(CHARMC) rings.o  -o pgm -language charm++

rings.o : rings.C rings.h rings.ci
	$(CHARMC) -c rings.ci rings.C

test: pgm
	./charmrun +p4 ./pgm $(TESTOPTS)

bgtest: pgm
	./charmrun +p4 ./pgm +x2 +y2 +z2 $(TESTOPTS)

clean:
	rm -f conv-host pgm *.o *.decl.h *.def.h *~ charmrun charmrun.exe pgm.exe pgm.pdb pgm.ilk
