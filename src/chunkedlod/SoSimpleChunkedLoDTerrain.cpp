///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
///
/// \file SoSimpleChunkedLoDTerrain.cpp
/// \author Radek Barton - xbarto33@stud.fit.vutbr.cz
/// \date 08.11.2006
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

// Coin inlcudes.
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/lists/SbIntList.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoTextureEnabledElement.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>

// Local includes.
#include <chunkedlod/SoSimpleChunkedLoDTerrain.h>
#include <profiler/PrProfiler.h>
#include <debug.h>

/******************************************************************************
* SoSimpleChunkedLoDTerrain - public
******************************************************************************/

SO_NODE_SOURCE(SoSimpleChunkedLoDTerrain)

void SoSimpleChunkedLoDTerrain::initClass()
{
  // Init class.
  SO_NODE_INIT_CLASS(SoSimpleChunkedLoDTerrain, SoShape, "Shape");
  SO_ENABLE(SoGLRenderAction, SoMaterialBindingElement);
  SO_ENABLE(SoGLRenderAction, SoCoordinateElement);
  SO_ENABLE(SoGLRenderAction, SoTextureCoordinateElement);
  SO_ENABLE(SoGLRenderAction, SoTextureEnabledElement);
  SO_ENABLE(SoGLRenderAction, SoLightModelElement);
  SO_ENABLE(SoGLRenderAction, SoNormalElement);
  SO_ENABLE(SoGLRenderAction, SoNormalBindingElement);
  SO_ENABLE(SoGLRenderAction, SoViewVolumeElement);
  SO_ENABLE(SoGLRenderAction, SoViewportRegionElement);
  SO_ENABLE(SoGetBoundingBoxAction, SoCoordinateElement);
}

SoSimpleChunkedLoDTerrain::SoSimpleChunkedLoDTerrain():
  coords(NULL), texture_coords(NULL), normals(NULL),
  view_volume(SbViewVolume()), viewport_region(SbViewportRegion()),
  tile_tree(NULL), distance_const(0.0f), is_texture(FALSE), is_normals(FALSE),
  map_size(2), tile_size(2), pixel_error(DEFAULT_PIXEL_ERROR),
  is_frustrum_culling(TRUE), is_freeze(FALSE), map_size_sensor(NULL),
  tile_size_sensor(NULL), pixel_error_sensor(NULL),
  frustrum_culling_sensor(NULL), freeze_sensor(NULL)
{
  // Init object.
  SO_NODE_CONSTRUCTOR(SoSimpleChunkedLoDTerrain);

  // Init fields.
  SO_NODE_ADD_FIELD(mapSize, (2));
  SO_NODE_ADD_FIELD(tileSize, (2));
  SO_NODE_ADD_FIELD(pixelError, (DEFAULT_PIXEL_ERROR));
  SO_NODE_ADD_FIELD(frustrumCulling, (TRUE));
  SO_NODE_ADD_FIELD(freeze, (FALSE));

  // Create sensors.
  this->map_size_sensor = new SoFieldSensor(mapSizeChangedCB, this);
  this->tile_size_sensor = new SoFieldSensor(tileSizeChangedCB, this);
  this->pixel_error_sensor = new SoFieldSensor(pixelErrorChangedCB, this);
  this->frustrum_culling_sensor = new SoFieldSensor(frustrumCullingChangedCB,
    this);
  this->freeze_sensor = new SoFieldSensor(freezeChangedCB, this);

  // Connect fields to sensors.
  this->map_size_sensor->attach(&(this->mapSize));
  this->tile_size_sensor->attach(&(this->tileSize));
  this->pixel_error_sensor->attach(&(this->pixelError));
  this->frustrum_culling_sensor->attach(&(this->frustrumCulling));
  this->freeze_sensor->attach(&(this->freeze));
}

