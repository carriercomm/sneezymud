#include "stdsneezy.h"

void TBeing::doFish(const char *direction){
  TRoom *rp;
  roomDirData *exitp;

  if(!(exitp = exitDir(getDirFromChar(direction))) || !direction){
    rp=roomp;
  } else {
    if(!exitp->to_room || !(rp = real_roomp(exitp->to_room))){
      rp=roomp;      
    }
  }

  if(rp->isUnderwaterSector()){
    sendTo("You can't fish underwater!\n\r");
    return;
  }
  if(!rp->isWaterSector()){
    sendTo("You can't fish on land!\n\r");
    return;
  }


  if(task){
    stopTask();
  }

  sendTo("You start fishing.\n\r");
  start_task(this, NULL, rp, TASK_FISHING, "", 2, inRoom(), 0, 0, 5);
}


TObj *catch_a_fish(TRoom *rp){
  TObj *fish=NULL;
  int nfresh=17, nmarine=24;
  const int freshfishes[]={13800, 13801, 13802, 13803, 13804, 13805, 13806,
			   13807, 13816, 13817, 13818, 13819, 13820, 13821,
			   13822, 13823, 13824};
  const int marinefishes[]={13808, 13809, 13810, 13811, 13812, 13813, 13814,
			    13815, 13825, 13826, 13827, 13828, 13829, 13830,
			    13831, 13832, 13833, 13834, 13835, 13836, 13837,
			    13838, 13839, 13840};
  float weightmod=(((float)(::number(0,100))-50.0)/100.0)+1.0;  // plus or minus 30%

  //  vlogf(LOG_PEEL, "weightmod=%f", weightmod);

  if(!::number(0,100)){
    weightmod=3;  // big one
  }
  

  if(rp->isOceanSector()){
    if(!::number(0,24)){
      fish=read_object(12445, VIRTUAL); // some random crap item
    } else {
      fish=read_object(marinefishes[::number(0,nmarine-1)], VIRTUAL);
      fish->setWeight((float)((float)fish->getWeight()*(float)weightmod));
      fish->setVolume(fish->getWeight()*200);
    }
  } else { // if(rp->isRiverSector()){  // river or pond or lake or whatever
    fish=read_object(freshfishes[::number(0,nfresh-1)], VIRTUAL);
    fish->setWeight(fish->getWeight()*weightmod);
    fish->setVolume(fish->getWeight()*200);
  }

  rp->setFished(rp->getFished()+1);

  return fish;
}


