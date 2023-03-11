///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Ter  rendered by the Geo Mip-Mapping algorithm.
/// \file SoSimpleGeoMipmapTerrain.cpp
/// \author Radek Barton - xbarto33
/// \date 29.01.2006
///
/// The scene graph node represents the terrain rendered by the Geo algorithm
/// Mip-Mapping.
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
#include <geomipmapping/SoSimpleGeoMipmapTerrain.h>

/******************************************************************************
* SoSimpleGeoMipmapTerrain - public
******************************************************************************/

SO_NODE_SOURCE(SoSimpleGeoMipmapTerrain)

void SoSimpleGeoMipmapTerrain::initClass()
{
  /* Inicializace tridy. */
  SO_NODE_INIT_CLASS(SoSimpleGeoMipmapTerrain, SoShape, "Shape");
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

SoSimpleGeoMipmapTerrain::SoSimpleGeoMipmapTerrain():
  coords(NULL), texture_coords(NULL), normals(NULL), view_volume(NULL),
  viewport_region(NULL), tile_tree(NULL), distance_const(0.0f),
  is_texture(FALSE), is_normals(FALSE),
  map_size(2), tile_size(2), pixel_error(DEFAULT_PIXEL_ERROR),
  is_frustrum_culling(TRUE), is_freeze(FALSE),
  map_size_sensor(NULL), tile_size_sensor(NULL), pixel_error_sensor(NULL),
  frustrum_culling_sensor(NULL), freeze_sensor(NULL)
{
  /* Inicializace tridy. */
  SO_NODE_CONSTRUCTOR(SoSimpleGeoMipmapTerrain);

  /* Inicializace poli */
  SO_NODE_ADD_FIELD(mapSize, (2));
  SO_NODE_ADD_FIELD(tileSize, (2));
  SO_NODE_ADD_FIELD(pixelError, (DEFAULT_PIXEL_ERROR));
  SO_NODE_ADD_FIELD(frustrumCulling, (TRUE));
  SO_NODE_ADD_FIELD(freeze, (FALSE));

  /* Vytvoreni senzoru. */
  map_size_sensor = new SoFieldSensor(mapSizeChangedCB, this);
  tile_size_sensor = new SoFieldSensor(tileSizeChangedCB, this);
  pixel_error_sensor = new SoFieldSensor(pixelErrorChangedCB, this);
  frustrum_culling_sensor = new SoFieldSensor(frustrumCullingChangedCB, this);
  freeze_sensor = new SoFieldSensor(freezeChangedCB, this);

  /* Napojeni senzoru na pole */
  map_size_sensor->attach(&mapSize);
  tile_size_sensor->attach(&tileSize);
  pixel_error_sensor->attach(&pixelError);
  frustrum_culling_sensor->attach(&frustrumCulling);
  freeze_sensor->attach(&freeze);
}

/******************************************************************************
* SoSimpleGeoMipmapTerrain - protected
******************************************************************************/

/* Staticke konstanty. */
const int SoSimpleGeoMipmapTerrain::DEFAULT_PIXEL_ERROR = 20;

void SoSimpleGeoMipmapTerrain::GLRender(SoGLRenderAction * action)
{
  if (!shouldGLRender(action))
  {
    return;
  }

  /* Ziskani informaci z grafu sceny. */
  SoState * state = action->getState();
  view_volume = &SoViewVolumeElement::get(state);
  viewport_region = &SoViewportRegionElement::get(state);

  /* Pri prvnim prubehu se vygeneruje dlazdice */
  static SbBool first_run = TRUE;
  if (first_run)
  {
    PR_START_PROFILE(preprocess);

    first_run = FALSE;

    /* Pracujeme pouze s tridimenzionalnimi geometrickymi souradnicemi
    a dvoudimenzionalnimi texturovymi souradnicemi. */
    assert(SoCoordinateElement::getInstance(state)->is3D() &&
      (SoTextureCoordinateElement::getInstance(state)->getDimension() == 2));

    /* Ziskani vrcholu a texturovych souradnic. */
    coords = SoCoordinateElement::getInstance(state)->getArrayPtr3();
    texture_coords = SoTextureCoordinateElement::getInstance(state)->
      getArrayPtr2();
    normals = SoNormalElement::getInstance(state)->getArrayPtr();

    /* Kontrlola velikosti mapy a dlazdice. */
    assert(((map_size - 1) % (tile_size - 1)) == 0);

    /* Celkovy pocet dlazdic. */
    int tile_count = (map_size - 1) / (tile_size - 1);
    tile_count = SbSqr(tile_count);

    /* Vytvoreni stromu dlazdic. */
    tile_tree = new SbGeoMipmapTileTree(tile_count, tile_size);
    initTree(0, SbBox2s(0, 0, map_size - 1, map_size - 1));
    PR_STOP_PROFILE(preprocess);
  }

  /* Neni-li algoritmus vypnut provedeme vyber urovni dlazdic a frustrum
  culling. */
  if (!is_freeze)
  {
    /* Vypocet konstanty pro vypocet vzdalenosti pro zvoleni dane urovne
    dlazdice. */
    distance_const = (view_volume->getNearDist() *
      viewport_region->getViewportSizePixels()[1]) /
      (pixel_error * view_volume->getHeight());

    recomputeTree(0, TRUE);
  }

  /* Inicializace vykreslovani. */
  beginSolidShape(action);
  SoNormalBindingElement::Binding norm_bind =
    SoNormalBindingElement::get(state);
  SoMaterialBundle mat_bundle = SoMaterialBundle(action);

  this->is_texture = (SoTextureEnabledElement::get(state) &&
    SoTextureCoordinateElement::getType(state) !=
    SoTextureCoordinateElement::NONE);
  this->is_normals = (this->normals && SoLightModelElement::get(state) !=
    SoLightModelElement::BASE_COLOR);

  mat_bundle.sendFirst();

  if (!is_normals)
  {
    norm_bind = SoNormalBindingElement::OVERALL;
  }

  if (norm_bind == SoNormalBindingElement::OVERALL)
  {
    glNormal3f(0.0f, 0.0f, 1.0f);
  }

  renderTree(action, 0);
  endSolidShape(action);
}

#define SEND_VERTEX(ind) index = (ind); \
   vertex.setPoint(coords[index]); \
   vertex.setTextureCoords(texture_coords[index]); \
   vertex.setNormal(normals[index]); \
   shapeVertex(&vertex);

void SoSimpleGeoMipmapTerrain::generatePrimitives(SoAction * action)
{
  SoPrimitiveVertex vertex;
  int index;

  SoState * state = action->getState();
  coords = SoCoordinateElement::getInstance(state)->getArrayPtr3();
  texture_coords = SoTextureCoordinateElement::getInstance(state)->
    getArrayPtr2();
  normals = SoNormalElement::getInstance(state)->getArrayPtr();

  /* Brutal-force vygenerovani triangle-stripu vyskove mapy. */
  for (int Y = 0; Y < (map_size - 1); ++Y)
  {
    beginShape(action, QUAD_STRIP);

    /* Prvni dva vrcholy pasu. */
    SEND_VERTEX((Y + 1) * map_size);
    SEND_VERTEX(Y * map_size);

    for (int X = 1; X < map_size; ++X)
    {
      /* Dalsi vrcholy pasu. */
      SEND_VERTEX(((Y + 1)  * map_size) + X);
      SEND_VERTEX((Y * map_size) + X);
    }
    endShape();
  }
}

void SoSimpleGeoMipmapTerrain::computeBBox(SoAction * action, SbBox3f & box,
  SbVec3f & center)
{
  /* Vypocet ohranicujiciho kvadru a jeho stredu. */
  if (tile_tree != NULL)
  {
    box = tile_tree->tiles[0].bounds;
  }
  /* Ohraniceni neni jeste spocitano v preprocesingu. */
  else
  {
    SoState * state = action->getState();
    const SoCoordinateElement * coords = SoCoordinateElement::getInstance(state);
    int map_size = mapSize.getValue();

    /* Vypocet ohraniceni podle dvou rohu vyskove mapy. */
    SbVec3f min = coords->get3(0);
    SbVec3f max = coords->get3(map_size * map_size - 1);
    max[2] = (max[1] - min[1]) * 0.5;
    min[2] = -max[2];
    box.setBounds(min, max);
  }
  center = box.getCenter();
}

SbBox3f SoSimpleGeoMipmapTerrain::initTree(const int index, SbBox2s coord_box)
{
  SbGeoMipmapTile & tile = tile_tree->tiles[index];

  if (index >= tile_tree->bottom_start)
  {
    /* Inicializace dlazdice na nejnizsi urovni. */
    initTile(tile, coord_box);
  }
  else
  {
    /* Vypocet indexu potomku. */
    int first_index = (index << 2) + 1;
    int second_index = first_index + 1;
    int third_index = second_index + 1;
    int fourth_index = third_index + 1;

    /* Zjisteni ohraniceni souradnic dlazdice. */
    SbVec2s min = coord_box.getMin();
    SbVec2s max = coord_box.getMax();
    SbVec2s center = SbVec2s((max + min) / 2);

    SbGeoMipmapTile * tiles = tile_tree->tiles;

    /* Inicializace odkazu na sousedy prvni dlazdice. */
    tiles[first_index].left = (tile.left == NULL) ? NULL :
      &tiles[((tile.left - tiles) << 2) + 2];
    tiles[first_index].right = &tiles[second_index];
    tiles[first_index].top = (tile.top == NULL) ? NULL :
      &(tiles[((tile.top - tiles) << 2) + 3]);
    tiles[first_index].bottom = &tiles[third_index];

    /* Inicializace odkazu na sousedy druhe dlazdice. */
    tiles[second_index].left = &tiles[first_index];
    tiles[second_index].right = (tile.right == NULL) ? NULL :
      &(tiles[((tile.right - tiles) << 2) + 1]);
    tiles[second_index].top = (tile.top == NULL) ? NULL :
      &(tiles[((tile.top - tiles) << 2) + 4]);
    tiles[second_index].bottom = &tiles[fourth_index];

    /* Inicializace odkazu na sousedy treti dlazdice. */
    tiles[third_index].left = (tile.left == NULL) ? NULL :
      &(tiles[((tile.left - tiles) << 2) + 4]);
    tiles[third_index].right = &tiles[fourth_index];
    tiles[third_index].top = &tiles[first_index];
    tiles[third_index].bottom = (tile.bottom == NULL) ? NULL :
      &(tiles[((tile.bottom - tiles) << 2) + 1]);

    /* Inicializace odkazu na sousedy ctvrte dlazdice. */
    tiles[fourth_index].left = &tiles[third_index];
    tiles[fourth_index].right = (tile.right == NULL) ? NULL :
      &(tiles[((tile.right - tiles) << 2) + 3]);
    tiles[fourth_index].top = &tiles[second_index];
    tiles[fourth_index].bottom = (tile.bottom == NULL) ? NULL :
      &tiles[((tile.bottom - tiles) << 2) + 2];

    /* Rekurzivni inicializace stromu a vypocet ohraniceni dlazdice. */
    tile.bounds.extendBy(initTree(first_index, SbBox2s(min[0], min[1],
      center[0], center[1])));
    tile.bounds.extendBy(initTree(second_index, SbBox2s(center[0], min[1],
      max[0], center[1])));
    tile.bounds.extendBy(initTree(third_index, SbBox2s(min[0], center[1],
      center[0], max[1])));
    tile.bounds.extendBy(initTree(fourth_index, SbBox2s(center[0], center[1],
      max[0], max[1])));
  }

  return tile.bounds;
}

inline void SoSimpleGeoMipmapTerrain::initTile(SbGeoMipmapTile & tile,
  SbBox2s coord_box)
{
  /* Alokace vsech urovni detailu dlazdice. */
  tile.levels = new SbGeoMipmapTileLevel[tile_tree->level_count];

  /* Ziskani souradnic dlazdice ve vyskove mape */
  SbVec2s min = coord_box.getMin();
  SbVec2s max = coord_box.getMax();

  /* Alokace a inicializace nejlepsi urovne detailu dlazdice. */
  tile.levels[0].vertices = new int[SbSqr(tile_tree->level_sizes[0])];
  int index = 0;
  for (int Y = min[1]; Y <= max[1]; ++Y)
  {
    for (int X = min[0]; X <= max[0]; ++X, ++index)
    {
      int coord_index = Y * map_size + X;
      const SbVec3f & vertex = coords[coord_index];

      /* Vypocet ohraniceni dlazdice a zapis indexu vrcholu do dlazdice. */
      tile.bounds.extendBy(vertex);
      tile.levels[0].vertices[index] = coord_index;
    }
  }

  tile.levels[0].error = 0.0f;

  /* Inicializace ostatnich urovni podle predchozich. */
  for (int I = 1; I < tile_tree->level_count; ++I)
  {
    int level_size = tile_tree->level_sizes[I];
    tile.levels[I].vertices = new int[SbSqr(level_size)];
    initLevel(tile.levels[I], tile.levels[I - 1], level_size);
  }

  /* Vypocet stredu dlazdice. */
  tile.center = tile.bounds.getCenter();
}

inline void SoSimpleGeoMipmapTerrain::initLevel(SbGeoMipmapTileLevel & level,
  SbGeoMipmapTileLevel & parent, const int level_size)
{
  int parent_size = (level_size << 1) - 1;
  float max_error = 0.0f;

  for (int Y = 0; Y < parent_size; ++Y)
  {
    for (int X = 0; X < parent_size; ++X)
    {
      /* U lichych indexu se spocita chyba. */
      if ((X & 0x01) || (Y & 0x01))
      {
        /* Chyba v hornim a dolnim radku se spocita horizontalne. */
        if ((Y == 0) || (Y == (parent_size - 1)))
        {
          int index = (Y * parent_size) + X;
          float tmp_error = SbAbs(coords[parent.vertices[index]][2] -
            ((coords[parent.vertices[index - 1]][2] +
            coords[parent.vertices[index + 1]][2]) * 0.5f));
          max_error = SbMax(max_error, tmp_error);
        }
        /* Chyba v levem a pravem sloupci se spocita vertikalne. */
        else if ((X == 0) || (X == (parent_size - 1)))
        {
          int index = (Y * parent_size) + X;
          float tmp_error = SbAbs(coords[parent.vertices[index]][2] -
            ((coords[parent.vertices[index - parent_size]][2] +
            coords[parent.vertices[index + parent_size]][2]) * 0.5f));
          max_error = SbMax(max_error, tmp_error);
        }
        /* Uprostred dlazdice se chyba spocita podle prave diagonaly. */
        else
        {
          int index = (Y * parent_size) + X;
          float tmp_error = SbAbs(coords[parent.vertices[index]][2] -
            ((coords[parent.vertices[index - parent_size + 1]][2] +
            coords[parent.vertices[index + parent_size - 1]][2]) * 0.5f));
          max_error = SbMax(max_error, tmp_error);
        }
      }
      /* Sude indexy se zaradi do triangulace. */
      else
      {
        level.vertices[((Y >> 1) * level_size) + (X >> 1)] = parent.vertices[
          (Y * parent_size) + X];
      }
    }
  }

  /* Ulozeni chyby a vypocet vzdalenosti pro zobrazeni urovne. */
  level.error = parent.error + max_error;
}

void SoSimpleGeoMipmapTerrain::recomputeTree(const int index,
  const SbBool render_parent)
{
  SbGeoMipmapTile & tile = tile_tree->tiles[index];

  /* Neni-li dlazdice pouze virtualni dlazdici */
  if (tile.levels != NULL)
  {
    /* Je-li dlazdice v pohledu kamery. */
    if (!is_frustrum_culling || (render_parent &&
      view_volume->intersect(tile.bounds)))
    {
      tile.level = pickLevel(tile);
    }
    else
    {
      tile.level = SbGeoMipmapTile::LEVEL_NONE;
    }
  }
  /* Dlazdice je virtualni. */
  else
  {
    /* Je-li dlazdice v pohledu kamery. */
    if (!is_frustrum_culling || (render_parent &&
      view_volume->intersect(tile.bounds)))
    {
      tile.level = 0;
    }
    else
    {
      tile.level = SbGeoMipmapTile::LEVEL_NONE;
    }

    /* Vypocet indexu potomku. */
    int first_index = (index << 2) + 1;
    int second_index = first_index + 1;
    int third_index = second_index + 1;
    int fourth_index = third_index + 1;

    /* Vykresleni potomku dlazdice. */
    recomputeTree(first_index, !tile.level);
    recomputeTree(second_index, !tile.level);
    recomputeTree(third_index, !tile.level);
    recomputeTree(fourth_index, !tile.level);
  }
}

#define GL_SEND_VERTEX(index) vertex_index = index; \
  if (is_texture) \
    glTexCoord2fv(texture_coords[vertex_index].getValue()); \
  if (is_normals) \
    glNormal3fv(normals[vertex_index].getValue()); \
  glVertex3fv(coords[vertex_index].getValue());

void SoSimpleGeoMipmapTerrain::renderTree(SoAction * action, const int index)
{
  SbGeoMipmapTile & tile = tile_tree->tiles[index];

  /* Ma-li se dlazdice vykreslit. */
  if (tile.level != SbGeoMipmapTile::LEVEL_NONE)
  {
    /* Neni-li dlazdice pouze virtualni dlazdici */
    if (tile.levels != NULL)
    {
      int vertex_index;

      /* Vyber vrcholu urovne dlazdice, ktera se ma vykreslit. */
      SbGeoMipmapTileLevel & level = tile.levels[tile.level];
      int size = tile_tree->level_sizes[tile.level];
      int * vertices = level.vertices;
      int max_x = size;
      int max_y = size;

      // zkracene vyhodnoceni !!!
      SbBool draw_right = (tile.right != NULL) && (tile.right->level !=
        tile.level) && (tile.right->level != SbGeoMipmapTile::LEVEL_NONE);
      SbBool draw_bottom = (tile.bottom != NULL) && (tile.bottom->level !=
        tile.level) && (tile.bottom->level != SbGeoMipmapTile::LEVEL_NONE);

      /* Napojeni sousedni dlazdice zprava. */
      if (draw_right)
      {
        max_x--;

        int right_level = tile.right->level;
        int right_size = tile_tree->level_sizes[right_level];

        /* Ma-li prava dlazdice mensi dataily. */
        if (right_level > tile.level)
        {
          int fan_size = (size - 1) / (right_size - 1);
          for (int Y = fan_size; Y < size; Y += fan_size)
          {
            glBegin(GL_TRIANGLE_FAN);
            GL_SEND_VERTEX(vertices[(Y * size) + size - 1]);
            int max_i = (Y == (size - 1)) && (draw_bottom) ? Y - 1 : Y;
            for (int I = max_i; I >= (Y - fan_size); --I)
            {
              GL_SEND_VERTEX(vertices[(I * size) + size - 2]);
            }
            GL_SEND_VERTEX(vertices[((Y - fan_size) * size) + size - 1]);
            glEnd();
          }
        }
        /* Ma-li prava dlazdice vetsi detaily. */
        else if (tile.right->level < tile.level)
        {
          int fan_size = (right_size - 1) / (size - 1);
          int * right_vertices = tile.right->levels[right_level].vertices;

          for (int Y = 0; Y < (size - 1); ++Y)
          {
            glBegin(GL_TRIANGLE_FAN);
            GL_SEND_VERTEX(vertices[(Y * size) + size - 2]);
            for (int I = (Y * fan_size); I <= (Y * fan_size) + fan_size; ++I)
            {
              GL_SEND_VERTEX(right_vertices[I * right_size]);
            }
            if ((Y != (size - 2)) || (!draw_bottom))
            {
              GL_SEND_VERTEX(vertices[((Y + 1) * size) + size - 2]);
            }
            glEnd();
          }
        }
      }

      /* Napojeni sousedni dlazdice zdola. */
      if (draw_bottom)
      {
        max_y--;

        int bottom_level = tile.bottom->level;
        int bottom_size = tile_tree->level_sizes[bottom_level];

        /* Ma-li spodni dlazdice mensi dataily. */
        if (bottom_level > tile.level)
        {
          int fan_size = (size - 1) / (bottom_size - 1);
          for (int X = fan_size; X < size; X += fan_size)
          {
            glBegin(GL_TRIANGLE_FAN);
            GL_SEND_VERTEX(vertices[((size - 1) * size) + X]);
            GL_SEND_VERTEX(vertices[((size - 1) * size) + X - fan_size]);
            int max_i = (X == (size - 1)) && (draw_right) ? X : X + 1;
            for (int I = (X - fan_size); I < max_i; ++I)
            {
              GL_SEND_VERTEX(vertices[((size - 2) * size) + I]);
            }
            glEnd();
          }
        }
        /* Ma-li spodni dlazdice vetsi detaily. */
        else if (tile.bottom->level < tile.level)
        {
          int fan_size = (bottom_size - 1) / (size - 1);
          int * bottom_vertices = tile.bottom->levels[bottom_level].vertices;

          for (int X = 0; X < (size - 1); ++X)
          {
            glBegin(GL_TRIANGLE_FAN);
            GL_SEND_VERTEX(vertices[((size - 2) * size) + X]);
            if ((X != (size - 2)) || (!draw_right))
            {
              GL_SEND_VERTEX(vertices[((size - 2) * size) + X + 1]);
            }
            for (int I = (X * fan_size) + fan_size; I >= (X * fan_size); --I)
            {
              GL_SEND_VERTEX(bottom_vertices[I]);
            }
            glEnd();
          }
        }
      }

      /* Vykresleni vnitrku dlazdice. */
      for (int Y = 0; Y < (max_y - 1); ++Y)
      {
        glBegin(GL_QUAD_STRIP);

        /* Prvni dva vrcholy pasu. */
        GL_SEND_VERTEX(vertices[(Y + 1) * size]);
        GL_SEND_VERTEX(vertices[Y * size]);

        for (int X = 1; X < max_x; ++X)
        {
          /* Vykresleni dalsich dvou vrcholu pasu. */
          GL_SEND_VERTEX(vertices[((Y + 1)  * size) + X]);
          GL_SEND_VERTEX(vertices[(Y * size) + X]);
        }
        glEnd();
      }
    }
    /* Dlazdice je virtualni. */
    else
    {
      /* Vypocet indexu potomku. */
      int first_index = (index << 2) + 1;
      int second_index = first_index + 1;
      int third_index = second_index + 1;
      int fourth_index = third_index + 1;

      /* Vykresleni potomku dlazdice. */
      renderTree(action, first_index);
      renderTree(action, second_index);
      renderTree(action, third_index);
      renderTree(action, fourth_index);
    }
  }
}

inline int SoSimpleGeoMipmapTerrain::pickLevel(const SbGeoMipmapTile & tile) const
{
  /* Vypocet vzdalenosti kamery od dlazdice. */
  SbVec3f camera_position = view_volume->getProjectionPoint();
  float distance = (tile.center - camera_position).sqrLength();

  /* Vyber urovne detailu dlazdice podle vzdalenosti od kamery. */
  for (int I = 0; I < tile_tree->level_count; ++I)
  {
    if (distance < SbSqr(tile.levels[I].error * distance_const))
    {
      return I - 1;
    }
  }
  return tile_tree->level_count - 1;
}

void SoSimpleGeoMipmapTerrain::mapSizeChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleGeoMipmapTerrain * instance =
    reinterpret_cast<SoSimpleGeoMipmapTerrain *>(_instance);
  instance->map_size = instance->mapSize.getValue();
}

