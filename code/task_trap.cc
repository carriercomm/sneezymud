//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//      "task.cc" - All functions related to tasks that keep mobs/PCs busy
//
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

int task_trap_door(TBeing *ch, cmdTypeT cmd, const char *, int pulse, TRoom *, TObj *)
{
  char buf1[256], buf2[256];
  int learning;
  TRoom *rp2;
  roomDirData *back = NULL, *exitp = NULL;
  int rc; 
  TThing *t, *t2;
  TMonster *guard;

  half_chop(ch->task->orig_arg, buf1, buf2);

  if (ch->isLinkdead() || (ch->in_room != ch->task->wasInRoom) ||
      !ch->hasTrapComps(buf2, TRAP_TARG_DOOR, 0) ||
      (ch->getPosition() <= POSITION_SITTING) ||
      !(exitp = ch->roomp->dir_option[ch->task->flags]) ||
      !ch->getDiscipline(DISC_LOOTING) ||
      !IS_SET(exitp->condition, EX_CLOSED)) {
    if (ch->getPosition() >= POSITION_RESTING) {
      act("You suddenly stop trapping the door for some reason.",
                 FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops trapping and looks about confused and embarrassed.",
                 TRUE, ch, 0, 0, TO_ROOM);
    }
    ch->stopTask();
    return FALSE; // returning FALSE lets command be interpreted 
  }

  if (ch->utilityTaskCommand(cmd) ||
      ch->nobrainerTaskCommand(cmd))
    return FALSE;

  // check for guards that prevent
  for (t = ch->roomp->stuff; t; t = t2) {
    t2 = t->nextThing;
    guard = dynamic_cast<TMonster *>(t);
    if (!guard)
      continue;
    if (!guard->isPolice() || !guard->canSee(ch) ||
         !guard->awake())
      continue;
    guard->doSay("Hey!  We don't allow any of that nonsense here!");

    if ((rc = guard->takeFirstHit(*ch)) == DELETE_VICT)
      return DELETE_THIS;
    else if (rc == DELETE_THIS) {
      delete guard;
      guard = NULL;
    }
    return FALSE;
  }

  if (ch->task->timeLeft < 0)  {
    // Made it to end, set trap 
    SET_BIT(exitp->condition, EX_TRAPPED);
    exitp->trap_info = ch->task->status;

    // this is number of 8-sided die to use for damage
    int trapdamage = ch->getDoorTrapDam(doorTrapT(ch->task->status));
    exitp->trap_dam = trapdamage;

    // and now for other side 
    if ((rp2 = real_roomp(exitp->to_room)) &&
           (back = rp2->dir_option[rev_dir[ch->task->flags]]) &&
           back->to_room == ch->in_room) {
      SET_BIT(back->condition, EX_TRAPPED);
      back->trap_info = ch->task->status;
      back->trap_dam = trapdamage;
    }
    ch->sendTo("The trap has been successfully set!\n\r");
    ch->hasTrapComps(buf2, TRAP_TARG_DOOR, -1);   // delete the components
    ch->stopTask();
    return FALSE;
  }
  switch (cmd) {
    case CMD_TASK_CONTINUE:
      learning = ch->getDoorTrapLearn(doorTrapT(ch->task->status));
      ch->task->calcNextUpdate(pulse, 
                 PULSE_MOBACT * (5 + ((100 - learning)/3)));

      switch (ch->task->timeLeft) {
	case 3:
	  ch->sendTrapMessage(buf2, TRAP_TARG_DOOR, 1);
	  ch->task->timeLeft--;
	  break;
	case 2:
	  ch->sendTrapMessage(buf2, TRAP_TARG_DOOR, 2);
	  ch->task->timeLeft--;
	  break;
	case 1:
	  ch->sendTrapMessage(buf2, TRAP_TARG_DOOR, 3);
	  ch->task->timeLeft--;
	  break;
	case 0:
	  ch->sendTrapMessage(buf2, TRAP_TARG_DOOR, 4);
	  ch->task->timeLeft--;
	  break;
      }

      // test for failure
      if (!ch->doesKnowSkill(SKILL_SET_TRAP) ||
          !bSuccess(ch, learning, SKILL_SET_TRAP)) {
        // trigger trap

        rc = ch->goofUpTrap(doorTrapT(ch->task->status), TRAP_TARG_DOOR);
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;
        ch->stopTask();
        return FALSE;
      }
      break;
    case CMD_ABORT:
    case CMD_STOP:
      act("You stop trying to trap.", FALSE, ch, NULL, 0, TO_CHAR);
      act("$n stops tinkering with the door.", FALSE, ch, NULL, 0, TO_ROOM);
      ch->stopTask();
      break;
    case CMD_TASK_FIGHTING:
      ch->sendTo("You stop setting the trap so that you can defend yourself!\n\r");
      ch->stopTask();
      break;
    default:
      if (cmd < MAX_CMD_LIST)
	warn_busy(ch);
      break;			// eat the command 
  }
  return TRUE;
}

