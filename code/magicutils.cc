// magicutils.cc

#include "stdsneezy.h"
#include "disease.h"
#include "components.h"

void TMonster::balanceMakeNPCLikePC()
{
  // This is predicated on balance ideas
  // If a player buys a pet/charm/etc, that pet has stats of an NPC
  // which for the most part are very different from a PC
  // e.g. 2*HP, 1/2*damage, etc
  // If they use this mob as a tank (legitimate) then things will be
  // very skewed.
  // PC damage hitting behing NPC HP tank
  // to get around this, tweak the NPC so that it resembles a "typical" PC

  // conversion factor of 1.75 to 0.9091
  float conv_num = 1.0 / (1.1 * 1.75);

  // modify HP
  float hpl = getHPLevel();
  hpl *= conv_num;
  setHPLevel(hpl);
  setHPFromHPLevel();

  // modify damage capacity
  float daml = getDamLevel();
  daml /= (conv_num * 1.5);
  setDamLevel(daml);

  // leave AC alone, should be OK
}

void SwitchStuff(TBeing *giver, TBeing *taker)
{
  TThing *t, *next;

  mud_assert(giver != NULL, "Something bogus in SwitchStuff()");
  mud_assert(taker != NULL, "Something bogus in SwitchStuff()");

  wearSlotT ij;
  for (ij = MIN_WEAR; ij < MAX_WEAR; ij++) {
    if (giver->equipment[ij]) {
      *taker += *giver->unequip(ij);
    }
  }

  for (t = giver->stuff; t; t = next) {
    next = t->nextThing;
    --(*t); 
    *taker += *t;
  }
  taker->setMoney(giver->getMoney());

  if (taker->getHit() > giver->getHit())
    taker->setHit(giver->getHit());

  taker->setExp(giver->getExp());

  if (dynamic_cast<TMonster *>(taker)) {
    taker->setClass(giver->getClass());

    classIndT cit;
    for (cit = MIN_CLASS_IND; cit < MAX_CLASSES; cit++)
      taker->setLevel(cit, giver->getLevel(cit));
    taker->calcMaxLevel();

    // transfer skills and disciplines
    delete taker->discs;
    taker->discs = giver->discs;
  }
  if (dynamic_cast<TMonster *>(giver))
    giver->discs = NULL;

  taker->setPiety(giver->getPiety());
  taker->setMana(giver->getMana());
  taker->setMove(giver->getMove());
//  taker->setMaxMove(giver->getMaxMove());   

  statTypeT iStat;
  for (iStat=MIN_STAT;iStat<MAX_STATS;iStat++) {
    taker->setStat(STAT_CHOSEN, iStat, giver->getStat(STAT_CHOSEN, iStat));
  }

  taker->setFaction(giver->getFaction());
#if FACTIONS_IN_USE
  taker->setPerc(giver->getPerc());
  for (factionTypeT i = MIN_FACTION; i < MAX_FACTIONS; i++)
    taker->setPercX(giver->getPercX(i),i);
#endif
  taker->setFactAct(giver->getFactAct());
  taker->setCaptive(giver->getCaptive());
  taker->setNextCaptive(giver->getNextCaptive());
  taker->setCaptiveOf(giver->getCaptiveOf());

  taker->age_mod = giver->age_mod;
  taker->desc = giver->desc;
  taker->player.time = giver->player.time;
}

int TBeing::doWearOffReturn()
{
  TBeing *mob = NULL, *per = NULL;
  TRoom *rp = NULL;

  if (!desc || !desc->original) {
    sendTo("What are you trying to return from?!\n\r");
    return FALSE;
  } else {
    sendTo("You return to your original body.\n\r");

    if ((specials.act & ACT_POLYSELF)) {
      mob = this;
      per = desc->original;

      act("$n turns liquid, and reforms as $N.", TRUE, mob, 0, per, TO_ROOM);

      --(*per);
      if (mob->roomp)
        *mob->roomp += *per;
      else {
        rp = real_roomp(ROOM_CS);
        *rp += *per;
      }

      SwitchStuff(mob, per);
      per->polyed = POLY_TYPE_NONE;
      per->affectFrom(SPELL_POLYMORPH);
      per->affectFrom(SKILL_DISGUISE);
      per->affectFrom(SPELL_SHAPESHIFT);
    }
    desc->character = desc->original;
    desc->original = NULL;

    desc->character->desc = desc;
    desc = NULL;


    if (IS_SET(specials.act, ACT_POLYSELF)) {
      return DELETE_THIS;
    }
  }
  return FALSE;
}
void DisguiseStuff(TBeing *giver, TBeing *taker)
{
  // do the generic polymorph stuff first */
  SwitchStuff(giver, taker);

  taker->setHit(giver->getHit());
  taker->setMaxHit(giver->hitLimit());
  taker->setMove(giver->getMove());
  taker->setMaxMove(giver->getMaxMove());
  taker->setMana(giver->getMana());
  taker->setMaxMana(giver->manaLimit());
}

void TMonster::failCharm(TBeing *ch)
{
  sendTo("You feel charmed, but the feeling fades.\n\r");
  setCharFighting(ch);
}

void TPerson::failCharm(TBeing *)
{
  sendTo("You feel charmed, but the feeling fades.\n\r");
}


void TBeing::failSleep(TBeing *ch)
{
  sendTo("You feel sleepy for a moment, but then you recover.\n\r");
  if (dynamic_cast<TMonster *>(this))
    if (Sleep && !fight() && (getPosition() > POSITION_SLEEPING))
      setCharFighting(ch);
}


void TBeing::failPara(TBeing *ch)
{
  sendTo("You feel frozen for a moment, but then you recover.\n\r");
  if (dynamic_cast<TMonster *>(this))
    if (!fight())
      setCharFighting(ch);
}

void TBeing::failCalm(TBeing *ch)
{
  sendTo("You feel happy and easy-going, but the effect soon fades.\n\r");
  if (dynamic_cast<TMonster *>(this))
    if (!fight())
      setCharFighting(ch);
}

void TBeing::spellWearOffSoon(spellNumT s)
{
  if (s == AFFECT_TRANSFORMED_HANDS ||
      s == AFFECT_TRANSFORMED_ARMS ||
      s == AFFECT_TRANSFORMED_LEGS ||
      s == AFFECT_TRANSFORMED_NECK ||
      s == AFFECT_TRANSFORMED_HEAD)
    s = SKILL_TRANSFORM_LIMB;

  if (s < MIN_SPELL || s >= MAX_SKILL || !discArray[s])
    return;

  if (discArray[s]->fadeAwaySoon) 
    sendTo("%s\n\r", discArray[s]->fadeAwaySoon);
  
  if (discArray[s]->fadeAwaySoonRoom)
    act(discArray[s]->fadeAwaySoonRoom, TRUE, this, 0, 0, TO_ROOM);
}

