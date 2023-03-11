///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Z�ladn�datov�prvky algoritmu ROAM.
/// \file SbROAMPrimitives.cpp
/// \author Radek Barto�- xbarto33
/// \date 16.09.2005
///
/// Algoritmus ROAM ve sv�statick��sti vyu��jako z�ladn�datovou
/// strukturu bin�n�strom trojheln�. Pro tyto trojheln�y je zde
/// definov�a t�a ::SbROAMTriangle. V dynamick��sti jsou pak poteba
/// dv�prioritn�fronty. Prvn�z nich je prioritn�fronta trojheln� na
/// rozd�en� Obsahuje trojheln�y t�y ::SbROAMSplitQueueTriangle.
/// Druhou pak prioritn�fronta trojheln� na spojen� Ta obsahuje diamondy
/// (�veice trojheln�) t�y ::SbROAMMergeQueueDiamond.
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

#include <roam/SbROAMPrimitives.h>

/******************************************************************************
* SbROAMTriangle - public
******************************************************************************/

SbROAMTriangle::SbROAMTriangle(int _first, int _second, int _apex, int _level):
  first(_first), second(_second), apex(_apex), level(_level), error(0.0f),
  radius(0.0)
{
  // nic
}

/******************************************************************************
* SbROAMSplitQueueTriangle - public
******************************************************************************/

SbROAMSplitQueueTriangle::SbROAMSplitQueueTriangle(SbROAMTriangle * _triangle,
  float _priority, SbROAMSplitQueueTriangle * _left,
  SbROAMSplitQueueTriangle * _right, SbROAMSplitQueueTriangle * _base,
  SbROAMMergeQueueDiamond * _diamond):
  triangle(_triangle), left(_left), right(_right), base(_base),
  diamond(_diamond),  priority(-_priority), index(-1)
{
  // nic
}

/******************************************************************************
* SbROAMMergeQueueDiamond - public
******************************************************************************/

SbROAMMergeQueueDiamond::SbROAMMergeQueueDiamond(
  SbROAMSplitQueueTriangle * _first,
  SbROAMSplitQueueTriangle * _second,
  SbROAMSplitQueueTriangle * _third,
  SbROAMSplitQueueTriangle * _fourth,
  float _priority):
  first(_first), second(_second), third(_third), fourth(_fourth),
  priority(_priority), index(-1)
{
  // nic
}
