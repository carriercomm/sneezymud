//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_warrior.h,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DISC_WARRIOR_H
#define __DISC_WARRIOR_H

// This is the WARRIOR discipline.

class CDWarrior : public CDiscipline
{
public:
// Level 2
    CSkill skKick;
//Level 10
    CSkill skBash;
//Level 12
    CSkill skHeadbutt;

//Level 15
    CSkill skRescue;

//Level 20
    CSkill skSmythe;
    CSkill skDisarm;
//Level 25
    CSkill skBerserk;
    CSkill skSwitch;

    CSkill skKneestrike;

    CDWarrior()
      : CDiscipline(),
      skKick(),
      skBash(),
      skHeadbutt(),
      skRescue(),
      skSmythe(),
      skDisarm(),
      skBerserk(),
      skSwitch(),
      skKneestrike() {
    }
    CDWarrior(const CDWarrior &a)
      : CDiscipline(a),
      skKick(a.skKick),
      skBash(a.skBash),
      skHeadbutt(a.skHeadbutt),
      skRescue(a.skRescue),
      skSmythe(a.skSmythe),
      skDisarm(a.skDisarm),
      skBerserk(a.skBerserk),
      skSwitch(a.skSwitch),
      skKneestrike(a.skKneestrike) {
    }
    CDWarrior & operator=(const CDWarrior &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skKick = a.skKick;
      skBash = a.skBash;
      skHeadbutt = a.skHeadbutt;
      skRescue = a.skRescue;
      skSmythe = a.skSmythe;
      skDisarm = a.skDisarm;
      skBerserk = a.skBerserk;
      skSwitch = a.skSwitch;
      skKneestrike = a.skKneestrike;
      return *this;
    }
    virtual ~CDWarrior() {}
    virtual CDWarrior * cloneMe() { return new CDWarrior(*this); }
private:
};

    int berserk(TBeing *);
    void repair(TBeing *, TObj *);

#endif

