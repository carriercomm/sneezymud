//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: cmd_egotrip.cc,v $
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
//      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
//
//      "egotrip.cc" - The egotrip command
//  
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"
#include "disc_fire.h"
#include "disc_wrath.h"
#include "disc_aegis.h"
#include "disease.h"

void TBeing::doEgoTrip(const char *arg)
{
  if (powerCheck(POWER_EGOTRIP))
    return;

  if (isPc() && !isImmortal()) {
    incorrectCommand();
    return;
  }

  string badsyn = "Syntax: egotrip <\"deity\" | \"bless\" | \"blast\" | \"damn\" | \"hate\" | \"cleanse\">\n\r";

//  char argument[256];
  string argument, sarg = arg, restarg;
  restarg = one_argument(sarg, argument);
  if (!argument.length()) {
    sendTo(badsyn.c_str());
    return;
  }
  if (is_abbrev(argument, "deity")) {
    sendTo("Forcing a deity pulse to occur.\n\r");
    TBeing *ch, *ch2;
    bool found = false;
    for (ch = character_list; ch; ch = ch2) {
      ch2 = ch->next;
      TMonster *tmons = dynamic_cast<TMonster *>(ch);
      if (!tmons)
        continue;
      if (!tmons->spec) 
        continue;
      int rc = tmons->checkSpec(tmons, CMD_MOB_ALIGN_PULSE, "", NULL);
      if (IS_SET_DELETE(rc, DELETE_THIS) || IS_SET_DELETE(rc, DELETE_VICT)) {
        delete tmons;
        tmons = NULL;
      }
      if (rc)
        found = true;
    }
    vlogf(5, "%s egotripped deities", getName());
    if (!found)
      sendTo("No deities in The World.\n\r");
    return;
    // deities
  } else if (is_abbrev(argument, "bless")) {
    if (!isImmortal() || !desc || !IS_SET(desc->autobits, AUTO_SUCCESS)) {
      sendTo("You must be immortal, and have auto-success enabled first.\n\r");
      return;
    }

    vlogf(5, "%s egotripped bless", getName());
    Descriptor *d;
    for (d = descriptor_list; d; d = d->next) {
      if (d->connected != CON_PLYNG)
        continue;
      TBeing *ch = d->character;
      if (!ch)
        continue;
#if 0
// doesn't work if not in room
      act("$N has graciously bestowned upon you $S blessing.",
            FALSE, ch, 0, this, TO_CHAR);
#else
      ch->sendTo("%s has graciously bestowed upon you %s blessing.\n\r",
            good_cap(ch->pers(this)).c_str(), hshr());
#endif
      bless(this, ch);
    }
    return;
  } else if (is_abbrev(argument, "blast")) {
    string target;
    one_argument(restarg, target);
    if (target.empty()) {
      sendTo("Syntax: egotrip blast <target>\n\r");
      return;
    }
    TBeing *ch = get_char_vis_world(this, target.c_str(), NULL, EXACT_NO);
    if (!ch) {
      sendTo("Could not locate character.\n\r");
      sendTo("Syntax: egotrip blast <target>\n\r");
      return;
    }
    vlogf(5, "%s egotrip blasted %s", getName(), ch->getName());
    if (ch->isPc() && ch->isImmortal() &&
        ch->GetMaxLevel() > GetMaxLevel()) {
      sendTo("Shame Shame, you shouldn't do that.\n\r");
      return;
    }
    act("You blast $N with a bolt of lightning.",
         FALSE, this, 0, ch, TO_CHAR);

    act("A bolt of lightning streaks down from the heavens right at your feet!",
         FALSE, ch, 0, 0, TO_CHAR);
    act("BZZZZZaaaaaappppp!!!!!",
         FALSE, ch, 0, 0, TO_CHAR);
    act("A bolt of lightning streaks down from the heavens right at $n's feet!",
         FALSE, ch, 0, 0, TO_ROOM);

    string worldBuf = "You smell burnt flesh as a bolt of lightning takes the hide off of ";
    worldBuf += ch->getName();
    worldBuf += "!\n\r";
    descriptor_list->worldSend(worldBuf.c_str(), this);

    soundNumT snd = pickRandSound(SOUND_EGOBLAST_1, SOUND_EGOBLAST_2);
    ch->roomp->playsound(snd, SOUND_TYPE_NOISE);

    // this just nails um, but shouldn't actually kill them
    if (ch->reconcileDamage(ch, ch->getHit()/2, DAMAGE_ELECTRIC) == -1) {
      delete ch;
      ch = NULL;
      return;
    }
    ch->setMove(ch->getMove()/2);
    
    return;
  } else if (is_abbrev(argument, "damn")) {
    if (!isImmortal() || !desc || !IS_SET(desc->autobits, AUTO_SUCCESS)) {
      sendTo("You must be immortal, and have auto-success enabled first.\n\r");
      return;
    }

    string target;
    one_argument(restarg, target);
    if (target.empty()) {
      sendTo("Syntax: egotrip damn <target>\n\r");
      return;
    }
    TBeing *ch = get_char_vis_world(this, target.c_str(), NULL, EXACT_NO);
    if (!ch) {
      sendTo("Could not locate character.\n\r");
      sendTo("Syntax: egotrip damn <target>\n\r");
      return;
    }
    
    vlogf(5, "%s egotrip damned %s", getName(), ch->getName());
    if (ch->isPc() && ch->isImmortal() &&
        ch->GetMaxLevel() > GetMaxLevel()) {
      sendTo("Shame Shame, you shouldn't do that.\n\r");
      return;
    }
    // at the person, cast the spell.
    TRoom *rp_o = roomp;
    if (ch != this) {
      --(*this);
      *ch->roomp += *this;
    }

    castFaerieFire(this, ch);
    curse(this, ch);

    if (ch != this) {
      --(*this);
      *rp_o += *this;
    }

    return;
  } else if (is_abbrev(argument, "hate")) {
    string target;
    one_argument(restarg, target);
    if (target.empty()) {
      sendTo("Syntax: egotrip hate <target>\n\r");
      return;
    }
    TBeing *ch = get_char_vis_world(this, target.c_str(), NULL, EXACT_NO);
    if (!ch) {
      sendTo("Could not locate character.\n\r");
      sendTo("Syntax: egotrip hate <target>\n\r");
      return;
    }
    
    TThing *t;
    for (t = roomp->stuff; t; t = t->nextThing) {
      TMonster *tmon = dynamic_cast<TMonster *>(t);
      if (!tmon)
        continue;
      if (tmon->Hates(ch, NULL))
        continue;
      tmon->addHated(ch);
      act("$N now hates $p.", false, this, ch, tmon, TO_CHAR);
    }
    return;
  } else if (is_abbrev(argument, "cleanse")) {
    if (!isImmortal() || !desc) {
      sendTo("You must be immortal to do this.\n\r");
      return;
    }

    vlogf(5, "%s egotripped cleanse", getName());

    TBeing       *tBeing;
    affectedData *tAff = NULL,
                 *tAffLast;

    for (tBeing = character_list; tBeing; tBeing = tBeing->next) {
      if (!tBeing->name) {
        vlogf(10, "Something with NULL name in world being list.");
        continue;
      }

      tAffLast = NULL;

      for (tAff = tBeing->affected; tAff;) {
        if (tAff->type != AFFECT_DISEASE) {
          tAffLast = tAff;
          tAff = tAff->next;
          continue;
        }

        tBeing->sendTo("%s has cured your %s.\n\r",
                       good_cap(getName()).c_str(),
                       DiseaseInfo[DISEASE_INDEX(tAff->modifier)].name);

        if (!strcmp("Lapsos", getName()))
          sendTo(COLOR_BASIC, "Your cure %s of: %s.\n\r",
                 tBeing->getName(),
                 DiseaseInfo[DISEASE_INDEX(tAff->modifier)].name);

        if (tAff->modifier == DISEASE_POISON) {
          tBeing->affectFrom(SPELL_POISON);
          tBeing->affectFrom(SPELL_POISON_DEIKHAN);
        }

        tBeing->diseaseStop(tAff);
        tBeing->affectRemove(tAff);

        if (tBeing->isPc() && tBeing->desc)
          tBeing->doSave(SILENT_YES);

        if (!tAffLast)
          tAff = tBeing->affected;
        else
          tAff = tAffLast;

        if (!tAff)
          break;
      }
    }

    return;
  } else {
    sendTo(badsyn.c_str());
    return;
  }
}