void SoSimpleChunkedLoDTerrain::GLRender(SoGLRenderAction * action)
{
  if (!this->shouldGLRender(action))
  {
    return;
  }

  // Get information from scene graph.
  SoState * state = action->getState();

  static SbBool first_run = TRUE;
  if (first_run)
  {
    first_run = FALSE;

    // Only 3D geometic coordinates and 2D texture coordinates are supported.
    assert(SoCoordinateElement::getInstance(state)->is3D() &&
      (SoTextureCoordinateElement::getInstance(state)->getDimension() == 2));

    // Check map and tile size values.
    assert(((this->map_size - 1) % (this->tile_size - 1)) == 0);

    // Get texture and geomety coordinates and normals.
    this->coords = SoCoordinateElement::getInstance(state)->getArrayPtr3();
    this->texture_coords = SoTextureCoordinateElement::getInstance(state)->
      getArrayPtr2();
    this->normals = SoNormalElement::getInstance(state)->getArrayPtr();

    // Count tile tree size.
    int tile_count = (this->map_size - 1) / (this->tile_size - 1);
    int level_size = SbSqr(tile_count);
    int tree_size = level_size;
    while (level_size > 1)
    {
      level_size >>= 2;
      tree_size+= level_size;
    }

    // Create tile tree.
    PR_START_PROFILE(preprocess);
    this->tile_tree = new SbChunkedLoDTileTree(tree_size, this->tile_size);
    initTree(0, SbBox2s(0, 0, this->map_size - 1, this->map_size - 1));
    PR_STOP_PROFILE(preprocess);

    // Init rendering.
    this->is_texture = (SoTextureEnabledElement::get(state) &&
      SoTextureCoordinateElement::getType(state) !=
      SoTextureCoordinateElement::NONE);
    this->is_normals = (this->normals && SoLightModelElement::get(state) !=
      SoLightModelElement::BASE_COLOR);
  }

  // If is't algorithm freezed, recompute displayed tiles from tree.
  if (!this->is_freeze)
  {
    // Update view volume, viewport.
    this->view_volume = SoViewVolumeElement::get(state);
    this->viewport_region = SoViewportRegionElement::get(state);

    // Recompute distance constant.
    distance_const = (this->view_volume.getNearDist() *
    this->viewport_region.getViewportSizePixels()[1]) /
    (this->pixel_error * this->view_volume.getHeight());
  }

  // Render tile tree.
  this->beginSolidShape(action);
  SoMaterialBundle mat_bundle = SoMaterialBundle(action);
  mat_bundle.sendFirst();
  this->renderTree(action, 0);
  this->endSolidShape(action);
}

void SoSimpleChunkedLoDTerrain::generatePrimitives(SoAction * action)
{
  SoState * state = action->getState();
  this->coords = SoCoordinateElement::getInstance(state)->getArrayPtr3();
  this->texture_coords = SoTextureCoordinateElement::getInstance(state)->
    getArrayPtr2();
  this->normals = SoNormalElement::getInstance(state)->getArrayPtr();

  // Brutal-force generation of height map triangles.
  for (int Y = 0; Y < (this->map_size - 1); ++Y)
  {
    beginShape(action, QUAD_STRIP);
    for (int X = 0; X < this->map_size; ++X)
    {
      int index;
      SoPrimitiveVertex vertex;

      // First vertex of strip.
      index = ((Y + 1) * this->map_size) + X;
      vertex.setPoint(this->coords[index]);
      vertex.setTextureCoords(this->texture_coords[index]);
      vertex.setNormal(this->normals[index]);
      shapeVertex(&vertex);

      // Second vertex of strip.
      index = index - this->map_size;
      vertex.setPoint(this->coords[index]);
      vertex.setTextureCoords(this->texture_coords[index]);
      vertex.setNormal(this->normals[index]);
      shapeVertex(&vertex);
    }
    endShape();
  }
}

