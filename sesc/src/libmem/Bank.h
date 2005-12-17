/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Basilio Fraguela
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
#ifndef BANK_H
#define BANK_H

#include "GStats.h"

#include "MemRequest.h"
#include "MemObj.h"

#include "CacheCore.h"
#include "Snippets.h"

class MemorySystem;

class Bank : public MemObj {
protected:
  static const int RBMAX=4, SEGMAX=9, BANKMAX=16;

  // to simplify assume equal everywhere
  static const int II=2;

  // precharge time
  static const int PC_TIME=8;

  const int HitDelay;
  const int MissDelay;

  //BBF: nReads=readHit + readMiss
  GStatsCntr    nWrites;
  GStatsCntr    readHit;
  GStatsCntr    readMiss;
  GStatsCntr    nWaits;
  GStatsCntr    bankWait;

  // TODO: Change busyUntil for a GenericPort
  Time_t busyUntil;

  Time_t segBusy[SEGMAX];
  Time_t bankBusy[SEGMAX][BANKMAX];
  Time_t bankPC[SEGMAX][BANKMAX];

  int    nextRBN[SEGMAX][BANKMAX];
  int    address[SEGMAX][BANKMAX][RBMAX];
	
  bool		isSharing, isSegm, isPipe, isIntlv, active;
  int		segment, subBank, rowSize, numRB;
  ReplacementPolicy	policy;


  int   id;

private:

  int  SEG(int addr) const { return (addr / rowSize) % segment; }
  int  BANK(int addr) const { return (addr / rowSize / segment * 2) % subBank; }
  bool RBHIT(int addr) const ;
  void DESTROYNEIGHBOR(int seg, int bank, int i);
  void initialize_bank(int rb_num, int rb_width, const char *p, 
		       int org, bool pipe, bool seg);

public:
  Bank(MemorySystem* current, const char *device_descr_section,
       const char *device_name=NULL);

  //BBF : dummybank ist jetzt kaputt Bank() {};	for dummy bank
  ~Bank() {}

  void DisableBankTill(Time_t);

  void returnAccess(MemRequest *mreq) {};

  void read(MemRequest *mreq);
  void write(MemRequest *mreq);

  void access(MemRequest *mreq);

  Time_t getNextFreeCycle() const;

  bool canAcceptStore(PAddr addr);

  virtual void invalidate(PAddr addr,ushort size,MemObj *oc);
};

#endif

