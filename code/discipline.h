//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: discipline.h,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DISCIPLINE_H
#define __DISCIPLINE_H 

#ifndef __SPELLS_H
#error Please include SPELLS.H before DISCIPLINE.H
#endif

// technically, these should be unsigned.
// I got lazy and didn't want to change all the return types and vars
// that this would impact in all the spells though.
const int SPELL_CRIT_SUCCESS   = (1<<0);
const int SPELL_SUCCESS        = (1<<1);
const int SPELL_FAIL           = (1<<2);
const int SPELL_CRIT_FAIL      = (1<<3);
const int SPELL_CRIT_FAIL_2    = (1<<4);
const int ITEM_DEAD            = ((1<<5));
const int CASTER_DEAD          = ((1<<6));
const int VICTIM_DEAD          = ((1<<7));
const int VICTIM_ALREADY_DELETED = ((1<<8));
const int SPELL_STOP_PARSING     = ((1<<9));

const int SPELL_FALSE          = (1<<10);
const int SPELL_ACTION         = (1<<11);
const int SPELL_CSUC_DEFAULT   = (1<<12);
const int SPELL_CSUC_DOUBLE    = (1<<13);
const int SPELL_CSUC_TRIPLE    = (1<<14);
const int SPELL_CSUC_KILL      = (1<<15);
const int SPELL_CFAIL_DEFAULT  = (1<<16);
const int SPELL_CFAIL_SELF     = (1<<17);
const int SPELL_CFAIL_OTHER    = (1<<18);
const int SPELL_SAVE           = (1<<19);
const int SPELL_FAIL_SAVE      = (1<<20);

// This is the base class for all other disciplines
// Changed max_skill from 95 - Rix 10/27/97
const byte MAX_SKILL_LEARNEDNESS=100;
const byte MAX_DISC_LEARNEDNESS=100;
const byte LEARNEDNESS_STEP=1;

// pracs to max a disc
const int PRACS_TO_MAX      = 60;

const char *describe_level(int n);
const char *describe_damage(int n);
const char *describe_armor(int n);
const char *describe_light(int n);
const char *what_does_it_open(const TKey *o);

// defines for TBEing::discs[] array

enum discNumT {
     DISC_NONE = -1,
     DISC_MAGE = 0,
     DISC_AIR,
     DISC_ALCHEMY,
     DISC_EARTH,
     DISC_FIRE,
     DISC_SORCERY,
     DISC_SPIRIT,
     DISC_WATER,
     DISC_CLERIC,
     DISC_WRATH,
     DISC_AFFLICTIONS,
     DISC_AEGIS,
     DISC_CURES,
     DISC_HAND_OF_GOD,
     DISC_WARRIOR,
     DISC_HTH,
     DISC_BRAWLING,
     DISC_PHYSICAL,
     DISC_SMYTHE,
     DISC_RANGER,
     DISC_RANGER_FIGHT,
     DISC_NATURE,
     DISC_ANIMAL,
     DISC_PLANTS,
     DISC_SURVIVAL,
     DISC_DEIKHAN,
     DISC_DEIKHAN_FIGHT,
     DISC_MOUNTED,
     DISC_DEIKHAN_AEGIS,
     DISC_DEIKHAN_CURES,
     DISC_DEIKHAN_WRATH,
     DISC_MONK,
     DISC_MEDITATION_MONK,
     DISC_LEVERAGE,
     DISC_MINDBODY,
     DISC_FOCUSED_ATTACKS,
     DISC_BAREHAND,
     DISC_THIEF,
     DISC_THIEF_FIGHT,
     DISC_MURDER,
     DISC_LOOTING,
     DISC_POISONS,
     DISC_STEALTH,
     DISC_TRAPS,
     DISC_SHAMAN,
     DISC_SHAMAN_FIGHT,
     DISC_SHAMAN_ALCHEMY,
     DISC_SHAMAN_HEALING,
     DISC_UNDEAD,
     DISC_DRAINING,
     DISC_TOTEM,
     DISC_WIZARDRY,
     DISC_FAITH,
     DISC_SLASH,
     DISC_BLUNT,
     DISC_PIERCE,
     DISC_RANGED,
     DISC_COMBAT,
     DISC_ADVENTURING,
     DISC_THEOLOGY,
     DISC_LORE,
     MAX_DISCS,
     MAX_SAVED_DISCS = 90
};
const discNumT MIN_DISC = discNumT(0);
extern discNumT & operator++(discNumT &, int);

class CDiscipline
{
  private:
    int uNatLearnedness;
    int uLearnedness;
    int uDoLearnedness;
  public:
    int ok_for_class;
    CDiscipline();
    CDiscipline(const CDiscipline &a);
    CDiscipline & operator= (const CDiscipline &a);
    virtual ~CDiscipline();

    virtual CDiscipline * cloneMe() = 0;  // a virtual constructor
    int getDoLearnedness() const;
    void setDoLearnedness(int);
    void setNatLearnedness(int);
    int getNatLearnedness() const;
    int getLearnedness() const;
    void setLearnedness(int);
    int useMana(byte, byte);
    double usePerc(byte, double);
};

class CMasterDiscipline
{
  public:
    CDiscipline *disc[MAX_DISCS]; 

    CMasterDiscipline();
    CMasterDiscipline(const CMasterDiscipline &a);
    CMasterDiscipline & operator=(const CMasterDiscipline &a);
    ~CMasterDiscipline();
};

discNumT getDisciplineNumber(spellNumT, int);
extern bool enforceHolySym(const TBeing *ch, spellNumT spell, bool checkDamage);
void CS(spellNumT spell_num);
void LogDam(const TBeing *, spellNumT spell_num, int);
void CF(spellNumT spell_num);
void SV(spellNumT spell_num);
void CS(const TBeing *, spellNumT spell_num);
void CF(const TBeing *, spellNumT spell_num);
void SV(const TBeing *, spellNumT spell_num);

//const struct disc_names_data
struct disc_names_data
{
  discNumT disc_num;
  int class_num;
  const char * const practice;
};

//const struct pc_class_names_data 
struct pc_class_names_data 
{
  classIndT class_lev_num;
  int class_num;
  const char * const name;
  const char * const capName;
};
      
#endif
