TOPDIR=./../..


ifeq ($(DBTYPE),DB_MYSQL)
exclude_dirs= oracle db2
else 
	ifeq ($(DBTYPE),DB_ORACLE)
	exclude_dirs= mysql db2
	else
		ifeq ($(DBTYPE),DB_DB2)
		exclude_dirs= mysql oracle
		else
		exclude_dirs= mysql oracle db2
		endif
	endif
endif 

export exclude_dirs

include $(TOPDIR)/cfg/Makefile.mk