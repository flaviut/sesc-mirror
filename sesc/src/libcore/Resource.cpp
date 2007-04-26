/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
                  Basilio Fraguela
                  James Tuck
                  Smruti Sarangi
                  Luis Ceze
		  Karin Strauss

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

#include <limits.h>

#include "Cluster.h"
#include "DInst.h"
#include "EnergyMgr.h"
#include "FetchEngine.h"
#include "GMemorySystem.h"
#include "GProcessor.h"
#include "LDSTBuffer.h"
#include "MemRequest.h"
#include "Port.h"
#include "Resource.h"


Resource::Resource(Cluster *cls, PortGeneric *aGen)
  : cluster(cls)
    ,gen(aGen)
{
  I(cls);
  if(gen)
    gen->subscribe();
}

Resource::~Resource()
{
  GMSG(!EventScheduler::empty(), "Resources destroyed with %Zu pending instructions"
       ,EventScheduler::size());
  
  if(gen)
    gen->unsubscribe();
}

void Resource::executed(DInst *dinst)
{
  cluster->executed(dinst);
}

RetOutcome Resource::retire(DInst *dinst)
{
  cluster->retire(dinst);
  dinst->destroy();
  return Retired;
}

/***********************************************/

MemResource::MemResource(Cluster *cls
                         ,PortGeneric *aGen
                         ,GMemorySystem *ms
                         ,int id
                         ,const char *cad)
  : Resource(cls, aGen)
  ,L1DCache(ms->getDataSource())
  ,memorySystem(ms)
{

  char cadena[100];
  sprintf(cadena,"%s(%d)", cad, id);
  
#ifdef SESC_NO_LDQ
  ldqCheckEnergy = new GStatsEnergyNull; // No stats
  ldqRdWrEnergy  = new GStatsEnergyNull; // No stats
#endif

#ifdef SESC_INORDER
  ldqCheckEnergyOutOrder = new GStatsEnergy("ldqCheckEnergy",cadena,id, ExecPower
                                    ,EnergyMgr::get("ldqCheckEnergy",id),"LSQ");

  ldqRdWrEnergyOutOrder = new GStatsEnergy("ldqRdWrEnergy",cadena,id, ExecPower
                                   ,EnergyMgr::get("ldqRdWrEnergy",id),"LSQ");

  ldqCheckEnergyInOrder = new GStatsEnergyNull; // No stats
  ldqRdWrEnergyInOrder  = new GStatsEnergyNull; // No stats

  ldqCheckEnergy = ldqCheckEnergyOutOrder; // No stats
  ldqRdWrEnergy  = ldqRdWrEnergyOutOrder; // No stats
  
  
  stqCheckEnergyOutOrder = new GStatsEnergy("stqCheckEnergy",cadena,id, ExecPower
                                    ,EnergyMgr::get("stqCheckEnergy",id),"LSQ");


  stqRdWrEnergyOutOrder = new GStatsEnergy("stqRdWrEnergy",cadena,id, ExecPower
                                   ,EnergyMgr::get("stqRdWrEnergy",id),"LSQ");
                                   
                                   
  stqCheckEnergyInOrder = new GStatsEnergy("stqCheckEnergyInOrder",cadena,id, ExecPower
                                    ,EnergyMgr::get("stqCheckEnergy",id),"LSQ");


  stqRdWrEnergyInOrder = new GStatsEnergy("stqRdWrEnergyInOrder",cadena,id, ExecPower
                                   ,EnergyMgr::get("stqRdWrEnergy",id),"LSQ");
                                   
  stqCheckEnergy = stqCheckEnergyOutOrder; // No stats
  stqRdWrEnergy  = stqRdWrEnergyOutOrder; // No stats
                           

  InOrderMode = true;
  OutOrderMode = false;
  currentMode = OutOrderMode;

#else
  ldqCheckEnergy = new GStatsEnergy("ldqCheckEnergy",cadena,id, ExecPower
                                    ,EnergyMgr::get("ldqCheckEnergy",id));

  ldqRdWrEnergy  = new GStatsEnergy("ldqRdWrEnergy",cadena,id, ExecPower
                                    ,EnergyMgr::get("ldqRdWrEnergy",id));


  stqCheckEnergy = new GStatsEnergy("stqCheckEnergy",cadena,id, ExecPower
                                    ,EnergyMgr::get("stqCheckEnergy",id));


  stqRdWrEnergy  = new GStatsEnergy("stqRdWrEnergy",cadena,id, ExecPower
                                    ,EnergyMgr::get("stqRdWrEnergy",id));
                                   
#endif

  iAluEnergy = new GStatsEnergy("iAluEnergy", cadena , id, ExecPower
                                ,EnergyMgr::get("iALUEnergy",id));
}

