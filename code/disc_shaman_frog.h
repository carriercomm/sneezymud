#ifndef __DISC_SHAMAN_FROG_H
#define __DISC_SHAMAN_FROG_H


class CDShamanFrog : public CDiscipline
{
public:
    CSkill skStormySkies;
    CSkill skAquaticBlast;
    CSkill skShapeShift;

    CDShamanFrog() 
      : CDiscipline(),
      skStormySkies(),
      skAquaticBlast(),
      skShapeShift() {
    }
    CDShamanFrog(const CDShamanFrog &a) 
      : CDiscipline(a),
      skStormySkies(a.skStormySkies),
      skAquaticBlast(a.skAquaticBlast),
      skShapeShift(a.skShapeShift) {
    }
    CDShamanFrog & operator=(const CDShamanFrog &a)  {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skStormySkies = a.skStormySkies;
      skAquaticBlast = a.skAquaticBlast;
      skShapeShift = a.skShapeShift;
      return *this;
    }
    virtual ~CDShamanFrog() {}
    virtual CDShamanFrog * cloneMe() { return new CDShamanFrog(*this); }
private:
};

    int stormySkies(TBeing *, TBeing *, int, byte);
    int stormySkies(TBeing *, TBeing *, TMagicItem *);
    int stormySkies(TBeing *, TBeing *);

    int aquaticBlast(TBeing *, TBeing *);
    int castAquaticBlast(TBeing *, TBeing *);
    int aquaticBlast(TBeing *, TBeing *, TMagicItem *);
    int aquaticBlast(TBeing *, TBeing *, int, byte, int);

    int shapeShift(TBeing *caster, int level, byte bKnown);
    int shapeShift(TBeing *caster, const char * buffer);
    int castShapeShift(TBeing *caster);


#endif


