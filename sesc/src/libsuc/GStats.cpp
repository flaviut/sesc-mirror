/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
                  Basilio Fraguela
                  Smruti Sarangi
		  Luis Ceze

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

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "GStats.h"
#include "ReportGen.h"

GStats::Container *GStats::store=0;

/*********************** GStatsCntr */

GStatsCntr::GStatsCntr(const char *format,
                       ...)
{
  I(format!=0);      // Mandatory to pass a description
  I(format[0] != 0); // Empty string not valid

  char *str;
  va_list ap;

  va_start(ap, format);
  str = getText(format, ap);
  va_end(ap);

  data = 0;

  name = str;
  subscribe();
}

GStatsCntr::~GStatsCntr()
{
  unsubscribe();
  free(name);
}

double GStatsCntr::getDouble() const
{
  return (double)data;
}

void GStatsCntr::reportValue() const
{
  Report::field("%s=%lld", name, data);
}

/*********************** GStatsAvg */

GStatsAvg::GStatsAvg(const char *format,...)
{
  char *str;
  va_list ap;

  va_start(ap, format);
  str = getText(format, ap);
  va_end(ap);

  data = 0;
  nData = 0;

  name = str;
  subscribe();
}

GStatsAvg::~GStatsAvg()
{
  unsubscribe();
  free(name);
}

double GStatsAvg::getDouble() const
{
  double result = 0;

  if(nData)
    result = (double)data / nData;

  return result;
}

void GStatsAvg::reportValue() const
{
  Report::field("%s:v=%g:n=%lld", name, getDouble(), nData);
}

/*********************** GStats */

char *GStats::getText(const char *format,
                      va_list ap)
{
  char strid[1024];

  vsprintf(strid, format, ap);

  return strdup(strid);
}

void GStats::subscribe()
{
  if( store == 0 )
    store = new Container;
  
  store->push_back(this);
}

void GStats::unsubscribe()
{
  bool found = false;
  I(store);
  
  for(ContainerIter i = store->begin(); i != store->end(); i++) {
    if(*i == this) {
      found = true;
      store->erase(i);
      break;
    }
  }

  GMSG(!found, "GStats::unsubscribe It should be in the list");
}

void GStats::report(const char *str)
{
  Report::field("BEGIN GStats::report %s", str);

  if (store) 
    for(ContainerIter i = store->begin(); i != store->end(); i++)
      (*i)->reportValue();

  Report::field("END GStats::report %s", str);
}

GStats *GStats::getRef(const char *str)
{
  for(ContainerIter i = store->begin(); i != store->end(); i++) {

    if(strcasecmp((*i)->name, str) == 0)
      return *i;
  }

  return 0;
}

/*********************** GStatsProfiler */

GStatsProfiler::GStatsProfiler(const char *format, ...)
{
  char *str;
  va_list ap;

  va_start(ap, format);
  str = getText(format, ap);
  va_end(ap);


  name = str;
  subscribe();
}

GStatsProfiler::~GStatsProfiler()
{
  unsubscribe();
  free(name);
}

void GStatsProfiler::sample(ulong key)
{
  ProfHash::iterator it = p.find(key);
  if(it != p.end())
    {
      (*it).second++;
    }
  else
    {
      p[key] = 1;
    }
}

void GStatsProfiler::reportValue() const 
{
  ProfHash::const_iterator it;
  for( it = p.begin(); it != p.end(); it++ )
    {
      Report::field("%s(%d)=%d",name,(*it).first,(*it).second);
    }
}

/*********************** GStatsMax */

GStatsMax::GStatsMax(const char *format,...)
{
  char *str;
  va_list ap;

  va_start(ap, format);
  str = getText(format, ap);
  va_end(ap);

  maxValue = 0;
  nData = 0;

  name = str;
  subscribe();
}

GStatsMax::~GStatsMax()
{
  unsubscribe();
  free(name);
}

void GStatsMax::reportValue() const
{
  Report::field("%s:max=%ld:n=%lld", name, maxValue, nData);
}