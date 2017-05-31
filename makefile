TOPDIR=.

include $(TOPDIR)/cfg/Makefile.db

exclude_dirs= include  bin  lib cfg sbin log doc
#code_dir=src

LIBPATH=$(TOPDIR)/lib
EXEPATH=$(TOPDIR)/bin

#���ݿ�����
#DBTYPE=NO_DB
#DBTYPE=DB_ORACLE
#DBTYPE=DB_MYSQL
DBTYPE=DB_DB2
export DBTYPE

#����ģʽ,��ӡ������־
#export DEBUG=DEBUG

#��������������  ע������������
#export BANKTYPE=PEOPLEBANK
#all:

DBNAME=$(shell awk 'FS="=" {if ($$0~/^dbservicename/) print $$2}' $(TOPDIR)/cfg/pkrmb.ini)
DBUSR=$(shell awk 'FS="=" {if ($$0~/^dbuser/) print $$2}' $(TOPDIR)/cfg/pkrmb.ini)
DBPWD=$(shell awk 'FS="=" {if ($$0~/^dbpassword/) print $$2}' $(TOPDIR)/cfg/pkrmb.ini)
export DBNAME
export DBPWD
export DBUSR
	
ifeq ($(DBTYPE),DB_ORACLE)
	export IN_DB=$(IN_ORA)
	export LID_DB=$(LID_ORA)
	
else
	ifeq ($(DBTYPE),DB_MYSQL)
		export IN_DB=$(IN_MSQL)
		export LID_DB=$(LID_MSQL)
	else
		ifeq ($(DBTYPE),DB_DB2)
			export IN_DB=$(IN_DB2)
			export LID_DB=$(LID_DB2)
		endif
	endif
endif

$(shell mkdir -p bin)
$(shell mkdir -p lib)

include $(TOPDIR)/cfg/Makefile.mk
#all:
#	make -C ${code_dir} all||exit 1;
	
clean:
	$(RM) $(LIBPATH)/*.a $(EXEPATH)/*
	$(MAKE) cleansub||exit 1;
	