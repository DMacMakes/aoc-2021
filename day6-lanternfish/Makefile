# vim: set noet 
CC=clang++-7
#CLANGVERSION = $(shell clang++ --version | grep ^gcc | sed 's/^.* //g')
CFLAGS = -g

ifeq (, $(shell which clang++-7))
 CC = clang++
endif

ifeq (, $(shell which clang++))
 CC = g++
endif

CFLAGS=-I. -I./include -pthread -std=c++17
#ifeq (, $(shell echo $MSYSTEM))
ifeq ( $(origin MSYSTEM ) , undefined )
 # not defined
else
 CFLAGS+= -D_MINGW64=1
endif

OUTDIR=build
SOURCES=main.cpp timer.cpp
#src/file.cpp src/locale.cpp src/reader.cpp src/vscan.cpp
EXE=lanternfish
# strip the src/ from the sources 
#SOURCESNOFOLDERS=main.cpp $(patsubst src/%.cpp, %.cpp, $(SROUCES))
OBJ=$(patsubst %.cpp, $(OUTDIR)/%.o, $(SOURCES))

all: build $(EXE)

# $@ is a variable for whatever is on left of rule
# $^ is first thing on right of it, I guess $> is the rest?

$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $@ $^ lib/libscn.a lib/libfmt.a

$(OUTDIR)/%.o : %.cpp 
	$(CC) $(CFLAGS) -c -o $@ $< 

build:
	@echo Going with compiler $(CC)
	@mkdir -p build

cleanvs: clean
	@rm -rf .vs

clean:
	@rm -rf build
	@rm -f $(EXE)
	@rm -f $(EXE).exe
	@rm -f $(EXE).pdb

cleanlibs:
	@rm build/*.o
	
