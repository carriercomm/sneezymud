//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: task_rest.cc,v $
// Revision 5.5  2001/07/05 21:25:54  peel
// Trying to fix cvs
// what a headache
//
// Revision 5.4  2001/06/20 04:27:24  jesus
// couple updates for lifeforce and farlook fix for stuff they shouldnt see
// will need review
//
// Revision 5.3  2001/06/09 07:35:45  jesus
// minor updates for shaman
//
// Revision 5.2  2001/06/03 07:58:14  jesus
// temporary fix to an annoying -hp bug with shaman
//
// Revision 5.1.1.2  2001/04/01 07:03:27  jesus
// shaman regen
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

int task_rest(TBeing *ch, cmdTypeT cmd, const char *arg, int pulse, TRoom *, TObj *)
{
  if (ch->isLinkdead() || (ch->getPosition() != POSITION_RESTING)) {
    ch->stopTask();
    return FALSE;
  }
  if (ch->utilityTaskCommand(cmd))
    return FALSE;
  switch(cmd) {
  case CMD_TASK_CONTINUE:
    // v3.1 : this was 2*regenTime
      ch->task->calcNextUpdate(pulse, 4 * ch->regenTime());
      if (!ch->task->status) {
        if (!ch->roomp->isRoomFlag(ROOM_NO_HEAL)) {
          ch->addToMana(1);
          ch->addToPiety(.10);
	  if (ch->hasClass(CLASS_SHAMAN) && !ch->affectedBySpell(SPELL_SHAPESHIFT)) {
	    if (1 > ch->getLifeforce()) {
	      ch->updateHalfTickStuff();
	    } else {
	      ch->addToLifeforce(-1);
	      ch->sendTo("Your lack of activity drains your precious lifeforce.\n\r");
	    }
	  } else {
	    ch->addToHit(1);
	  }
          if (ch->getMove() < ch->moveLimit())
            ch->addToMove(1);
          if (ch->desc && ch->ansi()) {
            ch->desc->updateScreenAnsi(CHANGED_HP);
            ch->desc->updateScreenAnsi(CHANGED_MANA);
            ch->desc->updateScreenAnsi(CHANGED_LIFEFORCE);
            ch->desc->updateScreenAnsi(CHANGED_PIETY);
            ch->desc->updateScreenAnsi(CHANGED_MOVE);
          } else if (ch->desc && ch->vt100()) {
            ch->desc->updateScreenVt100(CHANGED_HP);
            ch->desc->updateScreenVt100(CHANGED_PIETY);
            ch->desc->updateScreenVt100(CHANGED_MANA);
            ch->desc->updateScreenVt100(CHANGED_LIFEFORCE);
            ch->desc->updateScreenVt100(CHANGED_MOVE);
          }
        }
      }
      ch->updatePos();
      ch->task->status = 0;
      break;
  case CMD_ABORT:
  case CMD_STOP:
  case CMD_STAND:
      ch->stopTask();
      ch->doStand();
      break;
  case CMD_SLEEP:
      ch->stopTask();
      ch->doSleep(arg);
      break;
  case CMD_SIT:
      ch->stopTask();
      ch->doSit(arg);
      break;
  case CMD_REST:
      ch->sendTo("You are already nice and comfy.\n\r");
      break;
  case CMD_TASK_FIGHTING:
      ch->sendTo("You are unable to rest while under attack!\n\r");
      ch->cantHit += ch->loseRound(1);
      if (!::number(0,2))
        ch->cantHit += ch->loseRound(1);
      ch->stopTask();
      break;
  default:
      if (cmd < MAX_CMD_LIST)
        return FALSE;           // process command
      break;                    // eat the command
  }
  return TRUE;
}
