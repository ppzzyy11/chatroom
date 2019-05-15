#
# makefile
# puzeyu, 2019-05-06 23:07
#

exec = main.out 
CFLAGS = -lpthread -I/usr/local/include -L/usr/local/lib -levent -std=c++11

CC = g++

all:$(exec)
	@echo "makefile needs your attention"

$(exec):%.out:%.cpp
	-$(CC) $< $(CFLAGS) -o $@


clean:
	-rm $(exec)



# vim:ft=make
#
