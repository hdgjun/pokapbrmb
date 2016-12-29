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

ifdef DBTYPE
$(warning  $(DBEXTEND))
SRCS=$(wildcard *.$(DBEXTEND))

SRCS:=$(filter-out $(exclude_files),$(SRCS))

DBOBJS=$(SRCS:%.$(DBEXTEND)=%.c)
$(warning  $(DBOBJS))
OBJS=$(DBOBJS:%.c=%.o)
$(warning  $(OBJS))
DEPENDS=$(OBJS:%.o=%.d)
$(warning  $(DEPENDS))
else
SRCS=$(wildcard *.c)
SRCS:=$(filter-out $(exclude_files),$(SRCS))
OBJS=$(SRCS:%.c=%.o)
DEPENDS=$(SRCS:%.c=%.d)
endif

all:$(TARGET) $(DBLIB) $(LIB) subdirs  tagdirs

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
	$(CC) -o $@ $^ $(LDFLAGS)  $(LIDBS)
	$(CP) $@ $(EXEPATH)
	$(RM) *.d 

$(DBOBJS):%.c:%.pc
	$(PROC) $< $(DBFLAGS) include=$(DBINCLUD)
	$(RM) tp* *.lis *.o
	
$(OBJS):%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INDBS) $(LIDBS)


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

