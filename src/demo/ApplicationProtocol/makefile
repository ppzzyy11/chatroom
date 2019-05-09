#
# makefile
# pzy, 2019-05-08 10:47
#
CFLAGS = -pthread -lgtest -std=c++11 -Wall
exec = test_database.out
obj = database.o test_database.o 

all:$(obj)
	@echo "\033[34mmakefile needs your attention\033[1;37m"
	@echo "\033[1;32mgenerate \033[0;33m"$(exec)"\033[1;37m"
	$(CXX) $(obj) $(CFLAGS) -o $(exec)

$(obj):%.o:%.cpp
	@echo "\033[1;32mgenerate \033[0;33m"$@"\033[1;37m"
	$(CXX) -c $< $(CFLAGS) -o $@

#.PHONY
clean:
	@echo "\033[1;31m"
	rm $(exec) $(obj)
# vim:ft=make
#
