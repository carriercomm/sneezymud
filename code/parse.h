/*************************************************************************

      SneezyMUD - All rights reserved, SneezyMUD Coding Team

      "parse.h" - defines and headers related to player commands

*************************************************************************/

#ifndef __PARSE_H
#define __PARSE_H

#ifndef _STDIO_H
#error Please include <stdio.h> prior to parse.h
#endif

int search_block(const char *arg, const char * const *, bool);
int old_search_block(const char *, int, int, const char * const *, bool);
void argument_interpreter(const char *, char *, char *);
void argument_interpreter(sstring, sstring &, sstring &);
extern const char *one_argument(const char *argument, char *first_arg);
extern sstring one_argument(sstring argument, sstring & first_arg);
void only_argument(const char *argument, char *dest);

class commandInfo {
  public:
    const char *name;
    positionTypeT minPosition;
    int minLevel;

    commandInfo(const char *n, positionTypeT mp, int ml) :
      name(n),
      minPosition(mp),
      minLevel(ml)
    {
    }
    ~commandInfo();
};

enum cmdTypeT {
     CMD_NORTH,
     CMD_EAST,
     CMD_SOUTH,
     CMD_WEST,
     CMD_UP,
     CMD_DOWN,
     CMD_NE,
     CMD_NW,
     CMD_SE,
     CMD_SW,
     CMD_DRINK,
     CMD_EAT,
     CMD_WEAR,
     CMD_WIELD,
     CMD_LOOK,
     CMD_SCORE,
     CMD_CACKLE,
     CMD_SHOUT,
     CMD_TELL,
     CMD_INVENTORY,
     CMD_GET,
     CMD_SAY,
     CMD_SMILE,
     CMD_DANCE,
     CMD_KISS,
     CMD_CRAWL,
     CMD_LAUGH,
     CMD_GROUP,
     CMD_SHAKE,
     CMD_PUT,
     CMD_GROWL,
     CMD_SCREAM,
     CMD_INSULT,
     CMD_COMFORT,
     CMD_NOD,
     CMD_SIGH,
     CMD_SULK,
     CMD_HELP,
     CMD_WHO,
     CMD_EMOTE,
     CMD_ECHO,
     CMD_STAND,
     CMD_SIT,
     CMD_REST,
     CMD_SLEEP,
     CMD_WAKE,
     CMD_FORCE,
     CMD_TRANSFER,
     CMD_HUG,
     CMD_SNUGGLE,
     CMD_CUDDLE,
     CMD_NUZZLE,
     CMD_CRY,
     CMD_NEWS,
     CMD_EQUIPMENT,
     CMD_BUY,
     CMD_SELL,
     CMD_VALUE,
     CMD_LIST,
     CMD_DROP,
     CMD_GOTO,
     CMD_WEATHER,
     CMD_READ,
     CMD_POUR,
     CMD_GRAB,
     CMD_REMOVE,
     CMD_PUKE,
     CMD_SHUTDOW,
     CMD_SAVE,
     CMD_HIT,
     CMD_EXITS,
     CMD_GIVE,
     CMD_QUIT,
     CMD_STAT,
     CMD_GUARD,
     CMD_TIME,
     CMD_LOAD,
     CMD_PURGE,
     CMD_SHUTDOWN,
     CMD_IDEA,
     CMD_TYPO,
     CMD_BUG,
     CMD_WHISPER,
     CMD_CAST,
     CMD_AT,
     CMD_AS,
     CMD_ORDER,
     CMD_SIP,
     CMD_TASTE,
     CMD_SNOOP,
     CMD_FOLLOW,
     CMD_RENT,
     CMD_OFFER,
     CMD_POKE,
     CMD_ACCUSE,
     CMD_GRIN,
     CMD_BOW,
     CMD_OPEN,
     CMD_CLOSE,
     CMD_LOCK,
     CMD_UNLOCK,
     CMD_LEAVE,
     CMD_APPLAUD,
     CMD_BLUSH,
     CMD_BURP,
     CMD_CHUCKLE,
     CMD_CLAP,
     CMD_COUGH,
     CMD_CURTSEY,
     CMD_FART,
     CMD_FLEE,
     CMD_FONDLE,
     CMD_FROWN,
     CMD_GASP,
     CMD_GLARE,
     CMD_GROAN,
     CMD_GROPE,
     CMD_HICCUP,
     CMD_LICK,
     CMD_LOVE,
     CMD_MOAN,
     CMD_NIBBLE,
     CMD_POUT,
     CMD_PURR,
     CMD_RUFFLE,
     CMD_SHIVER,
     CMD_SHRUG,
     CMD_SING,
     CMD_SLAP,
     CMD_SMIRK,
     CMD_SNAP,
     CMD_SNEEZE,
     CMD_SNICKER,
     CMD_SNIFF,
     CMD_SNORE,
     CMD_SPIT,
     CMD_SQUEEZE,
     CMD_STARE,
     CMD_STRUT,
     CMD_THANK,
     CMD_TWIDDLE,
     CMD_WAVE,
     CMD_WHISTLE,
     CMD_WIGGLE,
     CMD_WINK,
     CMD_YAWN,
     CMD_SNOWBALL,
     CMD_WRITE,
     CMD_HOLD,
     CMD_FLIP,
     CMD_SNEAK,
     CMD_HIDE,
     CMD_BACKSTAB,
     CMD_SLIT,
     CMD_PICK,
     CMD_STEAL,
     CMD_BASH,
     CMD_TRIP,
     CMD_RESCUE,
     CMD_KICK,
     CMD_FRENCH,
     CMD_COMB,
     CMD_MASSAGE,
     CMD_TICKLE,
     CMD_PRAY,
     CMD_PAT,
     CMD_QUIT2,
     CMD_TAKE,
     CMD_INFO,
     CMD_SAY2,
     CMD_QUEST,
     CMD_CURSE,
     CMD_USE,
     CMD_WHERE,
     CMD_LEVELS,
     CMD_PEE,
     CMD_EMOTE3,// ,
     CMD_BEG,
     CMD_BLEED,
     CMD_CRINGE,
     CMD_DAYDREAM,
     CMD_FUME,
     CMD_GROVEL,
     CMD_HOP,
     CMD_NUDGE,
     CMD_PEER,
     CMD_POINT,
     CMD_PONDER,
     CMD_PUNCH,
     CMD_SNARL,
     CMD_SPANK,
     CMD_STEAM,
     CMD_TACKLE,
     CMD_TAUNT,
     CMD_WIZNET,
     CMD_WHINE,
     CMD_WORSHIP,
     CMD_YODEL,
     CMD_WIZLIST,
     CMD_CONSIDER,
     CMD_GIGGLE,
     CMD_RESTORE,
     CMD_RETURN,
     CMD_SWITCH,
     CMD_QUAFF,
     CMD_RECITE,
     CMD_USERS,
     CMD_PROTECT,
     CMD_NOSHOUT,
     CMD_WIZHELP,
     CMD_CREDITS,
     CMD_EMOTE2,// :
     CMD_EXTINGUISH,
     CMD_SLAY,
     CMD_DEPOSIT,
     CMD_WITHDRAW,
     CMD_BALANCE,
     CMD_SYSTEM,
     CMD_PULL,
     CMD_EDIT,
     CMD_SET,  // @set
     CMD_RSAVE,
     CMD_RLOAD,
     CMD_TRACK,
     CMD_WIZLOCK,
     CMD_HIGHFIVE,
     CMD_TITLE,
     CMD_WHOZONE,
     CMD_ASSIST,
     CMD_ATTRIBUTE,
     CMD_WORLD,
     CMD_BREAK,
     CMD_REFUEL,
     CMD_SHOW,
     CMD_BODYSLAM,
     CMD_SPIN,
     CMD_TRANCE_OF_BLADES,
     CMD_INVISIBLE,
     CMD_GAIN,
     CMD_TIMESHIFT,
     CMD_DISARM,
     CMD_PARRY,
     CMD_THINK,
     CMD_ENTER,
     CMD_FILL,
     CMD_SHOVE,
     CMD_SCAN,
     CMD_TOGGLE,
     CMD_BREATH,
     CMD_GT,
     CMD_WHAP,
     CMD_LOG,
     CMD_BEAM,
     CMD_CHORTLE,
     CMD_REPORT,
     CMD_WIPE,
     CMD_STOP,
     CMD_BONK,
     CMD_SCOLD,
     CMD_DROOL,
     CMD_RIP,
     CMD_STRETCH,
     CMD_SPLIT,
     CMD_COMMAND,
     CMD_DEATHSTROKE,
     CMD_PIMP,
     CMD_LIGHT,
     CMD_BELITTLE,
     CMD_PILEDRIVE,
     CMD_TAP,
     CMD_BET,
     CMD_STAY,
     CMD_PEEK,
     CMD_COLOR,
     CMD_HEADBUTT,
     CMD_SUBTERFUGE,
     CMD_THROW,
     CMD_EXAMINE,
     CMD_SCRIBE,
     CMD_BREW,
     CMD_GRAPPLE,
     CMD_FLIPOFF,
     CMD_MOO,
     CMD_PINCH,
     CMD_BITE,
     CMD_SEARCH,
     CMD_SPY,
     CMD_DOORBASH,
     CMD_PLAY,
     CMD_FLAG,
     CMD_QUIVPALM,
     CMD_FEIGNDEATH,
     CMD_SPRINGLEAP,
     CMD_MEND_LIMB,
     CMD_ABORT,
     CMD_SIGN,
     CMD_CUTLINK,
     CMD_LAYHANDS,
     CMD_WIZNEWS,
     CMD_MAIL,
     CMD_CHECK,
     CMD_RECEIVE,
     CMD_CLS,
     CMD_REPAIR,
     CMD_MEND,
     CMD_SACRIFICE,
     CMD_PROMPT,
     CMD_GLANCE,
     CMD_CHECKLOG,
     CMD_LOGLIST,
     CMD_DEATHCHECK,
     CMD_SET_TRAP,
     CMD_CHANGE,
     CMD_REDIT,
     CMD_OEDIT,
     CMD_FEDIT,
     CMD_FADD,
     CMD_JOIN,
     CMD_DEFECT,
     CMD_RECRUIT,
     CMD_MEDIT,
     CMD_DODGE,
     CMD_ALIAS,
     CMD_CLEAR,
     CMD_SHOOT,
     CMD_BLOAD,
     CMD_MOUNT,
     CMD_DISMOUNT,
     CMD_RIDE,
     CMD_POST,
     CMD_ASK,
     CMD_ATTACK,
     CMD_SHARPEN,
     CMD_KILL,
     CMD_ACCESS,
     CMD_MOTD,
     CMD_REPLACE,
     CMD_LIMBS,
     CMD_PRACTICE,
     CMD_GAMESTATS,
     CMD_BANDAGE,
     CMD_SETSEV,
     CMD_PEELPK,
     CMD_TURN,
     CMD_DEAL,
     CMD_PASS,
     CMD_MAKELEADER,
     CMD_NEWMEMBER,
     CMD_RMEMBER,
     CMD_HISTORY,
     CMD_DRAG,
     CMD_MOVE,
     CMD_MEDITATE,
     CMD_SCRATCH,
     CMD_CHEER,
     CMD_WOO,
     CMD_GRUMBLE,
     CMD_APOLOGIZE,
     CMD_SEND,
     CMD_AGREE,
     CMD_DISAGREE,
     CMD_BERSERK,
     CMD_TESTCODE,
     CMD_SPAM,
     CMD_RAISE,
     CMD_ROLL,
     CMD_BLINK,
     CMD_BRUTTEST,
     CMD_HOSTLOG,
     CMD_PRESS,
     CMD_TWIST,
     CMD_MID,
     CMD_TRACEROUTE,
     CMD_TASKS,
     CMD_VIEWOUTPUT,
     CMD_EVALUATE,
     CMD_EXEC,
     CMD_LOW,
     CMD_KNEESTRIKE,
     CMD_PUSH,
     CMD_RESIZE,
     CMD_DISBAND,
     CMD_LIFT,
     CMD_ARCH,
     CMD_BOUNCE,
     CMD_DISGUISE,
     CMD_RENAME,
     CMD_MARGINS,
     CMD_DESCRIPTION,
     CMD_POISON_WEAPON,
     CMD_GARROTTE,
     CMD_STAB,
     CMD_CUDGEL,
     CMD_PENANCE,
     CMD_SMITE,
     CMD_CHARGE,
     CMD_HURL,
     CMD_LOWER,
     CMD_REPLY,
     CMD_HEAVEN,
     CMD_CAPTURE,
     CMD_ACCOUNT,
     CMD_RELEASE,
     CMD_FAINT,
     CMD_GREET,
     CMD_TIP,
     CMD_BOP,
     CMD_JUMP,
     CMD_JUNK,
     CMD_NOJUNK,
     CMD_WHIMPER,
     CMD_SNEER,
     CMD_MOON,
     CMD_BOGGLE,
     CMD_SNORT,
     CMD_TANGO,
     CMD_ROAR,
     CMD_FLEX,
     CMD_TUG,
     CMD_CROSS,
     CMD_HOWL,
     CMD_GRUNT,
     CMD_WEDGIE,
     CMD_SCUFF,
     CMD_NOOGIE,
     CMD_BRANDISH,
     CMD_DUCK,
     CMD_BECKON,
     CMD_WINCE,
     CMD_HUM	,
     CMD_RAZZ,
     CMD_GAG	,
     CMD_AVERT,
     CMD_SALUTE,
     CMD_PET	,
     CMD_GRIMACE,
     CMD_SEEKWATER,
     CMD_CRIT,
     CMD_FORAGE,
     CMD_APPLY_HERBS,
     CMD_RESET,
     CMD_STOMP,
     CMD_EMAIL,
     CMD_CLIMB,
     CMD_DESCEND,
     CMD_SORT,
     CMD_SADDLE,
     CMD_UNSADDLE,
     CMD_SHOULDER_THROW,
     CMD_CHOP,
     CMD_DIVINE,
     CMD_OUTFIT,
     CMD_CLIENTS,
     CMD_DULL,
     CMD_ADJUST,
     CMD_BUTCHER,
     CMD_SKIN,
     CMD_TAN,
     CMD_TITHE,
     CMD_CHI,
     CMD_DISSECT,
     CMD_FINDEMAIL,
     CMD_ENGAGE,
     CMD_DISENGAGE,
     CMD_RESTRING,
     CMD_CONCEAL,
     CMD_COMMENT,
     CMD_CAMP,
     CMD_YOGINSA,
     CMD_FLY,
     CMD_LAND,
     CMD_ATTUNE,
     CMD_AFK,
     CMD_CONTINUE,
     CMD_SOOTH,
     CMD_SUMMON,
     CMD_CHARM,
     CMD_BEFRIEND,
     CMD_TRANSFIX,
     CMD_BARKSKIN,
     CMD_FERAL_WRATH,
     CMD_SKY_SPIRIT,
     CMD_EARTHMAW,
     CMD_TRANSFORM,
     CMD_EGOTRIP,
     CMD_CHIP,
     CMD_DIG,
     CMD_COVER,
     CMD_OPERATE,
     CMD_SPELLS,
     CMD_RITUALS,
     CMD_COMPARE,
     CMD_TEST_FIGHT,
     CMD_DONATE,
     CMD_ZONES,
     CMD_FACTIONS,
     CMD_CREATE,
     CMD_POWERS,
     CMD_WHITTLE,
     CMD_MESSAGE,
     CMD_SMOKE,
     CMD_CLIENTMESSAGE,
     CMD_SEDIT,
     CMD_RETRAIN,
     CMD_VISIBLE,
     CMD_TRIGGER,
     CMD_STORE,
     CMD_ZONEFILE,
     CMD_LOOT,
     CMD_TROPHY,
     CMD_FISH,
     CMD_GLOAD,
     CMD_PTELL,
     CMD_PSAY,
     CMD_PSHOUT,
     CMD_MINDFOCUS,
     CMD_PSIBLAST,
     CMD_MINDTHRUST,
     CMD_PSYCRUSH,
     CMD_KWAVE,
     CMD_TELEVISION,
     CMD_PLANT,
     CMD_COOK,
     CMD_DRIVE,
     CMD_PSIDRAIN,
     MAX_CMD_LIST,  // Keep this as last command in regular list

