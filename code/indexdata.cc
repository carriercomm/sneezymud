//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#include "statistics.h"

extern FILE *obj_f;
extern FILE *mob_f;

vector<mobIndexData>mob_index(0);
vector<objIndexData>obj_index(0);

indexData::indexData() :
  virt(0),
  pos(0),
  number(0),
  name(NULL),
  short_desc(NULL),
  long_desc(NULL),
  description(NULL),
  max_exist(-99),
  spec(0),
  weight(0)
{
}

indexData & indexData::operator= (const indexData &a)
{
  if (this == &a)
    return *this;

  virt = a.virt;
  pos = a.pos;
  number = a.number;
  delete [] name;
  delete [] short_desc;
  delete [] long_desc;
  delete [] description;
  name = mud_str_dup(a.name);
  short_desc = mud_str_dup(a.short_desc);
  long_desc = mud_str_dup(a.long_desc);
  description = mud_str_dup(a.description);

  max_exist = a.max_exist;
  spec = a.spec;
  weight = a.weight;

  return *this;
}

indexData::indexData(const indexData &a) :
  virt(a.virt),
  pos(a.pos),
  number(a.number),
  max_exist(a.max_exist),
  spec(a.spec),
  weight(a.weight)
{
  name = mud_str_dup(a.name);
  short_desc = mud_str_dup(a.short_desc);
  long_desc = mud_str_dup(a.long_desc);
  description = mud_str_dup(a.description);
}

indexData::~indexData()
{
  delete [] name;
  delete [] short_desc;
  delete [] long_desc;
  delete [] description;
}

mobIndexData::mobIndexData() :
  faction(-99),
  Class(-99),
  level(-99),
  race(-99)
{
}

mobIndexData & mobIndexData::operator= (const mobIndexData &a)
{
  if (this == &a)
    return *this;

  indexData::operator=(a);

  faction = a.faction;
  Class = a.Class;
  level = a.level;
  race = a.race;

  return *this;
}

mobIndexData::mobIndexData(const mobIndexData &a) :
  indexData(a),
  faction(a.faction),
  Class(a.Class),
  level(a.level),
  race(a.race)
{
}

mobIndexData::~mobIndexData()
{
}

objIndexData::objIndexData() :
  ex_description(NULL),
  max_struct(-99),
  armor(-99),
  where_worn(0),
  itemtype(MAX_OBJ_TYPES),
  value(-99)
{
}

objIndexData & objIndexData::operator= (const objIndexData &a)
{
  if (this == &a)
    return *this;

  indexData::operator=(a);

  // use copy operator;
  if (a.ex_description)
    ex_description = new extraDescription(*a.ex_description);
  else
    ex_description = NULL;

  max_struct = a.max_struct;
  armor = a.armor;
  where_worn = a.where_worn;
  itemtype = a.itemtype;
  value = a.value;

  return *this;
}

objIndexData::objIndexData(const objIndexData &a) :
  indexData(a),
  max_struct(a.max_struct),
  armor(a.armor),
  where_worn(a.where_worn),
  itemtype(a.itemtype),
  value(a.value)
{
  // use copy operator;
  if (a.ex_description)
    ex_description = new extraDescription(*a.ex_description);
  else
    ex_description = NULL;
}

objIndexData::~objIndexData()
{
  extraDescription *tmp;
  while ((tmp = ex_description)) {
    ex_description = tmp->next;
    delete tmp;
  }
}

