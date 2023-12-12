#
# (1)x86_64
#   make
#
# (2)aarch64
#   make CXX=aarch64-linux-gnu-g++ AR=aarch64-linux-gnu-ar ARCH=aarch64
#
CXX  ?= g++
AR   ?= ar
ARCH ?= $(shell uname -m)

BUILDIR = build/linux/$(ARCH)
LIBDIR  = lib/linux/$(ARCH)
FLAGS = -O3 --std=c++11

INCS = -I../spdlog/include -I./include
LIBS = -L$(LIBDIR) -lspdlog_wrapper -lspdlog -lpthread
SPDLOG_LIB = ../spdlog/build/$(ARCH)/libspdlog.a

all: bin/example.out $(LIBDIR)/libspdlog_wrapper.a

bin/example.out: example/main.cpp $(LIBDIR)/libspdlog_wrapper.a
	@if [ -e $(SPDLOG_LIB) ]; then \
        cp $(SPDLOG_LIB) $(LIBDIR); \
        echo "Copy file $(SPDLOG_LIB) to $(LIBDIR)"; \
    else \
        echo "\033[0;31m============================================================="; \
        echo "File $(SPDLOG_LIB) is not exist."; \
        echo "Please execute commands below to build libspdlog first:"; \
        echo "  cd ../spdlog && ./build-linux.sh $(ARCH) && cd -"; \
        echo "=============================================================\033[0m"; \
        exit 1; \
    fi
	$(CXX) $(INCS) $(FLAGS) -o $@ example/main.cpp $(LIBS)

$(LIBDIR)/libspdlog_wrapper.a: $(BUILDIR)/logger.o $(BUILDIR)/logger_config.o $(BUILDIR)/util.o $(BUILDIR)/ini.o
	$(AR) crv $@ $^

$(BUILDIR)/%.o: src/log/%.cpp
	$(shell if [ ! -e bin ]; then mkdir bin; fi)
	$(shell if [ ! -e $(BUILDIR) ]; then mkdir -p $(BUILDIR); fi)
	$(shell if [ ! -e $(LIBDIR)  ]; then mkdir -p $(LIBDIR); fi)
	$(CXX) -c $(INCS) $(FLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -rf bin build lib