void SoSimpleChunkedLoDTerrain::computeBBox(SoAction * action, SbBox3f & box,
  SbVec3f & center)
{
  // Return bounding box and center of tile tree if exists.
  if (this->tile_tree != NULL)
  {
    box = this->tile_tree->tiles[0].bounds;
  }
  // Compute bounding box from height map.
  else
  {
    SoState * state = action->getState();
    const SoCoordinateElement * coords =
      SoCoordinateElement::getInstance(state);
    this->map_size = this->mapSize.getValue();

    // Take two corners to compute.
    SbVec3f min = coords->get3(0);
    SbVec3f max = coords->get3(this->map_size * this->map_size - 1);
    max[2] = (max[1] - min[1]) * 0.5;
    min[2] = -max[2];
    box.setBounds(min, max);
  }
  center = box.getCenter();
}

/******************************************************************************
* SoSimpleChunkedLoDTerrain - protected
******************************************************************************/

// Init constants.
const int SoSimpleChunkedLoDTerrain::DEFAULT_PIXEL_ERROR = 20;

void SoSimpleChunkedLoDTerrain::mapSizeChangedCB(void * _instance,
  SoSensor * sensor)
{
  // Actualize map size field internal value.
  SoSimpleChunkedLoDTerrain * instance =
    reinterpret_cast<SoSimpleChunkedLoDTerrain *>(_instance);
  instance->map_size = instance->mapSize.getValue();
}

void SoSimpleChunkedLoDTerrain::tileSizeChangedCB(void * _instance,
  SoSensor * sensor)
{
  // Actualize tile size field internal value.
  SoSimpleChunkedLoDTerrain * instance =
    reinterpret_cast<SoSimpleChunkedLoDTerrain *>(_instance);
  instance->tile_size = instance->tileSize.getValue();
}

void SoSimpleChunkedLoDTerrain::pixelErrorChangedCB(void * _instance,
  SoSensor * sensor)
{
  // Actualize pixel error field internal value.
  SoSimpleChunkedLoDTerrain * instance =
    reinterpret_cast<SoSimpleChunkedLoDTerrain *>(_instance);
  instance->pixel_error = instance->pixelError.getValue();
}

void SoSimpleChunkedLoDTerrain::frustrumCullingChangedCB(void * _instance,
  SoSensor * sensor)
{
  // Actualize frustrum culling field internal value.
  SoSimpleChunkedLoDTerrain * instance =
    reinterpret_cast<SoSimpleChunkedLoDTerrain *>(_instance);
  instance->is_frustrum_culling = instance->frustrumCulling.getValue();
}

void SoSimpleChunkedLoDTerrain::freezeChangedCB(void * _instance,
  SoSensor * sensor)
{
  // Actualize freeze field internal value.
  SoSimpleChunkedLoDTerrain * instance =
    reinterpret_cast<SoSimpleChunkedLoDTerrain *>(_instance);
  instance->is_freeze = instance->freeze.getValue();
}

/******************************************************************************
* SoSimpleGeoMipmapTerrain - private
******************************************************************************/

SbBox3f SoSimpleChunkedLoDTerrain::initTree(const int index, SbBox2s coord_box)
{
  SbChunkedLoDTile & tile = this->tile_tree->tiles[index];

  // Recurse to tile's child tiles if not at bottom.
  if (((index << 2 ) + 4) < (this->tile_tree->tree_size))
  {
    // Count indices of succesors.
    int first_index = (index << 2) + 1;
    int second_index = first_index + 1;
    int third_index = second_index + 1;
    int fourth_index = third_index + 1;

    // Get corners and center of tile bounding area.
    const SbVec2s & min = coord_box.getMin();
    const SbVec2s & max = coord_box.getMax();
    SbVec2s center = SbVec2s((max + min) / 2);

    // Compute tile bounding box.
    tile.bounds.extendBy(initTree(first_index, SbBox2s(min[0], min[1],
      center[0], center[1])));
    tile.bounds.extendBy(initTree(second_index, SbBox2s(center[0], min[1],
      max[0], center[1])));
    tile.bounds.extendBy(initTree(third_index, SbBox2s(min[0], center[1],
      center[0], max[1])));
    tile.bounds.extendBy(initTree(fourth_index, SbBox2s(center[0], center[1],
      max[0], max[1])));
  }

  // Init this tile and return bounding box.
  this->initTile(tile, index, coord_box);
  return tile.bounds;
}

