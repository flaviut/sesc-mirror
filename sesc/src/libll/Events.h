/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
                  Basilio Fraguela
                  Milos Prvulovic

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
#ifndef EVENTS_H
#define EVENTS_H

#include "icode.h"

enum EventType {
  NoEvent = 0,
#ifdef TASKSCALAR
  SpawnEvent,
#endif
  PreEvent,
  PostEvent,
  FetchOpEvent,    // Memory rd/wr performed atomicaly. Notified as MemFence op
  MemFenceEvent,   // Release Consistency Barrier or memory fence
  AcquireEvent,    // Release Consistency Release
  ReleaseEvent,    // Release Consistency Ackquire
  FastSimBeginEvent,
  FastSimEndEvent,
  LibCallEvent,
  MaxEvent
};
// I(MaxEvent < SESC_MAXEVENT);

/* WARNING: If you add more events (MaxEvent) and MIPSInstruction.cpp
 * adds them, be sure that the Itext size also grows. (mint:globals.h)
 */

/* System events supported by sesc_postevent(....)
 */
static const int EvSimStop  = 1;
static const int EvSimStart = 2;

/*
 * The first 10 types are reserved for communicate with the
 * system. This means that you CAN NOT use sesc_preevent(...,3,...);
 */
static const int EvMinType  = 10;

extern icode_t invalidIcode;

#if defined(TASKSCALAR)
/* Event.cpp defined functions */
int  rsesc_become_safe(int pid);
int  rsesc_is_safe(int pid);
int  rsesc_is_versioned(int pid);
long rsesc_OS_prewrite(int pid, int addr, int iAddr, long flags);
void rsesc_OS_postwrite(int pid, int addr, int iAddr, long flags);
long rsesc_OS_read(int pid, int addr, int iAddr, long flags);
void rsesc_exception(int pid);
void mint_termination(int pid);
#endif

#endif   // EVENTS_H