#if !USE_SQL
// generate index table for object
void generate_obj_index()
{
  int bc;
  char buf[256];
  objIndexData *tmpi = NULL;

  // to prevent constant resizing (slows boot), declare an appropriate initial
  // size.  Should be smallest power of 2 that will hold everything
  obj_index.reserve(8192);

  rewind(obj_f);

  for (;;) {
    if (fgets(buf, sizeof(buf)-1, obj_f) != NULL) {
      if (*buf == '#') {
        if (tmpi) {
          // push the previous one into the stack

          obj_index.push_back(*tmpi);
          delete tmpi;
        }

        tmpi = new objIndexData();
        if (!tmpi) {
          perror("indexData");
          exit(0);
        }

        sscanf(buf, "#%d", &bc);
        tmpi->virt = bc;
        tmpi->pos = ftell(obj_f);
        tmpi->name = (tmpi->virt < 99999) ? fread_string(obj_f) : 
              mud_str_dup("omega");
        tmpi->short_desc = (tmpi->virt < 99999) ? fread_string(obj_f) :
              mud_str_dup("omega");
        tmpi->long_desc = (tmpi->virt < 99999) ? fread_string(obj_f) :
              mud_str_dup("omega");
        tmpi->description = (tmpi->virt < 99999) ? fread_string(obj_f) :
              mud_str_dup("omega");

      } else if (*buf == '$') {
        // End of File
        // #99999  -> caused one before to be added
        // $~  -> now here
        delete tmpi;
        break;
      } else if (*buf == 'E') {
        extraDescription *new_descr;

        new_descr = new extraDescription();
        new_descr->keyword = fread_string(obj_f);
        new_descr->description = fread_string(obj_f);
        new_descr->next = tmpi->ex_description;
        tmpi->ex_description = new_descr;
      }
    } else {
      vlogf(LOG_BUG, "generate indices (last bc: %d)", bc);
      exit(0);
    }
  }

  return;
}
#else
// generate index table for object
void generate_obj_index()
{
  objIndexData *tmpi = NULL;
  MYSQL_RES *res, *extra_res;
  MYSQL_ROW row, extra_row;
  MYSQL *extra_db;
  extraDescription *new_descr;

  // to prevent constant resizing (slows boot), declare an appropriate initial
  // size.  Should be smallest power of 2 that will hold everything
  obj_index.reserve(8192);

  extra_db=mysql_init(NULL);
  if(!mysql_real_connect(extra_db, NULL, NULL, NULL, 
	  (gamePort==BETA_GAMEPORT ? "sneezybeta" : "sneezy"), 0, NULL, 0)){
    vlogf(LOG_BUG, "Could not connect (1) to database 'sneezy'.");
    exit(0);
  }

  if(mysql_query(extra_db, "select vnum, name, description from extra order by vnum")){
    vlogf(LOG_BUG, "Database query failed: %s\n", mysql_error(extra_db));
    exit(0);
  }
  extra_res=mysql_use_result(extra_db);
  extra_row=mysql_fetch_row(extra_res);

  if(!db){
    vlogf(LOG_MISC, "Connecting to database.");
    db=mysql_init(NULL);
    if(!mysql_real_connect(db, NULL, NULL, NULL, 
	  (gamePort==BETA_GAMEPORT ? "sneezybeta" : "sneezy"), 0, NULL, 0)){
      vlogf(LOG_BUG, "Could not connect (1) to database 'sneezy'.");
      exit(0);
    }
  }

  if(mysql_query(db, "select vnum, name, short_desc, long_desc, max_exist, spec_proc, weight, max_struct, wear_flag, type, price from object order by vnum")){
    vlogf(LOG_BUG, "Database query failed: %s\n", mysql_error(db));
    exit(0);
  }
  res=mysql_use_result(db);

  while((row=mysql_fetch_row(res))){
    tmpi = new objIndexData();
    if (!tmpi) {
      perror("indexData");
      exit(0);
    }
    
    tmpi->virt=atoi(row[0]);
    tmpi->name=mud_str_dup(row[1]);
    tmpi->short_desc=mud_str_dup(row[2]);
    tmpi->long_desc=mud_str_dup(row[3]);
    tmpi->max_exist=atoi(row[4]);
    tmpi->spec=atoi(row[5]);
    tmpi->weight=atof(row[6]);
    tmpi->max_struct=atoi(row[7]);
    tmpi->where_worn=atoi(row[8]);
    tmpi->itemtype=atoi(row[9]);
    tmpi->value=atoi(row[10]);

    while(extra_row && atoi(extra_row[0])==tmpi->virt){
      new_descr = new extraDescription();
      new_descr->keyword = mud_str_dup(extra_row[1]);
      new_descr->description = mud_str_dup(extra_row[2]);
      new_descr->next = tmpi->ex_description;
      tmpi->ex_description = new_descr;

      extra_row=mysql_fetch_row(extra_res);
    }

    obj_index.push_back(*tmpi);
    delete tmpi;
  }

  mysql_free_result(res);

  mysql_free_result(extra_res);
  mysql_close(extra_db);

  return;
}
#endif