void SoSimpleGeoMipmapTerrain::tileSizeChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleGeoMipmapTerrain * instance =
    reinterpret_cast<SoSimpleGeoMipmapTerrain *>(_instance);
  instance->tile_size = instance->tileSize.getValue();
}

void SoSimpleGeoMipmapTerrain::pixelErrorChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleGeoMipmapTerrain * instance =
    reinterpret_cast<SoSimpleGeoMipmapTerrain *>(_instance);
  instance->pixel_error = instance->pixelError.getValue();
}

void SoSimpleGeoMipmapTerrain::frustrumCullingChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleGeoMipmapTerrain * instance =
    reinterpret_cast<SoSimpleGeoMipmapTerrain *>(_instance);
  instance->is_frustrum_culling = instance->frustrumCulling.getValue();
}

void SoSimpleGeoMipmapTerrain::freezeChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleGeoMipmapTerrain * instance =
    reinterpret_cast<SoSimpleGeoMipmapTerrain *>(_instance);
  instance->is_freeze = instance->freeze.getValue();
}

/******************************************************************************
* SoSimpleGeoMipmapTerrain - private
******************************************************************************/

SoSimpleGeoMipmapTerrain::~SoSimpleGeoMipmapTerrain()
{
  /* Uvolneni pameti. */
  delete tile_tree;
  delete map_size_sensor;
  delete tile_size_sensor;
  delete pixel_error_sensor;
  delete frustrum_culling_sensor;
  delete freeze_sensor;
}
