//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: disc_thief.h,v $
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DISC_THIEF_H
#define __DISC_THIEF_H

// This is the BASIC THIEF discipline.

class CDThief : public CDiscipline
{
public:
    CSkill skSwindle;
    CSkill skSneak;
    CSkill skStabbing;
    CSkill skRetreatThief;
    CSkill skKickThief;
    CSkill skPickLock;
    CSkill skBackstab;
    CSkill skSearch;
    CSkill skSpy;
    CSkill skSwitchThief;
    CSkill skSteal;
    CSkill skDetectTraps;
    CSkill skSubterfuge;
    CSkill skDisarmTraps;
    CSkill skCudgel;
    CSkill skHide;
    CSkill skDisarmThief;

    CDThief()
      : CDiscipline(),
      skSwindle(),
      skSneak(),
      skStabbing(),
      skRetreatThief(),
      skKickThief(),
      skPickLock(),
      skBackstab(),
      skSearch(),
      skSpy(),
      skSwitchThief(),
      skSteal(),
      skDetectTraps(),
      skSubterfuge(),
      skDisarmTraps(),
      skCudgel(),
      skHide(),
      skDisarmThief() {
    }
    CDThief(const CDThief &a)
      : CDiscipline(a),
      skSwindle(a.skSwindle),
      skSneak(a.skSneak),
      skStabbing(a.skStabbing),
      skRetreatThief(a.skRetreatThief),
      skKickThief(a.skKickThief),
      skPickLock(a.skPickLock),
      skBackstab(a.skBackstab),
      skSearch(a.skSearch),
      skSpy(a.skSpy),
      skSwitchThief(a.skSwitchThief),
      skSteal(a.skSteal),
      skDetectTraps(a.skDetectTraps),
      skSubterfuge(a.skSubterfuge),
      skDisarmTraps(a.skDisarmTraps),
      skCudgel(a.skCudgel),
      skHide(a.skHide),
      skDisarmThief(a.skDisarmThief) {
    }
    CDThief & operator=(const CDThief &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skSwindle = a.skSwindle;
      skSneak = a.skSneak;
      skStabbing = a.skStabbing;
      skRetreatThief = a.skRetreatThief;
      skKickThief = a.skKickThief;
      skPickLock = a.skPickLock;
      skBackstab = a.skBackstab;
      skSearch = a.skSearch;
      skSpy = a.skSpy;
      skSwitchThief = a.skSwitchThief;
      skSteal = a.skSteal;
      skDetectTraps = a.skDetectTraps;
      skSubterfuge = a.skSubterfuge;
      skDisarmTraps = a.skDisarmTraps;
      skCudgel = a.skCudgel;
      skHide = a.skHide;
      skDisarmThief = a.skDisarmThief;
      return *this;
    }
    virtual ~CDThief() {}
    virtual CDThief * cloneMe() { return new CDThief(*this); }
private:
};

    int sneak(TBeing *, spellNumT);
    int subterfuge(TBeing *, TBeing *);
    int pickLocks(TBeing *, const char *, const char *, const char *);
    int spy(TBeing *);
    int hide(TBeing *, spellNumT);
    int disguise(TBeing *, char *);

#endif

