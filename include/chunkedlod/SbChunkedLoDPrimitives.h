#ifndef SB_CHUNKED_LOD_PRIMITIVES_H
#define SB_CHUNKED_LOD_PRIMITIVES_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Basic data types of Chunked LoD algorithm.
/// \file SbChunkedLoDPrimitives.h
/// \author Radek Barton - xbarto33@stud.fit.vutbr.cz
/// \date 14.11.2006
/// There are defined basic data types of Chunked LoD algorithm in this file.
/// Tile quad-tree of class ::SbChunkedLoDTileTree splits input heightmap into
/// recursive structure of tiles of class ::SbChunkedLoDTile with gemetry on
/// different level of detail. Root node contains tile describing whole
/// terrain on coarsest level of detail and it has four children describing
/// quaters of root node on doubled level of detail. Child tiles are divided
/// recursively in same manner.
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
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/lists/SbIntList.h>
#include <Inventor/lists/SbVec3fList.h>

// Local includes.
#include <debug.h>
#include <utils.h>

/** Chunked LoD algorithm tile.
There is created quad-tree with this tiles serving for decision what level
of detail of terrain parts should be choosen during rendering. Tile contains
indices to heightmap vertices describing tile's geometry, static part of error
metric and bounding box for easy frustrum culling. */
struct SbChunkedLoDTile
{
  public:
    /* Methods. */
    /** Construtor.
    Creates instance of ::SbChunkedLoDTile class with given static part of
    error metric \e error, prepares list of vertex indices to size
    \e vertex_count and initializes them to zero values. Sets tile bunding box
    to \e bunds value.
    \param error Static part of error metric for this tile.
    \param vertex_count Suggested count of vertex indices for tile geometry.
    \param bounds Bounding box of this tile. */
    SbChunkedLoDTile(float error = 0.0f, int vertex_count = 0,
      SbBox3f bounds = SbBox3f());
    /** Copy contructor.
    Creates new instance of ::SbChunkedLoDTile class from \e old_tile instance.
    \param old_tile Old instance of tile. */
    SbChunkedLoDTile(const SbChunkedLoDTile & old_tile);
    /** Destructor.
    Destroys instance of ::SbChunkedLoDTile class. */
    ~SbChunkedLoDTile();
    /* Attributes. */
    /// Static part of error metric.
    float error;
    /// List of geometry vertex indices.
    SbIntList vertices;
    /// Bounding box of tile.
    SbBox3f bounds;
};

/// ::SbList template instance type for list of ::SbChunkedLoDTile tiles.
typedef SbList<SbChunkedLoDTile> SbChunkedLoDTileList;

/** Chunked LoD algorithm tile quad-tree.
This quad-tree contains tiles with indexed geometry, static part of error
metric and bounding boxes of class ::SbChunkedLoDTile. */
struct SbChunkedLoDTileTree
{
  public:
    /* Methods. */
    /** Construtor.
    Creates instance of ::SbChunkedLoDTileTree with given number of tiles
    in tree \e tree_size and initializes tiles in this tree to have square of
    \e tile_size number of vertex indices and default all other values.
    \param tree_size Number of tiles in tile quad-tree.
    \param tile_size Size of tile side. Number of its vertex indices is equal
      to square of this value. */
    SbChunkedLoDTileTree(int tree_size, int tile_size);
    /** Copy constructor.
    Creates new instance of ::SbChunkedLoDTileTree class from \e old_tree
    instance.
    \param old_tree Old instance of tile quad-tree. */
    SbChunkedLoDTileTree(const SbChunkedLoDTileTree & old_tree);
    /** Destructor.
    Destroys instance of ::SbChunkedLoDTileTree class. */
    ~SbChunkedLoDTileTree();
    /* Attributes. */
    /// Number of tiles in tile quad-tree.
    int tree_size;
    /// Size of tile geometry side.
    int tile_size;
    /// List of tile quad-tree tiles.
    SbChunkedLoDTileList tiles;
};

#endif
