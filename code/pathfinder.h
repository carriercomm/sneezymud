#ifndef __PATHFINDER_H
#define __PATHFINDER_H

class TPathTarget {
 public:
  virtual bool isTarget(int) const {
    return false;
  }

  virtual ~TPathTarget(){};
};


// findRoom(int vnum) = find room specified by vnum
class findRoom : public TPathTarget {
 private:
  int dest;

 public:
  findRoom(int);
  bool isTarget(int) const;
};

// findClutter(myself) = find closest room with stuff for a janitor to pick up
class findClutter : public TPathTarget {
 private:
  TBeing *myself;

 public:
  findClutter(TBeing *);
  bool isTarget(int) const;

};

// findPolice() = find closest police (guard or bouncer)
class findPolice : public TPathTarget {
 public:
  findPolice();
  bool isTarget(int) const;
};

// findOutdoors() = find closest outdoors room
class findOutdoors : public TPathTarget {
 public:
  findOutdoors();
  bool isTarget(int) const;
};

// findCorpse() = find the closest corpse in Grimhaven
class findCorpse : public TPathTarget {
 public:
  findCorpse();
  bool isTarget(int) const;
};

// findFire() = find closest fire in grimhaven
class findFire : public TPathTarget {
 public:
  findFire();
  bool isTarget(int) const;
};

// findBeing(sstring) = find closest being specified by name
class findBeing : public TPathTarget {
 private:
  sstring name;
 public:
  findBeing(sstring);
  bool isTarget(int) const;
};

// findWater() = find closest source of water
class findWater : public TPathTarget {
 public:
  findWater();
  bool isTarget(int) const;
};



class TPathFinder {
 private:
  TPathTarget pt;
  bool thru_doors;
  bool use_portals;
  int range;
  bool stay_zone;
  bool no_mob;

  int dest;
  int dist;

 public:
  void setRange(int);
  void setThruDoors(bool t){ thru_doors=t; }
  void setStayZone(bool t){ stay_zone=t; }
  void setUsePortals(bool t){ use_portals=t; }
  void setNoMob(bool t){ no_mob=t; }

  int getDest(){ return dest; }
  int getDist(){ return dist; }

  dirTypeT findPath(int, const TPathTarget &);

  TPathFinder();
  TPathFinder(int depth);

};




class pathData {
  public:
    dirTypeT direct;
    int source;
    bool checked;
    int distance;
    pathData() : direct(DIR_NONE), source(0), checked(false), distance(0) {}
    pathData(dirTypeT d, int s, bool c, int dist) : 
      direct(d), source(s), checked(c), distance(dist) {}
};



extern int go_ok(roomDirData *exitp);
extern int go_ok_smarter(roomDirData *exitp);



#endif