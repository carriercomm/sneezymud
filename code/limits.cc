extern "C" {
#include <unistd.h>
}
#include <cmath>

#include "stdsneezy.h"
#include "statistics.h"
#include "games.h"

#if 0
static const string ClassTitles(const TBeing *ch)
{
  int count = 0;
  classIndT i;
  char buf[256];

  for (i = MIN_CLASS_IND; i < MAX_CLASSES; i++) {
    if (ch->getLevel(i)) {
      if ((++count) > 1)
        sprintf(buf + strlen(buf), "/Level %d %s", 
                ch->getLevel(i), classNames[i].capName);
      else
        sprintf(buf, "Level %d %s",
                ch->getLevel(i), classNames[i].capName);
    }
  }
  return (buf);
}
#endif

int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{
  if (age < 15)
    return (p0);
  else if (age <= 29)
    return (int) (p1 + (((age - 15) * (p2 - p1)) / 15));        /* 15..29 */
  else if (age <= 44)
    return (int) (p2 + (((age - 30) * (p3 - p2)) / 15));        /* 30..44 */
  else if (age <= 59)
    return (int) (p3 + (((age - 45) * (p4 - p3)) / 15));        /* 45..59 */
  else if (age <= 79)
    return (int) (p4 + (((age - 60) * (p5 - p4)) / 20));        /* 60..79 */
  else
    return (p6);
}

short int TPerson::hitLimit() const
{
  return points.maxHit + graf((age()->year - getBaseAge() + 15), 2, 4, 17, 14, 8, 4, 3);
}

short int TPerson::manaLimit() const
{
  int iMax = 100;

  if (hasClass(CLASS_MAGIC_USER))
    iMax += getLevel(MAGE_LEVEL_IND) * 6;
#if 0
  else if (hasClass(CLASS_CLERIC))
    iMax += getLevel(CLERIC_LEVEL_IND) * 5;
  else if (hasClass(CLASS_DEIKHAN))
    iMax += GetMaxLevel() * 3;
#endif
  else if (hasClass(CLASS_RANGER))
    iMax += GetMaxLevel() * 3;
  else if (hasClass(CLASS_MONK))
    iMax += GetMaxLevel() * 3;

  iMax += points.maxMana;        /* bonus mana */

  return (iMax);
}

int TPerson::getMaxMove() const
{
  return 100 + age()->year - getBaseAge() + 15 + GetTotLevel() +
      plotStat(STAT_CURRENT, STAT_CON, 3, 18, 13);
}

short int TBeing::moveLimit() const
{
  int iMax = getMaxMove() + race->getMoveMod();

  iMax += points.maxMove;        /* move bonus */

  return (iMax);
}

double TBeing::pietyGain(double modif)
{
  // modif is disc_piety to simulate being of higher percentage then really am
  double gain;

  if (!isPc())
    return 0.0;

  if (getPiety() == 100.0)
    return 0.0;

  if (fight() || spelltask)
    return 0.0;

  if (!hasClass(CLASS_CLERIC) && !hasClass(CLASS_DEIKHAN))
    return 0.5;  // slow regen if they somehow lost it

#if FACTIONS_IN_USE
  double ppx;  // my factions power as a percent of the avg faction power 
  if (isUnaff())
    ppx = 50.0;  // flat regen
  else if (avg_faction_power)
    ppx = (FactionInfo[getFaction()].faction_power) / avg_faction_power * 100.;
  else if (FactionInfo[getFaction()].faction_power)
    ppx = 100.0;
  else
    ppx = 0.0;

  // no healing if weak member of weak faction
  if (((ppx + (getPerc()/5.0) + modif) < 100.0) && !isUnaff()) {
    // allow weak healing
    gain = (1.0 - (GetMaxLevel() * 0.01));
  } else {
    gain = ppx / 100.;
    gain *= (getPerc() + 100.0 + modif)/200.;
    // newbies will essentially get 3/4 of the below
    // at 20.0 : Average piety regen           : 8.14  (attempts : 531)
    // at 30.0 : Average piety regen           : 12.90 (attempts : 2091)
    // at L50, cleric has 100 piety, think we desire regen of 5
    // since this makes it REAL slow at low level, go with 10 or so instead
    gain *= 20.0;  // arbitrary
  }

  // lower this to make faction power drain quicker
  // 50.0 allows slow/constant growth
  // 40.0 allows slow/constant decrease
  double GAIN_DIVISOR = 45.0;

  if (GetMaxLevel() <= MAX_MORT) {
    if (!isUnaff()) {
      if ((gain/GAIN_DIVISOR) > FactionInfo[getFaction()].faction_power)
        gain = GAIN_DIVISOR * FactionInfo[getFaction()].faction_power;

      FactionInfo[getFaction()].faction_power -= gain/GAIN_DIVISOR;
      desc->session.perc -= gain/GAIN_DIVISOR;

      if (FactionInfo[getFaction()].faction_power < 0.0000)
        FactionInfo[getFaction()].faction_power = 0.0000;
    } else {
      // unaff's will drain from from highest powered faction
      int iFact = bestFactionPower();
  
      if ((gain/GAIN_DIVISOR) > FactionInfo[iFact].faction_power)
        gain = GAIN_DIVISOR * FactionInfo[iFact].faction_power;

      FactionInfo[iFact].faction_power -= gain/GAIN_DIVISOR;

      if (FactionInfo[iFact].faction_power < 0.0000)
        FactionInfo[iFact].faction_power = 0.0000;
    }
  }
#else
//  ppx = 50.0;
//  gain = ppx / 100.;
//  gain *= (50.0 + 100.0 + modif)/200.;
//  gain *= 25.0;  // arbitrary
  // simplify math
  gain = (150.0 + modif)/18.0;
#endif

  if (dynamic_cast<TPerson *>(this)) {
    stats.piety_gained_attempts++;
    stats.piety_gained += gain;
  }
  // limit the gain so we don't go over max
  if ((getPiety() + gain) > 100.0)
    gain = 100. - getPiety();
      
  return gain;
}

