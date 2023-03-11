///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Prioritn�fronta diamant na spojen�
/// \file SbROAMMergeQueue.cpp
/// \author Radek Barto�- xbarto33
/// \date 25.08.2005
///
/// Do t�o prioritn�fronty se vkl�aj�diamanty, kter�lze spojit na dva
/// trojheln�y a t� dva trojheln�y z pvodn�h �y z triangulace odstranit.
/// Toto se prov��tehdy, pokud je teba triangulaci zjednoduit a to tak, e
/// se vyjme diamant s minim�n�prioritou, spoj�se a vloi se zp� p�adn�/// vznikl�nov�diamanty.
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

#include <roam/SbROAMMergeQueue.h>
#include <roam/SbROAMPrimitives.h>

/******************************************************************************
* SbROAMMergeQueue - public
******************************************************************************/

SbROAMMergeQueue::SbROAMMergeQueue(const int init_size):
  SbHeap(heap_funcs, init_size)
{
  // nic
}

SbROAMMergeQueue::~SbROAMMergeQueue()
{
  // nic
}

void SbROAMMergeQueue::emptyQueue()
{
  SbHeap::emptyHeap(); // vyprazdneni fronty
}

void SbROAMMergeQueue::add(SbROAMMergeQueueDiamond * diamond)
{
  /* Vlozeni diamondu a aktualizace indexu ve fronte. */
  int index = SbHeap::add(diamond);
  diamond->index = index;
}

void SbROAMMergeQueue::remove(SbROAMMergeQueueDiamond * diamond)
{
  SbHeap::remove(diamond->index); // odstraneni diamondu z fronty
}

SbROAMMergeQueueDiamond * SbROAMMergeQueue::remove(
  SbROAMSplitQueueTriangle * triangle)
{
  /* Patri-li trojuhelnik pod nejaky diamond. */
  SbROAMMergeQueueDiamond * diamond = triangle->diamond;
  if (diamond != NULL)
  {
    /* Vsechny trojuhelniky diamondu uz nejsou jeho soucasti. */
    diamond->first->diamond = NULL;
    diamond->second->diamond = NULL;

    /* Nejde-li od degradovany diamond. */
    if (diamond->third != NULL)
    {
      diamond->third->diamond = NULL;
      diamond->fourth->diamond = NULL;
    }
    SbHeap::remove(diamond->index); // odstraneni diamondu z fronty
  }
  return diamond;
}

SbROAMMergeQueueDiamond * SbROAMMergeQueue::extractMin()
{
  /* Vyber trojuhelniku s maximalni prioritou. */
  return reinterpret_cast<SbROAMMergeQueueDiamond *>(SbHeap::extractMin());
}

SbROAMMergeQueueDiamond * SbROAMMergeQueue::getMin()
{
  /* Vraceni trojuhelniku s maximalni prioritou. */
  return reinterpret_cast<SbROAMMergeQueueDiamond *>(SbHeap::getMin());
}

SbROAMMergeQueueDiamond * SbROAMMergeQueue::operator[](const int index)
{
  return reinterpret_cast<SbROAMMergeQueueDiamond *>(SbHeap::operator[](index));
}

/******************************************************************************
* SbROAMMergeQueue - protected
******************************************************************************/

SbHeapFuncs SbROAMMergeQueue::heap_funcs = {getDiamondPriority, getDiamondIndex,
  setDiamondIndex};

float SbROAMMergeQueue::getDiamondPriority(void * diamond)
{
  /* Zjisteni priority diamondu ve fronte. */
  return reinterpret_cast<SbROAMMergeQueueDiamond * >(diamond)->priority;
}

int SbROAMMergeQueue::getDiamondIndex(void * diamond)
{
  /* Zjisteni priority diamondu ve fronte. */
  return reinterpret_cast<SbROAMMergeQueueDiamond * >(diamond)->index;
}

void SbROAMMergeQueue::setDiamondIndex(void * diamond, int index)
{
  /* Nataveni priority diamondu ve fronte. */
  reinterpret_cast<SbROAMMergeQueueDiamond * >(diamond)->index = index;
}