// return DELETE_THIS
int TBeing::spellWearOff(spellNumT s, safeTypeT safe)
{

  // Arguably, we should see effects falling off due to death, but it
  // looks real dumb on mobs...
//  if (!isPc() && getPosition() == POSITION_DEAD)
  if (!isPc() && safe)
    return FALSE;

  int rc;

  if (s == AFFECT_TRANSFORMED_HANDS ||
      s == AFFECT_TRANSFORMED_ARMS ||
      s == AFFECT_TRANSFORMED_LEGS ||
      s == AFFECT_TRANSFORMED_NECK ||
      s == AFFECT_TRANSFORMED_HEAD)
    s = SKILL_TRANSFORM_LIMB;


  if (s < MIN_SPELL || s >= MAX_SKILL || !discArray[s])
    return 0;

  if (discArray[s]->fadeAway) 
    sendTo("%s\n\r", discArray[s]->fadeAway);
  
  if (discArray[s]->fadeAwayRoom)
    act(discArray[s]->fadeAwayRoom, TRUE, this, 0, 0, TO_ROOM);

  if (s == SPELL_ENSORCER ||
      s == SPELL_CONJURE_AIR ||
      s == SPELL_CONJURE_EARTH ||
      s == SPELL_CONJURE_FIRE ||
      s == SPELL_CONJURE_WATER ||
      s == SPELL_CREATE_GOLEM ||
      s == SPELL_CONTROL_UNDEAD ||
      s == SPELL_VOODOO ||
      s == SPELL_CACAODEMON ||
      s == SPELL_RESURRECTION ||
      s == SPELL_DANCING_BONES) {
    rc = checkDecharm(FORCE_NO, safe);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }

  if (s == SPELL_GILLS_OF_FLESH) {
    // safe = already dead probably, so lets skip "panic your drowning" and
    // all the rest of what checkDrowning does too...
    if (!safe) {
      rc = checkDrowning();
      if (IS_SET_DELETE(rc, DELETE_THIS))
        return DELETE_THIS;
    }
  }

  return FALSE;
}

// returns DELETE_THIS
int TBeing::checkDecharm(forceTypeT force, safeTypeT safe)
{
  int j, rc = 0, mastLevel = 1;
  bool release = FALSE;
  TBeing *m;

  if (!(m = master) && !force) {  
    return FALSE;
  } else if (m) {
    mastLevel = m->GetMaxLevel();
  }
  mastLevel = 0 - mastLevel;

  stopFollower(FALSE);	// stop following the master 

  if (!isPc()) {
    int  mVn         = mobVnum();
    bool isElemental = (mVn == FIRE_ELEMENTAL  || mVn == WATER_ELEMENTAL ||
                        mVn == EARTH_ELEMENTAL || mVn == AIR_ELEMENTAL);
    TMonster *tMon = dynamic_cast<TMonster *>(this);
    if (force) {
      release = TRUE;
    } else if (isElemental && tMon && (::number(mastLevel, tMon->anger()) <= 0)) {
       release = TRUE;
    }
    if (!stuff) {
      for (j = MIN_WEAR; j < MAX_WEAR; j++) {
        if (equipment[j]) {
          release = FALSE;
          break;
        }
      }
    }
    if (!isElemental) {
      if (m) {
        act("$n looks elated as you release $m into the world.",
            TRUE, this, NULL, m, TO_VICT);
        act("$n looks elated as $e is released into the world.",
            TRUE, this, NULL, m, TO_NOTVICT);
        act("You feel free as your master releases you into the world.",
            TRUE, this, NULL, m, TO_CHAR);
      } else {
        act("$n is released into the world.",
            TRUE, this, NULL, NULL, TO_ROOM);
        act("You are released into the world.",
            TRUE, this, NULL, NULL, TO_CHAR);
      }
      return FALSE;
    }
    if (release && !safe) {
      if (!m || !::number(0,1)) {
        act("$n breaks free of the mortal plane and returns home.",
            TRUE, this, NULL, NULL, TO_ROOM);
        act("You break free of the mortal plane and return home.",
            TRUE, this, NULL, NULL, TO_CHAR);
      } else {
        act("$n growls at you then slowly fades away.",
            FALSE, this, NULL, m, TO_VICT);
        act("$n growls at $N then slowly fades away.",
            FALSE, this, NULL, m, TO_NOTVICT);
        act("You growl at $n then goes back home.",
            FALSE, this, NULL, m, TO_CHAR);
      }
      return DELETE_THIS;
    }

    REMOVE_BIT(specials.act, ACT_SENTINEL);
    dynamic_cast<TMonster *>(this)->addFeared(m);
  }
  if (!safe) {
    rc = doFlee("");
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }
  return FALSE;
}

TComponent *comp_from_object(TThing *item, spellNumT spell)
{
  TThing *t;
  TComponent *ret = NULL;

  item->findComp(&ret, spell);

  TOpenContainer *trc = dynamic_cast<TOpenContainer *>(item);
  if (trc && trc->isClosed())
    return ret;

  // item is wrong component or spellbag 
  for (t = item->stuff; t; t = t->nextThing) 
    t->findComp(&ret, spell);
  
  return ret;
}

