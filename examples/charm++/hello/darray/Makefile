CHARMC=../../../../bin/charmc $(OPTS)

OBJS = hello.o

all: hello

hello: $(OBJS)
	$(CHARMC) -language charm++ -o hello $(OBJS)

hello.decl.h: hello.ci
	$(CHARMC)  hello.ci

clean:
	rm -f *.decl.h *.def.h conv-host *.o hello charmrun charmrun.exe hello.exe hello.pdb hello.ilk

hello.o: hello.C hello.decl.h
	$(CHARMC) -c hello.C

test: all
	./charmrun ./hello +p2 10 $(TESTOPTS)

bgtest: all
	./charmrun ./hello +p2 10 +x2 +y2 +z1 $(TESTOPTS)