inline void SoSimpleChunkedLoDTerrain::initTile(SbChunkedLoDTile & tile,
  int index, SbBox2s coord_box)
{
  const SbVec2s & min = coord_box.getMin();
  const SbVec2s & max = coord_box.getMax();
  int min_x = min[0];
  int min_y = min[1];
  int max_x = max[0];
  int max_y = max[1];
  int inc_x = (max[0] - min[0]) / (this->tile_size - 1);
  int inc_y = (max[1] - min[1]) / (this->tile_size - 1);

  // Simplified intitialization for tiles on bottom level of tile tree.
  if (((index << 2) + 4) >= this->tile_tree->tree_size)
  {
    // Copy every vertex index within coordinates box.
    int vertex_index = 0;
    for (int Y = min_y; Y <= max_y; ++Y)
    {
      for (int X = min_x; X <= max_x; ++X, ++vertex_index)
      {
        int coord_index = Y * this->map_size + X;
        const SbVec3f & vertex = this->coords[coord_index];

        tile.bounds.extendBy(vertex);
        tile.vertices[vertex_index] = coord_index;
      }
    }

    // Full detailed tile.
    tile.error = (tile.bounds.getMax()[2] - tile.bounds.getMax()[2]) * 0.01f;
  }
  else
  {
    float max_error = 0.0f;
    int half_inc_x = inc_x >> 1;
    int half_inc_y = inc_y >> 1;

    int vertex_index = 0;
    for (int Y = min_y; Y <= max_y; Y+= inc_y)
    {
      for (int X = min_x; X <= max_x; X+= inc_x, ++vertex_index)
      {
        int this_index = Y * this->map_size + X;
        const SbVec3f & this_vertex = this->coords[this_index];

        // Don't compute error on borders.
        if ((Y < max_y) && (X < max_x))
        {
          // Horizontal direction.
          const SbVec3f & next_h_vertex = this->coords[this_index + inc_x];
          const SbVec3f & half_h_vertex = this->coords[this_index +
            half_inc_x];
          float tmp_error = SbAbs(half_h_vertex[2] - ((this_vertex[2] +
            next_h_vertex[2]) * 0.5f));
          max_error = SbMax(max_error, tmp_error);

          // Vertical direction.
          const SbVec3f & next_v_vertex = this->coords[this_index + (inc_y *
            this->map_size)];
          const SbVec3f & half_v_vertex = this->coords[this_index + (half_inc_y
            * this->map_size)];
          tmp_error = SbAbs(half_v_vertex[2] - ((this_vertex[2] + next_v_vertex[2])
            * 0.5f));
          max_error = SbMax(max_error, tmp_error);

          // Diagonal direction.
          const SbVec3f & next_d_vertex = this->coords[this_index + (inc_y *
            this->map_size) + inc_x];
          const SbVec3f & half_d_vertex = this->coords[this_index + (half_inc_y *
            this->map_size) + half_inc_x];
          tmp_error = SbAbs(half_d_vertex[2] - ((this_vertex[2] + next_d_vertex[2])
            * 0.5f));
          max_error = SbMax(max_error, tmp_error);
        }

        tile.bounds.extendBy(this_vertex);
        tile.vertices[vertex_index] = this_index;
      }
    }

    // Tile error si max error of its own geometry and geomety of all children.
    int child_index = index << 2;
    SbChunkedLoDTile & first_child = this->tile_tree->tiles[++child_index];
    tile.error = SbMax(max_error, first_child.error);
    SbChunkedLoDTile & second_child = this->tile_tree->tiles[++child_index];
    tile.error = SbMax(tile.error, second_child.error);
    SbChunkedLoDTile & third_child = this->tile_tree->tiles[++child_index];
    tile.error = SbMax(tile.error, third_child.error);
    SbChunkedLoDTile & fourth_child = this->tile_tree->tiles[++child_index];
    tile.error = SbMax(tile.error, fourth_child.error);
  }
}