int TMonster::manaGain()
{
  int gain;

  if (fight() || spelltask)
    return 0;

  gain = 1 + (2 * (GetTotLevel() / 9));
  gain += race->getManaMod();

  if (!getCond(FULL) || !getCond(THIRST))
    gain >>= 2;

  return (gain);
}

int TPerson::manaGain()
{
  int gain;

  if (fight() || spelltask)
    return 0;

  if (!desc)
    return 0;

  gain = graf((age()->year - getBaseAge() + 15), 2, 4, 6, 8, 10, 12, 14);

  // arbitrary multiplier
  // at 1.0 : Average mana regen            :  5.69  (attempts : 15624)
  // at 2.0 : Average mana regen            : 11.11  (attempts : 30584)
  // at L50, mage has 400 mana, think we want 20
  gain *= 4;

  if (hasClass(CLASS_MAGIC_USER))
    gain += gain;

  gain += race->getManaMod();

  if (!getCond(FULL) || !getCond(THIRST))
    gain >>= 2;

  stats.mana_gained_attempts++;
  stats.mana_gained += gain;

  return (gain);
}

int TMonster::hitGain()
{
  int gain;
  int num;

  // ok well. here's the deal, as of late March 2001, players are able to abuse
  // the slowness of mob healing to run and hide, then heal much faster than
  // the mob they were fighting. I'm going to roughly linearize the monster HP
  // gain formula to heal as fast (in terms of %hp) as PCs

  // evaluating the TPerson version of this function shows that:
  // a level 50 warrior should get about 1/20 hp a regen,
  // and a level 1 warrior should get about 8/20 hp a level
  // thus, we get... (.4-.35*(level/50.0))*hitLimit()
  // i'm also gonna cap level at 50 so that there is no way we can get negative values

  int oldgain = max(2, (GetMaxLevel() / 2));
  // the old version
  
  // base1 is the % for level 1, base50 is the percent for level 50, for easy adjusting

  double base1 = 0.125; // yeash complain complain making this much lower etc
  double base50 = 0.05;


  double level = (double)(min(50, (int)GetMaxLevel()));

  gain = (int)(( base1 - (base1-base50) * (level/50.0)) * (double)hitLimit());

  // just because i'm an asshole, i'm going to make sure this new value wont be lower
  // than the old value

  gain = max(oldgain,gain);
  
  if (fight())
    gain = 0;

  if (affectedBySpell(SKILL_CUDGEL))
    gain = oldgain;


  TBed * tb = dynamic_cast<TBed *>(riding);
  if (tb)
    tb->bedRegen(this, &gain, SILENT_NO);

  //gain += race->getHpMod(); This was meant for leveling.

  if (getCond(DRUNK) > 0)
    gain += 1 + (getCond(DRUNK) / 3);

  if (roomp && roomp->isRoomFlag(ROOM_HOSPITAL))
    gain *= 2;

  if ((num = inCamp())) {
    gain += (gain * num / 100);
  }

  return (gain);
}

int TPerson::hitGain()
{
  int gain;
  int num;

  if (hasClass(CLASS_SHAMAN)) {
    if (0 >= getLifeforce()) {
      return 0;
    }
  }

  if (!desc)
    return 0;

  if (fight())
    gain = 0;
  else {
    gain = graf((age()->year - getBaseAge() + 15), 2, 4, 5, 9, 4, 3, 2);
    gain += 4;
    gain = (int)((double)(gain)*plotStat(STAT_CURRENT,STAT_CON,.80,1.25,1.00));
  }

  // arbitrary multiplier
  // at 1.0 : Average HP regen              :  9.69  (attempts : 15487)
  // at 1.5 : Average HP regen              : 14.15  (attempts : 30465)
  // at 2.0 : Average HP regen              : 19.59  (attempts : 15844)
  // at 2.5 : Average HP regen              : 23.89  (attempts : 86643)
  // L50 warrior has 500 HP, desired regen of 25
  // L1 warrior has 25 HP, regen of 10
  gain *= max(20, (int) GetMaxLevel());
  gain /= 20;

  TBed * tb = dynamic_cast<TBed *>(riding);
  if (tb)
    tb->bedRegen(this, &gain, SILENT_NO);

  //gain += race->getHpMod(); This was meant for leveling.

  if (getCond(DRUNK) > 0)
    gain += 1 + (getCond(DRUNK) / 3);

  if (roomp && roomp->isRoomFlag(ROOM_HOSPITAL))
    gain *= 2;

  if ((num = inCamp())) {
    gain += (gain * num / 100);
  }

  stats.hit_gained_attempts++;
  stats.hit_gained += gain;
  return (gain);
}

int TBeing::moveGain()
{
  int gain, num;

  gain = plotStat(STAT_CURRENT, STAT_CON, 11, 41, 30);

  TBed * tb = dynamic_cast<TBed *>(riding);
  if (tb)
    tb->bedRegen(this, &gain, SILENT_YES);

  // moveMod is used to calc moveLimit().  don't use it

  if (isAffected(AFF_POISON))
    gain >>= 2;	// rightshift by 2 is div by 4

  if (!getCond(FULL) || !getCond(THIRST))
    gain >>= 2;	// rightshift by 2 is div by 4

  if ((num = inCamp())) {
    gain += (gain * num / 100);
  }

  if (roomp && roomp->isRoomFlag(ROOM_NO_HEAL))
    gain /= 3;
  if (roomp && roomp->isRoomFlag(ROOM_HOSPITAL))
    gain *= 2;

  gain = max(gain,1);

  if (dynamic_cast<TPerson *>(this)) {
    stats.move_gained_attempts++;
    stats.move_gained += gain;
  }

  return (gain);
}

