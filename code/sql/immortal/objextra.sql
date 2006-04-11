CREATE TABLE obj (
    vnum integer DEFAULT '0' NOT NULL,
    owner varchar(80) DEFAULT '' NOT NULL,
    name varchar(127) DEFAULT '' NOT NULL,
    short_desc varchar(127) DEFAULT '' NOT NULL,
    long_desc varchar(255) DEFAULT '' NOT NULL,
    action_desc varchar(255) DEFAULT '' NOT NULL,
    type integer DEFAULT '0' NOT NULL,
    action_flag integer DEFAULT '0' NOT NULL,
    wear_flag integer DEFAULT '0' NOT NULL,
    val0 integer DEFAULT '0' NOT NULL,
    val1 integer DEFAULT '0' NOT NULL,
    val2 integer DEFAULT '0' NOT NULL,
    val3 integer DEFAULT '0' NOT NULL,
    weight double precision DEFAULT '0.00' NOT NULL,
    price integer DEFAULT '0' NOT NULL,
    can_be_seen integer DEFAULT '0' NOT NULL,
    spec_proc integer DEFAULT '0' NOT NULL,
    max_exist integer DEFAULT '0' NOT NULL,
    max_struct integer DEFAULT '0' NOT NULL,
    cur_struct integer DEFAULT '0' NOT NULL,
    decay integer DEFAULT '0' NOT NULL,
    volume integer DEFAULT '0' NOT NULL,
    material integer DEFAULT '0' NOT NULL
);
CREATE TABLE objaffect (
    vnum integer DEFAULT '0' NOT NULL,
    owner varchar(80) DEFAULT '' NOT NULL,
    type integer DEFAULT '0' NOT NULL,
    mod1 integer DEFAULT '0' NOT NULL,
    mod2 integer DEFAULT '0' NOT NULL
);
CREATE TABLE objextra (
    vnum integer DEFAULT '0' NOT NULL,
    owner varchar(80) DEFAULT '' NOT NULL,
    name varchar(127) DEFAULT '' NOT NULL,
    description text NOT NULL
);
