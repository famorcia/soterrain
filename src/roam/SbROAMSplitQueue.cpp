///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Prioritn�fronta trojheln� na rozd�en�
/// \file SbROAMSplitQueue.cpp
/// \author Radek Barton - xbarto33
/// \date 25.08.2005
///
/// Do t�o prioritn�fronty se vkl�aj�trojheln�y, kter�je teba zaadit
/// do triangulace. Pokud je teba triangulaci d�e zjem�vat, vyjme se z fronty
/// trojheln� s maxim�n�prioritou a vlo�se zp� jeho potomci.
//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2006 Radek Barton
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
///////////////////////////////////////////////////////////////////////////////

#include <roam/SbROAMSplitQueue.h>
#include <roam/SbROAMPrimitives.h>

/******************************************************************************
* SbROAMSplitQueue - public
******************************************************************************/

SbROAMSplitQueue::SbROAMSplitQueue(const int init_size):
  SbHeap(heap_funcs, init_size)
{
  // nic
}

SbROAMSplitQueue::~SbROAMSplitQueue()
{
  // nic
}

void SbROAMSplitQueue::emptyQueue()
{
  SbHeap::emptyHeap(); // vyprazdneni fronty
}

void SbROAMSplitQueue::add(SbROAMSplitQueueTriangle * triangle)
{
  /* Vlozeni trojuhelniku a aktualizace indexu ve fronte. */
  int index = SbHeap::add(triangle);
  triangle->index = index;
}

void SbROAMSplitQueue::remove(SbROAMSplitQueueTriangle * triangle)
{
  SbHeap::remove(triangle->index); // odstraneni trojuhelniku z fronty
}

SbROAMSplitQueueTriangle * SbROAMSplitQueue::extractMax()
{
  /* Vyber trojuhelniku s maximalni prioritou. */
  return reinterpret_cast<SbROAMSplitQueueTriangle *>(SbHeap::extractMin());
}

SbROAMSplitQueueTriangle * SbROAMSplitQueue::getMax()
{
  /* vraceni trojuhelniku s maximalni prioritou. */
  return reinterpret_cast<SbROAMSplitQueueTriangle *>(SbHeap::getMin());
}

SbROAMSplitQueueTriangle * SbROAMSplitQueue::operator[](const int index)
{
  return reinterpret_cast<SbROAMSplitQueueTriangle*>
    (SbHeap::operator[](index));
}

/******************************************************************************
* SbROAMSplitQueue - private
******************************************************************************/

SbHeapFuncs SbROAMSplitQueue::heap_funcs = {getTrianglePriority,
  getTriangleIndex, setTriangleIndex};

inline float SbROAMSplitQueue::getTrianglePriority(void * triangle)
{
  /* Zjisteni priority trojuhelniku ve fronte. */
  return reinterpret_cast<SbROAMSplitQueueTriangle *>(triangle)->priority;
}

inline int SbROAMSplitQueue::getTriangleIndex(void * triangle)
{
  /* Zjisteni priority trojuhelniku ve fronte. */
  return reinterpret_cast<SbROAMSplitQueueTriangle *>(triangle)->index;
}

inline void SbROAMSplitQueue::setTriangleIndex(void * triangle, int index)
{
  /* Nataveni priority trojuhelniku ve fronte. */
  reinterpret_cast<SbROAMSplitQueueTriangle *>(triangle)->index = index;
}
