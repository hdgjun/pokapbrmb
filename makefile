TOPDIR=.

include $(TOPDIR)/cfg/Makefile.db

exclude_dirs= include  bin  lib cfg sbin log
#code_dir=src

LIBPATH=$(TOPDIR)/lib
EXEPATH=$(TOPDIR)/bin

#数据库类型
DBTYPE=DB_ORACLE
#DBTYPE=DB_MYSQL
export DBTYPE

#调试模式,打印所有日志
export DEBUG=DEBUG

#定义走人行流程  注释走商行流程
#export BANKTYPE=PEOPLEBANK
#all:

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
$(shell mkdir -p bin)
$(shell mkdir -p lib)

include $(TOPDIR)/cfg/Makefile.mk
#all:
#	make -C ${code_dir} all||exit 1;
	
clean:
	$(RM) $(LIBPATH)/*.a $(EXEPATH)/*
	$(MAKE) cleansub||exit 1;
	