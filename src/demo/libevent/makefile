#
# makefile
# puzeyu, 2019-05-06 23:07
#

exec = client.out 
CFLAGS = -lpthread -I/usr/local/include -L/usr/local/lib -levent

CC = gcc

all:$(exec)
	@echo "makefile needs your attention"

$(exec):%.out:%.c
	-$(CC) $< $(CFLAGS) -o $@


clean:
	-rm $(exec)



# vim:ft=make
#
