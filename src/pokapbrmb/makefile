TOPDIR=./../..

TARGET=pokapbrmb

LIBPATH=$(TOPDIR)/lib
EXEPATH=$(TOPDIR)/bin

MYSQLLIB= -ldbmysql -lmysqlclient_r

CFLAGS= -g  -I$(TOPDIR)/include/ 
LDFLAGS= -L$(TOPDIR)/lib -lpokalog -lcommon   -lpokafile -lpthread -lrt  -ldl  -lm  $(MYSQLLIB)

include $(TOPDIR)/cfg/Makefile.db
include $(TOPDIR)/cfg/Makefile.mk