#ifdef SESC_INORDER
void MemResource::setMode(bool mode)
{
  if(currentMode == mode)
    return;

  cluster->setMode(mode);

  currentMode = mode;

  if(mode == InOrderMode){
   ldqCheckEnergy = ldqCheckEnergyInOrder; // No stats
   ldqRdWrEnergy  = ldqRdWrEnergyInOrder; // No stats
   stqCheckEnergy = stqCheckEnergyInOrder; // No stats
   stqRdWrEnergy  = stqRdWrEnergyInOrder; // No stats
   currentMode = InOrderMode;
  }else{
   ldqCheckEnergy = ldqCheckEnergyInOrder; // No stats
   ldqRdWrEnergy  = ldqRdWrEnergyInOrder; // No stats
   stqCheckEnergy = stqCheckEnergyInOrder; // No stats
   stqRdWrEnergy  = stqRdWrEnergyInOrder; // No stats
   currentMode = OutOrderMode;
  }
}
#endif
/***********************************************/

FUMemory::FUMemory(Cluster *cls, GMemorySystem *ms, int id)
  : MemResource(cls, 0, ms, id, "FUMemory")
{
  I(ms);
  I(L1DCache);
}

StallCause FUMemory::canIssue(DInst *dinst)
{
  cluster->newEntry();
  
  // TODO: Someone implement a LDSTBuffer::fenceGetEntry(dinst);

  stqRdWrEnergy->inc();
  ldqRdWrEnergy->inc();


  return NoStall;
}

void FUMemory::simTime(DInst *dinst)
{
  const Instruction *inst = dinst->getInst();
  I(inst->isFence());
  I(!inst->isLoad() );
  I(!inst->isStore() );

  // All structures accessed (very expensive)
  iAluEnergy->inc();
  
  stqCheckEnergy->inc();
  stqRdWrEnergy->inc();

  ldqCheckEnergy->inc();
  ldqRdWrEnergy->inc();

  // TODO: Add prefetch for Fetch&op as a MemWrite
  dinst->doAtExecutedCB.schedule(1); // Next cycle
}

RetOutcome FUMemory::retire(DInst *dinst)
{
  const Instruction *inst = dinst->getInst();

  if( inst->getSubCode() == iFetchOp ) {
    if (L1DCache->canAcceptStore(static_cast<PAddr>(dinst->getVaddr())) == false)
      return NoCacheSpace;
    FUStore* r = (FUStore*) getCluster()->getResource(iStore);
#ifdef TS_CHERRY
    dinst->setCanBeRecycled();
    dinst->setMemoryIssued();
#endif
    if ( r->waitingOnFence() == true)
      return WaitForFence;
    else
      r->storeSent();
    DMemRequest::create(dinst, memorySystem, MemWrite);
  }else if( inst->getSubCode() == iMemFence ) {
    ((FUStore*)(getCluster()->getResource(iStore)))->doFence();
    dinst->destroy();
  }else if( inst->getSubCode() == iAcquire ) {
    // TODO: Consistency in LDST
    dinst->destroy();
  }else {
    I( inst->getSubCode() == iRelease );
    // TODO: Consistency in LDST
    dinst->destroy();
  }

  return Retired;
}

#ifdef SESC_CHERRY
void FUMemory::earlyRecycle(DInst *dinst)
{
  I(0);  // TODO
}
#endif

/***********************************************/

FULoad::FULoad(Cluster *cls, PortGeneric *aGen
               ,TimeDelta_t l
               ,TimeDelta_t lsdelay
               ,GMemorySystem *ms
               ,size_t maxLoads
               ,int id)
  : MemResource(cls, aGen, ms, id, "FULoad")
  ,ldqNotUsed("FULoad(%d)_ldqNotUsed", id)
  ,nForwarded("FULoad(%d):nForwarded", id)
  ,lat(l)
  ,LSDelay(lsdelay)
  ,freeLoads(maxLoads)
  ,misLoads(0)
{
  char cadena[100];
  sprintf(cadena,"FULoad(%d)", id);
  
  I(ms);
  I(freeLoads>0);
}