// This only returns components that are for spell-casting
TComponent *TBeing::findComponent(spellNumT spell) const
{
  TThing *primary, *secondary, *belt, *juju, *inventory;
  TComponent *item;
  wizardryLevelT wizlevel = WIZ_LEV_NONE;

  primary = heldInPrimHand();
  secondary = heldInSecHand();
  belt = equipment[WEAR_WAISTE];
  juju = equipment[WEAR_NECK];
  inventory = stuff;
  item = NULL;

// Let rangers have components anywhere if not fighting
  if (hasClass(CLASS_RANGER)) {
    if (fight())
      wizlevel = WIZ_LEV_COMP_EITHER;
    else 
      wizlevel = WIZ_LEV_COMP_BELT;
  } else {
    wizlevel = getWizardryLevel();
  }

  if (isPc()) {
    if (wizlevel <= WIZ_LEV_COMP_PRIM_OTHER_FREE) {
      if (primary)
	return comp_from_object(primary, spell);
      else
	return NULL;
    }
    if (wizlevel <= WIZ_LEV_COMP_EITHER) {
      if (primary || secondary) {
        if (primary)
	  item = comp_from_object(primary, spell);
        if (!item && secondary)
          item = comp_from_object(secondary, spell);
        return item;
      } else
	return NULL;
    }
    if (wizlevel <= WIZ_LEV_NO_MANTRA) {
      if (primary || secondary || inventory) {
	if (primary)
	  item = comp_from_object(primary, spell);
	if (!item && secondary)
	  item = comp_from_object(secondary, spell);
        if (!item && inventory) {
          TThing *t;
          for (t = stuff; t && !item; t = t->nextThing) {
            inventory = dynamic_cast<TObj *>(t);
            if (inventory)
              item = comp_from_object(inventory, spell);
          }
        }
	return item;
      } else
	return NULL;
    }
  }
  if (primary || secondary || belt || juju || inventory) {
    if (primary)
      item = comp_from_object(primary, spell);
    if (!item && secondary)
      item = comp_from_object(secondary, spell);
    if (!item && belt)
      item = comp_from_object(belt, spell);
    if (!item && juju)
      item = comp_from_object(juju, spell);
    if (!item && inventory) {
      TThing *t;
      for (t = stuff; t && !item; t = t->nextThing) {
        inventory = dynamic_cast<TObj *>(t);
        if (inventory)
          item = comp_from_object(inventory, spell);
      }
    }
    return item;
  } else
    return NULL;
}

static void missingComponent(const TBeing * ch)
{
  if (ch->hasClass(CLASS_RANGER)) {
    ch->sendTo("You seem to lack the proper materials to complete this magic skill.\n\r");
  } else {
    ch->sendTo("You seem to lack the proper materials to complete this spell.\n\r");
    act("$n kicks $mself as $e realizes $e just screwed up $s spell.",
      TRUE, ch, 0, 0, TO_ROOM);
  }
}

int TBeing::useComponent(TComponent *o, TBeing *vict, checkOnlyT checkOnly)
{
  unsigned int i;

  if (isImmortal())
    return TRUE;

  // Until a better solution, mobs dont need components. - Russ 
  if (!isPc())
    return TRUE;

// has already used component
  if (spelltask && spelltask->component) 
    return TRUE;

  if (!o) {
    missingComponent(this);
    return FALSE;
  }
  if (checkOnly) 
    return TRUE;
     
  for (i=0; (i<CompInfo.size()) && (o->getComponentSpell() != CompInfo[i].spell_num);i++);

  if (i>= CompInfo.size()) {
    vlogf(LOG_BUG,"useComponent had problem finding component for %s",
        o->getName());
    sendTo("Uh oh, something bogus happened.\n\r");
    return FALSE;
  }
  if (o->isPersonalized() && !isname(getName(), o->name)) {
    vlogf(LOG_MISC, "Mage %s using component %s that was personalized but not theirs!!! Reprimand at once.", getName(), o->name); 
    sendTo("You can't use a component that is personalized for someone else!");
    return FALSE;
  }
  if (vict && vict != this) {
    if (*CompInfo[i].to_caster && 
        *CompInfo[i].to_vict &&
        *CompInfo[i].to_other) {
      act(CompInfo[i].to_caster, TRUE, this, o, vict, TO_CHAR);
      act(CompInfo[i].to_vict, TRUE, this, o, vict, TO_VICT);
      act(CompInfo[i].to_other, TRUE, this, o, vict, TO_NOTVICT);
    } else if (*CompInfo[i].to_self && 
               *CompInfo[i].to_room) {
      act(CompInfo[i].to_self, TRUE, this, o, 0, TO_CHAR);
      act(CompInfo[i].to_room, TRUE, this, o, 0, TO_ROOM);
    } else {
      vlogf(LOG_BUG, "Bad component string.  component %d  (1)", i);
    }
  } else {
    if (*CompInfo[i].to_self && 
        *CompInfo[i].to_room) {
      act(CompInfo[i].to_self, TRUE, this, o, 0, TO_CHAR);
      act(CompInfo[i].to_room, TRUE, this, o, 0, TO_ROOM);
    } else {
      vlogf(LOG_BUG, "Bad component string.  component %d  (2)", i);
    }
  }

  // use up one charge
  if (o->getComponentCharges() > 1)
    o->addToComponentCharges(-1);
  else {
    // this should destroy it even if it is inside a spellbag
    act("$p is all used up and you discard it as worthless.",
      TRUE, this, o, 0, TO_CHAR);
    delete o;
    o = NULL;
  }
// Set it so that for chanting purposes the component has been used
  if (spelltask)
    spelltask->component = TRUE;

  return 1;
}

int TBeing::useComponentObj(TComponent *o, TObj *targ, checkOnlyT checkOnly)
{
  unsigned int i;

  if (isImmortal())
    return TRUE;

  // Until a better solution, mobs dont need components. - Russ
  if (!isPc())
    return TRUE;

// has already used component
  if (spelltask && spelltask->component) {
    return TRUE;
  }

  if (!o) {
    missingComponent(this);
    return FALSE;
  }
  if (checkOnly) {
    return TRUE;
  }

  for (i=0; (i<CompInfo.size()) && (o->getComponentSpell() != CompInfo[i].spell_num);i++);

  if (i>= CompInfo.size()) {
    vlogf(LOG_BUG,"useComponent had problem finding component for %s",
        o->shortDescr);
    sendTo("Uh oh, something bogus happened.\n\r");
    return FALSE;
  }

  if (targ) {
    if (*CompInfo[i].to_self_object && 
        *CompInfo[i].to_room_object) {
      act(CompInfo[i].to_self_object, TRUE, this, o, targ, TO_CHAR);
      act(CompInfo[i].to_room_object, TRUE, this, o, targ, TO_NOTVICT);
    } else if (*CompInfo[i].to_caster && 
               *CompInfo[i].to_other) {
      act(CompInfo[i].to_caster, TRUE, this, o, targ, TO_CHAR);
      act(CompInfo[i].to_other, TRUE, this, o, targ, TO_ROOM);
    } else {
      vlogf(LOG_BUG, "Bad component string.  component %d  (3)", i);
    }
  } else {
    vlogf(LOG_BUG, "Bad component string.  component %d  (4)", i);
  }

  if (o->getComponentCharges() > 1)
    o->addToComponentCharges(-1);
  else {
    // this should destroy it even if it is inside a spellbag
    act("$p is all used up and you discard it as worthless.",
      TRUE, this, o, 0, TO_CHAR);
    delete o;
    o = NULL;
  }
// Set it so that for chanting purposes the component has been used
  if (spelltask) 
    spelltask->component = TRUE;
  
  return 1;
}

