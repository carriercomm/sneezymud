
#ifndef __DISC_SHAMAN_CONTROL_H
#define __DISC_SHAMAN_CONTROL_H


class CDShamanControl : public CDiscipline
{
public:
    CSkill skEnthrallGhoul;
    CSkill skEnthrallDemon;
    CSkill skCacaodemon;
    CSkill skCreateGolem;

    CDShamanControl()
      : CDiscipline(),
      skEnthrallGhoul(),
      skEnthrallDemon(),
      skCacaodemon(),
      skCreateGolem() {
    }
    CDShamanControl(const CDShamanControl &a)
      : CDiscipline(a),
      skEnthrallGhoul(a.skEnthrallGhoul),
      skEnthrallDemon(a.skEnthrallDemon),
      skCacaodemon(a.skCacaodemon),
      skCreateGolem(a.skCreateGolem) {
    }
    CDShamanControl & operator=(const CDShamanControl &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skEnthrallGhoul = a.skEnthrallGhoul;
      skEnthrallDemon = a.skEnthrallDemon;
      skCacaodemon = a.skCacaodemon;
      skCreateGolem = a.skCreateGolem;
      return *this;
    }
    virtual ~CDShamanControl() {}
    virtual CDShamanControl * cloneMe() { return new CDShamanControl(*this); }
private:
};
    int enthrallGhoul(TBeing * caster, int level, byte bKnown);
    int enthrallGhoul(TBeing * caster);
    int castEnthrallGhoul(TBeing * caster);

    int enthrallDemon(TBeing * caster, int level, byte bKnown);
    int enthrallDemon(TBeing * caster);
    int castEnthrallDemon(TBeing * caster);

    void cacaodemon(TBeing *, const char *);
    int cacaodemon(TBeing *, const char *, int, byte);

    int createGolem(TBeing *);
    int createGolem(TBeing *, int, int, int, byte);

#endif
