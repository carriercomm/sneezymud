//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: components.h,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


/* ************************************************************************
*  file: components.h, Implementation of periodic components      DIKUMUD *
************************************************************************* */

#ifndef __COMPONENTS_H
#define __COMPONENTS_H

const unsigned int COMP_DECAY      =    (1<<0);
const unsigned int COMP_SPELL      =    (1<<1);
const unsigned int COMP_POTION     =    (1<<2);
const unsigned int COMP_SCRIBE     =    (1<<3);

const unsigned int CACT_PLACE      = (1<<0);
const unsigned int CACT_REMOVE     = (1<<1);
const unsigned int CACT_UNIQUE     = (1<<2);

const int BRIDGE_ROOM     =10020;
const int BRIDGE_ROOM2    =11000;

const int HOUR_SUNRISE    =-2;
const int HOUR_SUNSET     =-3;
const int HOUR_MOONRISE   =-4;
const int HOUR_MOONSET    =-5;
const int HOUR_DAY_BEGIN  =-6;
const int HOUR_DAY_END    =-7;

const int COMP_SUFFOCATE        =201;
const int COMP_DUST_STORM       =202;
const int COMP_CONJURE_AIR      =204;
const int COMP_FEATHERY_DESCENT =205;
const int COMP_FALCON_WINGS     =206;
const int COMP_ANTIGRAVITY      =207;
const int COMP_POWERSTONE       =211;
const int COMP_SHATTER          =212;
const int COMP_ILLUMINATE       =213;
const int COMP_DETECT_MAGIC     =214;
const int COMP_DISPEL_MAGIC     =215;
const int COMP_COPY             =216;
const int COMP_GALVANIZE        =217;
const int COMP_GRANITE_FISTS    =218;
const int COMP_SKY_ROCK         =219;
const int COMP_PEBBLE_SPRAY     =220;
const int COMP_SAND_BLAST       =221;
const int COMP_VOLC_ROCK        =222;
const int COMP_STONE_SKIN       =223;
const int COMP_TRAIL_SEEK       =224;
const int COMP_LEVITATE         =225;
const int COMP_FLARE            =226;
const int COMP_PIXIE_TORCH      =227;
const int COMP_FLAMING_SWORD    =228;
const int COMP_HELLFIRE         =231;
const int COMP_STUNNING_ARROW   =235;
const int COMP_BLOOD_SCORN_WOMAN=236;
const int COMP_ENERGY_DRAIN     =237;
const int COMP_DRAGON_BONE      =238;
const int COMP_COLOR_SPRAY      =239;
const int COMP_ACID_BLAST       =240;
const int COMP_ANIMATE          =241;
const int COMP_SORCERERS_GLOBE  =242;
const int COMP_BIND             =243;
const int COMP_TELEPORT         =244;
const int COMP_SENSE_LIFE       =246;
const int COMP_FARLOOK          =247;
const int COMP_SILENCE          =248;
const int COMP_STEALTH          =249;
const int COMP_CALM             =250;
const int COMP_ENSORCER         =251;
const int COMP_FEAR             =252;
const int COMP_INVISIBILITY     =253;
const int COMP_CLOUD_OF_CONCEAL =254;
const int COMP_DETECT_INVIS     =255;
const int COMP_DISPEL_INVIS     =256;
const int COMP_TELEPATHY        =257;
const int COMP_TRUE_SIGHT       =258;
const int COMP_POLYMORPH        =259;
const int COMP_ACCELERATE       =260;
const int COMP_HASTE            =261;
const int COMP_PIXIE_DUST       =262;
const int COMP_FUMBLE           =263;
const int COMP_PIXIE_TEAR       =264;
const int COMP_ICY_GRIP         =265;
const int COMP_ARCTIC_BLAST     =267;
const int COMP_ICEBERG_CORE     =268;
const int COMP_GILLS_OF_FLESH   =271;
const int COMP_BREATH_SARAHAGE  =272;
const int COMP_INFRAVISION      =273;
const int COMP_ENHANCE_WEAPON   =274;
const int COMP_FLIGHT           =275;
const int COMP_PROT_EARTH       =293;
const int COMP_TRANSFORM_LIMB   =298;
const int COMP_DISPEL_MAGIC_BREW=1405;
const int COMP_TRAIL_SEEK_BREW  =1408;
const int COMP_INFRAVISION_BREW =1411;
const int COMP_SORCERERS_GLOBE_BREW  =1412;
const int COMP_SENSE_LIFE_BREW   =1414;
const int COMP_INFRAVISION2_BREW=1415;
const int COMP_INVISIBILITY_BREW=1416;
const int COMP_TRUE_SIGHT_BREW  =1418;
const int COMP_GILLS_OF_FLESH_BREW   =1420;
const int COMP_PLASMA_MIRROR    =1421;

