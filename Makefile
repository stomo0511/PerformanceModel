#
UNAME = $(shell uname)
ifeq ($(UNAME),Linux)
  TMATRIX_ROOT = /home/stomo/cuda-workspace/Remote/TileMatrix
  CXX =	g++
endif
ifeq ($(UNAME),Darwin)
  TMATRIX_ROOT = /Users/stomo/WorkSpace/TileAlgorithm/TileMatrix
  CXX =	/usr/local/bin/g++ 
endif

#
PLASMA_ROOT = /opt/plasma-17.1
PLASMA_INC_DIR = $(PLASMA_ROOT)/include
PLASMA_LIB_DIR = $(PLASMA_ROOT)/lib
PLASMA_LIBS = -lcoreblas -lplasma
#
TMATRIX_INC_DIR = $(TMATRIX_ROOT)
TMATRIX_LIB_DIR = $(TMATRIX_ROOT)
TMATRIX_LIBS = -lTileMatrix
#
COREBLAS_INC_DIR = $(COREBLAS_ROOT)
COREBLAS_LIB_DIR = $(COREBLAS_ROOT)
COREBLAS_LIBS = -lCoreBlasTile
#
CXXFLAGS =	-fopenmp -m64 -O2
#
RLOBJS =		TileQR.o Kernels.o RightLooking.o
LLOBJS =		TileQR.o Kernels.o Progress.o LeftLooking.o
SPOBJS =		TileQR.o Kernels.o Progress.o StaticPipeline.o
DSOBJS =		TileQR.o Kernels.o Progress.o DynamicSched.o
RTOBJS =		TileQR.o Kernels.o RightLooking_Task.o

all: RL LL SP RT

RL:	$(RLOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(RLOBJS)

LL:	$(LLOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(LLOBJS)

SP:	$(SPOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(SPOBJS)
				
DS:	$(DSOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(DSOBJS)

RT:	$(RTOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(RTOBJS)

clean:
	rm -f $(RTOBJS) $(RLOBJS) $(LLOBJS) $(SPOBJS) $(DSOBJS) RT RL LL SP DS
