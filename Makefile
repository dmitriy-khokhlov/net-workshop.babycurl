# The target binary executible to build
LINK_TARGET = babycurl

# The whole list of object files required to build LINK_TARGET.
# This usually should contain all object files obtained from
# the source files compilation process.
OBJECT_FILES = main.o log.o tcp.o

all : $(LINK_TARGET)

$(LINK_TARGET) : $(OBJECT_FILES)
	gcc -o $@ $^

%.o : src/%.c
	gcc -c -o $@ $<

main.o : src/log.h src/tcp.h
log.o : src/log.h
tcp.o : src/log.h src/tcp.h

clean :
	rm -f $(OBJECT_FILES) $(LINK_TARGET)
