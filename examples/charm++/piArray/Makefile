CHARMC=../../../bin/charmc $(OPTS)

all: pgm

pgm: pgm.o
	$(CHARMC) pgm.o  -o pgm -language charm++

test: pgm
	./charmrun +p4 ./pgm 100000 100 $(TESTOPTS)

bgtest: pgm
	./charmrun +p4 ./pgm 100000 100 +x2 +y2 +z1 $(TESTOPTS)

pgm.o : pgm.C pgm.h pgm.decl.h
	$(CHARMC) -c pgm.C

pgm.decl.h: pgm.ci
	$(CHARMC) pgm.ci

clean:
	rm -f pgm *.o conv-host *.decl.h *.def.h *~ charmrun charmrun.exe pgm.exe pgm.pdb pgm.ilk
