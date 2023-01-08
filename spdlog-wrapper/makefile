
$(shell if [ ! -e bin ]; then mkdir bin; fi)
$(shell if [ ! -e build ]; then mkdir build; fi)
$(shell if [ ! -e lib/linux ]; then mkdir -p lib/linux; fi)

INCS = -I../spdlog/include -I./include
LIBS = -L./lib/linux -lspdlog_wrapper -lspdlog -lpthread
FLAGS = -O3

all: bin/example.out lib/linux/libspdlog_wrapper.a

bin/example.out: example/main.cpp lib/linux/libspdlog_wrapper.a
	g++ $(INCS) $(FLAGS) -o $@ example/main.cpp $(LIBS)

lib/linux/libspdlog_wrapper.a: build/logger.o build/logger_config.o build/util.o build/ini.o
	ar crv $@ $^

build/%.o: src/log/%.cpp
	g++ -c $(INCS) -I./include/log $(FLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -rf bin build logs lib/linux/libspdlog_wrapper.a