TOpal *find_biggest_charged_powerstone(TBeing *ch)
{
  TOpal *stone = NULL;
  TThing *t;
  int i;

  // Check through char's equipment 
  for (i = MIN_WEAR; i < MAX_WEAR; i++) {
    if (!(t = ch->equipment[i]))
      continue;

    t->powerstoneCheckCharged(&stone);
  }
  // Check through char's inventory 
  for (t = ch->stuff; t; t = t->nextThing) {
    t->powerstoneCheckCharged(&stone);
  }
  return stone;
}

int TBeing::mostPowerstoneMana() const
{
  TThing *t;
  int i, most = 0;

  // Check through char's equipment
  for (i = MIN_WEAR; i < MAX_WEAR; i++) {
    if (!(t = equipment[i]))
      continue;

    t->powerstoneMostMana(&most);
  }
  // Check through char's inventory
  for (t = stuff; t; t = t->nextThing) {
    t->powerstoneMostMana(&most);
  }
  return most;
}

const char *describe_level(int n)
{
  if (n < 10)
    return "very low level";
  else if (n < 20)
    return "low level";
  else if (n < 40)
    return "medium level";
  else if (n < 50)
    return "high level";
  else if (n < 60)
    return "very high level";
  else
    return "extremely high level";
}

const char *describe_damage(int n, const TBeing *tBeing)
{
#if 1
  if (!tBeing)
    return "a perfect amount";

  int tDiff = n - tBeing->GetMaxLevel();

  if (tDiff < -20)
    return "a horrid amount";
  else if (tDiff < -15)
    return "a sad amount";
  else if (tDiff < -10)
    return "a pathetic amount";
  else if (tDiff < -5)
    return "a decent amount";
  else if (tDiff <= -1)
    return "a near perfect amount";
  else if (tDiff == 0)
    return "a Perfect amount";
  else if (tDiff <= 2)
    return "a near perfect amount"; // This and -1 is where we confuse them.
  else if (tDiff < 5)
    return "a good amount";
  else if (tDiff < 10)
    return "a really good amount";
  else if (tDiff < 15)
    return "an extremely good amount";
  else
    return "way too much of an amount";

#else
  if (n < 3)
    return "a trivial amount";
  else if (n < 8)
    return "a fair amount";
  else if (n < 14)
    return "a moderate amount";
  else if (n < 18)
    return "quite a bit";
  else if (n < 25)
    return "a lot";
  else if (n < 35)
    return "an awful lot";
  else if (n < 45)
    return "an awesome amount";
  else
    return "a unexplainable amount";
#endif
}

const char *describe_armor(int n)
{
  if (n < 1)
    return "no";
  else if (n < 3)
    return "very little";
  else if (n < 8)
    return "little";
  else if (n < 14)
    return "some";
  else if (n < 18)
    return "a good amount of";
  else if (n < 25)
    return "a lot of";
  else if (n < 35)
    return "an awful lot of";
  else if (n < 45)
    return "an awesome amount of";
  else
    return "a unexplainable amount of";
}


const char *describe_light(int n)
{
  if (n < 3)
    return "dim";
  else if (n < 8)
    return "moderately-bright";
  else if (n < 15)
    return "bright";
  else if (n < 25)
    return "very bright";
  else if (n < 35)
    return "extremely intense";
  else
    return "blinding";
}

const char *what_does_it_open(const TKey *o)
{
  int i, x, vnum;
  TRoom *rp;
  TObj *k;
  roomDirData *ex;

  vnum = o->objVnum();
  for (i = 0; i < WORLD_SIZE; i++) {	// check if it opens a door 
    if ((rp = real_roomp(i))) {
      for (x = 0; x < 10; x++) {
	if ((ex = rp->dir_option[x]) && (ex->key == vnum)) {
	  if (IS_SET(ex->condition, EX_SECRET))
	    return "a secret door";
	  else
	    return "a door";
	}
      }
    }
  }
  for (k = object_list; k; k = k->next)	{ // check if it opens an item 
    TOpenContainer *trc = dynamic_cast<TOpenContainer *>(k);
    if (trc) {
      if (trc->getKeyNum() == vnum)
	return "a container";
    }
  }

  return "something undetectable";
}

// DELETE_THIS  means c got toasted
// DELETE_VICT  means v got toasted
int TBeing::rawSummon(TBeing *v)
{
  TBeing *tmp = NULL;
  TThing *t, *t2;
  int i, rc;
  wearSlotT j;

  act("You hear a small \"pop\" as $n disappears.", 
            FALSE, v, NULL, NULL, TO_ROOM);
  --(*v);
  *roomp += *v;

  act("You hear a small \"pop\" as $n appears in the middle of the room.", TRUE,
 v, NULL, NULL, TO_ROOM);
  act("$n has summoned you!", FALSE, this, NULL, v, TO_VICT);
  v->doLook("", CMD_LOOK);

  if (!v->isPc() && (v->GetMaxLevel() > GetMaxLevel())) {
    act("$N struggles, and all of $S items are destroyed!", TRUE, this, NULL, v, TO_CHAR);
    for (j = MIN_WEAR; j < MAX_WEAR; j++) {    // remove objects from victim 
      if (v->equipment[j] && 
          !dynamic_cast<TKey *>(v->equipment[j])) {
        TThing *t_o = v->unequip(j);
        delete t_o;
        t_o = NULL;
      }
    }
    for (t = v->stuff; t; t = t2) {
      t2 = t->nextThing;
      if (!dynamic_cast<TKey *>(t)) {
        --(*t);
        delete t;
        t = NULL;
      }
    }
    v->addHated(this);
  } else
    addToWait(combatRound(2));

  if (v->riding) {
    rc = v->fallOffMount(v->riding, POSITION_STANDING);
    if (IS_SET_DELETE(rc, DELETE_THIS)) {
      return DELETE_VICT;
    }
  }
  while ((t = v->rider)) {
    rc = t->fallOffMount(v, POSITION_STANDING);
    if (IS_SET_DELETE(rc, DELETE_THIS)) {
      delete t;
      t = NULL; 
    }
  }

#if 0
// too easy to abuse for PK, and somewhat silly too
  v->addToMove(-100);
  v->setMove(max(0, v->getMove()));
  v->updatePos();
#endif

  act("You are exhausted from interplanar travel.", FALSE, v, NULL, NULL, TO_CHAR);
  act("$n is exhausted from interplanar travel.", FALSE, v, NULL, NULL, TO_ROOM);

  // summon newbie to aggro zone far from GH, allow us to check for it
  vlogf(LOG_SILENT, "%s summoned %s to %s (%d)",
          getName(), v->getName(), roomp->getName(), inRoom());

  if (v->riding) {
    rc = v->riding->genericMovedIntoRoom(roomp, -1);
    if (IS_SET_DELETE(rc, DELETE_THIS)) {
      delete v->riding;
      v->riding = NULL;
      REM_DELETE(rc, DELETE_THIS);
    }
  } else {
    rc = v->genericMovedIntoRoom(roomp, -1);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_VICT;
  }

  for (t = v->roomp->stuff; t; t = t2) {
    t2 = t->nextThing;
    if (!(tmp = dynamic_cast<TBeing *>(t)))
      continue;
    if (!tmp->isPc() && ((IS_SET(tmp->specials.act, ACT_AGGRESSIVE)))) {
      i = ::number(0, 6);
      if (i || 1) {  // make fairly guaranteed
        act("$n sneers at you.", 1, tmp, NULL, this, TO_VICT);
        act("$n sneers at $N. Uh oh...there's gonna be a RUMBLE!", 
             1, tmp, NULL, this, TO_NOTVICT);

        rc = dynamic_cast<TMonster *>(tmp)->takeFirstHit(*this);
        if (IS_SET_DELETE(rc, DELETE_VICT)) {
          return DELETE_THIS;
        } else if (IS_SET_DELETE(rc, DELETE_THIS)) {
          delete tmp;
          tmp = NULL;
          return FALSE;
        }
      }
    }
  }

  disturbMeditation(v);

  return FALSE;
}

