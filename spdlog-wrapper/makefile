
$(shell if [ ! -e bin ]; then mkdir bin; fi)
$(shell if [ ! -e build ]; then mkdir build; fi)
$(shell if [ ! -e lib/linux ]; then mkdir -p lib/linux; fi)

CC ?= g++
AR ?= ar

INCS = -I../spdlog/include -I./include
LIBS = -L./lib/linux -lspdlog_wrapper -lspdlog -lpthread
SPDLOG_LIB = ../spdlog/build/libspdlog.a

FLAGS = -O3 --std=c++11

all: bin/example.out lib/linux/libspdlog_wrapper.a

bin/example.out: example/main.cpp lib/linux/libspdlog_wrapper.a
	@if [ -e $(SPDLOG_LIB) ]; then \
        cp $(SPDLOG_LIB) ./lib/linux; \
        echo "Copy file $(SPDLOG_LIB) to ./lib/linux"; \
    else \
        echo "\033[0;31m============================================================="; \
        echo "File $(SPDLOG_LIB) is not exist."; \
        echo "Please execute commands below to build libspdlog first:"; \
        echo "  cd ../spdlog && ./build-linux.sh && cd -"; \
        echo "=============================================================\033[0m"; \
        exit 1; \
    fi
	$(CC) $(INCS) $(FLAGS) -o $@ example/main.cpp $(LIBS)

lib/linux/libspdlog_wrapper.a: build/logger.o build/logger_config.o build/util.o build/ini.o
	$(AR) crv $@ $^

build/%.o: src/log/%.cpp
	$(CC) -c $(INCS) -I./include/log $(FLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -rf bin build logs lib/linux/libspdlog_wrapper.a
