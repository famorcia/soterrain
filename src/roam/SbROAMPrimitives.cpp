///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Align the data elements of the ROAM algorithm.
/// \file SbROAMPrimitives.cpp
/// \author Radek Barton - xbarto33
/// \date 16.09.2005
///
/// The ROAM algorithm in statistics is used as a data model
/// the structure of a bin n tree of a triple helix . It is here for these triplets
/// define the ::SbROAMTriangle. They are then needed in dynamism
/// two priority queues. The first of them is the priority queue of the three-wheeled na
/// split Contains triangles ::SbROAMSplitQueueTriangle.
/// The second is the priority queue of triples on the link. This contains diamonds
/// (many triples) t y ::SbROAMMergeQueueDiamond.
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