inline void SoSimpleChunkedLoDTerrain::morphVertex(const SbVec3f * coords,
  float morph, int center_index, int first_index, int second_index)
{
  // Morph between center vertex and average of first and second vertices.
  SbVec3f tmp_vertex = SbVec3f(coords[center_index].getValue());
  const SbVec3f & first_vertex = coords[first_index].getValue();
  const SbVec3f & second_vertex = coords[second_index].getValue();
  tmp_vertex[2] = (tmp_vertex[2] * morph) + ((first_vertex[2] +
    second_vertex[2]) * 0.5 * (1.0f - morph));
  glVertex3fv(tmp_vertex.getValue());
}

inline void SoSimpleChunkedLoDTerrain::renderSkirt(SoGLRenderAction * action,
  SbChunkedLoDTile & tile)
{
  const int * vertices = (const int *) tile.vertices.getArrayPtr();
  int max_x = this->tile_size;
  int max_y = this->tile_size;
  float skirt_height = (tile.bounds.getMax() - tile.bounds.getMin())[2] * 0.2;

  // Render top skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int X = 0; X < max_x; ++X)
  {
    int index = vertices[X];
    const SbVec3f & first_vertex = this->coords[index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[index].getValue());
    }

    glVertex3fv(first_vertex.getValue());
    glVertex3fv(second_vertex.getValue());
  }
  glEnd();

  // Render bottom skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int X = 0; X < max_x; ++X)
  {
    int index = vertices[(max_y - 1) * max_x + X];
    const SbVec3f & first_vertex = this->coords[index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[index].getValue());
    }

    glVertex3fv(second_vertex.getValue());
    glVertex3fv(first_vertex.getValue());
  }
  glEnd();

  // Render left skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int Y = 0; Y < max_y; ++Y)
  {
    int index = vertices[Y * max_x];
    const SbVec3f & first_vertex = this->coords[index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[index].getValue());
    }

    glVertex3fv(second_vertex.getValue());
    glVertex3fv(first_vertex.getValue());
  }
  glEnd();

  // Render right skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int Y = 0; Y < max_y; ++Y)
  {
    int index = vertices[Y * max_x + (max_x - 1)];
    const SbVec3f & first_vertex = this->coords[index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[index].getValue());
    }

    glVertex3fv(first_vertex.getValue());
    glVertex3fv(second_vertex.getValue());
  }
  glEnd();
}

