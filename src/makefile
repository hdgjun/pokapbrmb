TOPDIR=./..

exclude_dirs= xpp main

ifeq ($(DBTYPE),NO_DB)
exclude_dirs+= pokapbrmb pokadaemon db pokafile
TARGETDIRS=pokaftp
else
TARGETDIRS=pokapbrmb pokadaemon pokaftp
endif
include $(TOPDIR)/cfg/Makefile.mk