void TBeing::addAffects(const TObj *o)
{
  int i;

  for (i = 0; i < MAX_OBJ_AFFECT; i++)
    if (o->affected[i].location != APPLY_NONE)
      affectModify(o->affected[i].location, (long) o->affected[i].modifier, (long) o->affected[i].modifier2, o->obj_flags.bitvector, TRUE, SILENT_YES);
    else
      return;
}

// returns DELETE_THIS
int TThing::genericTeleport(silentTypeT silent, bool keepZone)
{
  int to_room;
  TRoom *rp;
  int rc;
  TThing *t;

  for (;;) {
    // note, all rooms below 100 are ignored

    if (keepZone) {
      int minroom = zone_table[roomp->getZone() - 1].top + 1;
      int maxroom = zone_table[roomp->getZone()].top;
      to_room = ::number(minroom, maxroom);
    } else {
      to_room = ::number(100, top_of_world);
    }
    if (!(rp = real_roomp(to_room)))
      continue;
    if (rp->isRoomFlag(ROOM_PRIVATE))
      continue;
    if (rp->isRoomFlag(ROOM_HAVE_TO_WALK))
      continue;
    if (rp->isRoomFlag(ROOM_DEATH))
      continue;
    if (rp->isFlyingSector())
      continue;
    if (zone_table[rp->getZone()].enabled == FALSE)
      continue;

    break;
  }

  if (!silent) {
    act("$n shimmers out of existence.", FALSE, this, NULL, NULL, TO_ROOM);
    act("You shimmer out of existence.", FALSE, this, NULL, NULL, TO_CHAR);
  }

  while ((t = rider)) {
    rc = t->fallOffMount(this, POSITION_STANDING);
    if (IS_SET_DELETE(rc, DELETE_THIS)) {
      delete t;
      t = NULL; 
    }
  }

  if (riding) {
    rc = fallOffMount(riding, POSITION_STANDING);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }

  --(*this);
  *rp += *this;

  if (!silent) {
    act("$n shimmers into existence.", FALSE, this, NULL, NULL, TO_ROOM);
    act("You shimmer into existence.", FALSE, this, NULL, NULL, TO_CHAR);

    TBeing *tbt = dynamic_cast<TBeing *>(this);
    if (tbt) {
      tbt->doLook("", CMD_LOOK);

      rc = tbt->genericMovedIntoRoom(rp, -1);
      if (IS_SET_DELETE(rc, DELETE_THIS))
        return DELETE_THIS;
    }
  }

  return FALSE;
}

void TMonster::elementalFix(TBeing *caster, spellNumT spell, bool flags)
{
  int level;

  level = caster->GetMaxLevel();
  level = ::number(level-3, level);
  level = max(level, 4);

  switch (spell) {
    case SPELL_CONJURE_AIR:
      // air elems fly, so let's account for this and make uhm weak
      level = (int) (0.9 * level);
      break;
    case SPELL_CONJURE_FIRE:
      // fire elms fly, so let's account for this and also make them weak
      level = (int) (0.9 * level);
      break;
    case SPELL_CONJURE_EARTH:
      level = (int) (1.0 * level);
      break;
    case SPELL_CONJURE_WATER:
      level = (int) (1.0 * level);
      break;
    case SPELL_ENTHRALL_SPECTRE:
      level = (int) (0.7 * level);
      break;
    case SPELL_ENTHRALL_GHAST:
      level = (int) (0.7 * level);
      break;
    case SPELL_ENTHRALL_GHOUL:
      level = (int) (0.8 * level);
      break;
    case SPELL_ENTHRALL_DEMON:
      level = (int) (0.8 * level);
      break;
    default:
      forceCrash("Bad spellNumT (%d) to elementalFix", spell);
      break;
  }
  // correct the level
  for (classIndT ij = MIN_CLASS_IND; ij < MAX_CLASSES; ij++)
    if (getLevel(ij))
      setLevel(ij, level);

  calcMaxLevel();

  float lvl = level;

  setHPLevel(lvl);
  setHPFromHPLevel();
  setACLevel(lvl);
  setACFromACLevel();
  setDamLevel(lvl);

  setHitroll(0);

  if (flags) {
  }
  genericCharmFix();
  return;
}

void TMonster::genericPetFix()
{
// for pets
}
// fix some values on charms/zombies, etc
void TMonster::genericCharmFix()
{
  setMoney(0);
  setExp(0);

  spec = 0;

  SET_BIT(specials.act, ACT_SENTINEL);
  REMOVE_BIT(specials.act, ACT_IMMORTAL);
  REMOVE_BIT(specials.act, ACT_AGGRESSIVE);
  REMOVE_BIT(specials.act, ACT_PROTECTOR);
  REMOVE_BIT(specials.act, ACT_PROTECTEE);

  setMalice(0);
  setDefMalice(0);
  setAnger(0);
  setDefAnger(0);
  setGreed(0);
  setDefGreed(0);
  setSusp(0);
  setDefSusp(0);

  setImmunity(IMMUNE_SLASH, 0);
  setImmunity(IMMUNE_BLUNT, 0);
  setImmunity(IMMUNE_PIERCE, 0);
  setImmunity(IMMUNE_NONMAGIC, 0);

  while (hates.clist)
    remHated(NULL, hates.clist->name);
  while (fears.clist)
    remFeared(NULL, fears.clist->name);

// adjusts the mob down to be more like a pc
// its ok cause has no exp

  balanceMakeNPCLikePC();
}

