/*************************************************************************

      SneezyMUD - All rights reserved, SneezyMUD Coding Team
      spec_objs.h : interface for calling objects special procedures

  ----------------------------------------------------------------------

  Special procedures for objects may be called under a number of different
  conditions.  These conditions are defined in this file with two 
  exceptions:  when cmd >= 0, a command is triggering the spec_proc, and
  when cmd == -1, the spec_proc is being triggered by a PULSE.

  A return value of zero (FALSE) indicates that nothing exceptional
  happened.  A non-zero return value (TRUE) indicates that some action
  needs to be taken (such as ignoring the players command).  When in doubt, 
  return FALSE.  A -1 indicates something died as a result.

*************************************************************************/

#ifndef __SPEC_OBJS_H
#define __SPEC_OBJS_H

const int SPEC_FOUNTAIN     =     1;     // spec number for a fountain
const int SPEC_BOARD        =     2 ;    // spec number for a bulletin board

const int NUM_OBJ_SPECIALS = 81;

struct TObjSpecs {
  bool assignable;
  const char *name;
  int (*proc) (TBeing *, cmdTypeT, const char *, TObj *, TObj *);
};

extern const int GET_OBJ_SPE_INDEX(int d);

extern TObjSpecs objSpecials[NUM_OBJ_SPECIALS + 1];

#endif
