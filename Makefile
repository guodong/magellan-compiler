all: magellanc

OBJS = parser.o \
		token.o  \
		main.o	 \
		codegen.o


LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic -L/usr/local/lib
LIBS = `$(LLVMCONFIG) --libs`
CPPFLAGS += -I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/ -g
LDFLAGS += -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib

clean:
	$(RM) -rf parser.cpp parser.hpp test magellanc token.cpp *.output $(OBJS)

codegen.cpp: codegen.h node.h

parser.cpp: parser.y
	bison -d -o $@ $<

parser.hpp: parser.cpp

token.cpp: token.l parser.hpp
	flex -o $@ $<

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

magellanc: $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: magellanc example.mag
	cat example.mag | ./magellanc


