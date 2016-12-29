CC=gcc
MAKE=make

AR=ar cr
RM= -rm -rf

CP=mv 

CFLAGS+=-Wall -g

dirs:=$(shell find . -maxdepth 1 -type d)
dirs:=$(basename $(patsubst ./%,%,$(dirs)))
dirs:=$(filter-out $(exclude_dirs),$(dirs))
dirs:=$(filter-out $(TARGETDIRS),$(dirs))
SUBDIRS := $(dirs)

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPENDS=$(SRCS:%.c=%.d)


all:$(TARGET)  $(LIB) subdirs  tagdirs

$(LIB):$(OBJS) 
	$(AR)  $@  $^
	$(CP) $@ $(LIBPATH) 

tagdirs:$(TARGETDIRS)
	for dir in $(TARGETDIRS);\
	do $(MAKE) -C $$dir all||exit 1;\
	done
	
	
subdirs:$(SUBDIRS)
	for dir in $(SUBDIRS);\
    do $(MAKE) -C $$dir all||exit 1;\
	done


$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(CP) $@ $(EXEPATH)


$(OBJS):%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)


-include $(DEPENDS)

$(DEPENDS):%.d:%.c
	set -e; rm -f $@; \
    $(CC) -MM $(CFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[:]*,\1.o $@:,g' < $@.$$$$ > $@; \
    rm $@.$$$$

cleansub:
	for dir in $(SUBDIRS);\
    do $(MAKE) -C $$dir cleansub||exit 1;\
    done
	for dir in $(TARGETDIRS);\
    do $(MAKE) -C $$dir cleansub||exit 1;\
    done
	$(RM) $(TARGET) $(LIB)  $(OBJS) $(DEPENDS)