sh_int TBeing::calcNewPracs(classIndT Class, bool forceBasic)
{
  sh_int prac;
  double num;
  //bool preReqs = FALSE;
  int combat = 0;
  bool doneCombat = FALSE;

  if (Class == MAGE_LEVEL_IND) {
    combat = getDiscipline(DISC_COMBAT)->getLearnedness() + getDiscipline(DISC_LORE)->getLearnedness();
  } else if (Class == CLERIC_LEVEL_IND || Class == DEIKHAN_LEVEL_IND) {
    combat = getDiscipline(DISC_COMBAT)->getLearnedness() + getDiscipline(DISC_THEOLOGY)->getLearnedness();
  } else {
    combat = getDiscipline(DISC_COMBAT)->getLearnedness();
  }

  if (combat >= MAX_DISC_LEARNEDNESS) {
    doneCombat = TRUE;
  }

  // A note on learning
  // as of March 2001, each class has the following specialized disciplines:
  // (Normal Spec disc is 60 pracs, weapon spec is 20, allow 1 weapon spec
  // for each of the 'fighting' classes)
  // Shaman:   8
  // Mages:    7
  // Monks:    4.33
  // Warriors: 4.33
  // Rangers:  5.33
  // Deikhans: 5.33
  // Thieves:  6.33
  // Clerics:  5

  // smart people should be able to learn 1/2 all possible learning
  // average people should learn 3/8
  // stupid people should learn 1/4
  // new - 2/3,1/2,3/3
  double minlearn = 1.0/3.0;
  double avlearn = 1.0/2.0;
  double maxlearn = 2.0/3.0;

  // I'd like to see all classes finish their prereqs at the same rate
  // thus leaving all the spec to be done after that
  // this gets messy, however.
  // i'll assume average person should finish basic discs at L30
  // thus basic learning should give a person 200/30 pracs a level = 6.67
  
  // thus, the next 20 levels should give 1/2 learning in however many base
  // disciplines.

  // using cleric as and example, this should be 5 discs * 60 pracs/disc = 300 pracs
  // 300 pracs * (3/8) learning = 112.5 pracs / 20 level = 5.625 pracs per level.

  // if we use the rate of basic learning, we can calculate the level when they
  // should have started learning advanced stuff

  // advanced level = 30 * (8/3) * (learnrate) = 80 * learnrate
  // like a putz, i decided this was too low and went to change it
  // so i plugged it into the statPlot with higher values, but FORGOT
  // to change the formulas we later derived using those values.
  // DON'T DO THAT

  // new things - we can force the range at which they finish basic training
  // for the moment i'd like to force between 22.5 and 37.5, because that will give a maxed
  // int person dependable 9 pracs a lev for basic training

  double deviation = 7.5;
  double avbasic = 30.0; // try to keep this 30, the balance docs follow that premise



  // so pracs per advanced level = (discs * 60 * learnrate) / (50 - 80 * learnrate) = pracs
  
  // ok... so lets do it!
  double discs = 0;

  // NOTE - this is what you should change if a class is getting too many/too few pracs
  // or if more disciplines are added.
  switch (Class) {
    case MAGE_LEVEL_IND:
      discs = 6.333;
      break;
    case CLERIC_LEVEL_IND:
      discs = 5.0;
      break;
    case WARRIOR_LEVEL_IND:
      discs = 4.666;
      break;
    case THIEF_LEVEL_IND:
      discs = 5.5;
      break;
    case DEIKHAN_LEVEL_IND:
      discs = 5.5;
      break;
    case MONK_LEVEL_IND:
      discs = 4.666;
      break;
    case RANGER_LEVEL_IND:
      discs = 5.5;
      break;
    case SHAMAN_LEVEL_IND:
      discs = 6.00;
      break; // I lowered this because I dont want shaman to jump too far ahead
             // I think this is a good start - Jesus
    case UNUSED1_LEVEL_IND:
    case UNUSED2_LEVEL_IND:
    case UNUSED3_LEVEL_IND:
    case MAX_SAVED_CLASSES:
      vlogf(LOG_BUG,"Got to a bad spot in calcNewPracs(), bad class");
      break;
  }
  
  
  double learnrate;


  if (!desc) {
    learnrate = plotStat(STAT_CURRENT, STAT_INT, minlearn, maxlearn, avlearn, 1.2);
  } else {
    learnrate = plotStat(STAT_NATURAL, STAT_INT, minlearn, maxlearn, avlearn, 1.2);
  }



  double advancedlevel = (double)((avbasic+(deviation*((avlearn - learnrate)/(avlearn-minlearn)))));
  //  double advancedlevel = (90.0/(8.0*learnrate));
  double basicpracs = (200.0/advancedlevel);
  double advancedpracs = (discs * 60.0 * learnrate)/(50.0 - advancedlevel);
  

  if (getLevel(Class) >= advancedlevel)
    num = advancedpracs;
  else
    num = basicpracs;
  
  float temp = num;

  #if 0


  // these are the default number of practices to use for each class
  // after they are done with their basic discs
  // we override these numbers appropriately
  float fMin = 5.0;
  float fMax = 9.0;
  float avg = 6.6;

  // override the defaults as appropriate (i.e., if we want person to
  // get practice gain appropriate for the basic discs
  switch (Class) {
    case MAGE_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_MAGE)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =5.0;
        fMax =8.0;
        avg =6.7;
      } 
      break;
    case CLERIC_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_CLERIC)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =5.0;
        fMax =8.0;
        avg =6.7;
      }
      break;
    case WARRIOR_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_WARRIOR)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =6.0;
        fMax =8.0;
        avg =7.0;
      }
      break;
    case THIEF_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_THIEF)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =5.0;
        fMax =8.0;
        avg =7.0;
      }
      break;
    case DEIKHAN_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_DEIKHAN)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        fMin =5.0;
        fMax =8.0;
        avg =7.0;
        preReqs = TRUE;
      }
      break;
    case MONK_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_MONK)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =5.0;
        fMax =8.0;
        avg =7.0;
      }
      break;
    case RANGER_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_RANGER)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =5.0;
        fMax =8.0;
        avg =6.7;
      }
      break;
    case SHAMAN_LEVEL_IND:
      if (!doneCombat || (getDiscipline(DISC_SHAMAN)->getLearnedness() < MAX_DISC_LEARNEDNESS) || forceBasic == 1) {
        preReqs = TRUE;
        fMin =5.0;
        fMax =8.0;
        avg =6.7;
      }
      break;
    case UNUSED1_LEVEL_IND:
    case UNUSED2_LEVEL_IND:
    case UNUSED3_LEVEL_IND:
    case MAX_SAVED_CLASSES:
      vlogf(LOG_BUG, "Bad class in calcNewPracs");
      break;
  } 

  if (!desc) {
    num = plotStat(STAT_CURRENT, STAT_INT, fMin, fMax, avg, 1.0);
  } else {
    num = plotStat(STAT_NATURAL, STAT_INT, fMin, fMax, avg, 1.0);
  }
