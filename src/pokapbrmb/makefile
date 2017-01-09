TOPDIR=./../..

TARGET=pokapbrmb

LIBPATH=$(TOPDIR)/lib
EXEPATH=$(TOPDIR)/bin

#MYSQLLIB = -ldbmysql -lmysqlclient_r

CFLAGS+= -g  -I$(TOPDIR)/include/  $(IN_DB)
LDFLAGS+= -L$(TOPDIR)/lib  -lpokalog    -lpokafile -lpthread -lrt  -lcommon $(LID_DB)

#include $(TOPDIR)/cfg/Makefile.db
include $(TOPDIR)/cfg/Makefile.mk