class compPlace
{
  public:
    // a range of rooms, should all be consecutive
    // if you only want 1 room, set room2 = -1
    int room1;
    int room2;

    // could periodically load/remove it on a mob, not real useful and
    // realize this is probably better done in zone file
    // use the constant MOB_NONE if loading in a room
    // this is used in conjunction with room stuff
    // so first selects the room, then looks for a mob of this type in that room
    // if no mob in that room, it'll return.
    int mob;

    
    int number;     /* Number of object to put/take */

    // bitvector for type of actions to perform, CACT codes
    unsigned int place_act;  

    /* dont load if more than this number in game */
    // ignored on take commands
    int max_number;

    // percentage chance of load occuring
    // take actions this should probably be 100%
    int variance;

    // a range of hours to use
    // if it shoudl happen at 1 time, set hour2 to -1
    // thus to place only for hours 10-15 and then remove
    // have the place actions be 10, 14, and the take be 15, -1
    // uses mud hours, so 0-47 range
    // if hours aren't valid criteria, set both values to -1
    byte hour1;
    byte hour2;
    byte day1;
    byte day2;
    byte month1;
    byte month2;

    // weather condition
    // uses the WEATHER_xx values from weather.h
    // BUT, in order to be used as bitvector, do 1<<WEATHER_xx on all
    int weather;

    // message string sent to room item loads in or is removed from
    const char *message;

    // message string sent to entire room range when item loads anywhere
    // in that range.
    // not used for removal at all
    const char *glo_msg;

    soundNumT sound;
    unsigned int sound_loop;

    compPlace(int r, int r2, int m, int mn, int pa, int mx, int v, byte h1, byte h2, byte m1, byte m2, byte d1, byte d2, int w, const char *msg, const char *gm, soundNumT snt = SOUND_OFF, unsigned int sl = 1) :
      room1(r),
      room2(r2),
      mob(m),
      number(mn),
      place_act(pa),
      max_number(mx),
      variance(v),
      hour1(h1),
      hour2(h2),
      day1(d1),
      day2(d2),
      month1(m1),
      month2(m2),
      weather(w),
      message(msg),
      glo_msg(gm),
      sound(snt),
      sound_loop(sl)
    {
    }

    // default ctor, do not use generally
    // needs to be public for vector to use
    compPlace() {}
};

extern vector<compPlace>component_placement;

class compInfo 
{
  public:
    int comp_num;
    spellNumT spell_num;
    const char *to_caster;
    const char *to_other;
    const char *to_vict;
    const char *to_self;
    const char *to_room;
    const char *to_self_object;
    const char *to_room_object;

    compInfo(spellNumT sn, const char *tc, const char *to, const char *tv, const char *ts, const char *tr, const char *tso, const char *tro);

    // don't call this ctor
    // must be public due to vector initializer calling it
    compInfo() {}
};

void assign_component_placement();

typedef struct {
  int comp_vnum;
  spellNumT spell_num;
  int usage;
} COMPINDEX;

extern vector<COMPINDEX>CompIndex;
extern vector<compInfo>CompInfo;


#endif
