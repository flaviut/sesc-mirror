/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by  Smruti Sarangi
                   Jose Renau
                 
This file is part of SESC.

SESC is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2, or (at your option) any later version.

SESC is    distributed in the  hope that  it will  be  useful, but  WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of  the GNU General  Public License along with
SESC; see the file COPYING.  If not, write to the  Free Software Foundation, 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef GENERGYD_H
#define GENERGYD_H

#include <stdio.h>
#include <ctype.h>

#include "estl.h"
#include "GStats.h"
#include "Config.h"
#include "SescConf.h"

enum EnergyGroup {
  NOT_VALID_ENERGY
  ,BPredEnergy
  ,RasEnergy
  ,BTBEnergy
  ,WindowPregEnergy
  ,WindowSelEnergy
  ,LSQPregEnergy
  ,LSQWakeupEnergy
  ,RenameEnergy
  ,WakeupEnergy
  ,IAluEnergy
  ,FPAluEnergy
  ,ResultBusEnergy
  ,ForwardBusEnergy
  ,ROBEnergy
  ,WrRegEnergy
  ,RdRegEnergy
  ,DTLBEnergy
  ,RdHitEnergy
  ,RdMissEnergy
  ,RdHalfHitEnergy
  ,WrHitEnergy
  ,WrMissEnergy
  ,WrHalfHitEnergy
  ,LineFillEnergy
  ,ITLBEnergy
  ,IRdHitEnergy
  ,IRdMissEnergy
  ,IWrHitEnergy
  ,IWrMissEnergy
  ,ILineFillEnergy
  ,RdRevLVIDEnergy
  ,WrRevLVIDEnergy
  ,RdLVIDEnergy
  ,WrLVIDEnergy
  ,CombWriteEnergy
  ,LVIDTableRdEnergy
  ,BusEnergy
  ,MaxEnergyGroup
};

enum PowerGroup {
  Not_Valid_Power,
  FetchPower, 
  IssuePower,
  MemPower,
  ExecPower,
  ClockPower,
  MaxPowerGroup
};

/*****************************************************
  * designed to manage energy functions
  ****************************************************/

/*
 * Right now this class is designed as entry point to get the energy
 * for processor 0. The idea is also to use it for implementing
 * dynamic voltage scaling.
 */

/***********************************************
  *            Definitions of stat classes
  ***********************************************/
#ifndef SESC_ENERGY

class EnergyStore {
private:
protected:
public:
  EnergyStore() {
  }
  double get(const char *block, const char *name, int procId=0) {
    return -1;
  }
  double get(const char *name, int procId=0) {
    return -1;
  }
  void dump(){}
};

class GStatsEnergyBase {
 public:
  double getDouble(){
    return 0.0;
  }
  void inc() {}
  void add(int v){}
};

class GStatsEnergy : public GStatsEnergyBase {
public:
  GStatsEnergy(const char *name, const char *block
	       ,int procId, EnergyGroup grp
	       ,double energy, const char *part="") {
  }
  ~GStatsEnergy() {
  }

  static double getTotalProc(int procId) {
    return 0;
  }
  
  double getDouble(){
    return 0.0;
  }
  void inc() {}
  void add(int v){}
};

class GStatsEnergyNull : public GStatsEnergyBase {
 public:
};

class GStatsEnergyCGBase {
public:
  GStatsEnergyCGBase(const char* str,int id) {}
  void subscribe(double v) {
  }
  void calcClockGate() {
  }
  double getDouble(){
    return 0.0;
  }
};

class GStatsEnergyCG {
public:
  GStatsEnergyCG(const char *name, const char* block
		 ,int procId, EnergyGroup grp
		 ,double energy, GStatsEnergyCGBase *b, const char *part="") {
  }
  void inc() {}
  void add(int v){}
  double getDouble() {
    return 0.0;
  }
};
#else // SESC_ENERGY

class EnergyStore {
private:
  const char *proc;

public:   
  static char *getStr(EnergyGroup d);
  static char *getEnergyStr(PowerGroup d);
  static char *getStr(PowerGroup d);
  static PowerGroup getPowerGroup(EnergyGroup d);

  EnergyStore();
  double get(const char *block, const char *name, int procId=0);
  double get(const char *name, int procId=0);
};

class GStatsEnergyBase {
 public:
  virtual double getDouble() const = 0;
  virtual void inc() = 0;
  virtual void add(int v) = 0;
};

class GStatsEnergyNull : public GStatsEnergyBase {
 public:
  double getDouble() const;
  void inc();
  void add(int v);
};

class GStatsEnergy : public GStatsEnergyBase, public GStats {
protected:
  typedef std::vector< std::vector<GStatsEnergy *> > EProcStoreType;
  typedef std::vector< std::vector<GStatsEnergy *> > EGroupStoreType;
  typedef HASH_MAP< const char *, std::vector<GStatsEnergy *> > EPartStoreType;

  static EProcStoreType  eProcStore;  // Energy store per processor
  static EGroupStoreType eGroupStore; // Energy store per group
  static EPartStoreType  ePartStore;  // Energy store per block

  const double  StepEnergy;
  long long int steps;
  
  int gid;

public:
  GStatsEnergy(const char *name, const char* block
	       ,int procId, EnergyGroup grp
	       ,double energy, const char *part="");
  ~GStatsEnergy();
  static double getTotalProc(int procId);
  static double getTotalGroup(EnergyGroup grp);
  static double getTotalPart(const char *part);
  static void dump(int procId);
  static void dump();

  void reportValue() const;

  int getGid() const { return gid; }

  virtual double getDouble() const;
  virtual void inc();
  virtual void add(int v);
};

class GStatsEnergyCGBase {
protected:
  Time_t lastCycleUsed;
  long  numCycles;
  char *name;
  int   id;

public:
  long getNumCycles(){ return numCycles; }
  GStatsEnergyCGBase(const char* str,int id);
  
  void use();
};

class GStatsEnergyCG : public GStatsEnergy {
protected:  
  GStatsEnergyCGBase *eb;
  double localE;
  double clockE;

public:
  GStatsEnergyCG(const char *name, const char* block
		 ,int procId, EnergyGroup grp
		 ,double energy, GStatsEnergyCGBase *b, const char *part="");

  double getDouble() const;
  void inc();
  void add(int v);
};
#endif

#endif