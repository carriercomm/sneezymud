//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __WIZPOWER_H
#define __WIZPOWER_H

// Wiz-Powers

enum wizPowerT {
     POWER_BUILDER,
     POWER_GOD,
     POWER_WIZARD,

     POWER_ACCESS,
     POWER_ACCOUNT,
     POWER_AT,
     POWER_BOARD_POLICE,
     POWER_BREATHE,
     POWER_CHANGE,
     POWER_CHECKLOG,
     POWER_CLIENTS,
     POWER_COLOR_LOGS,
     POWER_COMMENT,
     POWER_COMPARE,
     POWER_CRIT,
     POWER_CUTLINK,
     POWER_DEATHCHECK,
     POWER_ECHO,
     POWER_EDIT,
     POWER_EGOTRIP,
     POWER_FINDEMAIL,
     POWER_FLAG,
     POWER_FLAG_IMP_POWER,
     POWER_FORCE,
     POWER_GAMESTATS,
     POWER_GOTO,
     POWER_GOTO_IMP_POWER,
     POWER_HEAVEN,
     POWER_IMMORTAL_HELP,
     POWER_HOSTLOG,
     POWER_IMM_EVAL,
     POWER_INFO,
     POWER_INFO_TRUSTED,
     POWER_LOAD,
     POWER_LOAD_SET,
     POWER_LOAD_NOPROTOS,
     POWER_LOAD_LIMITED,
     POWER_LOAD_IMP_POWER,
     POWER_LOG,
     POWER_LOGLIST,
     POWER_LONGDESC,
     POWER_LOW,
     POWER_MEDIT,
     POWER_MEDIT_LOAD_ANYWHERE,
     POWER_MEDIT_IMP_POWER,
     POWER_MULTIPLAY,
     POWER_NOSHOUT,
     POWER_OEDIT,
     POWER_OEDIT_COST,
     POWER_OEDIT_APPLYS,
     POWER_OEDIT_WEAPONS,
     POWER_OEDIT_NOPROTOS,
     POWER_OEDIT_IMP_POWER,
     POWER_IMMORTAL_OUTFIT,
     POWER_PEE,
     POWER_POWERS,
     POWER_PURGE,
     POWER_PURGE_PC,
     POWER_PURGE_ROOM,
     POWER_PURGE_LINKS,
     POWER_QUEST,
     POWER_REDIT,
     POWER_REDIT_ENABLED,
     POWER_RENAME,
     POWER_REPLACE,
     POWER_REPLACE_PFILE,
     POWER_RESET,
     POWER_RESIZE,
     POWER_RESTORE,
     POWER_RESTORE_MORTAL,
     POWER_RLOAD,
     POWER_RSAVE,
     POWER_SEDIT,
     POWER_SEDIT_IMP_POWER,
     POWER_SEE_COMMENTARY,
     POWER_SEE_FACTION_SENDS,
     POWER_SET,
     POWER_SET_IMP_POWER,
     POWER_SETSEV,
     POWER_SETSEV_IMM,
     POWER_SHOW,
     POWER_SHOW_MOB,
     POWER_SHOW_OBJ,
     POWER_SHOW_TRUSTED,
     POWER_SHUTDOWN,
     POWER_SLAY,
     POWER_SNOOP,
     POWER_SNOWBALL,
     POWER_STAT,
     POWER_STAT_MOBILES,
     POWER_STAT_OBJECT,
     POWER_STAT_SKILL,
     POWER_STEALTH,
     POWER_SWITCH,
     POWER_SYSTEM,
     POWER_TIMESHIFT,
     POWER_TOGGLE,
     POWER_TOGGLE_INVISIBILITY,
     POWER_TRACEROUTE,
     POWER_TRANSFER,
     POWER_USERS,
     POWER_VIEW_IMM_ACCOUNTS,
     POWER_VISIBLE,
     POWER_WHERE,
     POWER_WIPE,
     POWER_WIZLOCK,
     POWER_WIZNET,
     POWER_WIZNET_ALWAYS,
     POWER_ZONEFILE_UTILITY,
     POWER_IDLED,

     MAX_POWER_INDEX
};
const wizPowerT MIN_POWER_INDEX = wizPowerT(0);
wizPowerT & operator++ (wizPowerT &, int);

#endif