       // a variety of "fake" commands used for a variety of purpsoses follow
       CMD_RESP_TOGGLE,
       CMD_RESP_UNTOGGLE,
       CMD_RESP_CHECKTOG,
       CMD_RESP_PERSONALIZE,
       CMD_RESP_ROOM_ENTER,
       CMD_RESP_UNFLAG,
       CMD_RESP_TOROOM,
       CMD_RESP_TOVICT,
       CMD_RESP_TONOTVICT,
       CMD_RESP_CHECKUNTOG,
       CMD_RESP_CHECKMAX,
       CMD_RESP_LINK,
       CMD_RESP_CODE_SEGMENT,
       CMD_RESP_RESIZE,
       CMD_RESP_CHECKLOAD,
       CMD_RESP_LOADMOB,
       CMD_RESP_PACKAGE,
       CMD_RESP_PULSE,
       CMD_RESP_CHECKROOM,
       CMD_RESP_CHECKNROOM,
       CMD_RESP_CHECKZONE,
       CMD_RESP_MOVETO,
       CMD_RESP_DESTINATION,
       CMD_RESP_CHECKPERSON,

       CMD_GENERIC_PULSE,
       CMD_GENERIC_QUICK_PULSE,
       CMD_GENERIC_CREATED,
       CMD_GENERIC_RESET,
       CMD_GENERIC_INIT,
       CMD_GENERIC_DESTROYED,

