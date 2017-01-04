TOPDIR=.

include $(TOPDIR)/cfg/Makefile.db

exclude_dirs= include  bin  lib cfg sbin log

LIBPATH=$(TOPDIR)/lib
EXEPATH=$(TOPDIR)/bin

#���ݿ�����
#DBTYPE=DB_ORACLE
DBTYPE=DB_MYSQL
export DBTYPE

#����ģʽ,��ӡ������־
export DEBUG=DEBUG

#��������������  ע������������
#export BANKTYPE=PEOPLEBANK


ifeq ($(DBTYPE),DB_ORACLE)
	export IN_DB=$(IN_ORA)
	export LID_DB=$(LID_ORA)
else
	ifeq ($(DBTYPE),DB_MYSQL)
		export IN_DB=$(IN_MSQL)
		export LID_DB=$(LID_MSQL)
	else
	$(warning  oracle_type)
	endif
endif



include $(TOPDIR)/cfg/Makefile.mk

clean:
	$(RM) $(LIBPATH)/*.a $(EXEPATH)/*
	$(MAKE) cleansub||exit 1;
	