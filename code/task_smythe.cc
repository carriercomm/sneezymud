//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: task_smythe.cc,v $
// Revision 5.10  2002/06/13 05:38:53  peel
// fixed up smything a bit
//
// Revision 5.9  2002/06/13 04:39:20  peel
// depreciation code was buggy, so I just removed it
// probably ok for players to repair to brand new anyway, gies them an
// advantage over repair shop
//
// Revision 5.8  2002/03/14 15:43:13  jesus
// *** empty log message ***
//
// Revision 5.7  2002/03/14 15:30:38  jesus
// *** empty log message ***
//
// Revision 5.6  2002/03/14 15:22:37  jesus
// made move drain skill dependant
//
// Revision 5.5  2002/01/10 00:45:49  peel
// more splitting up of obj2.h
//
// Revision 5.4  2001/09/25 12:44:43  jesus
// smythe fix
//
// Revision 5.3  2001/09/07 07:07:35  peel
// changed TThing->stuff to getStuff() and setStuff()
//
// Revision 5.2  2001/07/23 00:33:30  jesus
// added goofers dust
//
// Revision 5.1.1.4  2000/12/21 20:15:35  jesus
// *** empty log message ***
//
// Revision 5.1.1.3  2000/11/19 22:20:48  jesus
// smythe update
//
// Revision 5.1.1.2  2000/11/19 13:44:19  jesus
// smythe fixed
//
// Revision 5.1.1.1  1999/10/16 04:32:20  batopr
// new branch
//
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD 4.0 - All rights reserved, SneezyMUD Coding Team
//      "task.cc" - All functions related to tasks that keep mobs/PCs busy
//
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"
#include "obj_tool.h"

void TTool::findSmytheTools(TTool **forge, TTool **anvil)
{
  if (!*forge && getToolType() == TOOL_FORGE)
    *forge = this;
  else if (!*anvil && getToolType() == TOOL_ANVIL)
    *anvil = this;
}

int smythe_tools_in_room(int room, TTool **forge, TTool **anvil)
{
  TRoom *rp;
  TThing *t;

  if (!(rp = real_roomp(room)))
    return FALSE;

  for (t = rp->getStuff(); t; t = t->nextThing) {
    t->findSmytheTools(forge, anvil);
  }
  return (*forge && *anvil);
}

void smythe_stop(TBeing *ch)
{
  if (ch->getPosition() < POSITION_SITTING) {
    act("You stop smything, and look about confused.  Are you missing something?",
         FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops smything, and looks about confused and embarrassed.",
        FALSE, ch, 0, 0, TO_ROOM);
  }
  ch->stopTask();
}

void TThing::smythePulse(TBeing *ch, TObj *)
{
  smythe_stop(ch);
}

