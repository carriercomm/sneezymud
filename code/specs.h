//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: specs.h,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////




typedef struct special_proc_entry {
  const char *name;
  int (*proc)();
} spe;


extern spe mob_specials[NUM_MOB_SPECIALS + 1];