#endif

  prac = (int) num;
  temp = prac;
  num = num - prac;
  if (isTripleClass()) {
    prac *= 4;
    prac /= 6;
    num *= 4;
    num /= 6;
  } else if (isDoubleClass()) {
    prac *= 3;
    prac /= 4;
    num *= 3;
    num /= 4;
  } 
  int roll = ::number(1,99);
  if ((100.0*num) >= roll) {
    prac++;
  }
  if(isPc()) {
    vlogf(LOG_DASH, "%s gaining %d pracs roll = %d (%d + %4.2f) lev: %d, advancedlev: %5.2f", getName(),
	  prac, roll, (int)temp, num, getLevel(Class), advancedlevel);

  }
  return prac;


}

void TBeing::setPracs(sh_int prac, classIndT Class)
{
  switch (Class) {
    case MAGE_LEVEL_IND:
      practices.mage = prac;
      break;
    case CLERIC_LEVEL_IND:
      practices.cleric = prac;
      break;
    case THIEF_LEVEL_IND:
      practices.thief = prac;
      break;
    case WARRIOR_LEVEL_IND:
      practices.warrior = prac;
      break;
    case DEIKHAN_LEVEL_IND:
      practices.deikhan = prac;
      break;
    case RANGER_LEVEL_IND:
      practices.ranger = prac;
      break;
    case MONK_LEVEL_IND:
      practices.monk = prac;
      break;
    case SHAMAN_LEVEL_IND:
      practices.shaman = prac;
      break;
    case UNUSED1_LEVEL_IND:
    case UNUSED2_LEVEL_IND:
    case UNUSED3_LEVEL_IND:
    case MAX_SAVED_CLASSES:
      vlogf(LOG_BUG, "Bad class in setPracs");
  }
}

sh_int TBeing::getPracs(classIndT Class)
{
  switch (Class) {
    case MAGE_LEVEL_IND:
      return practices.mage;
    case CLERIC_LEVEL_IND:
      return practices.cleric;
    case THIEF_LEVEL_IND:
      return practices.thief;
    case WARRIOR_LEVEL_IND:
      return practices.warrior;
    case DEIKHAN_LEVEL_IND:
      return practices.deikhan;
    case RANGER_LEVEL_IND:
      return practices.ranger;
    case MONK_LEVEL_IND:
      return practices.monk;
    case SHAMAN_LEVEL_IND:
      return practices.shaman;
    case UNUSED1_LEVEL_IND:
    case UNUSED2_LEVEL_IND:
    case UNUSED3_LEVEL_IND:
    case MAX_SAVED_CLASSES:
      forceCrash("Bad class in getPracs");
  }
  return 0;
}

void TBeing::addPracs(sh_int pracs, classIndT Class)
{
  setPracs(getPracs(Class) + pracs, Class);
}

/* I redid this function to allow slight differences in slight amounts of */
/* player intel and wisdom. Before the changes, there was no difference in*/
/* a 3 wis/int and a 10 wis/int, and therefore people just lowered those  */
/* all the way to 3, and had 19 or 18 in other stats because of it.       */

void TPerson::advanceLevel(classIndT Class, TMonster *gm)
{
  sh_int prac = 0;

  setLevel(Class, getLevel(Class) + 1);
  calcMaxLevel();

  if (desc && GetMaxLevel() >= 40 &&  desc->career.hit_level40 == 0)
    desc->career.hit_level40 = time(0);
  if (desc && GetMaxLevel() >= 50) {
    if (desc->career.hit_level50 == 0)
      desc->career.hit_level50 = time(0);
#if 0
    if (isSingleClass()) {
      SET_BIT(desc->account->flags, ACCOUNT_ALLOW_DOUBLECLASS);
      sendTo(COLOR_BASIC, "<r>Congratulations on obtaining L50!<z>\n\rYou may now create <y>double-class characters<z>!\n\r");
      desc->saveAccount();
    }
    if (isDoubleClass()) {
      SET_BIT(desc->account->flags, ACCOUNT_ALLOW_TRIPLECLASS);
      sendTo(COLOR_BASIC, "<r>Congratulations on obtaining L50!<z>\n\rYou may now create <y>triple-class characters<z>!\n\r");
      desc->saveAccount();
    }
#endif
  }

  prac = calcNewPracs(Class, FALSE);
  addPracs(prac, Class);

  if (gm)
    pracPath(gm, Class, (ubyte) prac);

  doHPGainForLev(Class);

  classSpecificStuff();

  // if gaining to L51 (how the hell does this happen), give them freedoms
  if (GetMaxLevel() > MAX_MORT) {
    condTypeT i;
    for (i = MIN_COND; i < MAX_COND_TYPE; ++i)
      setCond(i, -1);
  }
}

