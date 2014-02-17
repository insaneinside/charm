CHARMDIR=../..
CHARMC=$(CHARMDIR)/bin/charmc

include ../cuda.mk

# Those looking for a less-naive way of specifying the target compute
# capability -- for example, one that supports devices of several different
# compute capabilities -- are encouraged to read the nVidia CUDA Compiler
# documentation starting at this link:
# <http://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html#virtual-architecture-feature-list>
NVCC_FLAGS = -c -use_fast_math -DGPU_MEMPOOL -arch=compute_$(CUDA_LEVEL) -code=sm_$(CUDA_LEVEL) #-DGPU_PROFILE -DGPU_TRACE #-DGPU_MEMPOOL_DEBUG -DGPU_WRQ_VERBOSE #-device-debug -deviceemu
NVCC_INC += -I..

all: libs
	cp libcudahybridapi.a $(CHARMDIR)/lib

libs: libcudahybridapi.a

install: libcudahybridapi.a
	cp libcudahybridapi.a $(CHARMDIR)/lib
#	cp hybridapi.o wrqueue.o $(CHARMDIR)/tmp

libcudahybridapi.a: hybridapi.o wrqueue.o
	-rm -f $@
	ar q $@ hybridapi.o wrqueue.o 

hybridapi.o: cuda-hybrid-api.cu cuda-hybrid-api.h
	$(NVCC) $(NVCC_FLAGS) $(NVCC_INC) -o hybridapi.o cuda-hybrid-api.cu

wrqueue.o: wrqueue.cu wrqueue.h wr.h 
	$(NVCC) $(NVCC_FLAGS) $(NVCC_INC) -o wrqueue.o wrqueue.cu

clean:
	rm -f *.o *.a 
