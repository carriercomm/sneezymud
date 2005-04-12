//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __PERSON_H
#define __PERSON_H

#include "being.h"

extern Descriptor *descriptor_list;
extern Descriptor *next_to_process;

class TPerson : public TBeing {
  private:
    ush_int base_age;
    TPerson();  // made private to make it uncallable

  public:
    FILE *tLogFile;
    sstring lastHost;
    sstring title; 
    int last_rent;
    //    bool toggles[MAX_TOG_INDEX];
    bool wizPowers[MAX_POWER_INDEX];
    unsigned int timer;

    TPerson(Descriptor *);
    TPerson(const TPerson &);
    TPerson & operator=(const TPerson &);
    virtual ~TPerson();

    virtual const sstring getName() const { return name; }

    void setBaseAge(ush_int num) {
      base_age = num;
    }
    virtual ush_int getBaseAge() const {
      return base_age;
    }
    virtual int getWait() const {
      if (desc && !isImmortal())
        return desc->wait;
      return 0;
    }
    virtual void setWait(int num) { 
      if (desc)
        desc->wait = num;
    }
    virtual unsigned int getTimer() const {
      return timer;
    }
    virtual void setTimer(unsigned int num) { 
      timer = num;
    }
    virtual void addToTimer(unsigned int num) { 
      timer += num;
    }

    void storeToSt(charFile *);
    void autoDeath();
    void resetChar();
    int genericLoadPC();
    void loadFromSt(charFile *);
    void initChar();
    void doStart();
    void setDimensions();
    void setTitle(bool);
    void rentAffectTo(saveAffectedData *);
    void setSelectToggles(TBeing *, classIndT, silentTypeT);
    void advanceSelectDisciplines(classIndT, int, silentTypeT);
    void doLevelSkillsLearn(discNumT, int, int);
    void setBaseAge();   // sets new base-age
    void saveRent(objCost *, bool, int);
    void loadRent();

    virtual void raiseLevel(classIndT);
    virtual void doUsers(const sstring &);
    virtual void doInvis(const sstring &);
    virtual void doVisible(const sstring &, bool);
    virtual void doMedit(const sstring &);
    virtual void doSEdit(const sstring &);
    virtual void doOEdit(const sstring &);
    virtual void doEdit(const sstring &);
    virtual void doRload(const sstring &);
    virtual void doRsave(const sstring &);
    virtual void doRedit(const sstring &);
    virtual void doTrans(const sstring &);
    virtual int doAt(const sstring &, bool);
    virtual void doAccess(const sstring &);
    virtual void doSet(const sstring &);
    virtual void doLow(const sstring &);
    virtual void doShutdow();
    virtual void doShutdown(const sstring &);
    virtual void doSnoop(const sstring &);
    virtual void doSwitch(const sstring &);
    virtual void doForce(const sstring &);
    virtual void doLoad(const sstring &);
    virtual void doCutlink(const sstring &);
    virtual void doPurge(const sstring &);
    virtual short int manaLimit() const;
    virtual short int hitLimit() const;
    virtual void setMaxHit(int);
    virtual int getMaxMove() const;
    virtual void reconcileHelp(TBeing *, double);
    virtual void reconcileHurt(TBeing *, double);
    virtual void doTitle(const sstring &);
    virtual sstring parseTitle(Descriptor *);
    virtual void addToWait(int);
    virtual void doBug(const sstring &);
    virtual void doIdea(const sstring &);
    virtual void doTypo(const sstring &);
    virtual bool hasQuestBit(int) const;
    virtual void setQuestBit(int);
    virtual void remQuestBit(int);
    void saveToggles();
    void loadToggles();
    virtual bool hasWizPower(wizPowerT) const;
    virtual void setWizPower(wizPowerT);
    virtual void remWizPower(wizPowerT);
    void saveWizPowers();
    void loadWizPowers();
    virtual void doColor(const sstring &);
    virtual void doChange(const sstring &);
    virtual void doStat(const sstring &);
    virtual void doShow(const sstring &);
    virtual bool isPc() const { return TRUE; }
    virtual void logf(const char *, ...);
    virtual int manaGain();
    virtual int hitGain();
    virtual int doQuit2();
    virtual void wizFileSave();
    virtual bool isDragonRideable() const { return false; }
    virtual void failCharm(TBeing *);
    virtual int learnFromDoingUnusual(learnUnusualTypeT, spellNumT, int);
    virtual int learnFromDoing(spellNumT, silentTypeT, unsigned int);
    virtual immortalTypeT isImmortal(int level = GOD_LEVEL1) const;
    void dropItemsToRoom(safeTypeT, dropNukeT);
    virtual TThing& operator+= (TThing& t);
    virtual void doPowers(const sstring &) const;
    void startLevels();
    void advanceLevel(classIndT);
    void doHPGainForLev(classIndT);
};

#endif