void TPerson::doHPGainForLev(classIndT Class)
{
  points.maxHit += hpGainForLevel(Class);
}

void TBeing::dropLevel(classIndT Class)
{
  double add_hp;
 
  if (GetMaxLevel() > MAX_MORT)
    return;

  if (GetMaxLevel() == 1)
    return;

  add_hp = hpGainForLevel(Class);

  addPracs(-calcNewPracs(Class, FALSE), Class);
  while (getPracs(Class) < 0) {
    addPracs(1, Class);
    discNumT dnt;
    for (dnt = MIN_DISC; dnt < MAX_DISCS; dnt++) {
      CDiscipline *cd = getDiscipline(dnt);
      if (cd && (cd->getLearnedness() > 0)) {
        cd->setNatLearnedness(cd->getNatLearnedness() - calcRaiseDisc(dnt, TRUE));
        break;  /* for loop */
      }
    }
    if (dnt == MAX_DISCS)
      break;  /* player has negitive pracs and no learning */
  }
    
  setLevel(Class,  getLevel(Class) - 1);

  if (getLevel(Class) < 1)
    setLevel(Class, 1);

  calcMaxLevel();

  points.maxHit -= max(1, (int) add_hp);
  if (points.maxHit < 1)
    points.maxHit = 1;

  setExp( min(getExpClassLevel(Class, getLevel(Class)), getExp()));

  if (points.exp < 0)
    points.exp = 0;
}

// tForm == false = update title
// tForm == true  = replace title
void TPerson::setTitle(bool tForm)
{
  char   tString[256] = "\0",
         tBuffer[256] = "\0";
  string tStString((title ? title : "ERROR OCCURED"));

  if (tForm) {
    sprintf(tString, "<n> the %s level %s",
          numberAsString(GetMaxLevel()).c_str(),
          getMyRace()->getSingularName().c_str());
  } else {
    sprintf(tString, "%s", numberAsString(GetMaxLevel() - 1).c_str());
    sprintf(tBuffer, "%s", numberAsString(GetMaxLevel()).c_str());

    while (tStString.find(tString) != string::npos)
      tStString.replace(tStString.find(tString), strlen(tString), tBuffer);

    strcpy(tString, tStString.c_str());
  }

  delete [] title;
  title = mud_str_dup(tString);
}


// Ok we're doing some wierd stuff in this function, so I'll document here.  For the exp cap,
// we're capping anything with:
// rawgain > (damage_of_hit / max_hp_of_mob) * (exp_for_level / min_kills_per_lev) 
// we also modify for number of classes, so that the cap is 1/2 as large for a dual class
// basically, if you play with the formula, you'll find that this limits the player so that
// in any particular fight, the player CANNOT gain more than 1/min_kills of his exp needed to
// move from his current level to the next.
// damage / max_hp is passed as a single integer, dam  (no damage = 0, full damage = 10000)
// exp_for_level is calculated from the exp needed for next lev minus exp needed for current lev
//   ie, (peak - curr)
// min_kills_per_lev is an AXIOM, since projected kills per lev is 17 + 1.25*lev (18-80, linear)
// we decided on gainmod = lev + 1 (2-50, linear)
// if the cap is being hit too much, it is a sign that the PCs have grown too powerful, or the
// gainmod formula is too strict.  if the cap isn't being hit (specifically when players are
// being pleveled) then gainmod is too lenient.             Dash - Jan 2001

// NOTE, gainmod is defined in multiple places, if you change it, change all of them please
// adding a small change so that low levs don't have a cap of 0 (max(lev*2,newgain))
// it looks like my logic for multiclasses isn't right, they're getting 1/#c as much exp as
// they should, even with the multiclass penalty... 01/01 - dash


