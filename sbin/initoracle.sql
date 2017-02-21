declare
      num   number;
      table_ varchar2(50);
      colum_ varchar2(30);
begin
    table_:='BUSINESSLIST_ATM';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table BUSINESSLIST_ATM
													(
													  ID           NUMBER(30) not null,
													  BANKNO       VARCHAR2(20),
													  NETNO        VARCHAR2(20),
													  PERCODE      VARCHAR2(30),
													  ACCOUNTNO    VARCHAR2(30),
													  BUSINESSDATE DATE,
													  BUSINESSID   VARCHAR2(50),
													  MONEYTOTAL   NUMBER(18,2),
													  INOROUT      CHAR(1),
													  COREID       VARCHAR2(30),
													  REMARK       VARCHAR2(200),
													  INSERTDATE   DATE default SYSDATE
													)' ;
			execute immediate 'alter table  BUSINESSLIST_ATM add constraint IX_BUSINESSLIST_ATM_ID primary key(ID)';
			execute immediate 'create index  IX_BUSINESSLIST_ATM_ACCOUNTNO on  BUSINESSLIST_ATM (ACCOUNTNO)';
			execute immediate 'create index  IX_BUSINESSLIST_ATM_COREID on  BUSINESSLIST_ATM (COREID)';
			execute immediate 'create index  IX_BUSINESSLIST_ATM_INSERDATE on  BUSINESSLIST_ATM (INSERTDATE)';
			execute immediate 'create index  IX_BUSINESSLIST_ATM_NETNO on  BUSINESSLIST_ATM (NETNO)';

    end if;
    


    table_:='BUSINESSLIST_CORE';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  BUSINESSLIST_CORE
													(
													  ID           VARCHAR2(50) not null,
													  BANKNO       VARCHAR2(20),
													  NETNO        VARCHAR2(20),
													  USERID       VARCHAR2(8),
													  BUSINESSDATE DATE,
													  BUSINESSID   VARCHAR2(50),
													  MONEYTOTAL   NUMBER(18,2),
													  INOROUT      CHAR(1),
													  ACCOUNTNO    VARCHAR2(30),
													  ACCOUNTNAME  VARCHAR2(30),
													  ACCOUNTTYPE  VARCHAR2(2),
													  PUBORPRI     CHAR(1),
													  CHANNEL      CHAR(1),
													  REMARK       VARCHAR2(200),
												    INSERTDATE   DATE default SYSDATE
                          )';
      execute immediate 'alter table  BUSINESSLIST_CORE add constraint IX_BUSINESSLIST_CORE_ID primary key (ID)';
      execute immediate 'create index  IX_BT_CORE_BUSINESSDATE on  BUSINESSLIST_CORE (BUSINESSDATE)';
      execute immediate 'create index  IX_BT_CORE_BUSINESSID on  BUSINESSLIST_CORE (BUSINESSID)';
      execute immediate 'create index  IX_BUSINESSLIST_CORE_ACCOUNTNO on  BUSINESSLIST_CORE (ACCOUNTNO)';
      execute immediate 'create index  IX_BUSINESSLIST_CORE_INSERDATE on  BUSINESSLIST_CORE (INSERTDATE)';
      execute immediate 'create index  IX_BUSINESSLIST_CORE_NETNO on  BUSINESSLIST_CORE (NETNO)';
    end if;
    

    table_:='BUSINESSLIST_DETAIL';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  BUSINESSLIST_DETAIL
        (
          ID         VARCHAR2(50) not null,
          MONVAL     NUMBER(8,2) not null,
          RECORDS    NUMBER(11),
          INSERTDATE DATE default SYSDATE
        )';
      execute immediate 'alter table  BUSINESSLIST_DETAIL  add constraint IX_BUSINESSLIST_DETAIL_ID primary key (ID, MONVAL)';
      execute immediate 'create index  IX_BT_DETAIL_INSERDATE on  BUSINESSLIST_DETAIL (INSERTDATE)';
    end if;
    
    table_:='DATECUT';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  DATECUT
                          (
                          ID       NUMBER(10) not null,
                          LASTDATE VARCHAR2(20),
                          CURDATE  VARCHAR2(20),
                          STATUS   CHAR(1),
                          CUTTIME  NUMBER(6),
                          ENDTIME  NUMBER(6)
                          )';
      execute immediate 'alter table  DATECUT  add constraint IX_DATECUT_ID primary key (ID)';
    end if;
    
    table_:='MONEYDATA_ATM';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  MONEYDATA_ATM
                          (
                            ID           NUMBER(30) not null,
                            PERCODE      VARCHAR2(24),
                            COLTIME      DATE,
                            MON          VARCHAR2(20),
                            MONTYPE      CHAR(3),
                            MONVAL       NUMBER(8,2),
                            MONVER       CHAR(4),
                            TRUEFLAG     CHAR(1),
                            QUANLITY     CHAR(2),
                            OPERDATE     DATE,
                            IMAGEPATH    VARCHAR2(256),
                            BUSINESSTYPE VARCHAR2(4),
                            BANKNO       VARCHAR2(20),
                            AGENCYNO     VARCHAR2(20),
                            BUSINESSID   VARCHAR2(30),
                            RESERVE1     VARCHAR2(50),
                            RESERVE2     VARCHAR2(50)
                          )';
      execute immediate 'alter table  MONEYDATA_ATM  add constraint IX_MONEYDATA_ATM_ID primary key (ID)';
      execute immediate 'create index  IX_MONEYDATA_ATM_BUSINESSID on  MONEYDATA_ATM (BUSINESSID)';
      execute immediate 'create index  IX_MONEYDATA_ATM_COLTIME on  MONEYDATA_ATM (COLTIME)';
      execute immediate 'create index  IX_MONEYDATA_ATM_MON on  MONEYDATA_ATM (MON)';
      execute immediate 'create index  IX_MONEYDATA_ATM_OPERDATE on  MONEYDATA_ATM (OPERDATE)';
    end if;
    
    
    table_:='ROUTE';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  ROUTE
                          (
                            ID          NUMBER(30) not null,
                            TARGETBANK  VARCHAR2(20),
                            FORWARDBANK VARCHAR2(20),
                            IPADDR      VARCHAR2(20),
                            PORT_       VARCHAR2(8),
                            LOCALDIR    VARCHAR2(100),
                            REMOTEDIR   VARCHAR2(100),
                            SERVICECODE NUMBER(20),
                            LASTDATE    VARCHAR2(20),
                            STARTTIME   NUMBER(6),
                            INTER_VAL   NUMBER(32),
                            USER_       VARCHAR2(20),
                            PASSWORD_   VARCHAR2(20),
                            TYPE_       NUMBER(1),
                            STATUS_     CHAR(1),
                            MODEL_      NUMBER default 0
                          )';
      execute immediate 'alter table  ROUTE  add constraint IX_ROUTE_1_ID primary key (ID)';
    end if;
    
    table_:='SKJL';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  SKJL
                          (
                            ID          NUMBER(30) not null,
                            BANKNO      VARCHAR2(20),
                            NETNO       VARCHAR2(20),
                            USERID      VARCHAR2(8),
                            PERCODE     VARCHAR2(30),
                            BUSINESSDATE    DATE,
                            ACCOUNTNO   VARCHAR2(30),
                            INSERTDATE  DATE
                          )';
      execute immediate 'alter table  SKJL  add constraint IX_SKJL_1_ID primary key (ID)';
      execute immediate 'create index  IX_SKJL_BUSINESSDATE on  SKJL (BUSINESSDATE)';
      execute immediate 'create index  IX_SKJL_ACCOUNTNO on  SKJL (ACCOUNTNO)';
    end if;

    table_:='ROUTERULE';
    select count(1) into num from user_tables where table_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create table  ROUTERULE
                          (
                            ID         NUMBER(30) not null,
                            ROUTEID    NUMBER(30),
                            FILEEXTEND VARCHAR2(10)
                          )';
      execute immediate 'alter table  ROUTERULE  add constraint IX_ROUTERULE_ID primary key (ID)';
    end if;
    
    
    table_:='BUSINESSLIST_ATM_SEQ';
    select count(1) into num from user_sequences where sequence_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create sequence  BUSINESSLIST_ATM_SEQ
                            minvalue 0
                            maxvalue 9999999999999999999999999999
                            start with 140
                            increment by 1
                            cache 20
                            cycle';
    end if;

    table_:='DATECUT_SEQ';
    select count(1) into num from user_sequences where sequence_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create sequence  DATECUT_SEQ
                          minvalue 0
                          maxvalue 9999999999
                          start with 20
                          increment by 1
                          cache 20
                          cycle';
    end if;

    table_:='MONEYDATA_ATM_SEQ';
    select count(1) into num from user_sequences where sequence_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create sequence  MONEYDATA_ATM_SEQ
                          minvalue 0
                          maxvalue 9999999999999999999999999999
                          start with 80
                          increment by 1
                          cache 20
                          cycle';
    end if;
    
    table_:='SKJL_SEQ';
    select count(1) into num from user_sequences where sequence_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create sequence  SKJL_SEQ
                          minvalue 0
                          maxvalue 9999999999999999999999999999
                          start with 20
                          increment by 1
                          cache 20
                          cycle';
    end if;
 
    table_:='SKJL_TRIGGER';
    select count(1) into num from user_triggers where trigger_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create or replace trigger SKJL_TRIGGER
                            before insert on SKJL
                            for each
                            row
                            begin
                                  select  SKJL_SEQ.Nextval    into:new.ID from sys.dual;
                            end;';
    end if;
       

    table_:='BUSINESSLIST_ATM_TRIGGER';
    select count(1) into num from user_triggers where trigger_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create or replace trigger BUSINESSLIST_ATM_TRIGGER
                            before insert on BUSINESSLIST_ATM
                            for each
                            row
                            begin
                                  select  BUSINESSLIST_ATM_SEQ.Nextval    into:new.ID from sys.dual;
                            end;';
    end if;
    


    table_:='DATECUT_TRIGGER';
    select count(1) into num from user_triggers where trigger_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create or replace trigger DATECUT_TRIGGER
                            before insert on DATECUT
                            for each
                            row
                            begin
                                  select  DATECUT_SEQ.Nextval    into:new.ID from sys.dual;
                            end;';
    end if;


    table_:='MONEYDATA_ATM_TRIGGER';
    select count(1) into num from user_triggers where trigger_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create or replace trigger MONEYDATA_ATM_TRIGGER
                          before insert on MONEYDATA_ATM
                          for each
                          row
                          begin
                                select  MONEYDATA_ATM_SEQ.Nextval    into:new.ID from sys.dual;
                          end;';
    end if;

    table_:='ROUTERULE_TRIGGER';
    select count(1) into num from user_triggers where trigger_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create or replace trigger ROUTERULE_TRIGGER
                            before insert on ROUTERULE
                            for each
                            row
                            begin
                                  select  DATECUT_SEQ.Nextval    into:new.ID from sys.dual;
                            end;';
    end if;
    
    table_:='ROUTE_TRIGGER';
    select count(1) into num from user_triggers where trigger_name = upper(table_) ;
    if num = 0 then
        execute immediate 'create or replace trigger ROUTE_TRIGGER
                            before insert on ROUTE
                            for each
                            row
                            begin
                                select  DATECUT_SEQ.Nextval    into:new.ID from sys.dual;
                            end;';
    end if;                        

    table_:='bundleinfo';
    colum_:='id';
    SELECT COUNT(1)INTO num from cols where table_name = upper(table_) and column_name = upper(colum_);
    IF num = 0 THEN
        execute immediate 'alter table bundleinfo add id varchar2(30)';
    END IF;
    
    table_:='monboxaddmon';
    colum_:='id';
    SELECT COUNT(1)INTO num from cols where table_name = upper(table_) and column_name = upper(colum_);
    IF num = 0 THEN
        execute immediate 'alter table monboxaddmon add id varchar2(30)';
    END IF;

end;
/