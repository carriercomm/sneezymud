//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: alignment_deity.cc,v $
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD - All rights reserved, SneezyMUD Coding Team
//      "alignment_deity.cc" - Special procedures for alignment_deity
//
///////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"

// returns DELETE_THIS if deity went boom
int personalize_object(TBeing *deity, TBeing *ch, int virt, int decay)
{
  char buf[256];
  TObj *obj;
  int rc;

  if (!(obj = read_object(virt, VIRTUAL))) {
    vlogf(10, "Error loading obj for personalize_object()");
    return FALSE;
  }
  obj->obj_flags.decay_time = decay;
  sprintf(buf, "This is the personalized object of %s", ch->getName());

  obj->swapToStrung();
  if ((virt == WEAPON_AVENGER1) || 
      (virt == WEAPON_AVENGER2) ||
      (virt == WEAPON_AVENGER3))
    obj->addObjStat(ITEM_NOPURGE);

  // this will make tokens free-rent
  if (virt == DEITY_TOKEN)
    obj->obj_flags.cost = 0;

  delete [] obj->action_description;
  obj->action_description = mud_str_dup(buf);

  if (deity) {
    sprintf(buf, "%s %s", fname(obj->name).c_str(), ch->getName());
    *deity += *obj; 
    rc = deity->doGive(buf);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  } else 
    *ch += *obj;

  if (obj->parent == deity) {
    sprintf(buf, "You can't seem to carry it.  I'll just put it down here.");
    deity->doSay(buf);
    rc = deity->doDrop("", obj);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }
  return FALSE;
}

// returns DELETE_THIS if deity went boom
int resize_personalize_object(TBeing *deity, TBeing *ch, int virt, int decay)
{
  char buf[256];
  TObj *obj;
  int rc;

  if (!(obj = read_object(virt, VIRTUAL))) {
    vlogf(10, "Error loading obj for resize_personalize_object()");
    return FALSE;
  }
  obj->obj_flags.decay_time = decay;
  sprintf(buf, "This is the personalized object of %s", ch->getName());

  // resize
  wearSlotT slot = slot_from_bit(obj->obj_flags.wear_flags);
  double player_perc = (100. * (double) ch->getHeight()) / 70.;

  double current_perc;
  if (race_vol_constants[mapSlotToFile( slot)])   //  A few of them are 0
    current_perc = (100. * (double) obj->getVolume()) / 
                              (double) race_vol_constants[mapSlotToFile( slot)];
  else
    current_perc = 100.;

  double diff = current_perc - player_perc;
  if (diff < 0.)
    diff = -diff;
  diff /= 100.;

  obj->setVolume((int) (player_perc * race_vol_constants[mapSlotToFile( slot)] / 100));
  // end resize

  obj->swapToStrung();
  if ((virt == WEAPON_AVENGER1) || (virt == WEAPON_AVENGER2) ||
      (virt == WEAPON_AVENGER3))
    obj->addObjStat(ITEM_NOPURGE);

  delete [] obj->action_description;
  obj->action_description = mud_str_dup(buf);

  if (deity) {
    sprintf(buf, "%s %s", fname(obj->name).c_str(), ch->getName());
    *deity += *obj; 
    rc = deity->doGive(buf);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  } else 
    *ch += *obj;

  if (obj->parent == deity) {
    sprintf(buf, "You can't seem to carry it.  I'll just put it down here.");
    deity->doSay(buf);
    rc = deity->doDrop("", obj);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }

  return FALSE;
}

