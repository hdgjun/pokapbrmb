TOPDIR=./../..


ifeq ($(DBTYPE),DB_MYSQL)
exclude_dirs= oracle
$(warning  exclude_dirs :$(exclude_dirs))
else
$(warning  exclude_dirs :$(exclude_dirs))
endif 

ifeq ($(DBTYPE),DB_ORACLE)
exclude_dirs= mysql
$(warning  exclude_dirs :$(exclude_dirs))
else
$(warning  exclude_dirs :$(exclude_dirs))
endif

export exclude_dirs

include $(TOPDIR)/cfg/Makefile.mk