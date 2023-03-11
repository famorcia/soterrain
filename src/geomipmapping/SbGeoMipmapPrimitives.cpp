///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Z ladn datov prvky algorithm Geo Mip-mapping.
/// \file SbGeoMipmapPrimitives.cpp
/// \author Radek Barton - xbarto33
/// \date 30.01.2006
///
/// The basic data types of the Geo Mip-Mapping algorithm are defined here.
/// The quadrant tile tree in t  ::SbGeoMipmapTileTree recursively
/// splits the current age map into tiles of SbGeoMipmapTile. In his
/// on the upper levels, the tiles are only virtual, i.e. those that do not contain
/// /// their own geometry, but only world descendants. Tiles on the lowest level
/// the tree contains an array of individual equal tile details in t
/// /// ::SbGeoMipmapTileLevel, only for geometry it includes
/// //////////////////////////////////////////////////////////////////////////////
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

#include <geomipmapping/SbGeoMipmapPrimitives.h>

/******************************************************************************
* SbGeoMipmapTileLevel - public
******************************************************************************/

SbGeoMipmapTileLevel::SbGeoMipmapTileLevel(float _error, int * _vertices):
  error(_error), vertices(_vertices)
{
  // nic
}

SbGeoMipmapTileLevel::~SbGeoMipmapTileLevel()
{
  /* Uvolneni pameti. */
  delete[] vertices;
}

/******************************************************************************
* SbGeoMipmapTile - public
******************************************************************************/

/* Staticke konstanty. */
const int SbGeoMipmapTile::LEVEL_NONE = -1;

SbGeoMipmapTile::SbGeoMipmapTile(SbGeoMipmapTileLevel * _levels,
  SbGeoMipmapTile * _left, SbGeoMipmapTile * _right, SbGeoMipmapTile * _top,
  SbGeoMipmapTile * _bottom, int _level, SbBox3f _bounds, SbVec3f _center):
  levels(_levels), left(_left), right(_right), top(_top), bottom(_bottom),
  level(_level), bounds(_bounds), center(_center)
{
  // nic
}

SbGeoMipmapTile::~SbGeoMipmapTile()
{
  /* Uvolneni pameti. */
  delete[] levels;
}

/******************************************************************************
* SbGeoMipmapTileTree - public
******************************************************************************/

SbGeoMipmapTileTree::SbGeoMipmapTileTree(int _tile_count, int _tile_size):
  tile_count(_tile_count), tile_size(_tile_size), tree_size(0), level_count(0),
  level_sizes(NULL), tiles(NULL)
{
  /* Vypocet velikosti stromu dlazdic. */
  tree_size = tile_count;
  int tmp = tile_count;
  while (tmp > 1)
  {
    tmp >>= 2;
    tree_size+= tmp;
  }

  /* Vypocet zacatku dlazdic na nejnizzsi urovni. */
  bottom_start = tree_size - tile_count;

  /* Vypocet poctu urovni detailu v jedne dlazdici. */
  level_count = ilog2(tile_size - 1) + 1;

  /* Alokace a inicializace pole velikosti jednotlivych urovni detailu
  jedne dlazdice. */
  level_sizes = new int[level_count];
  level_sizes[0] = tile_size;
  for (int I = 1; I < level_count; ++I)
  {
    // predpoklad licheho cisla
    level_sizes[I] = (level_sizes[I - 1] >> 1) + 1;
  }

  /* Alokace pole dlazdic. */
  tiles = new SbGeoMipmapTile[tree_size];
}

SbGeoMipmapTileTree::~SbGeoMipmapTileTree()
{
  delete[] level_sizes;
  delete[] tiles;
}

/******************************************************************************
* SbGeoMipmapTileTree - private
******************************************************************************/

SbGeoMipmapTileTree::SbGeoMipmapTileTree(const SbGeoMipmapTileTree & old_tree)
{
  // nic
}