StallCause FULoad::canIssue(DInst *dinst)
{
  int freeEntries = freeLoads;
#ifdef SESC_MISPATH
  freeEntries -= misLoads;
#endif
  if( freeEntries <= 0 ) {
    I(freeEntries == 0); // Can't be negative
    return OutsLoadsStall;
  }

  cluster->newEntry();

  LDSTBuffer::getLoadEntry(dinst);
  cluster->getGProcessor()->getLSQ()->insert(dinst);

  if (dinst->isFake())
    misLoads++;
  else
    freeLoads--;

  ldqRdWrEnergy->inc();
 
  return NoStall;
}

void FULoad::simTime(DInst *dinst)
{
  Time_t when = gen->nextSlot()+lat;

  // The check in the LD Queue is performed always, for hit & miss
  iAluEnergy->inc();
  
  stqCheckEnergy->inc(); // Check st-ld forwarding

  cluster->getGProcessor()->getLSQ()->executed(dinst);
  ldqRdWrEnergy->inc();
  ldqCheckEnergy->inc();        

  if (dinst->isLoadForwarded()) {
#ifdef SESC_CHERRY
    dinst->markResolved();
    cluster->getGProcessor()->propagateUnresolvedLoad();
#endif

    dinst->doAtExecutedCB.scheduleAbs(when+LSDelay);
    // forwardEnergy->inc(); // TODO: CACTI == a read in the STQ
    nForwarded.inc();
  }else{
    if(dinst->isDeadInst()) {
      // dead inst, just make it fly through the pipeline
      dinst->doAtExecuted();
    } else {
      cacheDispatchedCB::scheduleAbs(when, this, dinst);
    }
  }
}

void FULoad::executed(DInst* dinst)
{
  Resource::executed(dinst);
}

void FULoad::cacheDispatched(DInst *dinst)
{
  I( !dinst->isLoadForwarded() );

  if(!L1DCache->canAcceptLoad(static_cast<PAddr>(dinst->getVaddr()))) {
    cacheDispatchedCB::schedule(7, this, dinst); //try again later
    return;
  }

#ifdef SESC_CHERRY
  dinst->markResolved();
  cluster->getGProcessor()->propagateUnresolvedLoad();
#endif

  I( !dinst->isLoadForwarded() );
  // LOG("[0x%p] %lld 0x%lx read", dinst, globalClock, dinst->getVaddr());
  if(!L1DCache->canAcceptLoad(static_cast<PAddr>(dinst->getVaddr()))) {
    Time_t when = gen->nextSlot();
    //try again
    // +1 because when we have unilimited ports (or occ) 0, this will be an
    // infinite loop
    cacheDispatchedCB::scheduleAbs(when+1, this, dinst);
  } else {

    DMemRequest::create(dinst, memorySystem, MemRead);
  }
}

RetOutcome FULoad::retire(DInst *dinst)
{
  ldqNotUsed.sample(freeLoads);

  cluster->retire(dinst);
  cluster->getGProcessor()->getLSQ()->remove(dinst);

  if (!dinst->isFake() && !dinst->isEarlyRecycled())
    freeLoads++;

  dinst->destroy();

  // ldqRdWrEnergy->inc(); // Loads do not update fields at retire, just update pointers

  return Retired;
}

#ifdef SESC_CHERRY
void FULoad::earlyRecycle(DInst *dinst)
{
  I(!dinst->isEarlyRecycled());

  //  MSG("0x%x load recycled @%lld",dinst->getInst()->currentID(), globalClock);
  if (dinst->isFake())
    misLoads--;
  else
    freeLoads++;

  dinst->setEarlyRecycled();
}
#endif

#ifdef SESC_MISPATH
void FULoad::misBranchRestore()
{ 
  misLoads = 0; 
}
#endif
/***********************************************/

FUStore::FUStore(Cluster *cls, PortGeneric *aGen
                 ,TimeDelta_t l
                 ,GMemorySystem *ms
                 ,size_t maxStores
                 ,int id)
  : MemResource(cls, aGen, ms, id, "FUStore")
  ,stqNotUsed("FUStore(%d)_stqNotUsed", id)
  ,nDeadStore("FUStore(%d):nDeadStore", id)
  ,lat(l)
  ,freeStores(maxStores)
  ,misStores(0)
  ,pendingFence(false)
  ,nOutsStores(0)
  ,nFences("FUStore(%d):nFences", id)
  ,fenceStallCycles("FUStore(%d):fenceStallCycles", id)
{

  I(freeStores>0);
}

