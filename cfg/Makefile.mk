CC=gcc
PROC=proc
MAKE=make
AR=ar cr
RM= -rm -rf
CP=mv

CFLAGS+=-Wall -g   -D$(DBTYPE) 


EXTRA_C_FLAGS=""
HARDWAREPLAT=$(shell uname -m)

$(warning  HARDWAREPLAT :$(HARDWAREPLAT))

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
	SRCS:=$(wildcard *.sqc)
	ifneq ($(SRCS),)
$(warning  SRCS :$(SRCS))
	SRCS1:=$(filter-out $(exclude_files),$(SRCS))
	OBJS:=$(SRCS1:%.sqc=%.c)
	OBJS:=$(OBJS:%.c=%.o)
	else
	SRCS:=$(wildcard *.c)
$(warning  SRCS :$(SRCS))
	SRCS1:=$(filter-out $(exclude_files),$(SRCS))
	OBJS=$(SRCS1:%.c=%.o)
	endif
endif

$(warning  OBJS :$(OBJS))

.SUFFIXES: .sqc .pc .c .o
.sqc.c:
	db2 prep $< target c package using $< bindfile
	db2 bind $(subst sqc,bnd,$<)

.pc.c:
	$(PROC) $< $(DBFLAGS) include=$(DBINCLUD) SQLCHECK=FULL USERID=$(DBUSR)/$(DBPWD)@$(DBNAME)
.c.o:
	$(CC) -c $(CFLAGS) $(LDFLAGS) $^
	
all:INIT $(TARGET) $(SUBTARGET)  $(LIB) subdirs  tagdirs ENDING
INIT:
ifeq ($(DBTYPE),DB_DB2)
	db2 connect to $(DBNAME) user $(DBUSR) using $(DBPWD)
endif
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

ENDING:
ifeq ($(DBTYPE),DB_DB2)
	db2 connect reset
	db2 terminate
endif	
cleansub:
	for dir in $(SUBDIRS);\
    do $(MAKE) -C $$dir cleansub||exit 1;\
    done
	for dir in $(TARGETDIRS);\
    do $(MAKE) -C $$dir cleansub||exit 1;\
    done
	$(RM) $(TARGET) $(LIB)  $(OBJS) $(DEPENDS)