void TTool::smythePulse(TBeing *ch, TObj *o)
{
  TTool *forge = NULL, *anvil = NULL;
  int percent;
  int movemod = ::number(10,25);
  int movebonus = ::number(1,((ch->getSkillValue(SKILL_SMYTHE) / 10)));
  const int HEATING_TIME = 3;

  // sanity check
  if ((getToolType() != TOOL_HAMMER) ||
      !smythe_tools_in_room(ch->in_room, &forge, &anvil) ||
      (ch->getPosition() < POSITION_RESTING)) {
    smythe_stop(ch);
    return;
  }

  if (movebonus > movemod) {
    movebonus = 5;
  }

  if (ch->getRace() == RACE_DWARF) {
    ch->addToMove(-movemod);
    ch->addToMove(movebonus);
    ch->addToMove(4);
  } else {
    ch->addToMove(-movemod);
    ch->addToMove(movebonus);
  }
  if (ch->getMove() < 10) {
    act("You are much too tired to continue repairing $p.", FALSE, ch, o, this, TO_CHAR);
    act("$n stops repairing, and wipes sweat from $s brow.", FALSE, ch, o, this, TO_ROOM);
    ch->stopTask();
    return;
  }
  if (ch->task->status < HEATING_TIME) {
    if (!ch->task->status) {
      act("$n allows $p to heat in $P.", FALSE, ch, o, forge, TO_ROOM);
      act("You allow $p to heat in $P.", FALSE, ch, o, forge, TO_CHAR);
    } else {
      act("$n continues to let $p heat in $P.", FALSE, ch, o, forge, TO_ROOM);
      act("You continue to let $p heat in $P.", FALSE, ch, o, forge, TO_CHAR);
    }
    ch->task->status++;
  } else if (ch->task->status == HEATING_TIME) {
    act("$n removes $p from $P, as it glows red hot.", FALSE, ch, o, forge, TO_ROOM);
    act("You remove $p from $P, as it glows red hot.", FALSE, ch, o, forge, TO_CHAR);
    ch->task->status++;
  } else {
    act("$n pounds $p on an anvil with $s hammer.",
            FALSE, ch, o, 0, TO_ROOM);
    act("You pound $p on an anvil with your hammer.",
            FALSE, ch, o, 0, TO_CHAR);
    addToToolUses(-1);
    if (getToolUses() <= 0) {
      ch->sendTo("Your %s breaks due to overuse.\n\r", fname(name).c_str());
      act("$n looks startled as $e breaks $P while hammering.", FALSE, ch, o, this, TO_ROOM);
      makeScraps();
      ch->stopTask();
      delete this;
      return;
    }
    if (o->getMaxStructPoints() <= o->getStructPoints()) {
      act("$n finishes repairing $p and proudly smiles.", FALSE, ch, o, forge, TO_ROOM);
      act("You finish repairing $p and smile triumphantly.", FALSE, ch, o, forge, TO_CHAR);
      act("You let $p cool down.", FALSE, ch, o, 0, TO_CHAR);
      act("$n lets $p cool down.", FALSE, ch, o, 0, TO_ROOM);
      ch->stopTask();
      return;
    }
    if ((percent = ::number(1, 101)) != 101)    // 101 is complete failure
      percent -= ch->getDexReaction() * 3;

    if (percent < ch->getSkillValue(SKILL_SMYTHE))
      o->addToStructPoints(1);
    else
      o->addToStructPoints(-1);

    if (o->getStructPoints() <= 1) {
      act("$n screws up repairing $p and utterly destroys it.", FALSE, ch, o, forge, TO_ROOM);
      act("You screw up repairing $p and utterly destroy it.", FALSE, ch, o, forge, TO_CHAR);
      makeScraps();
      ch->stopTask();
      delete o;
      return;
    }
    // task can continue forever, so don't bother decrementing the timer
  }
}

int task_smythe(TBeing *ch, cmdTypeT cmd, const char *v_name, int pulse, TRoom *, TObj *)
{
  TThing *w = NULL, *t;
  TObj *o = dynamic_cast<TObj *>(ch->heldInSecHand());
  int learning;

  for(t=ch->getStuff();t;t=t->nextThing){
    if((o=dynamic_cast<TObj *>(t)) && isname(v_name, o->name))
      break;
  }

  // sanity check
  if (ch->isLinkdead() || (ch->in_room < 0) ||
      !o ||
      !isname(ch->task->orig_arg, o->name)) {
    smythe_stop(ch);
    return FALSE;  // returning FALSE lets command be interpreted
  }
  if(ch->utilityTaskCommand(cmd) || ch->nobrainerTaskCommand(cmd))
    return FALSE;

  switch (cmd) {
  case CMD_TASK_CONTINUE:
    if (!(w = ch->heldInPrimHand())) {
        smythe_stop(ch);
        return FALSE;  // returning FALSE lets command be interpreted
    }
    if (ch->task->status) {
      learning = ch->getSkillValue(SKILL_SMYTHE);
      ch->task->calcNextUpdate(pulse, 2 * PULSE_MOBACT);
      if (bSuccess(ch, learning, SKILL_SMYTHE)) {
	w->smythePulse(ch, o);
      } else if (!(::number(0,1)))
      act("$n examines $p carefully.", FALSE, ch, o, 0, TO_ROOM);
      act("You carefully examine $p.", FALSE, ch, o, 0, TO_CHAR);
      return FALSE;
    } else {
      ch->task->calcNextUpdate(pulse, 2 * PULSE_MOBACT);
      w->smythePulse(ch, o);
      // w may be invalid here
      return FALSE;
    }
  case CMD_ABORT:
  case CMD_STOP:
      act("You stop trying to repair $p.", FALSE, ch, o, 0, TO_CHAR);
      ch->sendTo("Isn't there a professional around here somewhere?\n\r");
      act("$n stops repairing $p.", FALSE, ch, o, 0, TO_ROOM);
      if (ch->task->status > 0) {
        act("You let $p cool down.", FALSE, ch, o, 0, TO_CHAR);
        act("$n lets $p cool down.", FALSE, ch, o, 0, TO_ROOM);
      }
      ch->stopTask();
      break;
  case CMD_TASK_FIGHTING:
      ch->sendTo("You are unable to continue repairing while under attack!\n\r");
      ch->stopTask();
      break;
  default:
      if (cmd < MAX_CMD_LIST)
        warn_busy(ch);
      break;                    // eat the command
  }
  return TRUE;
}