inline void SoSimpleChunkedLoDTerrain::renderSkirt(SoGLRenderAction * action,
  SbChunkedLoDTile & tile, float morph)
{
  const int * vertices = (const int *) tile.vertices.getArrayPtr();
  int max_x = this->tile_size;
  int max_y = this->tile_size;
  float skirt_height = (tile.bounds.getMax() - tile.bounds.getMin())[2] * 0.2;

  // Render top skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int X = 0; X < max_x; ++X)
  {
    int index = X;
    int vertex_index = vertices[index];
    const SbVec3f & first_vertex = this->coords[vertex_index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[vertex_index].getValue());
    }

    if (X % 2)
    {
      // Morph between left and right vertices.
      this->morphVertex(this->coords, morph, vertex_index, vertex_index - 1,
        vertex_index + 1);
    }
    else
    {
      glVertex3fv(first_vertex.getValue());
    }
    glVertex3fv(second_vertex.getValue());
  }
  glEnd();

  // Render bottom skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int X = 0; X < max_x; ++X)
  {
    int index = (max_y - 1) * max_x + X;
    int vertex_index = vertices[index];
    const SbVec3f & first_vertex = this->coords[vertex_index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[vertex_index].getValue());
    }

    glVertex3fv(second_vertex.getValue());
    if (X % 2)
    {
      // Morph between left and right vertices.
      this->morphVertex(this->coords, morph, vertex_index, vertex_index - 1,
        vertex_index + 1);
    }
    else
    {
      glVertex3fv(first_vertex.getValue());
    }
  }
  glEnd();

  // Render left skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int Y = 0; Y < max_y; ++Y)
  {
    int index = Y * max_x;
    int vertex_index = vertices[index];
    const SbVec3f & first_vertex = this->coords[vertex_index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[vertex_index].getValue());
    }

    glVertex3fv(second_vertex.getValue());
    if (Y % 2)
    {
      // Morph between previous and next vertices.
      int prev_index = index - max_x;
      int next_index = index + max_x;
      this->morphVertex(this->coords, morph, vertex_index,
        vertices[prev_index], vertices[next_index]);
    }
    else
    {
      glVertex3fv(first_vertex.getValue());
    }
  }
  glEnd();

  // Render right skirt strip.
  glBegin(GL_QUAD_STRIP);
  for (int Y = 0; Y < max_y; ++Y)
  {
    int index = Y * max_x + (max_x - 1);
    int vertex_index = vertices[index];
    const SbVec3f & first_vertex = this->coords[vertex_index];
    SbVec3f second_vertex = SbVec3f(first_vertex);
    second_vertex[2]-= skirt_height;

    if (this->is_texture)
    {
      glTexCoord2fv(this->texture_coords[vertex_index].getValue());
    }

    if (Y % 2)
    {
      // Morph between previous and next vertices.
      int prev_index = index - max_x;
      int next_index = index + max_x;
      this->morphVertex(this->coords, morph, vertex_index,
        vertices[prev_index], vertices[next_index]);
    }
    else
    {
      glVertex3fv(first_vertex.getValue());
    }
    glVertex3fv(second_vertex.getValue());
  }
  glEnd();
}

inline void SoSimpleChunkedLoDTerrain::renderTile(SoGLRenderAction * action,
  SbChunkedLoDTile & tile)
{
  const int * vertices = (const int *) tile.vertices.getArrayPtr();
  int max_x = this->tile_size;
  int max_y = this->tile_size;

  // Render tile geometry
  for (int Y = 0; Y < (max_y - 1); ++Y)
  {
    glBegin(GL_QUAD_STRIP);
    for (int X = 0; X < max_x; ++X)
    {
      int top_index = vertices[(Y * max_x) + X];
      int bottom_index = vertices[((Y + 1) * max_x) + X];

      // Render bottom vertices of strip.
      if (this->is_texture)
      {
        glTexCoord2fv(this->texture_coords[bottom_index].getValue());
      }
      if (this->is_normals)
      {
        glNormal3fv(this->normals[bottom_index].getValue());
      }
      glVertex3fv(this->coords[bottom_index].getValue());

      // Render current vertices of strip.
      if (this->is_texture)
      {
        glTexCoord2fv(this->texture_coords[top_index].getValue());
      }
      if (this->is_normals)
      {
        glNormal3fv(this->normals[top_index].getValue());
      }
      glVertex3fv(this->coords[top_index].getValue());
    }
    glEnd();
  }
}

