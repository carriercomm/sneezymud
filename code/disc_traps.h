//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DISC_TRAPS_H
#define __DISC_TRAPS_H

// This is the THIEF TRAPS discipline.

class CDTraps : public CDiscipline
{
public:
    CSkill skSetTrapsCont;   
    CSkill skSetTrapsDoor;   
    CSkill skSetTrapsMine;   
    CSkill skSetTrapsGren;   

    CDTraps()
      : CDiscipline(),
      skSetTrapsCont(),
      skSetTrapsDoor(),
      skSetTrapsMine(),
      skSetTrapsGren()
    {
    }
    CDTraps(const CDTraps &a)
      : CDiscipline(a),
      skSetTrapsCont(a.skSetTrapsCont),
      skSetTrapsDoor(a.skSetTrapsDoor),
      skSetTrapsMine(a.skSetTrapsMine),
      skSetTrapsGren(a.skSetTrapsGren)
    {
    }
    CDTraps & operator=(const CDTraps &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skSetTrapsCont = a.skSetTrapsCont;
      skSetTrapsDoor = a.skSetTrapsDoor;
      skSetTrapsMine = a.skSetTrapsMine;
      skSetTrapsGren = a.skSetTrapsGren;
      return *this;
    }
    virtual ~CDTraps() {}
    virtual CDTraps * cloneMe() { return new CDTraps(*this); }
private:
};


#endif
