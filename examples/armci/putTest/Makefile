CHARMC=../../../bin/charmc $(OPTS)

all: put

put: put.c
	$(CHARMC) -o put put.c -language armci -balancer RotateLB 

test: put
	./charmrun +p2 ./put +vp2 $(TESTOPTS)

bgtest: put
	./charmrun +p1 ./put +vp2 +x2 +y1 +z1 $(TESTOPTS)

clean:
	rm -f put charmrun *.o charmrun.exe put.exe put.pdb put.ilk
