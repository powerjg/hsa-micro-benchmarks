


test: kernel.o main.cpp kernel.h
	g++ -o test kernel.o main.cpp -L/opt/rocm/lib -lhsa-runtime64

kernel.o kernel.h: kernel.cl
	../../CLOC/bin/snack.sh -c -amdllvm /opt/rocm/hcc-1.0/compiler -mcpu carrizo -libgcn /opt/rocm/lib kernel.cl 

clean:
	rm -f kernel.o test kernel.h
