#ifndef __DISC_SHAMAN_ARMADILLO_H
#define __DISC_SHAMAN_ARMADILLO_H

// This is the ARMADILLO discipline for shaman.

class CDShamanArmadillo : public CDiscipline
{
public:
    CSkill skThornflesh;
    CSkill skAqualung;

    CDShamanArmadillo() 
      : CDiscipline(),
      skThornflesh(),
      skAqualung() {
    }
    CDShamanArmadillo(const CDShamanArmadillo &a) 
      : CDiscipline(a),
      skThornflesh(a.skThornflesh),
      skAqualung(a.skAqualung) {
    }
    CDShamanArmadillo & operator=(const CDShamanArmadillo &a)  {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skThornflesh = a.skThornflesh;
      skAqualung = a.skAqualung;
      return *this;
    }
    virtual ~CDShamanArmadillo() {}
    virtual CDShamanArmadillo * cloneMe() { return new CDShamanArmadillo(*this); }
private:
};

    int thornflesh(TBeing *);
    int castThornflesh(TBeing *);
    int thornflesh(TBeing *, int, byte);

    int aqualung(TBeing *, TBeing *);
    int castAqualung(TBeing *, TBeing *);
    void aqualung(TBeing *, TBeing *, TMagicItem *);
    int aqualung(TBeing *, TBeing *, int, byte);

#endif
