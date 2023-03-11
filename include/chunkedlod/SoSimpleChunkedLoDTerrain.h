#ifndef SO_SIMPLE_CHUNKED_LOD_TERRAIN_H
#define SO_SIMPLE_CHUNKED_LOD_TERRAIN_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Terrain rendered by Chunked LoD algorithm.
/// \file SoSimpleChunkedLoDTerrain.h
/// \author Radek Barton - xbarto33
/// \date 08.11.2006
///
/// This is a scene graph node representing terrain rendered by Chunked LoD
/// algorithm. To use this node you must prepend ::SoCoordinate3 node with input
/// heightmap coordinates, set \e mapSize field to size of side of that
/// heightmap and then set \e tileSize field to desired size of tiles on any
/// level of detail. \e mapSize \p - \p 1 have to be dividable by \e mapSize
/// \p - \p 1 and this fraction have to be \p 2^n where \p n is whole positive
/// number. Moreover \e mapSize and \e tileSize have to be even but not
/// necessarily \p 2^n \p + \p 1 as it is in ::SoSimpleROAMTerrain node.
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
#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/sensors/SoFieldSensor.h>

// OpenGL includes.
#if defined(__WIN32__) || defined(_WIN32)
  #include <windows.h>
#endif
#include <GL/gl.h>

// Standard includes.
#include <assert.h>

// Local includes.
#include <chunkedlod/SbChunkedLoDPrimitives.h>