void TBeing::rawBlind(int level, int duration, saveTypeT save)
{
  affectedData aff;
  int bf_mod = 0, knows_bf;

  aff.type = SPELL_BLINDNESS;
  aff.bitvector = AFF_BLIND;
  aff.level = level;
  aff.duration = duration;

  if (save)
    duration /= 2;

  if((knows_bf=doesKnowSkill(SKILL_BLINDFIGHTING)))
    bf_mod=(getSkillValue(SKILL_BLINDFIGHTING)+24)/25;

  aff.location = APPLY_SPELL_HITROLL;
  aff.modifier = (save ? -40 : -80);           // Make hitroll worse
  if(knows_bf) // reduce modifier for blindfighting skill
    aff.modifier/=bf_mod;
  affectJoin(NULL, &aff, AVG_DUR_NO, AVG_EFF_YES);

  aff.location = APPLY_ARMOR;
  aff.modifier = (save ? 20 : 40);          // Make AC Worse!
  if(knows_bf) // reduce modifier for blindfighting skill
    aff.modifier/=bf_mod;
  affectJoin(NULL, &aff, AVG_DUR_NO, AVG_EFF_YES);

  aff.location = APPLY_DEX;
  aff.modifier = (save ? -10 : -30);          // Make Dex Worse!
  if(knows_bf) // reduce modifier for blindfighting skill
    aff.modifier/=bf_mod;
  affectJoin(NULL, &aff, AVG_DUR_NO, AVG_EFF_YES);
}

int TBeing::rawSleep(int level, int duration, int crit, saveTypeT save)
{
  int rc = FALSE;

  affectedData aff;

  aff.type = SPELL_SLUMBER;
  aff.duration = duration;
  aff.level = level;
  aff.modifier = 0;
  aff.location = APPLY_NONE;
  aff.bitvector = AFF_SLEEP;

  aff.duration *= crit;
  aff.duration /= (save ? 2 : 1);

  // we've already applied a raw immunity check to prevent entirely
  // however, let immunities also decrease duration
  aff.duration *= (100 - getImmunity(IMMUNE_SLEEP));
  aff.duration /= 100;

  affectTo(&aff);

  if (getPosition() > POSITION_SLEEPING) {
    act("You feel very sleepy...  All you want is a bed...   ZZZZZZ....",
              FALSE, this, NULL, NULL, TO_CHAR);
     act("You drift peacefully off to dreamland.",
              FALSE, this, NULL, NULL, TO_CHAR);
     act("$n falls asleep!", TRUE, this, NULL, NULL, TO_ROOM);
  }
  if (riding) {
    rc = fallOffMount(riding, POSITION_SITTING);
    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_THIS;
  }
  if (fight()) {
    stopFighting();
  }
  setPosition(POSITION_SLEEPING);

  // stop all fighting me too
  TThing *t;
  for (t = roomp->stuff; t; t = t->nextThing) {
    TBeing *ch = dynamic_cast<TBeing *>(t);
    if (!ch)
      continue;
    if (ch->fight() == this)
      ch->stopFighting();
  }
 
  return FALSE;
}

// take a given limb and determine how much blood to drop, then call dropPool
int TBeing::dropBloodLimb(wearSlotT limb)
{
  int amt;

  switch(limb){
    case WEAR_HEAD:      amt=5; break;
    case WEAR_NECK:      amt=9; break;
    case WEAR_BODY:
    case WEAR_BACK:      amt=20; break;
    case WEAR_ARM_R:
    case WEAR_ARM_L:     amt=9; break;
    case WEAR_WRIST_R:
    case WEAR_WRIST_L:   amt=3; break;
    case WEAR_HAND_R:
    case WEAR_HAND_L:    amt=3; break;
    case WEAR_FINGER_R:
    case WEAR_FINGER_L:  amt=1; break;
    case WEAR_WAISTE:    amt=10; break;
    case WEAR_LEGS_R:
    case WEAR_LEGS_L:    amt=9; break;
    case WEAR_FOOT_R:
    case WEAR_FOOT_L:    amt=3; break;
    case WEAR_EX_LEG_R:
    case WEAR_EX_LEG_L:  amt=9; break;
    case WEAR_EX_FOOT_R:
    case WEAR_EX_FOOT_L: amt=3; break;
    default: return FALSE;
  }

  dropPool(amt, LIQ_BLOOD);

  if (desc)
    desc->career.ounces_of_blood+=amt;


  return TRUE;
}

// assumes you have already checked for immunites, etc
int TBeing::rawBleed(wearSlotT pos, int duration, silentTypeT silent, checkImmunityT immcheck)
{
  affectedData aff;
  char buf[256];

  mud_assert(pos >= MIN_WEAR && pos < MAX_WEAR && 
             pos != HOLD_RIGHT && pos != HOLD_LEFT &&
             slotChance(pos), "Bogus slot on raw bleed");

  // not sure what this is for???
  if (immcheck) {
    if (isImmune(IMMUNE_BLEED,
        duration == PERMANENT_DURATION ? 100 : (duration - 100)/6))
      return FALSE;
  }

  aff.type = AFFECT_DISEASE;
  aff.level = pos;
  aff.duration = duration;
  aff.location = APPLY_NONE;
  aff.modifier = DISEASE_BLEEDING;
  aff.bitvector = 0;

  // we've already applied a raw immunity check to prevent entirely
  // however, let immunities also decrease duration
  aff.duration *= (100 - getImmunity(IMMUNE_BLEED));
  aff.duration /= 100;

  affectTo(&aff);
  disease_start(this, &aff);

  dropBloodLimb(pos);

  if (!silent) {
    sendTo("%sYour %s has started to bleed!%s\n\r", 
        red(), describeBodySlot(pos).c_str(), norm());
    sprintf(buf, "$n's %s has begun to bleed!", describeBodySlot(pos).c_str());
    act(buf, TRUE, this, NULL, NULL, TO_ROOM);
  }

  return TRUE;
}