int task_fishing(TBeing *ch, cmdTypeT cmd, const char *, int pulse, TRoom *rp, TObj *)
{
  TTool *bait=NULL;
  TThing *t=NULL, *tpole=NULL;
  char buf[256];
  TObj *fish=NULL, *pole=NULL;
  int baitmax=10000, baitchance=0;
  int polemax=50000, polechance=0;
  int catchchance=0;


  if(ch->utilityTaskCommand(cmd) || ch->nobrainerTaskCommand(cmd))
    return FALSE;

  // basic tasky safechecking
  if (ch->isLinkdead() || (ch->in_room != ch->task->wasInRoom)){
    act("You cease fishing.",
        FALSE, ch, 0, 0, TO_CHAR);
    act("$n stops fishing.",
        TRUE, ch, 0, 0, TO_ROOM);
    ch->stopTask();
    return FALSE; // returning FALSE lets command be interpreted
  }

  // find our bait here
  for(t=ch->stuff;t;t=t->nextThing){
    bait=dynamic_cast<TTool *>(t);

    if(bait){
      if(bait->getToolType() != TOOL_FISHINGBAIT){
	bait=NULL;
      } else {
	break;
      }
    }
  }
  if(!bait){
    ch->sendTo("You need to have some bait to fish.\n\r");
    ch->stopTask();
    return FALSE;
  }
  
  // find our pole here
  if((!(tpole=ch->heldInPrimHand()) && !(tpole = ch->heldInSecHand())) ||
     !isname("fishingpole", tpole->name)){
    ch->sendTo("You need to hold a fishing pole to fish!\n\r");
    ch->stopTask();
    return FALSE;
  }
  if(!(pole=dynamic_cast<TObj *>(tpole))){
    vlogf(LOG_BUG, "Hmm got a fishing pole that isn't a TObj");
    ch->sendTo("You need to hold a fishing pole to fish!\n\r");
    ch->stopTask();
    return FALSE;
  }



  /*
    do generic checks here
   */


  if(rp && !rp->isWaterSector()){
    ch->sendTo("You can't fish on land!\n\r");
    ch->stopTask();
    return FALSE;
  }

  if (ch->task && ch->task->timeLeft < 0){
    ch->sendTo("You pack up and stop fishing.\n\r");
    ch->stopTask();
    return FALSE;
  }


  switch (cmd) {
    case CMD_TASK_CONTINUE:
      ch->task->calcNextUpdate(pulse, PULSE_MOBACT * 5);

      switch (ch->task->timeLeft) {
	case 2:
	  // check for out of bait
	  bait->addToToolUses(-1);
	  if (bait->getToolUses() <= 0) {
	    act("Oops, you're out of bait.",
		FALSE, ch, NULL, 0, TO_CHAR);
	    act("$n looks startled as $e realizes that $e is out of bait.",
		FALSE, ch, NULL, 0, TO_ROOM);
	    ch->stopTask();
	    delete bait;
	    return FALSE;
	  }


	  sprintf(buf, "You bait %s with $p.", pole->shortDescr);
	  act(buf, FALSE, ch, bait, 0, TO_CHAR);

	  sprintf(buf, "$n baits %s with $p.", pole->shortDescr);
          act(buf, TRUE, ch, bait, 0, TO_ROOM);
          ch->task->timeLeft--;
          break;
	case 1:
          act("You cast your line out.",
              FALSE, ch, NULL, 0, TO_CHAR);
	  act("$n casts $s line out.",
              TRUE, ch, NULL, 0, TO_ROOM);
          ch->task->timeLeft--;
          break;
	case 0:
	  baitchance=(int)(((float)((float)(bait->rentCost()*2)/(float)baitmax))*25);
	  polechance=(int)(((float)((float)(pole->rentCost()*2)/(float)polemax))*25);
	  catchchance=::number(1,100);
	  

#if 0
	  vlogf(LOG_PEEL, "fishing: baitcost=%i, bait=%i, pole=%i, catch=%i",
		bait->rentCost(), baitchance, polechance, catchchance);
#endif	
  
	  if(bSuccess(ch, ch->getSkillValue(SKILL_FISHING), SKILL_FISHING) &&
	     (catchchance<(baitchance+polechance)) &&
	     (fish=catch_a_fish(rp)) &&
	     (::number(5,10) > rp->getFished())){
            *ch += *fish;

	    act("You reel in $p!",
		FALSE, ch, fish, 0, TO_CHAR);
	    act("$n reels in $p!",
		TRUE, ch, fish, 0, TO_ROOM);
	  } else {
	    act("You didn't catch anything.",
		FALSE, ch, NULL, 0, TO_CHAR);
	    act("$n doesn't catch anything.",
		TRUE, ch, NULL, 0, TO_ROOM);

	    if(rp->getFished()>10){
	      act("This place seems all fished out.",
		  FALSE, ch, NULL, 0, TO_CHAR);
	    }
	  }
	  ch->stopTask();
          break;
      }
      break;
    case CMD_ABORT:
    case CMD_STOP:
      act("You cease fishing.",
          FALSE, ch, 0, 0, TO_CHAR);
      act("$n stops fishing.",
          TRUE, ch, 0, 0, TO_ROOM);
      ch->stopTask();
      break;
    case CMD_TASK_FIGHTING:
      ch->sendTo("You have not yet mastered the art of fighting while fishing.\n\r");
      ch->stopTask();
      break;
    default:
      if (cmd < MAX_CMD_LIST)
        warn_busy(ch);
      break;                    // eat the command
  }
  return TRUE;
}

