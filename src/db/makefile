TOPDIR=./../..


ifeq ($(DBTYPE),DB_MYSQL)
exclude_dirs= oracle
else 
	ifeq ($(DBTYPE),DB_ORACLE)
	exclude_dirs= mysql
	else
		exclude_dirs= mysql oracle
	endif
endif 

export exclude_dirs

include $(TOPDIR)/cfg/Makefile.mk