


test: kernel.o main.cpp kernel.h
	g++ -o test kernel.o main.cpp -L/opt/hsa/lib -lhsa-runtime64 -lelf

kernel.o kernel.h: kernel.cl
	cloc -q -c kernel.cl

clean:
	rm -f kernel.o test kernel.h