// generate index table for monster file 
void generate_mob_index()
{
  char buf[256];
  mobIndexData *tmpi = NULL;

  // to prevent constant resizing (slows boot), declare an appropriate initial
  // size.  Should be smallest power of 2 that will hold everything
  mob_index.reserve(4096);

  rewind(mob_f);

  // start by reading
  if (fgets(buf, sizeof(buf)-1, mob_f) == NULL)
    return;

  for (;;) {
    int bc;
    if (*buf == '#') {
      if (tmpi) {
        // push the previous one into the stack
        mob_index.push_back(*tmpi);
        delete tmpi;
      }
      sscanf(buf, "#%d", &bc);
      if (bc >= 99999)  // terminator
        break;

      // start a new data member
      tmpi = new mobIndexData();
      if (!tmpi) {
        perror("mobIndexData");
        exit(0);
      }

      tmpi->virt = bc;
      tmpi->pos = ftell(mob_f);

      // read the strings
      tmpi->name = fread_string(mob_f);
      tmpi->short_desc = fread_string(mob_f);
      tmpi->long_desc = fread_string(mob_f);
      tmpi->description = fread_string(mob_f);

      int rc;
      long spac;
      long spaf;
      long fac;
      float facp;
      char let;
      float mult;
      rc = fscanf(mob_f, "%ld %ld %ld %f %c %f\n",
         &spac, &spaf, &fac, &facp, &let, &mult);
      if (rc != 6) {
        vlogf(LOG_BUG, "Error during mobIndexSetup(1) %d", bc);
        exit(0);
      }

      tmpi->faction = fac;
 
      long Class;
      long lev;
      long hitr;
      float arm;
      float hp;
      float daml;
      int damp;
      rc = fscanf(mob_f, "%ld %ld %ld %f %f %f+%d \n",
          &Class, &lev, &hitr, &arm, &hp, &daml, &damp);
      if (rc != 7) {
        vlogf(LOG_BUG, "Error during mobIndexSetup(2) %d (rc=%d)", bc, rc);
        exit(0);
      }

      tmpi->Class = Class;
      tmpi->level = lev;

      long mon;
      long race;
      long wgt;
      long hgt;
      rc = fscanf(mob_f, "%ld %ld %ld %ld \n",
          &mon, &race, &wgt, &hgt);
      if (rc != 4) {
        vlogf(LOG_BUG, "Error during mobIndexSetup(3) %d", bc);
        exit(0);
      }

      tmpi->race = race;
      tmpi->weight = wgt;

      long some_stat;
      statTypeT local_stat;
      for (local_stat = MIN_STAT; local_stat < MAX_STATS_USED; local_stat++)
        fscanf(mob_f, " %ld ", &some_stat);

      long mpos;
      long dpos;
      long sex;
      long spec;
      rc = fscanf(mob_f, "%ld %ld %ld %ld \n",
          &mpos, &dpos, &sex, &spec);
      if (rc != 4) {
        vlogf(LOG_BUG, "Error during mobIndexSetup(4) %d", bc);
        exit(0);
      }

      tmpi->spec = spec;

      long some_imm;
      immuneTypeT local_imm;
      for (local_imm = MIN_IMMUNE; local_imm < MAX_IMMUNES; local_imm++)
        fscanf(mob_f, " %ld ", &some_imm);

      long mat;
      long cbs;
      long vis;
      long maxe;
      rc = fscanf(mob_f, "%ld %ld %ld %ld \n",
          &mat, &cbs, &vis, &maxe);
      if (rc != 4) {
        vlogf(LOG_BUG, "Error during mobIndexSetup(5) %d", bc);
        exit(0);
      }

      tmpi->max_exist = (gamePort == BETA_GAMEPORT ? 9999 : maxe);

      // check for sounds and just account for them if found
      if (let == 'L') {
        char * snds = fread_string(mob_f);
        char * dsts = fread_string(mob_f);
        delete [] snds;
        delete [] dsts;
      }

      // handle some stat counters
      if (lev <= 5) {
        stats.mobs_1_5++;
      } else if (lev <= 10) {
        stats.mobs_6_10++;
      } else if (lev <= 15) {
        stats.mobs_11_15++;
      } else if (lev <= 20) {
        stats.mobs_16_20++;
      } else if (lev <= 25) {
        stats.mobs_21_25++;
      } else if (lev <= 30) {
        stats.mobs_26_30++;
      } else if (lev <= 40) {
        stats.mobs_31_40++;
      } else if (lev <= 50) {
        stats.mobs_41_50++;
      } else if (lev <= 60) {
        stats.mobs_51_60++;
      } else if (lev <= 70) {
        stats.mobs_61_70++;
      } else if (lev <= 100) {
        stats.mobs_71_100++;
      } else {
        stats.mobs_101_127++;
      }
      // end stat counters
    }

      // setup for next critter
    if (fgets(buf, sizeof(buf)-1, mob_f) == NULL) {
      vlogf(LOG_BUG, "Error during mobIndexSetup(6) %d", bc);
      exit(0);
    }
  }
  return;
}