// returns DELETE_THIS if deity goes boom
// returns DELETE_VICT if ch dies
static int reward_or_punish(TBeing *deity, TBeing *ch)
{
#if FACTIONS_IN_USE
  int percent;
  char buf[128];
  int rc;

  // I updated this for 4.x - bat
  // perc is gonna be much more free floating
  // pcs need 1.5 perc per level to gain, so lets use that as a baseline
  double d_percent = ch->getPerc();
  d_percent /= 1.5;
  d_percent /= (double) ch->GetMaxLevel();
  // d_perc now represents a baseline value for FP regardless of level

  // throw in a scale
  // i.e. if I have 1.5% FP per level, this equates to an X on a 0-100 scale
  d_percent *= 50.0;   // arbitrary

  percent = (int) d_percent;
  percent = min(max(0, percent), 100);

vlogf(5, "%s had a percent of %d", ch->getName(), percent);
  if (number(10, 90) < percent) {
    sprintf(buf, "%s, you have faithfully practiced your beliefs.", ch->getName());
    deity->doSay(buf);
    deity->doSay("Here is your reward.");
    // default will always do a nice thing, top ten do something great 
    int num = ::number(percent, 100);
vlogf(5, "reward loop val=%d", num);
    switch (num) {
      case 90:
        // Nice token that decays in 100 ticks. 
        deity->doSay("You are a paragon of virtue.");
        if (personalize_object(deity, ch, DEITY_TOKEN, 100) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 91:
        // Nice token that decays in 150 ticks. 
        deity->doSay("You are a paragon of virtue.");
        if (personalize_object(deity, ch, DEITY_TOKEN, 150) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 92:
      case 93:
        // Nice token that decays in 200 ticks and sanc 
        deity->doSay("You are a paragon of virtue.");
        if (!deity->doesKnowSkill(SPELL_SANCTUARY))
          deity->setSkillValue(SPELL_SANCTUARY,120);

        sanctuary(deity,ch);
        if (personalize_object(deity, ch, DEITY_TOKEN, 200) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 94:
      case 95:
        // 200 tick token and sanc and part restore. 
        deity->doSay("You are a paragon of virtue.");
        sprintf(buf, "%s part", ch->getName());
        deity->doRestore(buf);
        if (!deity->doesKnowSkill(SPELL_SANCTUARY))
          deity->setSkillValue(SPELL_SANCTUARY,120);

        sanctuary(deity,ch);
        if (personalize_object(deity, ch, DEITY_TOKEN, 200) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 96:
      case 97:
        // 200 tick token and sanc and full restore. 
        deity->doSay("You are virtually an avatar.");
        sprintf(buf, "%s full", ch->getName());
        deity->doRestore(buf);
        if (!deity->doesKnowSkill(SPELL_SANCTUARY))
          deity->setSkillValue(SPELL_SANCTUARY,120);

        sanctuary(deity,ch);
        if (personalize_object(deity, ch, DEITY_TOKEN, 200) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 98:
        // 250 tick token and sanc and full restore. 
        deity->doSay("You are virtually an avatar.");
        sprintf(buf, "%s full", ch->getName());
        deity->doRestore(buf);
        if (!deity->doesKnowSkill(SPELL_SANCTUARY))
          deity->setSkillValue(SPELL_SANCTUARY,120);

        sanctuary(deity,ch);
        if (personalize_object(deity, ch, DEITY_TOKEN, 250) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 99:
        // 300 tick token and sanc and full restore. 
        deity->doSay("You are virtually an avatar.");
        sprintf(buf, "%s full", ch->getName());
        deity->doRestore(buf);
        if (!deity->doesKnowSkill(SPELL_SANCTUARY))
          deity->setSkillValue(SPELL_SANCTUARY,120);

        sanctuary(deity,ch);
        if (personalize_object(deity, ch, DEITY_TOKEN, 300) == DELETE_THIS)
          return DELETE_THIS;
        break;
      case 100:
        // 500 tick token and sanc and full restore. 
        deity->doSay("You are an avatar of our beliefs.");
        sprintf(buf, "%s full", ch->getName());
        deity->doRestore(buf);
        if (!deity->doesKnowSkill(SPELL_SANCTUARY))
          deity->setSkillValue(SPELL_SANCTUARY,120);

        sanctuary(deity,ch);
        if (personalize_object(deity, ch, DEITY_TOKEN, 500) == DELETE_THIS)
          return DELETE_THIS;
        break;
      default:
        sprintf(buf, "%s part", ch->getName());
        deity->doRestore(buf);
        break;
    }
    deity->doAction(ch->getName(), CMD_PAT);
  } else {
    sprintf(buf, "%s, you have failed to practice what you preach.", ch->getName());
    deity->doSay(buf);
    deity->doSay("Here is your punishment");
    int num = number(0, percent);
vlogf(5, "punishment loop val=%d", num);
    switch (num) {
       // default sucks, but not as bad as the 10 lowest ones. 
      case 0:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        act("$n waves $s arms and utters the words 'jhakki phranc'", FALSE, deity, NULL, NULL, TO_ROOM);
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        if (!deity->doesKnowSkill(SPELL_CURSE))
          deity->setSkillValue(SPELL_CURSE,120);
        curse(deity,ch);
        if (!deity->doesKnowSkill(SPELL_POISON))
          deity->setSkillValue(SPELL_POISON,120);
        poison(deity,ch);
        act("$N seems totally weakened.", FALSE, deity, NULL, ch, TO_NOTVICT);
        act("You feel totally weakened.", FALSE, deity, NULL, ch, TO_VICT);
        ch->setHit(1);
        ch->setMana(1);
        ch->setMove(1);
        deity->doSay("Let this be a lesson to you.");
        break;
      case 1:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        if (!deity->doesKnowSkill(SPELL_CURSE))
          deity->setSkillValue(SPELL_CURSE,120);
        curse(deity,ch);
        if (!deity->doesKnowSkill(SPELL_POISON))
          deity->setSkillValue(SPELL_POISON,120);
        act("$n waves $s arms and utters the words 'jhakki phranc'", FALSE, deity, NULL, NULL, TO_ROOM);
        poison(deity,ch);
        act("$N seems weakened.", FALSE, deity, NULL, ch, TO_NOTVICT);
        act("You feel weakened.", FALSE, deity, NULL, ch, TO_VICT);
        ch->setHit(1);
        ch->setMove(1);
               deity->doSay("Let this be a lesson to you.");
        break;
      case 2:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        act("$n waves $s arms and utters the words 'jhakki phranc'", FALSE, deity, NULL, NULL, TO_ROOM);
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        if (!deity->doesKnowSkill(SPELL_CURSE))
          deity->setSkillValue(SPELL_CURSE,120);
        curse(deity,ch);
        act("$N seems weakened.", FALSE, deity, NULL, ch, TO_NOTVICT);
        act("You feel weakened.", FALSE, deity, NULL, ch, TO_VICT);
        ch->setHit(1);
        deity->doSay("Let this be a lesson to you.");
        break;
      case 3:
      case 4:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);
        act("$n waves $s arms and utters the words 'jhakki phranc'", FALSE, deity, NULL, NULL, TO_ROOM);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        act("$N seems weakened.", FALSE, deity, NULL, ch, TO_NOTVICT);
        act("You feel weakened.", FALSE, deity, NULL, ch, TO_VICT);
        ch->setHit(1);
        deity->doSay("Let this be a lesson to you.");
        break;
      case 5:
      case 6:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        if (!deity->doesKnowSkill(SPELL_CURSE))
          deity->setSkillValue(SPELL_CURSE,120);
        curse(deity,ch);
        if (!deity->doesKnowSkill(SPELL_POISON))
          deity->setSkillValue(SPELL_POISON,120);
        poison(deity,ch);
        deity->doSay("Let this be a lesson to you.");
        break;
      case 7:
      case 8:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        if (!deity->doesKnowSkill(SPELL_POISON))
          deity->setSkillValue(SPELL_POISON,120);
        poison(deity,ch);
        deity->doSay("Let this be a lesson to you.");
        break;
      case 9:
      case 10:
        deity->doSay("You have gone extraordinarily astray from your beliefs.");
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        if (!deity->doesKnowSkill(SPELL_CURSE))
          deity->setSkillValue(SPELL_CURSE,120);
        curse(deity,ch);
        deity->doSay("Let this be a lesson to you.");
        break;
      case 70:                                                                  
      case 71:                                                                  
      case 72:                                                                  
      case 73:                                                                  
      case 74:                                                                  
      case 75:                                                                  
      case 76:                                                                  
      case 77:                                                                  
      case 78:                                                                  
      case 79:                                                                  
      case 80:
      case 81:
      case 82:
      case 83:
      case 84:
      case 85:
      case 86:
      case 87:
      case 88:
      case 89:
        // dont do anything to these guys who just rolled a bad roll. 
        break;
      default:
        if (!deity->doesKnowSkill(SPELL_DISPEL_MAGIC))
          deity->setSkillValue(SPELL_DISPEL_MAGIC,120);

        rc = dispelMagic(deity,ch);
        if (IS_SET_DELETE(rc, DELETE_VICT))
          return DELETE_VICT;

        break;
    }
    deity->doAction(ch->getName(), CMD_FART);
  }
#endif
  return TRUE;
}

static void simple_deity_poof(TMonster *deity, sh_int targ_rm)
{
  if (deity->inRoom() == targ_rm)
    return;

  act("$n disappears in a cloud of mushrooms.", 
          TRUE, deity, NULL, NULL, TO_ROOM);
  --(*deity);
  thing_to_room(deity, targ_rm);
  act("$n appears with an explosion of rose-petals.", 
         TRUE, deity, 0, NULL, TO_ROOM);
}

// These guys step thru the player lists, and either reward a
// player for correct alignment, or punish for deviance. 
int alignment_deity(TBeing *, cmdTypeT cmd, const char *, TMonster *me, TObj *)
{
  TBeing *tmp_ch;
  Descriptor *d, *d2;
  int room;

  if (cmd != CMD_MOB_ALIGN_PULSE)
    return FALSE;

  switch (cmd) {
    case CMD_MOB_ALIGN_PULSE:
      room = me->in_room;

      for (d = descriptor_list; d; d = d2) {
        d2 = d->next;
        if ((tmp_ch = d->character) && !d->connected) {
          if (!number(0, 30)) {
            vlogf(5, "%s in room %d reward/punishing %s",
                me->getName(), room, tmp_ch->getName());
            simple_deity_poof(me, tmp_ch->inRoom());

            int rc = reward_or_punish(me, tmp_ch);
            if (IS_SET_DELETE(rc, DELETE_VICT)) {
              delete tmp_ch;
              tmp_ch = NULL;
            }
            if (IS_SET_DELETE(rc, DELETE_THIS)) {
              vlogf(10, "Bad news in alignment_deity(). BUG BRUTIUS");
              simple_deity_poof(me, room);
              return DELETE_THIS;
            }
          }
        }
      }
      simple_deity_poof(me, room);
      break;
    default:
      break;
  }
  return TRUE;
}