// assumes you have already checked for immunities, etc
int TBeing::rawInfect(wearSlotT pos, int duration, silentTypeT silent, checkImmunityT immcheck)
{
  affectedData aff;
  char buf[256];

  if (immcheck) {
    if (isImmune(IMMUNE_DISEASE,
        duration == PERMANENT_DURATION ? 100 : (duration - 100)/6))
      return FALSE;
  }

  aff.type = AFFECT_DISEASE;
  aff.level = pos;
  aff.duration = duration;
  aff.modifier = DISEASE_INFECTION;
  aff.location = APPLY_NONE;
  aff.bitvector = 0;

  // we've already applied a raw immunity check to prevent entirely
  // however, let immunities also decrease duration
  aff.duration *= (100 - getImmunity(IMMUNE_DISEASE));
  aff.duration /= 100;

  affectTo(&aff);
  disease_start(this, &aff);

  if (!silent) {
    sendTo("Your %s has become totally infected!\n\r", describeBodySlot(pos).c_str());
    sprintf(buf, "$n's %s has become totally infected!", describeBodySlot(pos).c_str());
    act(buf, TRUE, this, NULL, NULL, TO_ROOM);
  }

  return TRUE;
}

void TBeing::spellMessUp(spellNumT spell)
{
  int num;
  int type = 0;

  if (!discArray[spell] || !*discArray[spell]->name) {
    vlogf(LOG_BUG,"Bad spell/skill number in spellMessUp %d", spell);
    return;
  }

  if ((discArray[spell]->typ == SPELL_MAGE) ||
           (discArray[spell]->typ == SPELL_RANGER) ||
           (discArray[spell]->typ == SPELL_SHAMAN)) {
    type = 0;
  } else if ((discArray[spell]->typ == SPELL_CLERIC) || 
           (discArray[spell]->typ == SPELL_DEIKHAN)) {
    type = 1;       
  }
#if 0
  switch (getDisciplineNumber(spell, FALSE)) {
    case DISC_AIR:
    case DISC_WATER:
    case DISC_FIRE:
    case DISC_EARTH:
    case DISC_SPIRIT:
    case DISC_ALCHEMY:
    case DISC_MAGE:
    case DISC_RANGER:
    case DISC_SHAMAN:
      type = 0;
    case DISC_CURES:
    case DISC_AFFLICTIONS:
    case DISC_WRATH:
    case DISC_HAND_OF_GOD:
    case DISC_AEGIS:
    case DISC_CLERIC:
    case DISC_DEIKHAN:
      type = 1;
    default:
      vlogf(LOG_BUG, "Undefined spell (%d) in spellMessUp", spell);
      return;
  }
#endif
  num = ::number(1,6);

  switch(num) {
    case 1:
    case 2:
      if (type == 1)
        act("Your brain is jumbled and confused, and you flub the prayer.",
             FALSE, this, 0, 0, TO_CHAR); 
      else
        act("Your brain is jumbled and confused, and you flub the spell.",
             FALSE, this, 0, 0, TO_CHAR); 
      act("$n must have done something wrong.",
              FALSE, this, 0, 0, TO_ROOM); 
      break;
    case 3:
      if (getWizardryLevel() < WIZ_LEV_NO_GESTURES &&
          IS_SET(discArray[spell]->comp_types, COMP_GESTURAL)) {
        // requires gestures
        act("Darn it!  You mess up one of the intricate gestures.",
              FALSE, this, 0, 0, TO_CHAR); 
        act("$n must have done something wrong.",
              FALSE, this, 0, 0, TO_ROOM); 
        break;
      } // otherwise drop through for different text
    case 4:
      if (getWizardryLevel() < WIZ_LEV_NO_GESTURES &&
          IS_SET(discArray[spell]->comp_types, COMP_GESTURAL)) {
        // requires gestures
        if (type == 1)
          act("You clumsily perform the prayer's gestures, and things seem to have gone wrong.",
              FALSE, this, 0, 0, TO_CHAR); 
        else
          act("You clumsily perform the spell's gestures, and things seem to have gone wrong.",
              FALSE, this, 0, 0, TO_CHAR); 
        act("$n must have done something wrong.",
              FALSE, this, 0, 0, TO_ROOM); 
        break;
      } // otherwise drop through for different text
    case 5:
      if (getWizardryLevel() < WIZ_LEV_NO_MANTRA &&
          IS_SET(discArray[spell]->comp_types, COMP_VERBAL)) {
        // requires incantation
        act("Oops...  You mis-spoke part of the incantation.",
              FALSE, this, 0, 0, TO_CHAR); 
        act("$n must have done something wrong.",
              FALSE, this, 0, 0, TO_ROOM); 
        break;
      } // otherwise drop through for different text
    case 6:
      if (getWizardryLevel() < WIZ_LEV_NO_MANTRA &&
          IS_SET(discArray[spell]->comp_types, COMP_VERBAL)) {
        // requires incantation
        act("You trip over your tongue and mis-speak the incantation.",
              FALSE, this, 0, 0, TO_CHAR); 
        act("$n must have done something wrong.",
              FALSE, this, 0, 0, TO_ROOM); 
        break;
      } // otherwise drop through for different text
    default:
      act("You aren't sure what, but something seems to have gone wrong.",
            FALSE, this, 0, 0, TO_CHAR); 
      act("$n must have done something wrong.",
            FALSE, this, 0, 0, TO_ROOM); 
  }
  return;
}