inline void SoSimpleChunkedLoDTerrain::renderTile(SoGLRenderAction * action,
  SbChunkedLoDTile & tile, float morph)
{
  const int * vertices = (const int *) tile.vertices.getArrayPtr();
  int max_x = this->tile_size;
  int max_y = this->tile_size;

  // Render tile geometry
  for (int Y = 0; Y < (max_y - 1); ++Y)
  {
    glBegin(GL_QUAD_STRIP);
    for (int X = 0; X < max_x; ++X)
    {
      int prev_index = vertices[((Y - 1) * max_x) + X];
      int top_index = vertices[(Y * max_x) + X];
      int bottom_index = vertices[((Y + 1) * max_x) + X];
      int next_index = vertices[((Y + 2) * max_x) + X];

      // Render bottom vertices of strip.
      if (this->is_texture)
      {
        glTexCoord2fv(this->texture_coords[bottom_index].getValue());
      }
      if (this->is_normals)
      {
        glNormal3fv(this->normals[bottom_index].getValue());
      }

      // Lichy radek?
      if (Y % 2)
      {
        // Lichy slopec licheho radku?
        if (X % 2)
        {
          // Morph between left and right vertices.
          this->morphVertex(this->coords, morph, bottom_index,
            bottom_index - 1, bottom_index + 1);
        }
        // Sudy slopec licheho radku.
        else
        {
          glVertex3fv(this->coords[bottom_index].getValue());
        }
      }
      // Sudy radek.
      else
      {
        // Lichy slopec sudeho radku?
        if (X % 2)
        {
          // Morph between top-left and bottom-right vertices.
          this->morphVertex(this->coords, morph, bottom_index,
            top_index - 1, next_index + 1);
        }
        // Sudy slopec sudeho radku.
        else
        {
          // Morph between top and bottom vertices.
            this->morphVertex(this->coords, morph, bottom_index, top_index,
            next_index);
        }
      }

      // Render current vertices of strip.
      if (this->is_texture)
      {
        glTexCoord2fv(this->texture_coords[top_index].getValue());
      }
      if (this->is_normals)
      {
        glNormal3fv(this->normals[top_index].getValue());
      }

      // Lichy radek?
      if (Y % 2)
      {
        // Lichy slopec licheho radku?
        if (X % 2)
        {
          // Morph between top-left and bottom-right vertices.
          this->morphVertex(this->coords, morph, top_index, prev_index - 1,
            bottom_index + 1);
        }
        // Sudy slopec licheho radku.
        else
        {
          // Morph between top and bottom vertices.
          this->morphVertex(this->coords, morph, top_index, prev_index,
            bottom_index);
        }
      }
      // Sudy radek.
      else
      {
        // Lichy slopec sudeho radku?
        if (X % 2)
        {
          // Morph between left and right vertices.
          this->morphVertex(this->coords, morph, top_index, top_index - 1,
            top_index + 1);
        }
        // Sudy slopec sudeho radku.
        else
        {
          glVertex3fv(this->coords[top_index].getValue());
        }
      }
    }
    glEnd();
  }
}

void SoSimpleChunkedLoDTerrain::renderTree(SoGLRenderAction * action,
  const int index)
{
  // Compute distance from camera to tile.
  SbChunkedLoDTile & tile = tile_tree->tiles[index];
  SbVec3f camera_position = this->view_volume.getProjectionPoint();
  float distance = (tile.bounds.getCenter() - camera_position).sqrLength();

  // Recurse if tile isn't fine enough and tile isn't at bottom level of tree.
  if ((((index << 2) + 4) < (this->tile_tree->tree_size)) &&
    (distance < SbSqr(tile.error * distance_const)))
  {
    int first_index = (index << 2) + 1;
    int second_index = first_index + 1;
    int third_index = second_index + 1;
    int fourth_index = third_index + 1;

    this->renderTree(action, first_index);
    this->renderTree(action, second_index);
    this->renderTree(action, third_index);
    this->renderTree(action, fourth_index);
  }
  else
  {
    // Render tile if is in view volume or frustrum culling is disabled.
    if (!this->is_frustrum_culling || this->view_volume.intersect(tile.bounds))
    {
      distance = sqrt(distance);
      float morph = (distance_const * tile.error) / distance;
      morph = SbClamp(2.0f * ((2.0f * morph) - 1.0f), 0.0f, 1.0f);

      Debug(morph);

      if (morph < 1.0f)
      {
        this->renderTile(action, tile, morph);
        this->renderSkirt(action, tile, morph);
      }
      else
      {
        this->renderTile(action, tile);
        this->renderSkirt(action, tile);
      }
    }
  }
}

SoSimpleChunkedLoDTerrain::~SoSimpleChunkedLoDTerrain()
{
  // Free allocated memory.
  delete this->tile_tree;
  delete this->map_size_sensor;
  delete this->tile_size_sensor;
  delete this->pixel_error_sensor;
  delete this->frustrum_culling_sensor;
  delete this->freeze_sensor;
}
