///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// T�a roziuj��t�u SoGroup o monosti profilov��
/// \file SoProfileGroup.cpp
/// \author Radek Barto�- xbarto33
/// \date 18.04.2006
///
/// Umonuje zm�it dobu str�enou vykreslov�� uzl obsaench v t�o skupin�
/// za pomoc�profileru ve statick�t�e ::PrProfiler.
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

#include <profiler/SoProfileGroup.h>

#ifdef PROFILE

SO_NODE_SOURCE(SoProfileGroup)

/******************************************************************************
* SoProfileGroup - public
******************************************************************************/

void SoProfileGroup::initClass()
{
  /* Inicializace tridy. */
  SO_NODE_INIT_CLASS(SoProfileGroup, SoGroup, "Group");
}


SoProfileGroup::SoProfileGroup():
  ticks_per_usec(0), last_render_time(0.0)
{
  /* Inicializace tridy. */
  SO_NODE_CONSTRUCTOR(SoProfileGroup);
}

SoProfileGroup::SoProfileGroup(int numChildren):
  SoGroup(numChildren),
  ticks_per_usec(0), last_render_time(0.0)
{
  /* Inicializace tridy. */
  SO_NODE_CONSTRUCTOR(SoProfileGroup);
}

void SoProfileGroup::GLRender(SoGLRenderAction * action)
{
  /* Zmereni doby trvani rendrovani. */
  PR_START_OBJ_PROFILE(SoProfileGroup_GLRender, this);
  SoGroup::GLRender(action);
  PR_STOP_OBJ_PROFILE(SoProfileGroup_GLRender, this);

  /* Vypocet teto doby v mirkosekundach. */
  long count = PrProfiler::getResultCount();
  PrTimeStamp time = PrProfiler::getResult(count - 1).time_stamp
    - PrProfiler::getResult(count - 2).time_stamp;
  last_render_time = SbTime(0, time);
}

SbTime SoProfileGroup::getLastRenderTime() const
{
  // vraceni casu posledniho vykreslovani
  return last_render_time;
}

/******************************************************************************
* SoProfileGroup - private
******************************************************************************/

SoProfileGroup::~SoProfileGroup()
{
  // nic
}

#else // PROFILE

SO_NODE_SOURCE(SoProfileGroup)

/******************************************************************************
* SoProfileGroup - public
******************************************************************************/

void SoProfileGroup::initClass()
{
  /* Inicializace tridy. */
  SO_NODE_INIT_CLASS(SoProfileGroup, SoGroup, "Group");
}


SoProfileGroup::SoProfileGroup()
{
  /* Inicializace tridy. */
  SO_NODE_CONSTRUCTOR(SoProfileGroup);
}

SoProfileGroup::SoProfileGroup(int numChildren):
  SoGroup(numChildren)
{
  /* Inicializace tridy. */
  SO_NODE_CONSTRUCTOR(SoProfileGroup);
}

/******************************************************************************
* SoProfileGroup - private
******************************************************************************/

SoProfileGroup::~SoProfileGroup()
{
  // nic
}

#endif // PROFILE
