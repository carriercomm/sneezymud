

#ifndef __DISC_SHAMAN_SKUNK_H
#define __DISC_SHAMAN_SKUNK_H


class CDShamanSkunk : public CDiscipline
{
public:
    CSkill skTurnSkill;

    CDShamanSkunk()
      : CDiscipline(),
      skTurnSkill() {
    }
    CDShamanSkunk(const CDShamanSkunk &a)
      : CDiscipline(a),
      skTurnSkill(a.skTurnSkill) {
    }
    CDShamanSkunk & operator=(const CDShamanSkunk &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skTurnSkill = a.skTurnSkill;
      return *this;
    }
    virtual ~CDShamanSkunk() {}
    virtual CDShamanSkunk * cloneMe() { return new CDShamanSkunk(*this); }

private:
};


#endif

