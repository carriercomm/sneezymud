/*****************************************************************************

  SneezyMUD++ - All rights reserved, SneezyMUD Coding Team.

  "cmd_save.cc"
  Primary function and rountines used by the save command.

  Created 5/17/?? - ??(??)

******************************************************************************/

#include "stdsneezy.h"

void doSaveMOEdit(TBeing *ch, const char *tArg)
{
  string  tStThing(""),
          tStValue(""),
          tStArg(tArg);
  int     tValue;
  bool    saveMethod = false;
  char    tString[256];
  TThing *tThing;

  tStArg = two_arg(tStArg, tStThing, tStValue);

  if (!ch->isImmortal() || !ch->desc || !ch->isPc())
    return;

  if (!ch->hasWizPower(POWER_MEDIT) &&
      !ch->hasWizPower(POWER_OEDIT)) {
    ch->sendTo("You cannot use this, go away little one.\n\r");
    return;
  }

  if (!ch->roomp) {
    vlogf(LOG_BUG, "Player doing save without a room!  [%s]", ch->getName());
    return;
  }

  if (!(tThing = searchLinkedList(tStThing, ch->roomp->stuff)) &&
      !(tThing = searchLinkedList(tStThing, ch->stuff))) {
    ch->sendTo("I don't see that here, do you?\n\r");
    return;
  }

  if (dynamic_cast<TBeing *>(tThing) &&
      (dynamic_cast<TBeing *>(tThing))->isPc()) {
    ch->sendTo("Kinky.  Did you buy them dinner first?\n\r");
    (dynamic_cast<TBeing *>(tThing))->sendTo("%s just tried to save you for later use!\n\r",
                                             ch->getName());
    return;
  }

  if (dynamic_cast<TMonster *>(tThing))
    saveMethod = false;
  else if (dynamic_cast<TObj *>(tThing))
    saveMethod = true;
  else {
    ch->sendTo("You cannot save that, go away little one.\n\r");
    return;
  }

  if (tStValue.empty()) {
    if (!ch->hasWizPower(POWER_WIZARD)) {
      ch->sendTo("Syntax: save <thing> <vnum>\n\r");
      return;
    }

    if (!saveMethod)
      tValue = (dynamic_cast<TMonster *>(tThing))->mobVnum();
    else
      tValue = (dynamic_cast<TObj *>(tThing))->objVnum();
  } else
    tValue = atoi(tStValue.c_str());

  sprintf(tString, "save %s %d", tStThing.c_str(), tValue);

  if (!saveMethod) {
    if (ch->hasWizPower(POWER_MEDIT))
      ch->doMedit(tString);
    else
      ch->sendTo("You do not have access to medit.  Therefore you cannot save mobiles.\n\r");
  } else {
    if (ch->hasWizPower(POWER_OEDIT))
      ch->doOEdit(tString);
    else
      ch->sendTo("You do not have access to oedit.  Therefore you cannot save objects.\n\r");
  }
}

void TBeing::doSave(silentTypeT silent, const char *tArg = NULL)
{
  objCost  tCost;

  verifyWeightVolume();

  if (!isPc())
    return;

  if (!desc) {
    vlogf(LOG_BUG, "%s tried to doSave while link dead.", getName());
    return;
  }

  if (isImmortal()) {
    if (!tArg || !*tArg) {
      wizFileSave();

      if (!IS_SET(desc->account->flags, ACCOUNT_IMMORTAL)) {
        // log the account as an immortal one.
        SET_BIT(desc->account->flags, ACCOUNT_IMMORTAL);
        desc->saveAccount();
      }
    } else {
      doSaveMOEdit(this, tArg);
      return;
    }
  }

  if (!silent)
    sendTo("Saving.\n\r");

  if (dynamic_cast<TMonster *>(this) && IS_SET(specials.act, ACT_POLYSELF)) {
    TPerson *tPerson = desc->original;
    if (!tPerson) {
      vlogf(LOG_BUG, "BAD SAVE OF POLY!");
      return;
    }

    // for a poly, we want to swap all the gear that is on the poly back
    // to the original char, save the person in that state, and then swap
    // it all back.  Fortunately, the original person shouldn't have anything
    // on them, so we can pretty much blindly dump back and forth.
    TThing * t;
    while ((t = stuff)) {
      --(*t);
      *tPerson += *t;
    }

    wearSlotT wearIndex;
    for (wearIndex = MIN_WEAR; wearIndex < MAX_WEAR; wearIndex++) {
      if (equipment[wearIndex]) {
        TThing * obj = unequip(wearIndex);
        tPerson->equipChar(obj, wearIndex, SILENT_YES);
      }
    }

    tPerson->setExp(getExp());
    tPerson->setMoney(getMoney());
    tPerson->classSpecificStuff();
    tPerson->recepOffer(NULL, &tCost);
    tPerson->saveRent(&tCost, FALSE, 0);

    saveChar(ROOM_AUTO_RENT);

    // now that we've saved, put all equipment back on the poly
    while ((t = tPerson->stuff)) {
      --(*t);
      *this += *t;
    }

    wearSlotT wearIndex;
    for (wearIndex = MIN_WEAR; wearIndex < MAX_WEAR; wearIndex++) {
      if (tPerson->equipment[wearIndex]) {
        TThing * obj = tPerson->unequip(wearIndex);
        equipChar(obj, wearIndex, SILENT_YES);
      }
    }

    return;
  } else {
    classSpecificStuff();
    recepOffer(NULL, &tCost);
    dynamic_cast<TPerson *>(this)->saveRent(&tCost, FALSE, 0);

    saveChar(ROOM_AUTO_RENT);
  }
}