/** Terrain rendered by Chunked LoD algorithm.
This is a scene graph node representing terrain rendered by Chunked LoD
algorithm. To use this node you must prepend ::SoCoordinate3 node with input
heightmap coordinates, set \e mapSize field to size of side of that heightmap
and then set \e tileSize field to desired size of tiles on any level of detail.
\e mapSize \p - \p 1 have to be dividable by \e mapSize \p - \p 1 and this
fraction have to be \p 2^n where \p n is whole positive number. Moreover
\e mapSize and \e tileSize have to be odd but not necessarily \p 2^n \p +
\p 1 as it is in ::SoSimpleROAMTerrain node. */
class SoSimpleChunkedLoDTerrain : public SoShape
{
  SO_NODE_HEADER(SoSimpleChunkedLoDTerrain);
  public:
    /* Methods. */
    /** Run-time class initialisation.
    This method must be called before any instance of
    ::SoSimpleChunkedLoDTerrain class is created. */
    static void initClass();
    /** Constructor.
    Creates instance of ::SoSimpleChunkedLoDTerrain class. */
    SoSimpleChunkedLoDTerrain();
    /* Fields. */
    /// Size of side of input height map.
    SoSFInt32 mapSize;
    /// Size of side of tile.
    SoSFInt32 tileSize;
    /// Desired pixel error of rendered terrain.
    SoSFInt32 pixelError;
    /// Flag of enabled frustum culling.
    SoSFBool frustrumCulling;
    /// Flag of frozen algorithm.
    SoSFBool freeze;
  protected:
    /* Methods. */
    /** Renders terrain.
    Creates tile quad-tree from input heightmap when called first time. Then
    and anytime when called again parses that tree to decide which tiles on
    appropriate level of detail should be rendered accordingly to set pixel
    error. Tiles which are inside view frustum or all of them if frustum
    culling is disabled are rendered during this parsing.
    \param action Object with scene graph information. */
    virtual void GLRender(SoGLRenderAction * action);
    /** Creates terrain geometry triangles.
    Creates all triangles from input heightmap in brutal-force manner for
    collision detection, ray picking and other purposes.
    \param action Object with scene graph information. */
    virtual void generatePrimitives(SoAction * action);
    /** Computes bounding box and its center.
    Computes bounding box of whole terrain from input heightmap and its center
    and returns them in \e box and \e center parameters.
    \param action Object with scene graph information.
    \param box Resulting bounding box of terrain.
    \param center Center of resulting bounding box of terrain. */
    virtual void computeBBox(SoAction * action, SbBox3f & box,
      SbVec3f & center);
    /* Callbacks. */
    /** Callback for change of SoSimpleChunkedLoDTerrain::mapSize field.
    Sets internal value of SoSimpleChunkedLoDTerrain::mapSize field to
    new value when this fields has changed.
    \param instance Pointer to affected ::SoSimpleChunkedLoDTerrain class
      instance.
    \param sensor Sensor which called this callback. */
    static void mapSizeChangedCB(void * instance, SoSensor * sensor);
    /** Callback for change of SoSimpleChunkedLoDTerrain::tileSize field.
    Sets internal value of SoSimpleChunkedLoDTerrain::tileSize field to
    new value when this fields has changed.
    \param instance Pointer to affected ::SoSimpleChunkedLoDTerrain class
      instance.
    \param sensor Sensor which called this callback. */
    static void tileSizeChangedCB(void * instance, SoSensor * sensor);
    /** Callback for change of SoSimpleChunkedLoDTerrain::pixelError field.
    Sets internal value of SoSimpleChunkedLoDTerrain::pixelError field to
    new value when this fields has changed.
    \param instance Pointer to affected ::SoSimpleChunkedLoDTerrain class
      instance.
    \param sensor Sensor which called this callback. */
    static void pixelErrorChangedCB(void * instance, SoSensor * sensor);
    /** Callback for change of SoSimpleChunkedLoDTerrain::frustrumCulling field.
    Sets internal value of SoSimpleChunkedLoDTerrain::frustrumCulling field to
    new value when this fields has changed.
    \param instance Pointer to affected ::SoSimpleChunkedLoDTerrain class
      instance.
    \param sensor Sensor which called this callback. */
    static void frustrumCullingChangedCB(void * instance, SoSensor * sensor);
    /** Callback for change of SoSimpleChunkedLoDTerrain::freeze field.
    Sets internal value of SoSimpleChunkedLoDTerrain::freeze field to
    new value when this fields has changed.
    \param instance Pointer to affected ::SoSimpleChunkedLoDTerrain class
      instance.
    \param sensor Sensor which called this callback. */
    static void freezeChangedCB(void * instance, SoSensor * sensor);
    /* Elements shortcuts. */
    /// Coordinates of input heightmap.
    const SbVec3f * coords;
    /// Coordinates of heightmap texture.
    const SbVec2f * texture_coords;
    /// Normals of input heightmap.
    const SbVec3f * normals;
    /// Current view volume. 
    SbViewVolume view_volume;
    /// Current viewport region.
    SbViewportRegion viewport_region;
    /* Internal data. */
    /// Tile quad-tree.
    SbChunkedLoDTileTree * tile_tree;
    /// Distance constant for coumputing dynamic part of error metric.
    float distance_const;
    /// Flag that texture is pressent and should be rendered.
    SbBool is_texture;
    /// Flag that normals are present and should be rendered.
    SbBool is_normals;
    /* Internal fields values. */
    /// Internal value of SoSimpleChunkedLoDTerrain::mapSize field.
    int map_size;
    /// Internal value of SoSimpleChunkedLoDTerrain::tileSize field.
    int tile_size;
    /// Internal value of SoSimpleChunkedLoDTerrain::pixelError field.
    int pixel_error;
    /// Internal value of SoSimpleChunkedLoDTerrain::frustrumCulling field.
    SbBool is_frustrum_culling;
    /// Internal value of SoSimpleChunkedLoDTerrain::freeze field.
    SbBool is_freeze;
    /* Sensors. */
    /// Sensor watching SoSimpleChunkedLoDTerrain::mapSize field changes.
    SoFieldSensor * map_size_sensor;
    /// Sensor watching SoSimpleChunkedLoDTerrain::tileSize field changes.
    SoFieldSensor * tile_size_sensor;
    /// Sensor watching SoSimpleChunkedLoDTerrain::pixelError field changes.
    SoFieldSensor * pixel_error_sensor;
    /// Sensor watching SoSimpleChunkedLoDTerrain::frustrumCulling field changes.
    SoFieldSensor * frustrum_culling_sensor;
    /// Sensor watching SoSimpleChunkedLoDTerrain::freeze field changes.
    SoFieldSensor * freeze_sensor;
    /* Constants. */
    /// Constants for default pixel error of tile.
    static const int DEFAULT_PIXEL_ERROR;
  private:
    /* Methods. */
    /** Initialises tile quad-tree.
    Intializes tile quad-tree in recursive way starting with root or subroot
    tile on index \e index with coordinates of input heightmap bounded by
    \e coord_box rectangle. It parses tree bottom-up acctually but should be
    called with \e index value setted to zero.
    \param index Index of root tile, should be setted always to zero.
    \param coord_box Bounding rectangle of input heightmap coordinates. */
    SbBox3f initTree(const int index, SbBox2s coord_box);
    /** Initialises quad-tree tile.
    Initialises quad-tree tile \e tile geometry, static part of error metric
    and bounding box. Parametr \e index is index of tile in quad-tree and
    \e coord_box parameter is bounding rectangle of input heightmap
    coordinates.
    \param tile Initialised quad-tree tile.
    \param index Index of tile in quad-tree
    \param coord_box Bounding rectangle of input heightmap coordinates. */
    inline void initTile(SbChunkedLoDTile & tile, int index,
      SbBox2s coord_box);
    /** Renders vertex morphed.
    Sends vertex coordinate to rendering depending on \e morph morphing factor.
    If morphing factor is equal or lower than zero computes and sends average
    vertex of vertices on \e first_index and  \e second_index indices in
    \e coords coordinate array. If morphing factor is eqal or greater than one
    sends vertex on index \e center_index. It morphs between this two vertices
    with morphing factor otherwise.
    \param coords Array of heightmap vertices.
    \param morph Morphing factor.
    \param center_index Index of center vertex.
    \param first_index Index of first vertex to theirs average morph.
    \param second_index Index of second vertex to theirs average morp. */
    inline void morphVertex(const SbVec3f * coords, float morph,
      int center_index, int first_index, int second_index);
    /** Renders tile skirt.
    Renders skirt around tile \e tile to fill gaps between neightbouring tiles
    on different level of detail.
    \param action Object with scene graph informations.
    \param tile Rendered skirt tile. */
    inline void renderSkirt(SoGLRenderAction * action, SbChunkedLoDTile & tile);
    /** Renders tile skirt (morphed version).
    Renders skirt around tile \e tile with morphing factor \e morph to fill
    gaps between neightbouring tiles on different level of detail.
    \param action Object with scene graph informations.
    \param tile Rendered skirt tile.
    \param morph Morphing factor of rendering. */
    inline void renderSkirt(SoGLRenderAction * action, SbChunkedLoDTile & tile,
      float morph);
    /** Renders tile.
    Renders \e tile tile geometry.
    \param action Object with scene graph informations.
    \param tile Rendered tile. */
    inline void renderTile(SoGLRenderAction * action, SbChunkedLoDTile & tile);
    /** Renders tile (morphed version).
    Renders \e tile tile geometry with morphing factor \e morph.
    \param action Object with scene graph informations.
    \param tile Rendered tile.
    \param morph Morphing factor of rendering. */
    inline void renderTile(SoGLRenderAction * action, SbChunkedLoDTile & tile,
      float morph);
    /** Renders tile tree.
    Renders whole terrain on appropriate level of detail starting with root
    tile on index \e index in tile quad-tree. This index should be always setted
    to zere when called not recursively.
    \param action Object with scene graph informations.
    \param index Index of root tile, should be always setted to zero. */
    void renderTree(SoGLRenderAction * action, const int index);
    /** Destructor.
    Privatised because Coin handles nodes memory frees itself. */
    virtual ~SoSimpleChunkedLoDTerrain();
};

#endif
