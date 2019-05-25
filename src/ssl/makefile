#
# makefile
# pzy, 2019-05-08 10:47
#
CFLAGS = -std=c++11 -Wall -levent -lssl -lcrypto -levent_openssl -pthread
exec = server.out client.out
obj = server.o

all:$(exec)
	@echo "\033[34mmakefile needs your attention\033[1;37m"
	@echo "\033[1;32mgenerate \033[0;33m"$(exec)"\033[1;37m"


$(exec):%.out:%.cpp
	@echo "\033[1;32mgenerate \033[0;33m"$@"\033[1;37m"
	$(CXX) $< $(CFLAGS) -o $@

#.PHONY
clean:
	@echo "\033[1;31m"
	rm $(exec) $(obj)
# vim:ft=make
#
