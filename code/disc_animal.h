#ifndef __DISC_ANIMAL_H
#define __DISC_ANIMAL_H

// This is the ANIMAL discipline.

class CDAnimal : public CDiscipline
{
public:
    CSkill skBeastCharm;

    CDAnimal()
      : CDiscipline(),
        skBeastCharm() {
    }
    CDAnimal(const CDAnimal &a)
      : CDiscipline(a),
        skBeastCharm(a.skBeastCharm) {
    }
    CDAnimal & operator=(const CDAnimal &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skBeastCharm = a.skBeastCharm;
      return *this;
    }
    virtual ~CDAnimal() {}
    virtual CDAnimal * cloneMe() { return new CDAnimal(*this); }
private:
};

    int beastSoother(TBeing *, TBeing *);
    int beastSoother(TBeing *, TBeing *, TMagicItem *);
    int beastSoother(TBeing *, TBeing *, int, byte);
 
    int transfix(TBeing *, TBeing *);
    int transfix(TBeing *, TBeing *, int, byte);
 
    int beastSummon(TBeing *, const char *);
    int beastSummon(TBeing *, const char *, int, byte);


#endif