int task_trap_container(TBeing *ch, cmdTypeT cmd, const char *, int pulse, TRoom *, TObj *obj)
{
  int learning;
  int rc; 
  TThing *t, *t2;
  TMonster *guard;
  TRealContainer *cont = NULL;

  if (ch->isLinkdead() || (ch->in_room != ch->task->wasInRoom) ||
      !ch->hasTrapComps(ch->task->orig_arg, TRAP_TARG_CONT, 0) ||
      !obj ||
      !(cont = dynamic_cast<TRealContainer *>(obj)) ||
      !cont->isClosed() ||
      (ch->getPosition() <= POSITION_SITTING) ||
      !ch->getDiscipline(DISC_LOOTING)) {
    if (ch->getPosition() >= POSITION_RESTING) {
      act("You suddenly stop trapping the container for some reason.",
                 FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops trapping and looks about confused and embarrassed.",
                 TRUE, ch, 0, 0, TO_ROOM);
    }
    ch->stopTask();
    return FALSE; // returning FALSE lets command be interpreted 
  }

  if (ch->utilityTaskCommand(cmd) ||
      ch->nobrainerTaskCommand(cmd))
    return FALSE;

  // check for guards that prevent
  for (t = ch->roomp->stuff; t; t = t2) {
    t2 = t->nextThing;
    guard = dynamic_cast<TMonster *>(t);
    if (!guard)
      continue;
    if (!guard->isPolice() || !guard->canSee(ch) ||
         !guard->awake())
      continue;
    guard->doSay("Hey!  We don't allow any of that nonsense here!");

    if ((rc = guard->takeFirstHit(*ch)) == DELETE_VICT)
      return DELETE_THIS;
    else if (rc == DELETE_THIS) {
      delete guard;
      guard = NULL;
    }
    return FALSE;
  }

  if (ch->task->timeLeft < 0)  {
    // Made it to end, set trap 
    cont->addContainerFlag(CONT_TRAPPED);
    cont->setContainerTrapType(doorTrapT(ch->task->status));

    // this is number of 8-sided die to use for damage
    int trapdamage = ch->getContainerTrapDam(doorTrapT(ch->task->status));
    cont->setContainerTrapDam(trapdamage);

    ch->sendTo("The trap has been successfully set!\n\r");
    ch->hasTrapComps(ch->task->orig_arg, TRAP_TARG_CONT, -1);
    ch->stopTask();
    return FALSE;
  }
  switch (cmd) {
    case CMD_TASK_CONTINUE:
      learning = ch->getContainerTrapLearn(doorTrapT(ch->task->status));
      ch->task->calcNextUpdate(pulse, 
                 PULSE_MOBACT * (5 + ((100 - learning)/3)));

      switch (ch->task->timeLeft) {
	case 3:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_CONT, 1);
	  ch->task->timeLeft--;
	  break;
	case 2:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_CONT, 2);
	  ch->task->timeLeft--;
	  break;
	case 1:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_CONT, 3);
	  ch->task->timeLeft--;
	  break;
	case 0:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_CONT, 4);
	  ch->task->timeLeft--;
	  break;
      }

      // test for failure
      if (!ch->doesKnowSkill(SKILL_SET_TRAP) ||
          !bSuccess(ch, learning, SKILL_SET_TRAP)) {
        // trigger trap
        rc = ch->goofUpTrap(doorTrapT(ch->task->status), TRAP_TARG_CONT);
        if (IS_SET_DELETE(rc, DELETE_ITEM)) {
          delete cont;
          cont = NULL;
        }
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;

        ch->stopTask();
        return FALSE;
      }
      break;
    case CMD_ABORT:
    case CMD_STOP:
      act("You stop trying to trap $p.", FALSE, ch, cont, 0, TO_CHAR);
      act("$n stops tinkering with $p.", FALSE, ch, cont, 0, TO_ROOM);
      ch->stopTask();
      break;
    case CMD_TASK_FIGHTING:
      ch->sendTo("You stop setting the trap so that you can defend yourself!\n\r");
      ch->stopTask();
      break;
    default:
      if (cmd < MAX_CMD_LIST)
	warn_busy(ch);
      break;			// eat the command 
  }
  return TRUE;
}