       CMD_ROOM_ENTERED,
       CMD_ROOM_ATTEMPTED_EXIT,
  
       CMD_OBJ_HITTING,
       CMD_OBJ_HIT,
       CMD_OBJ_MISS,
       CMD_OBJ_BEEN_HIT,
       CMD_OBJ_THROWN,
       CMD_OBJ_PUT_INSIDE_SOMETHING,
       CMD_OBJ_HAVING_SOMETHING_PUT_INTO,
       CMD_OBJ_STUCK_IN,
       CMD_OBJ_PULLED_OUT,
       CMD_OBJ_USED,
       CMD_OBJ_TOLD_TO_PLAYER,
       CMD_OBJ_GOTTEN,
       CMD_OBJ_WEATHER_TIME,
       CMD_OBJ_WAGON_INIT,
       CMD_OBJ_WAGON_UNINIT,
       CMD_OBJ_MOVEMENT,
       CMD_OBJ_MOVE_IN,
       CMD_OBJ_SATON,
       CMD_OBJ_EXPELLED,
       CMD_OBJ_START_TO_FALL,
       CMD_OBJ_OWNER_HIT,
     CMD_OBJ_OPENED,
       CMD_ARROW_GLANCE,
       CMD_ARROW_MISSED,
       CMD_ARROW_EMBED,
       CMD_ARROW_RIPPED,
       CMD_ARROW_DODGED,
       CMD_ARROW_HIT_OBJ,
       CMD_ARROW_INTO_ROOM,
       CMD_ARROW_SHOT,

