TOPDIR=./../..

LIB=libcommon.a


ifeq ($(DBTYPE),NO_DB)
exclude_files= datecut.c monrule.c
endif
INCPATH=$(TOPDIR)/include/
LIBPATH=$(TOPDIR)/lib
CFLAGS+= -I$(INCPATH) $(IN_DB)

LDFLAGS= -lpthread  -lrt -lpokalog -lm


include $(TOPDIR)/cfg/Makefile.mk