StallCause FUStore::canIssue(DInst *dinst)
{
  int freeEntries = freeStores;
#ifdef SESC_MISPATH
  freeEntries -= misStores;
#endif
  if( freeEntries <= 0 ) {
    I(freeEntries == 0); // Can't be negative
    return OutsStoresStall;
  }

  cluster->newEntry();

  LDSTBuffer::getStoreEntry(dinst);
  cluster->getGProcessor()->getLSQ()->insert(dinst);

  if (dinst->isFake()) {
    misStores++;
  }else{
    freeStores--;
  }

  stqRdWrEnergy->inc();

  return NoStall;
}

void FUStore::simTime(DInst *dinst)
{
  dinst->doAtExecutedCB.scheduleAbs(gen->nextSlot()+lat);
}

void FUStore::executed(DInst *dinst)
{
  stqRdWrEnergy->inc(); // Update fields
  ldqCheckEnergy->inc(); // Check st-ld replay traps


#ifdef SESC_CHERRY
  dinst->markResolved();
  cluster->getGProcessor()->propagateUnresolvedStore();
#endif

  cluster->executed(dinst);
  cluster->getGProcessor()->getLSQ()->executed(dinst);

#ifdef SESC_CHERRY
  if (dinst->isEarlyRecycled()) {
    dinst->setMemoryIssued();

    DMemRequest::create(dinst, memorySystem, MemWrite);
  }
#endif
}

void FUStore::doRetire(DInst *dinst)
{
  stqNotUsed.sample(freeStores);

  LDSTBuffer::storeLocallyPerformed(dinst);
  cluster->getGProcessor()->getLSQ()->remove(dinst);
  cluster->retire(dinst);

  if (!dinst->isEarlyRecycled() && !dinst->isFake())
    freeStores++;

  stqRdWrEnergy->inc(); // Read value send to memory, and clear fields


}

RetOutcome FUStore::retire(DInst *dinst)
{
  if (dinst->isDeadInst() || dinst->isFake() || dinst->isEarlyRecycled()
#ifdef SESC_CHERRY
      || dinst->isMemoryIssued()
#endif
      ) {
    doRetire(dinst);

    if (dinst->isDeadStore())
      nDeadStore.inc();

#ifdef SESC_CHERRY
    if (dinst->isMemoryIssued() && !dinst->hasCanBeRecycled())
      dinst->setCanBeRecycled();
    else
      dinst->destroy();
#else
    dinst->destroy();
#endif
    return Retired;
  }

  if(L1DCache->getNextFreeCycle() > globalClock)
    return NoCachePorts;

  if (!L1DCache->canAcceptStore(static_cast<PAddr>(dinst->getVaddr())) ) {
    return NoCacheSpace;
  }


  // Note: The store is retired from the LDSTQueue as soon as it is send to the
  // L1 Cache. It does NOT wait until the ack from the L1 Cache is received

#ifdef SESC_CHERRY
  dinst->setCanBeRecycled();
  dinst->setMemoryIssued();
#endif

  gen->nextSlot();

  // used for updating mint on unlock
  if (dinst->getPendEvent())
    dinst->getPendEvent()->call();
  if (waitingOnFence() == true)
    return WaitForFence;
  else
    storeSent();

  DMemRequest::create(dinst, memorySystem, MemWrite);

  doRetire(dinst);

  return Retired;
}

#ifdef SESC_CHERRY
void FUStore::earlyRecycle(DInst *dinst)
{
  I(!dinst->isEarlyRecycled());

  dinst->setEarlyRecycled();

  if (dinst->isDeadStore() && !dinst->isFake()) {
    freeStores++;
    return;
  }

  if (!dinst->isFake())
    freeStores++;
  
  if (dinst->isResolved()) {
    dinst->setMemoryIssued();

    DMemRequest::create(dinst, memorySystem, MemWrite);
  }
}
#endif

void FUStore::storeCompleted()
{
  I(nOutsStores > 0);
  nOutsStores--;
  if (nOutsStores == 0)
    pendingFence = false;
}

void FUStore::doFence() {
  if (nOutsStores > 0) pendingFence = true; 
  nFences.inc(); 
}