void TTrap::makeTrapLand(TBeing *ch, doorTrapT status, const char *args)
{
  // this should be a number between 1-50
  int trapdamage = ch->getMineTrapDam(status);

  int stdflags = TRAP_EFF_MOVE | TRAP_EFF_NORTH |
          TRAP_EFF_EAST | TRAP_EFF_SOUTH | TRAP_EFF_WEST |
          TRAP_EFF_UP | TRAP_EFF_DOWN | TRAP_EFF_NE | TRAP_EFF_NW |
          TRAP_EFF_SE | TRAP_EFF_SW;

  // figure out criteria to add TRAP_EFF_ROOM later

  setTrapLevel(trapdamage);

  // this should probably be variable
  setTrapCharges(5);
  setTrapDamType(status);
  setTrapEffectType(stdflags);

  ch->sendTo("You have successfully constructed a land mine!\n\r");
  ch->hasTrapComps(args, TRAP_TARG_MINE, -1);
  ch->stopTask();

  *ch += *this;
}

int task_trap_mine(TBeing *ch, cmdTypeT cmd, const char *, int pulse, TRoom *, TObj *)
{
  int learning;
  int rc; 
  TThing *t, *t2;
  TMonster *guard;
  TObj *obj;

  if (ch->isLinkdead() || (ch->in_room != ch->task->wasInRoom) ||
      !ch->hasTrapComps(ch->task->orig_arg, TRAP_TARG_MINE, 0) ||
      (ch->getPosition() <= POSITION_SITTING) ||
      !ch->getDiscipline(DISC_LOOTING)) {
    if (ch->getPosition() >= POSITION_RESTING) {
      act("You suddenly stop construction of your land mine for some reason.",
                 FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops trapping and looks about confused and embarrassed.",
                 TRUE, ch, 0, 0, TO_ROOM);
    }
    ch->stopTask();
    return FALSE; // returning FALSE lets command be interpreted 
  }

  if (ch->utilityTaskCommand(cmd) ||
      ch->nobrainerTaskCommand(cmd))
    return FALSE;

  // check for guards that prevent
  for (t = ch->roomp->stuff; t; t = t2) {
    t2 = t->nextThing;
    guard = dynamic_cast<TMonster *>(t);
    if (!guard)
      continue;
    if (!guard->isPolice() || !guard->canSee(ch) ||
         !guard->awake())
      continue;
    guard->doSay("Hey!  We don't allow any of that nonsense here!");

    if ((rc = guard->takeFirstHit(*ch)) == DELETE_VICT)
      return DELETE_THIS;
    else if (rc == DELETE_THIS) {
      delete guard;
      guard = NULL;
    }
    return FALSE;
  }

  if (ch->task->timeLeft < 0)  {
    // Made it to end, set trap 
    if (!(obj = read_object(ST_LANDMINE, VIRTUAL))) {
      vlogf(LOG_BUG, "Unable to load mine for mine creation");
      ch->sendTo("Serious problem, contact a god.\n\r");
      ch->stopTask();
      return FALSE;
    }

    obj->makeTrapLand(ch, doorTrapT(ch->task->status), ch->task->orig_arg);
    return FALSE;
  }
  switch (cmd) {
    case CMD_TASK_CONTINUE:
      learning = ch->getMineTrapLearn(doorTrapT(ch->task->status));
      ch->task->calcNextUpdate(pulse, 
                 PULSE_MOBACT * (5 + ((100 - learning)/3)));

      switch (ch->task->timeLeft) {
	case 3:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_MINE, 1);
	  ch->task->timeLeft--;
	  break;
	case 2:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_MINE, 2);
	  ch->task->timeLeft--;
	  break;
	case 1:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_MINE, 3);
	  ch->task->timeLeft--;
	  break;
	case 0:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_MINE, 4);
	  ch->task->timeLeft--;
	  break;
      }

      // test for failure
      if (!ch->doesKnowSkill(SKILL_SET_TRAP) ||
          !bSuccess(ch, learning, SKILL_SET_TRAP)) {
        // trigger trap
        rc = ch->goofUpTrap(doorTrapT(ch->task->status), TRAP_TARG_MINE);
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;

        ch->stopTask();
        return FALSE;
      }
      break;
    case CMD_ABORT:
    case CMD_STOP:
      act("You stop trying to build a land mine.", FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops tinkering with $s land mine.", FALSE, ch, 0, 0, TO_ROOM);
      ch->stopTask();
      break;
    case CMD_TASK_FIGHTING:
      ch->sendTo("You stop setting the trap so that you can defend yourself!\n\r");
      ch->stopTask();
      break;
    default:
      if (cmd < MAX_CMD_LIST)
	warn_busy(ch);
      break;			// eat the command 
  }
  return TRUE;
}