       CMD_MOB_GIVEN_ITEM,
       CMD_MOB_GIVEN_COINS,
       CMD_MOB_ALIGN_PULSE,
       CMD_MOB_KILLED_NEARBY,
       CMD_MOB_MOVED_INTO_ROOM,
       CMD_MOB_VIOLENCE_PEACEFUL,
       CMD_MOB_COMBAT,

       CMD_TASK_FIGHTING,
       CMD_TASK_CONTINUE,
};
extern cmdTypeT & operator++(cmdTypeT &c, int);
const cmdTypeT MIN_CMD = cmdTypeT(0);

extern commandInfo *commandArray[MAX_CMD_LIST];
extern cmdTypeT searchForCommandNum(const char *);
extern void half_chop(const char *sstring, char *arg1, char *arg2);
extern bool _parse_name(const char *arg, char *name);
extern void makeLower(char *);
extern bool is_abbrev(const char *, const char *, multipleTypeT multiple = MULTIPLE_NO, exactTypeT exact = EXACT_NO);
extern bool is_abbrev(const sstring &, const sstring &, multipleTypeT multiple = MULTIPLE_NO, exactTypeT exact = EXACT_NO);
extern char *uncap(char *s);
extern char *cap(char *s);
extern char *fread_string(FILE *);
extern void trimString(sstring &);


class sstring : public string {
public:
  sstring() : string(){}
  sstring(const char *str) : string(str?str:"") {}
  sstring(const string &str) : string(str) {}
};


template<class T> T convertTo(const sstring s)
{
  T x;
  istringstream is(s);
  if(!(is >> x)) // let failure convert to 0 with no warning.  we relied on
    x=0;         // this (undefined) behavior with atoi, so we need it now

  return x;
}



#endif