void gain_exp(TBeing *ch, double gain, int dam)
{
  classIndT i;
  double newgain = 0;
  double oldcap = 0;
  bool been_here = false;

  if (!ch->isPc() && ch->isAffected(AFF_CHARM)) {
    // do_nothing so they get no extra exp
    return;
  } else if (ch->roomp && ch->roomp->isRoomFlag(ROOM_ARENA)) {
    // do nothing, these rooms safe
    return;
  } else if (!ch->isImmortal()) {
    if (gain > 0) {
      gain /= ch->howManyClasses();
      if (ch->isPc()) {
        for (i = MIN_CLASS_IND; i < MAX_CLASSES; i++) {
          double peak2 = getExpClassLevel(i,ch->getLevel(i) + 2);
          double peak = getExpClassLevel(i,ch->getLevel(i) + 1);
          double curr = getExpClassLevel(i,ch->getLevel(i));
	  double gainmod = ((1.15*ch->getLevel(i)) ); // removed +1
          if (ch->getLevel(i)) {
            if (!been_here && gain > ((double)(dam)*(peak-curr))/(gainmod*(double)(ch->howManyClasses()*10000))+2.0 && dam > 0) {
              been_here = TRUE; // don't show multiple logs for multiclasses
              // the 100 turns dam into a %
              newgain = ((double)(dam)*(peak-curr))/(gainmod*(double)(ch->howManyClasses()*10000)) + 1.0;
	      // 05/24/01 - adding a 'soft' cap here
	      oldcap = newgain;
	      double softmod = (1.0 - pow( 1.1 , -1.0*(gain/newgain))) + 1.0;     
	      // this gives us a range of 1-2
	      newgain *= softmod;
	      //newgain = (newgain*0.95) +  (((float)::number(0,100))*newgain)/1000.0;
	      // don't need this anymore since no hard cap - dash
	      if (gain < newgain)
		newgain = gain;
	      vlogf(LOG_DASH, "%s(L%d) vs %s(L%d)    (%5.2f soft <- %5.2f hard)",
                    ch->getName(), ch->getLevel(i),(ch->specials.fighting) ?  ch->specials.fighting->getName() : "n/a",
                    (ch->specials.fighting)?ch->specials.fighting->GetMaxLevel() : -1, (gain/newgain), (gain/oldcap));
	      vlogf(LOG_DASH, "   gain: %6.2f   oldc: %6.2f   newc: %6.2f   softm: %6.2f",
		    gain, oldcap, softmod, newgain);
	      gain = newgain;
            }
            // intentionally avoid having L50's get this message
            if ((ch->getExp() >= peak2) && (ch->GetMaxLevel() < MAX_MORT)) {
              ch->sendTo(COLOR_BASIC, "<R>You must gain at a guild or your exp will max 1 short of next level.<1>\n\r");
              ch->setExp(peak2);
              return;
            } else if (ch->getExp() >= peak) {
              // do nothing..this rules! Tell Brutius Hey, I didnt get any exp? 
            } else if ((ch->getExp() + gain >= peak) && (ch->GetMaxLevel() < MAX_MORT)) {
              ch->sendTo(COLOR_BASIC, "<G>You have gained enough to be a Level %d %s.<1>\n\r", 
			 ch->getLevel(i)+1, classNames[i].capName);
              ch->sendTo(COLOR_BASIC, "<R>You must gain at a guild or your exp will max 1 short of next level.<1>\n\r");
              if (ch->getExp() + gain >= peak2) {
                ch->setExp(peak2- 1);
                return;
              }
	    }
	    // if(gain > ((peak - curr) / gainmod)) {
#if 0
	    if (!been_here && gain > ((double)(dam)*(peak-curr))/(gainmod*(double)(ch->howManyClasses()*10000))+2.0 && dam > 0) { 
	      been_here = TRUE; // don't show multiple logs for multiclasses
	      // the 100 turns dam into a %
	      newgain = ((double)(dam)*(peak-curr))/(gainmod*(double)(ch->howManyClasses()*10000)) + 2.0;
	      vlogf(LOG_DASH, "%s(L%d) vs %s(L%d) cap: D: %d, E: %f, C: %f (%fx), MK: %f, %d classes",
		    ch->getName(), ch->getLevel(i), (ch->specials.fighting) ?  ch->specials.fighting->getName() : "n/a", 
		    (ch->specials.fighting) ?  ch->specials.fighting->GetMaxLevel() : -1, dam/100 + 1, gain,	
		    newgain, (gain/newgain), gainmod, ch->howManyClasses());
	    }  
#endif
	  }
	}
      }
#if 0
      // Theoretically, a players peak - curr / gainmod is extremely reasonable
      // for a veteran player hitting mobs above their level but not too far
      // We may need to watch for the backstabbers
      if(dam > 0) {
	double peak = getExpClassLevel(i,ch->getLevel(i) + 1);
	double curr = getExpClassLevel(i,ch->getLevel(i));
	double gainmod = ((ch->getLevel(i))) + 1.0; // removed +1
	newgain = ((double)(dam)*(peak-curr))/(gainmod*(double)(ch->howManyClasses()*10000)) + 2;
	// the 100 compensates for dam
	gain = min(gain, newgain); 
	//	gain += (rand()%(int)(gain*.1))-(gain*.05);
	gain = (gain*0.95) +  (((float)::number(0,100))*gain)/1000.0;
      }
#endif
      ch->addToExp(gain);
      // we check mortal level here...
      // an imm can go mort to test xp gain (theoretically)
      // for level > 50, the peak would have flipped over, which is bad
      if (ch->isPc() && ch->GetMaxLevel() <= MAX_MORT) {
	for (i = MIN_CLASS_IND; i < MAX_CLASSES; i++) {
	  double peak2 = getExpClassLevel(i,ch->getLevel(i) + 2);
	  if (ch->getLevel(i)) {
	    if (ch->getExp() > peak2)
              ch->setExp(peak2 - 1);
	  }
	}
      }
    }
    if (gain < 0) {
      ch->addToExp(gain);
      if (ch->getExp() < 0)
        ch->setExp(0);
    }
  }
}

void TFood::findSomeFood(TFood **last_good, TBaseContainer **last_cont, TBaseContainer *cont) 
{
  // get item closest to spoiling.
  if (*last_good && obj_flags.decay_time > (*last_good)->obj_flags.decay_time)
    return;

  *last_good = this;
  *last_cont = cont;
}

