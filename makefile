TOPDIR=.

exclude_dirs= include  bin  lib cfg sbin log

LIBPATH=$(TOPDIR)/lib
EXEPATH=$(TOPDIR)/bin

include $(TOPDIR)/cfg/Makefile.mk

clean:
	$(RM) $(LIBPATH)/*.a $(EXEPATH)/*
	$(MAKE) cleansub||exit 1;
	