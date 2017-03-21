CC=gcc
PROC=proc
MAKE=make
AR=ar cr
RM= -rm -rf
CP=mv

CFLAGS+=-Wall -g   -D$(DBTYPE) 

ifdef DEBUG
CFLAGS+= -D$(DEBUG)
endif

ifdef BANKTYPE
CFLAGS+= -D$(BANKTYPE)
endif

dirs:=$(shell find . -maxdepth 1 -type d)
dirs:=$(basename $(patsubst ./%,%,$(dirs)))
dirs:=$(filter-out $(exclude_dirs),$(dirs))
dirs:=$(filter-out $(TARGETDIRS),$(dirs))
SUBDIRS := $(dirs)

SRCS:=$(wildcard *.pc)
ifneq ($(SRCS),)
SRCS:=$(filter-out $(exclude_files),$(SRCS))
OBJS:=$(SRCS:%.pc=%.c)
OBJS:=$(OBJS:%.c=%.o)
else
SRCS:=$(wildcard *.c)
$(warning  SRCS :$(SRCS))
SRCS1:=$(filter-out $(exclude_files),$(SRCS))
OBJS=$(SRCS1:%.c=%.o)
endif

$(warning  OBJS :$(OBJS))


.SUFFIXES: .sqc .pc .c .o
.pc.c:
	$(PROC) $< $(DBFLAGS) include=$(DBINCLUD)
#	$(CC) -c $(CFLAGS) $(LDFLAGS) $^
.c.o:
	$(CC) -c $(CFLAGS) $(LDFLAGS) $^
	
all:$(TARGET) $(SUBTARGET)  $(LIB) subdirs  tagdirs 

$(LIB):$(OBJS) 
	$(AR)  $@  $^
	$(CP) $@ $(LIBPATH) 
	$(RM) tp* *.lis
	
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	$(CP) $@ $(EXEPATH)

$(SUBTARGET):
	cp $(EXEPATH)/$(TARGET)  $(EXEPATH)/$(SUBTARGET)

tagdirs:$(TARGETDIRS)
	for dir in $(TARGETDIRS);\
	do $(MAKE) -C $$dir all||exit 1;\
	done
	
	
subdirs:$(SUBDIRS)
	for dir in $(SUBDIRS);\
    do $(MAKE) -C $$dir all||exit 1;\
	done

cleansub:
	for dir in $(SUBDIRS);\
    do $(MAKE) -C $$dir cleansub||exit 1;\
    done
	for dir in $(TARGETDIRS);\
    do $(MAKE) -C $$dir cleansub||exit 1;\
    done
	$(RM) $(TARGET) $(LIB)  $(OBJS) $(DEPENDS)

