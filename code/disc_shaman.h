//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_shaman.h,v $
// Revision 5.1.1.2  2001/01/11 19:56:53  jesus
// shaman stuff
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


#ifndef __DISC_SHAMAN_H
#define __DISC_SHAMAN_H

// This is the SHAMAN discipline.

class CDShaman : public CDiscipline
{
public:
    CSkill skShieldOfMists;
    CSkill skRootControl;
    CSkill skEnthrallSpectre;
    CSkill skEnthrallGhast;
    CSkill skEnthrallGhoul;
    CSkill skEnthrallDemon;
    CSkill skCacaodemon;
    CSkill skVoodoo;
    CSkill skControlUndead;
    CSkill skLivingVines;
    CSkill skResurrection;
    CSkill skDancingBones;
    CDShaman()
      : CDiscipline(),
      skShieldOfMists(),
      skRootControl(),
      skEnthrallSpectre(),
      skEnthrallGhast(),
      skEnthrallGhoul(),
      skEnthrallDemon(),
      skCacaodemon(),
      skVoodoo(),
      skControlUndead(),
      skLivingVines(),
      skResurrection(),
      skDancingBones() { 
    }
    CDShaman(const CDShaman &a)
      : CDiscipline(a),
      skShieldOfMists(a.skShieldOfMists),
      skRootControl(a.skRootControl),
      skEnthrallSpectre(a.skEnthrallSpectre),
      skEnthrallGhast(a.skEnthrallGhast),
      skEnthrallGhoul(a.skEnthrallGhoul),
      skEnthrallDemon(a.skEnthrallDemon),
      skCacaodemon(a.skCacaodemon),
      skVoodoo(a.skVoodoo),
      skControlUndead(a.skControlUndead),
      skLivingVines(a.skLivingVines),
      skResurrection(a.skResurrection),
      skDancingBones(a.skDancingBones) {
    }
    CDShaman & operator=(const CDShaman &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skShieldOfMists = a.skShieldOfMists;
      skRootControl = a.skRootControl;
      skEnthrallSpectre = a.skEnthrallSpectre;
      skEnthrallGhast = a.skEnthrallGhast;
      skEnthrallGhoul = a.skEnthrallGhoul;
      skEnthrallDemon = a.skEnthrallDemon;
      skCacaodemon = a.skCacaodemon;
      skVoodoo = a.skVoodoo;
      skControlUndead = a.skControlUndead;
      skLivingVines = a.skLivingVines;
      skResurrection = a.skResurrection;
      skDancingBones = a.skDancingBones;
      return *this;
    }
    virtual ~CDShaman() {}
    virtual CDShaman * cloneMe() { return new CDShaman(*this); }
private:
};

    int createGolem(TBeing *);
    int createGolem(TBeing *, int, int, int, byte);

    void controlUndead(TBeing *, TBeing *);
    void controlUndead(TBeing *, TBeing *, TMagicItem *);
    int controlUndead(TBeing *, TBeing *, int, byte);

    int voodoo(TBeing *, TObj *);
    int voodoo(TBeing *, TObj *, TMagicItem *);
    int voodoo(TBeing *, TObj *, int, byte);

    int dancingBones(TBeing *, TObj *);
    int dancingBones(TBeing *, TObj *, TMagicItem *);
    int dancingBones(TBeing *, TObj *, int, byte);

    int enthrallSpectre(TBeing * caster, int level, byte bKnown);
    int enthrallSpectre(TBeing * caster);
    int castEnthrallSpectre(TBeing * caster);

    int enthrallGhast(TBeing * caster, int level, byte bKnown);
    int enthrallGhast(TBeing * caster);
    int castEnthrallGhast(TBeing * caster);

    int enthrallGhoul(TBeing * caster, int level, byte bKnown);
    int enthrallGhoul(TBeing * caster);
    int castEnthrallGhoul(TBeing * caster);

    int enthrallDemon(TBeing * caster, int level, byte bKnown);
    int enthrallDemon(TBeing * caster);
    int castEnthrallDemon(TBeing * caster);

    void cacaodemon(TBeing *, const char *);
    int cacaodemon(TBeing *, const char *, int, byte);

    int resurrection(TBeing *, TObj *);
    int resurrection(TBeing *, TObj *, TMagicItem *);
    int resurrection(TBeing *, TObj *, int, byte);

    int shieldOfMists(TBeing *, TBeing *);
    int castShieldOfMists(TBeing *, TBeing *);
    void shieldOfMists(TBeing *, TBeing *, TMagicItem *);
    int shieldOfMists(TBeing *, TBeing *, int, byte);

#endif

