///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
///
/// \file SbChunkedLoDPrimitives.cpp
/// \author Radek Barton - xbarto33
/// \date 14.11.2006
///
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

// Coin includes.

// Local includes.
#include <chunkedlod/SbChunkedLoDPrimitives.h>

/******************************************************************************
* SbChunkedLoDTile - public
******************************************************************************/

SbChunkedLoDTile::SbChunkedLoDTile(float _error, int vertex_count, SbBox3f _bounds):
  error(_error), vertices(SbIntList(vertex_count)), bounds(_bounds)
{
  for (int I = 0; I < vertex_count; I++)
  {
    this->vertices.append(0);
  }
}

SbChunkedLoDTile::SbChunkedLoDTile(const SbChunkedLoDTile & old_tile):
  error(old_tile.error), vertices(old_tile.vertices),
  bounds(old_tile.bounds)
{
  // Nothing more.
}

SbChunkedLoDTile::~SbChunkedLoDTile()
{
  // Nothing.
}

/******************************************************************************
* SbChunkedLoDTileTree - public
******************************************************************************/

SbChunkedLoDTileTree::SbChunkedLoDTileTree(int _tree_size, int _tile_size):
  tree_size(_tree_size), tile_size(_tile_size),
  tiles(SbChunkedLoDTileList(_tree_size))
{
  int vertex_count = SbSqr(this->tile_size);
  for (int I = 0; I < this->tree_size; I++)
  {
    this->tiles.append(SbChunkedLoDTile(0.0f, vertex_count, SbBox3f()));
  }
}

SbChunkedLoDTileTree::SbChunkedLoDTileTree(
  const SbChunkedLoDTileTree & old_tree):
  tree_size(old_tree.tree_size), tile_size(old_tree.tile_size),
  tiles(old_tree.tiles)
{
  // Nothing more.
}

SbChunkedLoDTileTree::~SbChunkedLoDTileTree()
{
  // Nothing.
}
