//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: db.h,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


/* ************************************************************************
*  file: db.h , Database module.                          Part of DIKUMUD *
*  Usage: Loading/Saving chars booting world.                             *
************************************************************************* */


#ifndef __DB_H
#define __DB_H

#ifndef __STRUCTS_H
#error Please include STRUCTS.H prior to DB.H
#endif

// Strings for the mud name
// also: WELC_MESSG needs to be updated if these change
// also: SNEEZY_ADMIN
extern const char * const MUD_NAME;
extern const char * const MUD_NAME_VERS;

const char * const DFLT_DIR  ="lib";           /* default data directory     */

// const char * const WORLD_FILE       ="tinywld.use"; /* room definitions*/
const char * const WORLD_FILE       ="tinyworld.wld"; /* room definitions*/
const char * const MOB_FILE         ="tinymob.use"; /* monster prototypes*/
const char * const OBJ_FILE         ="tinyobj.use"; /* object prototypes */
const char * const ZONE_FILE ="tinyworld.zon"; /* zone defs & command tables */

const char * const CREDITS_FILE="txt/credits";   /* for the credits command */
const char * const NEWS_FILE        ="txt/news";  /* for the 'news' command */
const char * const STORY_FILE ="txt/story";     /* Stargazers pimpy story     */
const char * const WIZNEWS_FILE	 ="txt/wiznews";
const char * const MOTD_FILE  ="txt/motd";      /* messages of today */
const char * const WIZMOTD_FILE     ="txt/wizmotd";   /* MOTD for immorts */

const char * const TIME_FILE        ="time"; /* game calendar information  */
const char * const IDEA_FILE        ="txt/ideas"; /* for the 'idea'-command */
const char * const TYPO_FILE        ="txt/typos";     /*         'typo'     */
const char * const BUG_FILE         ="txt/bugs";      /*         'bug'      */
const char * const SOCMESS_FILE ="actions"; /* messgs for social acts     */
const char * const HELP_PATH	 ="help/";   /* for HELP <keywrd>          */
const char * const IMMORTAL_HELP_PATH="help/_immortal";
const char * const BUILDER_HELP_PATH="help/_builder";
const char * const SKILL_HELP_PATH  ="help/_skills";
const char * const SPELL_HELP_PATH  ="help/_spells";
const char * const HELP_PAGE_FILE   ="help/general";  /* for HELP <CR> */
const char * const WIZLIST_FILE     ="txt/wizlist";   /* for WIZLIST   */

const char * const CLASSHELP        ="help/classes overview";
const char * const RACEHELP         ="help/races overview";
const char * const STATHELP         ="help/characteristics overview";

const char * const MUDADMIN_EMAIL   ="mudadmin@sneezy.stanford.edu";
const char * const CODERS_EMAIL     ="coders@sneezy.stanford.edu";

/* public procedures in db.c */

void bootDb(void);
int create_entry(char *name);
void zone_update(void);
int real_object(int);
int real_mobile(int);

enum readFileTypeT {
     REAL,
     VIRTUAL
};

class resetCom {
  public:
    char command; 
    int if_flag; 
    int arg1;   
    int arg2;  
    int arg3; 
    int arg4;
    char character;
    resetCom();
    resetCom(const resetCom &t);
    ~resetCom();
    resetCom & operator =(const resetCom &t);
};

class zoneData
{
  public:
    char *name;             // name of this zone                  
    int lifespan;           // how long between resets (minutes)  
    int age;                // current age of this zone (minutes) 
    int top;                // upper limit for rooms in this zone 
    int reset_mode;         // conditions for reset (see below)   
    bool enabled;           // whether zone is enabled
    byte zone_value;
    unsigned int num_mobs;
    double mob_levels;
    double min_mob_level;
    double max_mob_level;

    vector<resetCom>cmd;          // command table for reset
  
    zoneData();
    zoneData(const zoneData &t);
    ~zoneData();
    zoneData & operator= (const zoneData &t);
};

class extraDescription;

class indexData {
  public:
    int virt;   
    long pos;  
    short int number;
    char *name;
    char *short_desc;
    char *long_desc;
    char *description;

    short max_exist;        // for objs and mobs
    int spec;
    float weight;

    indexData();
    indexData(const indexData &);
    indexData & operator= (const indexData &a);
    virtual ~indexData();
};

class objIndexData : public indexData
{
  public:
    extraDescription *ex_description;  // extra descriptions
    byte max_struct;
    sh_int armor;
    unsigned int where_worn;
    ubyte itemtype;
    int value;

    objIndexData();
    objIndexData(const objIndexData &);
    objIndexData & operator=(const objIndexData &);
    virtual ~objIndexData();
};

class mobIndexData : public indexData
{
  public:
    long faction;
    long Class;
    long level;
    long race;

    mobIndexData();
    mobIndexData(const mobIndexData &);
    mobIndexData & operator=(const mobIndexData &);
    virtual ~mobIndexData();
};

class resetQElement
{
  public:
    unsigned int zone_to_reset;   
    resetQElement *next;	

  resetQElement() :
    zone_to_reset(0),
    next(NULL)
  {
  }
};

const int WORLD_SIZE = 40000;
const int ZONE_ROOM_RANDOM = -99;

#endif