void TBeing::gainCondition(condTypeT condition, int value)
{
  int intoxicated, i = 0;
  char buf[160], tmpbuf[40], buf2[256];
  TThing *t = NULL;

  if (getCond(condition) == -1)        // No change 
    return;

  intoxicated = (getCond(DRUNK) > 0);

  if (value > 0) {
    // adjust for race
    // i.e. if code says I should gain 1, but I am an ogre, figure I may
    // only gain 0.5 or so...  Same situation, hobbits might get 2
    // this is racial metabolism vs human norm
    switch (condition) {
      case FULL:
        value = (int) (value * getMyRace()->getFoodMod()); 
        value = max(1, value);
	// lets take body mass into consideration for hunger
	value = (int) (value * 180.0 / getWeight());
	value = max(1, value);

        break;
      case THIRST:
	// lets take body mass into consideration for thirst
	value = (int) (value * 180.0 / getWeight());
	value = max(1, value);
	break;
      case DRUNK:
        value = (int) (value * getMyRace()->getDrinkMod()); 
        value = max(1, value);
	// lets take body mass into consideration for drunkenness
	// this may need some revision
	// At the momeny we are scaling linearly and taking drunkenness factors
	// normalized to a 180LB person
	value = (int) (value * 180.0 / getWeight());
	value = max(1, value);

	// modify for SKILL_ALCOHOLISM
	value = (int)((double) value * (double)(((double)(200 - getSkillValue(SKILL_ALCOHOLISM)) / 200)));

        break;
      case MAX_COND_TYPE:
        break;
    }
  }
  value = min(value, 24);

  setCond(condition, getCond(condition) + value);
  if (getCond(condition) < 0)
    setCond(condition, 0);
  if (getCond(condition) > 24)
    setCond(condition, 24);

  // this is just a warning message
  // if amt is 0, they get an actual message from the periodic stuff
  int amt = getCond(condition);
  if (amt <= 2 && amt > 0) {
    switch (condition) {
      case FULL:
        sendTo("You can feel your stomach rumbling.\n\r");
        break;
      case THIRST:
        sendTo("You begin to feel your mouth getting dry.\n\r");
        break;
      case MAX_COND_TYPE:
      case DRUNK:
        break;
    }
  }
  if (amt)
    return;

  // beyond here, auto eat

  if ((condition == FULL) && desc && (desc->autobits & AUTO_EAT) && awake() &&
      (!task || task->task == TASK_SIT || task->task == TASK_REST)) {
    // Check to see if they are in center square to use statue Russ 01/06/95
    if ((in_room == ROOM_CS) && (GetMaxLevel() <= 3)) {
      // execute instantly, makes no sense to que this
      parseCommand("pray statue", FALSE);
      return;
    }
    TFood *last_good = NULL;
    TBaseContainer *last_cont = NULL;
    for (i = MIN_WEAR; i < MAX_WEAR; i++) {
      if (!(t = equipment[i]))
        continue;

      t->findSomeFood(&last_good, &last_cont, NULL); 
    }
    for (t = stuff; t; t = t->nextThing) 
      t->findSomeFood(&last_good, &last_cont, NULL);
            
    if (last_good && !last_cont) {
      sprintf(buf, "eat %s", fname(last_good->name).c_str());
      addCommandToQue(buf);
    } else if (last_good && last_cont) {
      sprintf(buf, "%s", last_cont->name);
      add_bars(buf);
      strcpy(tmpbuf, fname(last_good->name).c_str());
      sprintf(buf2, "get %s %s", tmpbuf, buf);
      addCommandToQue(buf2);
      sprintf(buf2, "eat %s", tmpbuf);
      addCommandToQue(buf2);
    }
    return;
  }
  if ((condition == THIRST) && desc && (desc->autobits & AUTO_EAT) && awake() &&
      (!task || task->task == TASK_SIT || task->task == TASK_REST)) {
    // Check to see if fountain is in room and drink from that before
    // anything else - Russ 01/06/95
    for (t = roomp->stuff; t; t = t->nextThing) {
      if (dynamic_cast<TObj *>(t) && (t->spec == SPEC_FOUNTAIN)) {
        parseCommand("drink fountain", FALSE);
        return;
      }
    }
    TDrinkCon *last_good = NULL;
    TBaseContainer *last_cont = NULL;
    for (i = MIN_WEAR, last_good = NULL; i < MAX_WEAR; i++) {
      if (!(t = equipment[i]))
        continue;
      t->findSomeDrink(&last_good, &last_cont, NULL);
    }
    for (t = stuff; t; t = t->nextThing) 
      t->findSomeDrink(&last_good, &last_cont, NULL);
    
    if (last_good && !last_cont) {
      sprintf(buf, "drink %s", fname(last_good->name).c_str());
      addCommandToQue(buf);
      return;
    }
    if (last_good && last_cont) {
      sprintf(buf, "%s", last_cont->name);
      add_bars(buf);
      if (getPosition() == POSITION_RESTING) 
        addCommandToQue("sit");
            
      strcpy(tmpbuf, fname(last_good->name).c_str());
      sprintf(buf2, "get %s %s", tmpbuf, buf);
      addCommandToQue(buf2);
      sprintf(buf2, "drink %s", tmpbuf);
      addCommandToQue(buf2);
      sprintf(buf2, "put %s %s", tmpbuf, buf);
      addCommandToQue(buf2);
      return;
    }
  }
  switch (condition) {
    case DRUNK:
      if (intoxicated)
        sendTo("You are now sober.\n\r");
      return;
    default:
      break;
  }
}

// returns DELETE_THIS
int TBeing::checkIdling()
{
  if (desc && desc->connected >= CON_REDITING)
    return FALSE;

  if (isImmortal())
    return FALSE;

  if (getTimer() == 10) {
    if (specials.was_in_room == ROOM_NOWHERE &&
        inRoom() != ROOM_NOWHERE && inRoom() != ROOM_STORAGE) {
      specials.was_in_room = in_room;
      if (fight()) {
        if (fight()->fight())
          fight()->stopFighting();
        stopFighting();
      }
      act("$n disappears into the void.", TRUE, this, 0, 0, TO_ROOM);
      sendTo("You have been idle, and are pulled into a void.\n\r");
      --(*this);
      thing_to_room(this, ROOM_VOID);
      doSave(SILENT_NO);
    }
  } else if (getTimer() == 20) {
    if (in_room != ROOM_STORAGE) {
      if (in_room != ROOM_NOWHERE)
        --(*this);

      vlogf(LOG_SILENT, "%s booted from game for inactivity.", getName());
      thing_to_room(this, ROOM_DUMP);

      // this would be done by ~TPerson
      // but generates an error.  So we do it here in "safe" mode instead
      TPerson *tper = dynamic_cast<TPerson *>(this);
      if (tper)
        tper->dropItemsToRoom(SAFE_YES, NUKE_ITEMS);

      delete desc;
      desc = NULL;
      return DELETE_THIS;
    }
#if 0 
  }
#else 
  } else if (desc && desc->original && (desc->original->getTimer() >= 20)) {
    TBeing *mob = NULL;
    TBeing *per = NULL;

    if ((specials.act & ACT_POLYSELF)) {
      mob = this;
      per = desc->original;
      act("$n turns liquid, and reforms as $N.", TRUE, mob, 0, per, TO_ROOM);
      --(*per);
      *mob->roomp += *per;
      SwitchStuff(mob, per);
      per->polyed = POLY_TYPE_NONE;
    }
    desc->character = desc->original;
    desc->original = NULL;

    desc->character->desc = desc;
    desc = NULL;
    per->setTimer(10);
    per->checkIdling();
    if (IS_SET(specials.act, ACT_POLYSELF)) {
      // WTF is this for?  wouldn't a return make more sense?
forceCrash("does this get called?  checkIdle");

      // this would be done by ~TPerson
      // but generates an error.  So we do it here in "safe" mode instead
      TPerson *tper = dynamic_cast<TPerson *>(this);
      if (tper)
        tper->dropItemsToRoom(SAFE_YES, NUKE_ITEMS);

      return DELETE_THIS;
    }

  }