void TBeing::nothingHappens(silentTypeT silent_caster) const
{
  soundNumT snd = pickRandSound(SOUND_CAST_FAIL_01, SOUND_CAST_FAIL_02);

  if (!silent_caster)
    roomp->playsound(snd, SOUND_TYPE_MAGIC);
  else {
    TThing *t;
    for (t = roomp->stuff; t; t = t->nextThing) {
      TBeing *tbt = dynamic_cast<TBeing *>(t);
      if (!tbt || tbt == this)
        continue;
      tbt->playsound(snd, SOUND_TYPE_MAGIC);
    }
  }

  int num = ::number(0,17);
  switch(num) {
    default:
    case 0:
      if (!silent_caster)
        sendTo("Nothing seems to happen.\n\r");
      act("Nothing seems to happen.", TRUE, this, 0, 0, TO_ROOM);
      break;
    case 1:
      if (!silent_caster)
        sendTo("Nothing happens.\n\r");
      act("Nothing happens.", TRUE, this, 0, 0, TO_ROOM);
      break;
    case 2:
      if (!silent_caster)
        act("Uh oh, maybe you ought to try that again.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("Humor the little mage and pretend the spell worked.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 3:
      if (!silent_caster)
        act("That didn't work...ONE MORE TIME!",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n's spell didn't work.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 4:
      if (!silent_caster)
        act("Nope, nuh uh, nada, zip, the big mage fizzle.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("Chant, chant, wave hands, wave hands, mages suck.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 5:
      if (!silent_caster)
        act("Damn!  Missed again.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("The mage casts and misses!",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 6:
      if (!silent_caster)
        act("The forces of magic fail to come forth.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("The forces of magic fail to come forth.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 7:
      if (!silent_caster)
        act("Try as you might, your magic fails you.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("Try as $n might, the magic fails.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 8:
      if (!silent_caster)
        act("Your attempt at magic is unsuccessful.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n's attempt at magic is unsuccessful.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 9:
      if (!silent_caster)
        act("Your spell dissipates without effect.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n's magic dissipates without any effect.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 10:
      if (!silent_caster)
        act("Your mind lacks the focus to control the magic.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n's magic starts to form, but then collapses.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 11:
      if (!silent_caster)
        act("Your thoughts go awry, and the magic fades harmlessly.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n looks perplexed and $s magic fades harmlessly.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 12:
      if (!silent_caster)
        act("You're pretty sure that should have worked, but no such luck.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n blinks in bewilderment.  Perhaps $e was expecting something to happen...?",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 13:
      if (!silent_caster)
        act("Dang, you forgot part of the incantation and cease casting.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n throws $s hands up in disgust.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 14:
      if (!silent_caster)
        act("Something seems amiss, and you give up on your spell.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n acts like $s spell is finished, but the magic ain't there.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 15:
      if (!silent_caster)
        act("You slip up and manage to fill the air with goose feathers.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n fills the air with goose feathers.  Neat!",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 16:
      if (!silent_caster)
        act("You make an error and sparks seem to surround you.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("$n makes a mistake, and becomes surrounded by magical sparks.",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
    case 17:
      if (!silent_caster)
        act("DAMN! Screwed up again!.",
                 FALSE, this, NULL, NULL, TO_CHAR);
      act("Chant...Chant...Wave hands...Wave hands...Mages suck!",
                 FALSE, this, NULL, NULL, TO_ROOM);
      break;
  }
}

bool TBeing::canDoSummon() const
{
  if (roomp->isFlyingSector())
    return FALSE;

  if ((inRoom() >= 9710 && inRoom() <= 9795))
    return FALSE;
  else
    return TRUE;
}

bool TBeing::isSummonable() const
{
  if (roomp->isFlyingSector())
    return FALSE;

  if ((inRoom() >= 9710 && inRoom() <= 9795))
    return FALSE;
  else
    return TRUE;
}

bool genericBless(TBeing *c, TBeing *v, int level, bool crit)
{
  affectedData aff1, aff2;

  aff1.type = SPELL_BLESS;
  aff1.level = level;
  aff1.duration = (1 + level) * UPDATES_PER_MUDHOUR;
  aff1.location = APPLY_SPELL_HITROLL;
  aff1.modifier = 10;
  aff1.bitvector = 0;

  aff2.type = aff1.type;
  aff2.level = level;
  aff2.duration = (1 + level) * UPDATES_PER_MUDHOUR;
  aff2.location = APPLY_IMMUNITY;
  aff2.modifier = IMMUNE_NONMAGIC;
  aff2.modifier2 = 5;
  aff2.bitvector = 0;

  if (crit) {
    aff1.duration += 9 * UPDATES_PER_MUDHOUR;
    aff1.modifier = 2;
    aff2.duration += 9 * UPDATES_PER_MUDHOUR;
    aff2.modifier2 *= 2;
  }

  bool success = false;
  if (v->affectJoin(c, &aff1, AVG_DUR_NO, AVG_EFF_YES))
    success = TRUE;

  if (success) {
    if (v->affectJoin(c, &aff2, AVG_DUR_NO, AVG_EFF_YES))
      success = TRUE;
  } else {
    if (v->affectJoin(c, &aff2, AVG_DUR_NO, AVG_EFF_YES, FALSE))
      success = TRUE;
  }
  return success;
}

void genericDisease(TBeing *vict, int level)
{
  // assumes check for isImmune already made
  affectedData aff;

  aff.type = AFFECT_DISEASE;
  aff.level = 0;
  aff.location = APPLY_NONE;
  aff.bitvector = 0;

  if (level < 30) {
    if (50 + level < ::number(0,99)) {
      aff.duration = level * UPDATES_PER_MUDHOUR / 3;
      aff.modifier = DISEASE_FLU;
    } else {
      aff.modifier = DISEASE_COLD;
      aff.duration = level * UPDATES_PER_MUDHOUR / 3;
    }
  } else {
    if (20 + level < ::number(0,99)) {
      aff.duration = level * UPDATES_PER_MUDHOUR / 3;
      aff.modifier = DISEASE_LEPROSY;
    } else {
      aff.modifier = DISEASE_FLU;
      aff.duration = level * UPDATES_PER_MUDHOUR / 3;
    }
  }

  // we've already applied a raw immunity check to prevent entirely
  // however, let immunities also decrease duration
  aff.duration *= (100 - vict->getImmunity(IMMUNE_DISEASE));
  aff.duration /= 100;

  vict->affectTo(&aff);
  disease_start(vict, &aff);
}

void genericCurse(TBeing *c, TBeing *v, int level, spellNumT spell)
{
  affectedData aff1, aff2;

  aff1.type = spell;
  aff1.level = level;
  aff1.duration = 12 * UPDATES_PER_MUDHOUR;
  aff1.bitvector = AFF_CURSE;
  aff1.location = APPLY_SPELL_HITROLL;
  aff1.modifier = - min(5, level/3);
  aff1.duration = (int) (c->percModifier() * aff1.duration);

  aff2.type = aff1.type;
  aff2.level = aff1.level;
  aff2.duration = aff1.duration;
  aff2.bitvector = aff1.bitvector;

  aff2.location = APPLY_IMMUNITY;
  aff2.modifier = IMMUNE_PARALYSIS;             /* Make worse */
  aff2.modifier2 = - min(5, level/3);

  v->affectTo(&aff1);
  v->affectTo(&aff2);
}

string displayDifficulty(spellNumT skill)
{
  switch (discArray[skill]->task) {
    case TASK_TRIVIAL:
      return "Trivial";
    case TASK_EASY:
      return "Easy";
    case TASK_NORMAL:
      return "Normal";
    case TASK_DIFFICULT:
      return "Difficult";
    case TASK_DANGEROUS:
      return "Dangerous";
    case TASK_HOPELESS:
      return "Hopeless";
    case TASK_IMPOSSIBLE:
      return "Near-impossible";
  }
  return "BOGUS, tell a god";
}
