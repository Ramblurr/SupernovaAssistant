drop table if exists warppoints;
drop table if exists systembodies;
drop table if exists system;
 -- drop index geoidx;
drop table if exists geodata;
drop table if exists PDATA;
drop table if exists items;
drop table if exists itemcomp;
drop table if exists designcomp;
drop table if exists designs;
drop table if exists expl;
drop table if exists pmapdata;
drop table if exists orbdata;
drop table if exists fleets;
drop table if exists stockpiles;
drop table if exists colonies;
drop table if exists csvdata;
drop table if exists systemscans;
drop table if exists systemprobes;
drop table if exists csvdata;
drop table if exists research;
drop table if exists inst;
drop table if exists instcomp;


create table system (
        sysname character(30) not null,
        sysscan character,
);

alter table system add primary key (sysname);

create table systembodies (
        pname           character(20) not null primary key,
        sysname         character(30) not null references system on delete cascade,
        orbit           smallint not null,
        suborbit        character,
        type            character(20),
        orbdist         float,
        diameter        integer,
        atmosphere      character(20),
        explored        character,
        pmap            character,
        csv             character,
        pop             character,
        geo             character,
        attrition       character(20)
);

create table warppoints (
        id              integer not null primary key,
        sysname         character(30) not null references system on delete cascade,
        orbdist         float,
        class           character,
        destsys         character(30) references system (sysname) on delete cascade
);

create table geodata (
        pname           character(20) not null references systembodies on delete cascade,
        resource        character(30) not null,
        yield           integer
);

create unique index GeoIdx on geodata (
        pname asc,
        resource asc
);

create table PDATA (
        pname           character(20) not null references systembodies on delete cascade,
        item            character(30) not null,
        id              integer not null,
        value           varchar(1024) not null
);

create unique index PIdx on PDATA (
        pname asc,
        item asc,
        id asc
);

create table items (
        iname           character(80) not null primary key,
        size            integer not null,
        text            varchar(16384) not null
);

create table itemcomp (
        iname           character(80) not null references items on delete cascade,
        anz             integer not null,
        resource        character(80)
);

create table research (
        iname           character(80) not null, 
        generation      integer not null,
	researched      char
);

create unique index RIdx on research (
        iname asc,
);


create table designs (
        dname           character(80) not null,
        type            character(80) not null,
        size            integer not null,
        text            varchar(3500) not null
);

create table designcomp (
        dname           character(80) not null,
        anz             integer not null,
        item            character(80) not null 
);

alter table designs add primary key (dname);

alter table designcomp add foreign key (dname) references designs(dname) on delete cascade;

create table expl (
        fleet           integer not null,
        type            character(1) not null,
 -- (T)ech, (W)arppoint, (I)tem, (N)one
        item            character(200), 
        amount          integer,
	find            varchar(3700) not null
);

create table pmapdata (
        pname           character(20) not null unique references systembodies on delete cascade,
        pmap            varchar(3000) not null
);

create table orbdata (
        pname           character(20) not null unique references systembodies on delete cascade,
        orb             varchar(2048) not null
);


create table csvdata (
        pname           character(20) not null references systembodies on delete cascade,
        csv             varchar(2048) not null
);


create table fleets (
        fid             integer not null,
        fname           character(80) not null,
	location        character(80) not null,
	mass            integer not null,
	fuel		integer not null,
	fuelmax		integer not null,
        ap              integer not null,
        text            varchar(3500) not null
);

alter table fleets add primary key (fid);

create table stockpiles (
        popid           integer not null,
        planet          character(80) not null,
        item            character(80) not null,
        anz             integer not null
);

create table colonies (
        popid           integer not null,
        planet          character(80) not null,
        inst            character(80) not null,
        anz             integer not null
);

create table inst (
        iname           character(80) not null primary key,
        size            integer not null,
        text            varchar(16384) not null
);

create table instcomp (
        iname           character(80) not null references inst on delete cascade,
        anz             integer not null,
        resource        character(80)
);

create table systemscans (
	sysname character(30) not null primary key references system on delete cascade,
	text    varchar(4096) not null
);

create table systemprobes (
        sysname         character(20) not null unique,
        sysprobe        varchar(4096) not null
);