#endif
  return FALSE;
}

int TBeing::moneyMeBeing(TThing *mon, TThing *sub)
{
  int rc = mon->moneyMeMoney(this, sub);
  if (IS_SET_DELETE(rc, DELETE_THIS))
    return DELETE_ITEM;
  return FALSE;
}

int ObjFromCorpse(TObj *c)
{
  TThing *t, *t2, *t3;
  int rc;

  for (t = c->stuff; t; t = t2) {
    t2 = t->nextThing;
    --(*t);

    TObj *tobj = dynamic_cast<TObj *>(t);
    if (tobj && tobj->isObjStat(ITEM_NEWBIE) && !tobj->stuff &&
          (c->in_room > 80) && (c->in_room != ROOM_DONATION)) {
      sendrpf(c->roomp, "The %s explodes in a flash of white light!\n\r", fname(tobj->name).c_str());
      delete tobj;
      tobj = NULL;
      continue;
    }

    if ((t3 = c->parent)) {
      *(t3) += *t;
      rc = t3->moneyMeBeing(t, c);
      if (IS_SET_DELETE(rc, DELETE_ITEM))
        delete t;
    } else if (c->in_room != ROOM_NOWHERE)
      *c->roomp += *t;
  }
  return TRUE;
}

void TBeing::classSpecificStuff()
{
  if (hasClass(CLASS_WARRIOR)) 
    setMult(1.0);
  
  if (hasClass(CLASS_MONK)) {
    // from balance note
    // we desire monks to be getting 5/7 sqrt(lev) hits per round
    // barehand prof is linear from L1-L30 : n = 10/3 * L 
    // barehand spec is linear roughly L31-L50 n = (L-30) * 5
    // solve for L as a function of n
    double value = 0;
    if (doesKnowSkill(SKILL_BAREHAND_PROF))
      value += 3.0 * getSkillValue(SKILL_BAREHAND_PROF) / 10.0;
    if (doesKnowSkill(SKILL_BAREHAND_SPEC))
      value += getSkillValue(SKILL_BAREHAND_SPEC) / 5.0;

    // value is roughly their actual level, but based on skill instead
    value = min(max(1.0, value), 50.0);
 
    // now make it into numHits
    value = 5.0 * sqrt(value) / 7.0;

    // adjust for speed
    value = value * plotStat(STAT_CURRENT, STAT_SPE, 0.8, 1.25, 1.0);

    // give at least 1 hit per hand
    // reverse engineering, we realize 2.0 comes around L7.8 = 26%barehand
    // so this is also a newbie normalization
    value = max(value, 2.0);

    setMult(value);
  }
}

// computes the time it takes to get back 1 point of hp,mana and move
// it uses worst of the three
int TBeing::regenTime()
{
  int iTime = 100;

  if (getHit() < hitLimit())
    iTime = min(iTime, hitGain());

  if (getMove() < moveLimit())
    iTime = min(iTime, moveGain());

  if (getMana() < manaLimit())
    iTime = min(iTime, manaGain());

  iTime = max(iTime, 1);

  // iTime now holds the lesser of my mana/move/hp gain (per update)
  // regen time for 1 point should simply be the inverse multiplied
  // by how long a points-update is
  iTime = PULSE_UPDATE / iTime;
  return iTime;
}

int TBeing::hpGainForClass(classIndT Class) const
{
  int hpgain = 0;

  // add for classes first
  if (hasClass(CLASS_MAGIC_USER) && Class == MAGE_LEVEL_IND)
    hpgain += ::number(3,7); // old 2,8

  if (hasClass(CLASS_CLERIC) && Class == CLERIC_LEVEL_IND)
    hpgain += ::number(5,7); // old 4,8

  if (hasClass(CLASS_WARRIOR) && Class == WARRIOR_LEVEL_IND)
    hpgain += ::number(6,11); // old 5,12

  if (hasClass(CLASS_THIEF) && Class == THIEF_LEVEL_IND)
    hpgain += ::number(4,8); // old 3,9

  if (hasClass(CLASS_DEIKHAN) && Class == DEIKHAN_LEVEL_IND)
    hpgain += ::number(6,9); // old 5,10

  if (hasClass(CLASS_MONK) && Class == MONK_LEVEL_IND)
    hpgain += ::number(4,7); // old 3,8

  if (hasClass(CLASS_RANGER) && Class == RANGER_LEVEL_IND)
    hpgain += ::number(6,8); // old 5,9

  if (hasClass(CLASS_SHAMAN) && Class == SHAMAN_LEVEL_IND)
    hpgain += ::number(3,8); 

  return hpgain;
}


int TBeing::hpGainForLevel(classIndT Class) const
{
  double hpgain = 0;

  hpgain = (double) hpGainForClass(Class);  

  hpgain *= (double) getConHpModifier();

  hpgain /= (double) howManyClasses();

  if(roll_chance(hpgain - (int) hpgain)){
    hpgain += 1.0;
  }

  return max(1, (int)hpgain);
}


