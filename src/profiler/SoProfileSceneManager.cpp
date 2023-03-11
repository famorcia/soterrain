///////////////////////////////////////////////////////////////////////////////
//  Profilovaci knihovna
///////////////////////////////////////////////////////////////////////////////
/// Trida rozsirujici tridu SoSceneManager o moznosti profilovani.
/// \file SoProfileSceneManager.cpp
/// \author Radek Barto�- xbarto33
/// \date 05.07.2005
///
/// Tento manaer sc�y umo�je zjistit jakou dobu str�il procesor po�ta�
/// vykreslov�� sc�y. Pro pouit�je teba nahradit vchoz�manaer sc�y
/// t�to a inicializovat profilovac�knihovnu ve t��::PrProfiler.
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

#ifdef PROFILE

#include <profiler/SoProfileSceneManager.h>

/******************************************************************************
* PrProfiler
*******************************************************************************
* public
******************************************************************************/

SoProfileSceneManager::SoProfileSceneManager():
  SoSceneManager(), ticks_per_usec(0), last_render_time(0, 0)
{
  /* Zjisteni poctu taktu CPU za mikrosekundu */
  ticks_per_usec = PrProfiler::getTicksPerUsec();
}

SoProfileSceneManager::~SoProfileSceneManager()
{
  // nic
}

void SoProfileSceneManager::render(const SbBool clearwindow,
  const SbBool clearzbuffer)
{
  /* Zmereni doby trvani rendrovani. */
  PR_START_OBJ_PROFILE(SoProfileSceneManager_render, this);
  SoSceneManager::render(clearwindow, clearzbuffer);
  PR_STOP_OBJ_PROFILE(SoProfileSceneManager_render, this);

  /* Vypocet teto doby v mirkosekundach. */
  long count = PrProfiler::getResultCount();
  PrTimeStamp time = PrProfiler::getResult(count - 1).time_stamp
    - PrProfiler::getResult(count - 2).time_stamp;
  last_render_time = SbTime(0, time);
}

SbTime SoProfileSceneManager::getLastRenderTime() const
{
  return last_render_time; // vraceni doby posledniho vykresleni
}

#endif