#ifdef SESC_MISPATH
void FUStore::misBranchRestore() 
{ 
  misStores = 0; 
}
#endif


/***********************************************/
FUGeneric::FUGeneric(Cluster *cls
                     ,PortGeneric *aGen
                     ,TimeDelta_t l
                     ,GStatsEnergyCG *eb
  )
  :Resource(cls, aGen)
  ,lat(l)
  ,fuEnergy(eb)
{
}

StallCause FUGeneric::canIssue(DInst *dinst)
{
  cluster->newEntry();

  return NoStall;
}

void FUGeneric::simTime(DInst *dinst)
{
  dinst->doAtExecutedCB.scheduleAbs(gen->nextSlot()+lat);
}

void FUGeneric::executed(DInst *dinst)
{
  fuEnergy->inc();
  cluster->executed(dinst);
}

#ifdef SESC_CHERRY
void FUGeneric::earlyRecycle(DInst *dinst)
{
  I(0);
}
#endif
#ifdef SESC_INORDER
void FUGeneric::setMode(bool mode)
{
  cluster->setMode(mode);
  // Nothing
}
#endif

/***********************************************/

FUBranch::FUBranch(Cluster *cls, PortGeneric *aGen, TimeDelta_t l, int mb)
  :Resource(cls, aGen)
   ,lat(l)
   ,freeBranches(mb)
{
  I(freeBranches>0);
}

StallCause FUBranch::canIssue(DInst *dinst)
{
  if (freeBranches == 0)
    return OutsBranchesStall;

  cluster->newEntry();
  
  freeBranches--;

  return NoStall;
}

void FUBranch::simTime(DInst *dinst)
{
  dinst->doAtExecutedCB.scheduleAbs(gen->nextSlot()+lat);
}

void FUBranch::executed(DInst *dinst)
{
#ifndef SESC_BRANCH_AT_RETIRE
  // TODO: change it to remove getFetch only call missBranch when a
  // boolean is set? Backup done at fetch 
  if (dinst->getFetch()) {
    dinst->getFetch()->unBlockFetch();
    IS(dinst->setFetch(0));
#ifdef SESC_MISPATH
    cluster->getGProcessor()->misBranchRestore(dinst);
#endif
  }
#endif

  freeBranches++;

#ifdef SESC_CHERRY
  dinst->markResolved();
  cluster->getGProcessor()->propagateUnresolvedBranch();
#endif

  cluster->executed(dinst);
}

#ifdef SESC_BRANCH_AT_RETIRE
RetOutcome FUBranch::retire(DInst *dinst)
{
  // TODO: change it to remove getFetch only call missBranch when a
  // boolean is set? Backup done at fetch 
  if (dinst->getFetch()) {
    dinst->getFetch()->unBlockFetch();
    IS(dinst->setFetch(0));
#ifdef SESC_MISPATH
    cluster->getGProcessor()->misBranchRestore(dinst);
#endif
  }

  cluster->retire(dinst);
  dinst->destroy();

  return Retired;
}
#endif

#ifdef SESC_CHERRY
void FUBranch::earlyRecycle(DInst *dinst)
{
  I(0);  // TODO
}
#endif
#ifdef SESC_INORDER
void FUBranch::setMode(bool mode)
{
  cluster->setMode(mode);
  // Nothing
}
#endif

/***********************************************/

FUEvent::FUEvent(Cluster *cls)
  :Resource(cls, 0)
{
}

StallCause FUEvent::canIssue(DInst *dinst)
{
  return NoStall;
}

void FUEvent::simTime(DInst *dinst)
{
  I(dinst->getInst()->getEvent() == PostEvent);
  // memfence, Relase, and Acquire are passed to FUMemory
  //
  // PreEvent, FastSimBegin, and FastSimEnd are not simulated as
  // instructions through the pipeline

  CallbackBase *cb = dinst->getPendEvent();

  // If the preEvent is created with a vaddr the event handler is
  // responsible to canIssue the instruction execution.
  I( dinst->getVaddr() == 0 );
  cb->call();

  cluster->executed(dinst);
}

#ifdef SESC_CHERRY
void FUEvent::earlyRecycle(DInst *dinst)
{
  I(0);  // TODO
}
#endif

#ifdef SESC_INORDER
void FUEvent::setMode(bool mode)
{
  cluster->setMode(mode);
  // Nothing
}
#endif
