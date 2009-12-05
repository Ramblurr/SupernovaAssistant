PRAGMA foreign_keys = ON;
drop table if exists dbase_info;
drop table if exists warppoints;
drop table if exists systembodies;
drop table if exists systems;
drop table if exists geodata;
drop table if exists pdata;
drop table if exists items;
drop table if exists itemcomp;
drop table if exists itemeffects;
drop table if exists designcomp;
drop table if exists designs;
drop table if exists expl;
drop table if exists pmapdata;
drop table if exists pmapterrain;
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

create table dbase_info (
        component character(30) not null,
        version integer not null
);
create table systems (
        sysname character(30) not null primary key,
        sysscan character
);

create table systembodies (
        pname           character(20) not null primary key,
        sysname         character(30) not null,
        orbit           smallint not null,
        suborbit        character,
        type            character(20),
        orbdist         float,
        diameter        integer,
        atmosphere      character(20)
        FOREIGN KEY(sysname) REFERENCES systems(sysname) ON DELETE CASCADE
);

create table warppoints (
        id              integer not null primary key,
        sysname         character(30) not null,
        orbdist         float,
        class           character,
        destsys         character(30),
        FOREIGN KEY (sysname) REFERENCES systems(sysname) ON DELETE CASCADE,
        FOREIGN KEY (destsys) REFERENCES systems(sysname) ON DELETE CASCADE
);

create table geodata (
        pname           character(20) not null,
        resource        character(30) not null,
        yield           integer,
        FOREIGN KEY(pname) REFERENCES systembodies(pname) ON DELETE CASCADE
);

create unique index GeoIdx on geodata (
        pname asc,
        resource asc
);

create table pdata (
        pname           character(20) not null,
        item            character(30) not null,
        id              integer not null,
        value           varchar(1024) not null,
        FOREIGN KEY (pname) REFERENCES systembodies(pname) ON DELETE CASCADE
);

create unique index PIdx on pdata (
        pname asc,
        item asc,
        id asc
);

create table items (
        iname            character(80) not null primary key,
        category         character(80) not null,
        subcategory      character(80),
        size             integer,
        desc             varchar(16384) not null,
        structure        integer
);

create table itemcomp (
        iname           character(80) not null,
        quantity        integer not null,
        resource        character(80) not null,
        FOREIGN KEY(iname) REFERENCES items(iname) ON DELETE CASCADE
);

create table itemeffects (
        iname           character(80),
        effect          character(80) not null,
        value           integer not null,
        prettyvalue     character(80),
        counter         character(80),
        FOREIGN KEY(iname) REFERENCES items(iname) ON DELETE CASCADE
);

create table research (
        iname           character(80) not null, 
        generation      integer not null,
	researched      char
);

create unique index RIdx on research (
        iname asc
);

create table designs (
        dname           character(80) not null primary key,
        type            character(80) not null,
        class           character(1) not null
);

-- holds the components for designs
create table designcomp (
        dname           character(80) not null,
        quantity             integer not null,
        item            character(80) not null,
        FOREIGN KEY (dname) REFERENCES designs(dname) ON DELETE CASCADE
);

create table expl (
        fleet           integer not null,
        type            character(1) not null,
        item            character(200), 
        amount          integer,
	find            varchar(3700) not null
);

create table pmapdata (
        pname           character(20) not null unique,
        temperature     int not null,
        axialtilt       int not null,
        gravity         float not null,
        ocean           character(30) not null,
        microorganisms  character(30) not null,
        pollution       character(30) not null,
        radiation       character(30) not null,
        text            varchar(3000) not null,
        FOREIGN KEY (pname) REFERENCES systembodies(pname) ON DELETE CASCADE
);

create table pmapterrain (
        pname           character(20) not null,
        name           character(30) not null,
        percent        integer,
        FOREIGN KEY (pname) REFERENCES systembodies(pname) ON DELETE CASCADE
);

create table orbdata (
        pname           character(20) not null unique,
        orb             varchar(2048) not null,
        FOREIGN KEY (pname) REFERENCES systembodies(pname) ON DELETE CASCADE
);


create table csvdata (
        pname           character(20) not null,
        csv             varchar(2048) not null,
        FOREIGN KEY (pname) REFERENCES systembodies(pname) ON DELETE CASCADE
);


create table fleets (
        fid             integer not null primary key,
        fname           character(80) not null,
	location        character(80) not null,
	mass            integer not null,
	fuel		integer not null,
	fuelmax		integer not null,
        ap              integer not null,
        text            varchar(3500) not null
);

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
	sysname character(30) not null primary key,
        startype character(30) not null,
        starsize character(15) not null,
	text    varchar(4096) not null,
        FOREIGN KEY (sysname) REFERENCES systems(sysname) ON DELETE CASCADE
);

create table systemprobes (
        sysname         character(20) not null unique,
        sysprobe        varchar(4096) not null
);