void TTrap::makeTrapGrenade(TBeing *ch, doorTrapT status, const char *args)
{
  // this should be a number between 1-50
  int trapdamage = ch->getGrenadeTrapDam(status);

  setTrapLevel(trapdamage);

  // this should probably be variable
  setTrapCharges(1);
  setTrapDamType(status);
  setTrapEffectType(TRAP_EFF_THROW);

  ch->sendTo("You have successfully constructed a grenade!\n\r");
  ch->hasTrapComps(args, TRAP_TARG_GRENADE, -1);
  ch->stopTask();

  *ch += *this;
}

int task_trap_grenade(TBeing *ch, cmdTypeT cmd, const char *, int pulse, TRoom *, TObj *)
{
  int learning;
  int rc; 
  TThing *t, *t2;
  TMonster *guard;
  TObj *obj;

  if (ch->isLinkdead() || (ch->in_room != ch->task->wasInRoom) ||
      !ch->hasTrapComps(ch->task->orig_arg, TRAP_TARG_GRENADE, 0) ||
      (ch->getPosition() <= POSITION_SITTING) ||
      !ch->getDiscipline(DISC_LOOTING)) {
    if (ch->getPosition() >= POSITION_RESTING) {
      act("You suddenly stop construction of your grenade for some reason.",
                 FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops trapping and looks about confused and embarrassed.",
                 TRUE, ch, 0, 0, TO_ROOM);
    }
    ch->stopTask();
    return FALSE; // returning FALSE lets command be interpreted 
  }

  if (ch->utilityTaskCommand(cmd) ||
      ch->nobrainerTaskCommand(cmd))
    return FALSE;

  // check for guards that prevent
  for (t = ch->roomp->stuff; t; t = t2) {
    t2 = t->nextThing;
    guard = dynamic_cast<TMonster *>(t);
    if (!guard)
      continue;
    if (!guard->isPolice() || !guard->canSee(ch) ||
         !guard->awake())
      continue;
    guard->doSay("Hey!  We don't allow any of that nonsense here!");

    if ((rc = guard->takeFirstHit(*ch)) == DELETE_VICT)
      return DELETE_THIS;
    else if (rc == DELETE_THIS) {
      delete guard;
      guard = NULL;
    }
    return FALSE;
  }

  if (ch->task->timeLeft < 0)  {
    // Made it to end, set trap 
    if (!(obj = read_object(ST_GRENADE, VIRTUAL))) {
      vlogf(LOG_BUG, "Unable to load grenade for grenade creation");
      ch->sendTo("Serious problem, contact a god.\n\r");
      ch->stopTask();
      return FALSE;
    }

    obj->makeTrapGrenade(ch, doorTrapT(ch->task->status), ch->task->orig_arg);

    return FALSE;
  }
  switch (cmd) {
    case CMD_TASK_CONTINUE:
      learning = ch->getGrenadeTrapLearn(doorTrapT(ch->task->status));
      ch->task->calcNextUpdate(pulse, 
                 PULSE_MOBACT * (5 + ((100 - learning)/3)));

      switch (ch->task->timeLeft) {
	case 3:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_GRENADE, 1);
	  ch->task->timeLeft--;
	  break;
	case 2:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_GRENADE, 2);
	  ch->task->timeLeft--;
	  break;
	case 1:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_GRENADE, 3);
	  ch->task->timeLeft--;
	  break;
	case 0:
	  ch->sendTrapMessage(ch->task->orig_arg, TRAP_TARG_GRENADE, 4);
	  ch->task->timeLeft--;
	  break;
      }

      // test for failure
      if (!ch->doesKnowSkill(SKILL_SET_TRAP) ||
          !bSuccess(ch, learning, SKILL_SET_TRAP)) {
        // trigger trap
        rc = ch->goofUpTrap(doorTrapT(ch->task->status), TRAP_TARG_GRENADE);
        if (IS_SET_DELETE(rc, DELETE_THIS))
          return DELETE_THIS;

        ch->stopTask();
        return FALSE;
      }
      break;
    case CMD_ABORT:
    case CMD_STOP:
      act("You stop trying to build a grenade.", FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops tinkering with $s grenade.", FALSE, ch, 0, 0, TO_ROOM);
      ch->stopTask();
      break;
    case CMD_TASK_FIGHTING:
      ch->sendTo("You stop setting the trap so that you can defend yourself!\n\r");
      ch->stopTask();
      break;
    default:
      if (cmd < MAX_CMD_LIST)
	warn_busy(ch);
      break;			// eat the command 
  }
  return TRUE;
}
