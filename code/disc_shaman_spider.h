
#ifndef __DISC_SHAMAN_SPIDER_H
#define __DISC_SHAMAN_SPIDER_H

class CDShamanSpider : public CDiscipline
{
public:
    CSkill skSticksToSnakes;
    CSkill skControlUndead;

    CDShamanSpider()
      : CDiscipline(),
      skSticksToSnakes(),
      skControlUndead() { 
    }
    CDShamanSpider(const CDShamanSpider &a)
      : CDiscipline(a),
      skSticksToSnakes(a.skSticksToSnakes),
      skControlUndead(a.skControlUndead) {
    }
    CDShamanSpider & operator=(const CDShamanSpider &a) {
      if (this == &a) return *this;
      CDiscipline::operator=(a);
      skSticksToSnakes = a.skSticksToSnakes;
      skControlUndead = a.skControlUndead;
      return *this;
    }
    virtual ~CDShamanSpider() {}
    virtual CDShamanSpider * cloneMe() { return new CDShamanSpider(*this); }
private:
};

    void controlUndead(TBeing *, TBeing *);
    void controlUndead(TBeing *, TBeing *, TMagicItem *);
    int controlUndead(TBeing *, TBeing *, int, byte);

    int sticksToSnakes(TBeing *, TBeing *);
    int sticksToSnakes(TBeing *, TBeing *, TMagicItem *);
    int sticksToSnakes(TBeing *, TBeing *, int, byte);

#endif

