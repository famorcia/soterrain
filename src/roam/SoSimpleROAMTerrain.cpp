///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Ter� vykreslovan algoritmem ROAM.
/// \file SoSimpleROAMTerrain.cpp
/// \author Radek Barto�- xbarto33
/// \date 25.08.2005
///
/// Uzel grafu sc�y reprezentuj��ter� vykreslovan algoritmem ROAM. Pro
/// pouit�je teba uzlu pedadit uzel s koordin�y typu \p SoCordinate3
/// obsahuj��vkovou mapu ter�u a nastavit jej�rozm�y. Velikost vkov�/// mapy mus�bt �vercov�o velikosti strany 2^n + 1, kde n je cel�kladn�/// �slo.
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

#include <roam/SoSimpleROAMTerrain.h>

SO_NODE_SOURCE(SoSimpleROAMTerrain)

/******************************************************************************
* SoSimpleROAMTerrain - public
******************************************************************************/

void SoSimpleROAMTerrain::initClass()
{
  /* Inicializace tridy. */
  SO_NODE_INIT_CLASS(SoSimpleROAMTerrain, SoShape, "Shape");
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

SoSimpleROAMTerrain::SoSimpleROAMTerrain():
  coords(NULL), texture_coords(NULL), normals(NULL), view_volume(NULL),
  viewport_region(NULL), triangle_tree(NULL), tree_size(0), level(0),
  lambda(0.0f), split_queue(NULL), merge_queue(NULL),
  is_texture(FALSE), is_normals(FALSE),
  map_size(2), pixel_error(DEFAULT_PIXEL_ERROR),
  triangle_count(DEFAULT_TRIANGLE_COUNT), is_frustrum_culling(TRUE),
  is_freeze(FALSE),
  map_size_sensor(NULL), pixel_error_sensor(NULL), triangle_count_sensor(NULL),
  frustrum_culling_sensor(NULL), freeze_sensor(NULL)
{
  /* Inicializace tridy. */
  SO_NODE_CONSTRUCTOR(SoSimpleROAMTerrain);

  /* Inicializace poli */
  SO_NODE_ADD_FIELD(mapSize, (2));
  SO_NODE_ADD_FIELD(pixelError, (DEFAULT_PIXEL_ERROR));
  SO_NODE_ADD_FIELD(triangleCount, (DEFAULT_TRIANGLE_COUNT));
  SO_NODE_ADD_FIELD(frustrumCulling, (TRUE));
  SO_NODE_ADD_FIELD(freeze, (FALSE));

  /* Vytvoreni senzoru. */
  map_size_sensor = new SoFieldSensor(mapSizeChangedCB, this);
  pixel_error_sensor = new SoFieldSensor(pixelErrorChangedCB, this);
  triangle_count_sensor = new SoFieldSensor(triangleCountChangedCB, this);
  frustrum_culling_sensor = new SoFieldSensor(frustrumCullingChangedCB, this);
  freeze_sensor = new SoFieldSensor(freezeChangedCB, this);

  /* Napojeni senzoru na pole */
  map_size_sensor->attach(&mapSize);
  pixel_error_sensor->attach(&pixelError);
  triangle_count_sensor->attach(&triangleCount);
  frustrum_culling_sensor->attach(&frustrumCulling);
  freeze_sensor->attach(&freeze);

  /* Inicializace internich struktur. */
  split_queue = new SbROAMSplitQueue();
  merge_queue = new SbROAMMergeQueue();
}

/******************************************************************************
* SoSimpleROAMTerrain - protected
******************************************************************************/

/* Staticke konstanty. */
const int SoSimpleROAMTerrain::DEFAULT_PIXEL_ERROR = 6;
const int SoSimpleROAMTerrain::DEFAULT_TRIANGLE_COUNT = 5000;

#define GL_SEND_VERTEX(index) vertex_index = index; \
  if (is_texture) \
    glTexCoord2fv(texture_coords[vertex_index].getValue()); \
  if (is_normals) \
    glNormal3fv(normals[vertex_index].getValue()); \
  glVertex3fv(coords[vertex_index].getValue());

void SoSimpleROAMTerrain::GLRender(SoGLRenderAction * action)
{
  if (!shouldGLRender(action))
  {
    return;
  }

  /* Ziskani informaci z grafu sceny. */
  SoState * state = action->getState();
  view_volume = &SoViewVolumeElement::get(state);
  view_volume->getViewVolumePlanes(planes);
  viewport_region = &SoViewportRegionElement::get(state);

  /* Pri prvnim prubehu se vygeneruje strom trojuhelniku */
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

    /* Vypocet levelu jako 2 * log2(map_size - 1) */
    int tmp_size = map_size - 1;
    level = 0;
    while (tmp_size > 1)
    {
      level += 2;
      tmp_size >>= 1;
    }

    /* Zjisteni poctu trojuhelniku ve stromu jako 2^(level + 1) - 1. */
    tree_size = (1 << (level + 1)) - 1;

    /* Vypocet indexu rohovych vertexu mapy. */
    int top_left = 0;
    int top_right = map_size;
    int bottom_right = top_right * top_right - 1;
    int bottom_left = bottom_right - top_right-- + 1; // !! vedlejsi efekt

    /* Alokace trojuhelniku a vytvoreni stromu. */
    triangle_tree = new SbROAMTriangle[tree_size];
    triangle_tree[1] = SbROAMTriangle(top_left, bottom_right, top_right, 1);
    triangle_tree[2] = SbROAMTriangle(bottom_right, top_left, bottom_left, 1);
    initTriangle(triangle_tree, 1);
    initTriangle(triangle_tree, 2);

    /* Dva koreny binarniho stromu trojuhelniku. */
    SbROAMTriangle * triangle_1 = &triangle_tree[1];
    SbROAMTriangle * triangle_2 = &triangle_tree[2];

    SbROAMSplitQueueTriangle * root_1 =
      new SbROAMSplitQueueTriangle(triangle_1, computePriority(triangle_1));
    SbROAMSplitQueueTriangle * root_2 =
      new SbROAMSplitQueueTriangle(triangle_2, computePriority(triangle_2));

    /* Jejich vzajemne propojeni. */
    root_1->base = root_2;
    root_2->base = root_1;

    /* Vlozeni korenu do fronty trojuhelniku. */
    split_queue->add(root_1);
    split_queue->add(root_2);

    PR_STOP_PROFILE(preprocess);
  }

  if (!is_freeze)
  {
    /* Aktualizace lambdy pro aktualni okno. */
    lambda = (view_volume->getNearDist() *
      viewport_region->getViewportSizePixels()[1]) /
      (view_volume->getHeight());

    /* Prepocitani priority v prioritni fronte na rozdeleni. */
    int size = split_queue->size();
    for (int I = 1; I <= size; ++I)
    {
      SbROAMSplitQueueTriangle * triangle = (*split_queue)[I];
      triangle->setPriority(this->computePriority(triangle->triangle));
    }
    split_queue->buildHeap(NULL, NULL);

    /* Prepocitani priority v prioritni fronte na spojeni. */
    size = merge_queue->size();
    for (int J = 1; J <= size; ++J)
    {
      SbROAMMergeQueueDiamond * diamond = (*merge_queue)[J];

      float priority = PRIORITY_MIN;
      float first_priority = diamond->first->getPriority();
      float second_priority = diamond->second->getPriority();
      priority = SbMax(priority, first_priority);
      priority = SbMax(priority, second_priority);

      if (diamond->third != NULL)
      {
        float third_priority = diamond->third->getPriority();
        float fourth_priority = diamond->fourth->getPriority();
        priority = SbMax(priority, third_priority);
        priority = SbMax(priority, fourth_priority);
      }
      diamond->setPriority(priority);
    }
    merge_queue->buildHeap(NULL, NULL);

    /* Smycka generovani triangulace pro konstantni pocet trojuhelniku. */
    while (TRUE)
    {
      if ((split_queue->size() > triangle_count) ||
        (split_queue->getMax()->getPriority() < pixel_error))
      {
        if (merge_queue->size() > 0)
        {
          if (split_queue->getMax()->getPriority() <=
            merge_queue->getMin()->getPriority())
          {
            break;
          }
          else
          {
            merge(merge_queue->getMin());
          }
        }
        else
        {
          break;
        }
      }
      else
      {
        /* Ziskani a rozdeleni trojuhelniku. */
        SbROAMSplitQueueTriangle * parent = split_queue->getMax();
        if (parent->triangle->level == level)
        {
          parent->setPriority(split_queue, PRIORITY_MIN);
        }
        else
        {
          SbROAMSplitQueueTriangle * left_child = NULL;
          SbROAMSplitQueueTriangle * right_child = NULL;
          forceSplit(parent, left_child, right_child);
        }
      }
    }
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

  /* Vykresleni trojuhelniku. */
  glBegin(GL_TRIANGLES);
  for (int I = 1; I <= split_queue->size(); ++I)
  {
    SbROAMTriangle * triangle = (*split_queue)[I]->triangle;
    int vertex_index;

    /* Vykresleni trojuhelniku. */
    GL_SEND_VERTEX(triangle->first);
    GL_SEND_VERTEX(triangle->apex);
    GL_SEND_VERTEX(triangle->second);
  }
  glEnd();

  endSolidShape(action);
}

#define SEND_VERTEX(ind) index = (ind); \
   vertex.setPoint(coords[index]); \
   vertex.setTextureCoords(texture_coords[index]); \
   vertex.setNormal(normals[index]); \
   shapeVertex(&vertex);

void SoSimpleROAMTerrain::generatePrimitives(SoAction * action)
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

void SoSimpleROAMTerrain::computeBBox(SoAction * action, SbBox3f & box,
  SbVec3f & center)
{
  /* Vypocet ohranicujiciho kvadru a jeho stredu. */
  SoState * state = action->getState();
  const SoCoordinateElement * coords = SoCoordinateElement::getInstance(state);
  int map_size = mapSize.getValue();

  /* Vypocet ohraniceni podle dvou rohu vyskove mapy. */
  SbVec3f min = coords->get3(0);
  SbVec3f max = coords->get3(map_size * map_size - 1);
  max[2] = (max[1] - min[1]) * 0.5;
  min[2] = -max[2];
  box.setBounds(min, max);

  center = box.getCenter();
}

/******************************************************************************
* SoSimpleROAMTerrain - protected
******************************************************************************/

void SoSimpleROAMTerrain::initTriangle(SbROAMTriangle * triangle_tree,
  const int index)
{
  /* Rodicovsky trojuhelnik, ktery se inicializuje */
  SbROAMTriangle & parent = triangle_tree[index];

  /* Vrcholy rodicovskeho trojuhelniku. */
  const SbVec3f & first = coords[parent.first];
  const SbVec3f & second = coords[parent.second];
  const SbVec3f & apex = coords[parent.apex];

  /* Dokud neni spodni patro stromu, inicializace potomku. */
  if (parent.level < level)
  {
    /* Naleznuti potomku. */
    int left_index = (index << 1) + 1;
    int right_index = left_index + 1;
    SbROAMTriangle & left_child = triangle_tree[left_index];
    SbROAMTriangle & right_child = triangle_tree[right_index];

    /* Nastaveni vrcholu leveho potomka */
    left_child.first = parent.apex;
    left_child.second = parent.first;
    left_child.apex = (parent.first + parent.second) >> 1;
    left_child.level = parent.level + 1;

    /* Nastaveni vrcholu praveho potomka */
    right_child.first = parent.second;
    right_child.second = parent.apex;
    right_child.apex = (parent.first + parent.second) >> 1;
    right_child.level = parent.level + 1;

    /* Inicializace potomku. */
    initTriangle(triangle_tree, left_index);
    initTriangle(triangle_tree, right_index);

    /* Vypocet "podpadku" jako maximum chyby obou potomku + rozdil vysky
    vrcholu s pravym uhlem a prumene vysky obou vrcholu prepony. */
    parent.error = SbMax(left_child.error, right_child.error) +
      SbAbs(apex[2] - (first[2] + second[2]) * 0.5f);

    /* Vypocet polomeru kuloplochy ohranicujici trojuhelnik. */
    const SbVec3f & left_apex = coords[left_child.apex];
    const SbVec3f & right_apex = coords[right_child.apex];
    float left_radius = (apex - left_apex).length() + left_child.radius;
    float right_radius = (apex - right_apex).length() + right_child.radius;
    parent.radius = SbMax(left_radius, right_radius);
  }
  else
  {
    /* Trojuhelniky na dne stromu maji nulovou metriku. */
    parent.error = PRIORITY_MIN;
    parent.radius = 0.0;
  }
}

inline SbBool SoSimpleROAMTerrain::isInViewVolume(const SbVec3f first,
  const SbVec3f second, const SbVec3f apex) const
{
  /* Priznaky polorovin, ve kterych se nachazeji jednotlive vrcholy
  trojuhelniku. */
  int first_flags = 0x00;
  int second_flags = 0x00;
  int apex_flags = 0x00;
  int mask = 0x01;

  /* Pro vsech sest orezavacich rovin otestujeme kazdy vrchol trojuhelniku
  na pritomnost v poloprostoru. */
  for (int I = 0; I < 6; ++I, mask <<= 1)
  {
    if (!planes[I].isInHalfSpace(first))
    {
      first_flags |= mask;
    }
    if (!planes[I].isInHalfSpace(second))
    {
      second_flags |= mask;
    }
    if (!planes[I].isInHalfSpace(apex))
    {
      apex_flags |= mask;
    }
  }

  /* Je-li nektery z vrcholu primo v pohledu kamery vykreslime vzdy. */
  if ((first_flags & second_flags & apex_flags) == 0x00)
  {
    return TRUE;
  }
  else
  {
    /* Jsou-li vsechny vrcholy mimo, vratime vysledek potencialni viditelnosti
    trojuhelniku. */
    return !((first_flags & second_flags) && (second_flags & apex_flags)
      && (apex_flags & first_flags));
  }
}

inline float SoSimpleROAMTerrain::computePriority(
  const SbROAMTriangle * triangle) const
{
  /* Ziskani vrcholu trojuhelniku a pozice kamery. */
  SbVec3f camera_position = view_volume->getProjectionPoint();
  const SbVec3f & first = coords[triangle->first];
  const SbVec3f & second = coords[triangle->second];
  const SbVec3f & apex = coords[triangle->apex];

  /* Vzdalenost kamery od spolecneho vrcholu obou trojuhelniku. */
  float distance = (camera_position - apex).length();

  /* Je-li kamera prilis blizko trojuhelniku, vyresleni vzdy. */
  if (distance < triangle->radius)
  {
    return PRIORITY_MAX;
  }
  else
  {
    /* Neni-li tak blizko, zjisteni jestli je trojuhelnik videt. */
    if (!this->is_frustrum_culling || this->isInViewVolume(first, second,
      apex))
    {
      return lambda * triangle->error / (distance - triangle->radius);
    }
    else
    {
      return PRIORITY_MIN; // trojuhelnik neni videt
    }
  }
}

inline void SoSimpleROAMTerrain::reconnectNeighbour(
  SbROAMSplitQueueTriangle * triangle,
  SbROAMSplitQueueTriangle * old_neighbour,
  SbROAMSplitQueueTriangle * new_neighbour)
{
  /* Vymena spravneho souseda trojuhelniku. */
  if (triangle != NULL)
  {
    if (triangle->left == old_neighbour)
    {
      triangle->left = new_neighbour;
    }
    else if (triangle->right == old_neighbour)
    {
      triangle->right = new_neighbour;
    }
    else if (triangle->base == old_neighbour)
    {
      triangle->base = new_neighbour;
    }
  }
}

void SoSimpleROAMTerrain::simpleSplit(SbROAMSplitQueueTriangle * parent,
  SbROAMSplitQueueTriangle *& left_child,
  SbROAMSplitQueueTriangle *& right_child)
{
  /* Odstraneni rodice z fronty trojuhelniku na rodeleni a diamondu
  na spojeni. */
  split_queue->remove(parent);
  delete merge_queue->remove(parent);

  /* Ziskani potomku rozdelovaneho trojuhelniku. */
  int child_index = ((parent->triangle - triangle_tree) << 1) + 1;
  SbROAMTriangle * left_triangle = &triangle_tree[child_index];
  SbROAMTriangle * right_triangle = &triangle_tree[child_index + 1];

  /* Vytvoreni a vraceni potomku.  */
  left_child = new SbROAMSplitQueueTriangle(left_triangle,
    computePriority(left_triangle));
  right_child = new SbROAMSplitQueueTriangle(right_triangle,
    computePriority(right_triangle));

  /* Vlozeni potomku do fronty. */
  split_queue->add(left_child);
  split_queue->add(right_child);

  /* Propojeni potomku. */
  left_child->left = right_child;
  right_child->right = left_child;
  left_child->base = parent->left;
  reconnectNeighbour(parent->left, parent, left_child);
  right_child->base = parent->right;
  reconnectNeighbour(parent->right, parent, right_child);
}

void SoSimpleROAMTerrain::forceSplit(SbROAMSplitQueueTriangle * parent,
  SbROAMSplitQueueTriangle *& left_child,
  SbROAMSplitQueueTriangle *& right_child)
{
  /* Rozdeleni rodice. */
  simpleSplit(parent, left_child, right_child);

  /* Promenne pro vytvoreni diamondu. */
  float first_priority = parent->getPriority();
  float second_priority = PRIORITY_MIN;
  SbROAMSplitQueueTriangle * first = left_child;
  SbROAMSplitQueueTriangle * second = right_child;
  SbROAMSplitQueueTriangle * third = NULL;
  SbROAMSplitQueueTriangle * fourth = NULL;

  /* Ma-li rodic bazoveho souseda. */
  if (parent->base != NULL)
  {
    /* Je-li tento na stejne urovni ve stromu trojuhelniku. */
    if (parent->triangle->level == parent->base->triangle->level)
    {
      /* Rozdeli se take bazovy soused. */
      SbROAMSplitQueueTriangle * base = parent->base;
      SbROAMSplitQueueTriangle * left_base = NULL;
      SbROAMSplitQueueTriangle * right_base = NULL;
      simpleSplit(base, left_base, right_base);

      /* Propojeni diamondu. */
      left_base->right = right_child;
      right_child->left = left_base;
      right_base->left = left_child;
      left_child->right = right_base;

      /* Nastaveni priorit a trojuhelniku diamondu. */
      second_priority = base->getPriority();
      third = left_base;
      fourth = right_base;

      delete base;
    }
    else
    {
      /* Rozdeli se take bazovy soused. */
      SbROAMSplitQueueTriangle * base = parent->base;
      SbROAMSplitQueueTriangle * left_base = NULL;
      SbROAMSplitQueueTriangle * right_base = NULL;
      forceSplit(base, left_base, right_base);

      /* Nalezeni a rozdeleni propojky. */
      SbROAMSplitQueueTriangle * link = left_base->base == parent ? left_base :
        right_base;
      SbROAMSplitQueueTriangle * left_link = NULL;
      SbROAMSplitQueueTriangle * right_link = NULL;
      simpleSplit(link, left_link, right_link);

      /* Propojeni diamondu. */
      left_link->right = right_child;
      right_child->left = left_link;
      right_link->left = left_child;
      left_child->right = right_link;

      /* Nastaveni priorit a trojuhelniku diamondu. */
      second_priority = link->getPriority();
      third = left_link;
      fourth = right_link;

      delete link;
    }
  }

  delete parent;

  /* Priprava reprezentace diamondu v prioritni fronte. */
  float priority = SbMax(first_priority, second_priority);
  SbROAMMergeQueueDiamond * diamond = new SbROAMMergeQueueDiamond(first, second,
    third, fourth, priority);
  first->diamond = diamond;
  second->diamond = diamond;

  /* Nejde-li od degradovany diamond. */
  if (third != NULL)
  {
    third->diamond = diamond;
    fourth->diamond = diamond;
  }

  /* Vlozeni diamondu do fronty na rozdeleni. */
  merge_queue->add(diamond);
}

void SoSimpleROAMTerrain::halfMerge(SbROAMSplitQueueTriangle * left_child,
  SbROAMSplitQueueTriangle * right_child,
  SbROAMSplitQueueTriangle *& parent)
{
  int parent_index = (left_child->triangle - triangle_tree - 1) / 2;
  SbROAMTriangle * parent_triangle = &triangle_tree[parent_index];

  /* Vlozeni otce do fronty na rozdeleni a odstraneni potomku */
  parent = new SbROAMSplitQueueTriangle(parent_triangle,
    computePriority(parent_triangle));
  split_queue->remove(left_child);
  split_queue->remove(right_child);
  split_queue->add(parent);

  /* Napojeni otce na sousedy. */
  parent->left = left_child->base;
  reconnectNeighbour(left_child->base, left_child, parent);
  parent->right = right_child->base;
  reconnectNeighbour(right_child->base, right_child, parent);

  /* Neni-li posledni uroven. */
  int level = parent_triangle->level;
  if (level > 1)
  {
    /* Sousede a uroven spojeneho trojuhelniku. */
    SbROAMSplitQueueTriangle * left_neighbour = parent->left;
    SbROAMSplitQueueTriangle * right_neighbour = parent->right;

    /* Muze-li vzniknout novy nedegradovany diamond. */
    if ((left_neighbour != NULL) && (right_neighbour != NULL))
    {
      /* Je-li rodic ze stejne urovne jako jeho nejblizsi sousede. */
      if ((left_neighbour->triangle->level == level) &&
        (right_neighbour->triangle->level == level) &&
        (left_neighbour->left->triangle->level == level))
      {
        /* Ctyri trojuhelniky noveho diamodnu */
        SbROAMSplitQueueTriangle * first = NULL;
        SbROAMSplitQueueTriangle * second = NULL;
        SbROAMSplitQueueTriangle * third = NULL;
        SbROAMSplitQueueTriangle * fourth = NULL;

        /* Je-li nove vytvoreny trojuhelnik pravym synem. */
        if ((parent_index - 1) % 2)
        {
          first = right_neighbour;
          second = parent;
          third = left_neighbour;
          fourth = left_neighbour->left;
        }
        else // je levym synem
        {
          first = parent;
          second = left_neighbour;
          third = right_neighbour->right;
          fourth = right_neighbour;
        }

        /* Nalezeni otcovskych trojuhelniku. */
        int left_index = (left_neighbour->triangle - triangle_tree - 1) / 2;
        int right_index = (right_neighbour->triangle - triangle_tree - 1) / 2;
        SbROAMTriangle * first_triangle = &triangle_tree[left_index];
        SbROAMTriangle * second_triangle = &triangle_tree[right_index];

        /* Vznik noveho diamondu. */
        float first_priority = computePriority(first_triangle);
        float second_priority = computePriority(second_triangle);
        float priority = SbMax(first_priority, second_priority);
        SbROAMMergeQueueDiamond * new_diamond =
          new SbROAMMergeQueueDiamond(first, second, third, fourth, priority);
        first->diamond = new_diamond;
        second->diamond = new_diamond;
        third->diamond = new_diamond;
        fourth->diamond = new_diamond;

        /* Vlozeni diamondu do fronty na rozdeleni. */
        merge_queue->add(new_diamond);
      }
    }
    else
    {
      /* Muze-li vzniknout degradovany diamond. */
      if ((left_neighbour != NULL) || (right_neighbour != NULL))
      {
        /* Muze-li vzniknout s levym sousedem. */
        if (left_neighbour != NULL)
        {
          if (left_neighbour->triangle->level == level)
          {
            /* Nalezeni otcovskeho trojuhelniku. */
            int first_index = (parent_index - 1) / 2;
            SbROAMTriangle * first_triangle = &triangle_tree[first_index];

            /* Vznik noveho diamondu. */
            SbROAMMergeQueueDiamond * new_diamond =
              new SbROAMMergeQueueDiamond(parent, left_neighbour, NULL, NULL,
              computePriority(first_triangle));
            parent->diamond = new_diamond;
            left_neighbour->diamond = new_diamond;

            /* Vlozeni diamondu do fronty na rozdeleni. */
            merge_queue->add(new_diamond);
          }
        }
        else // muze vzniknout s pravym sousedem
        {
          if (right_neighbour->triangle->level == level)
          {
            /* Nalezeni otcovskeho trojuhelniku. */
            int first_index = (parent_index - 1) / 2;
            SbROAMTriangle * first_triangle = &triangle_tree[first_index];

            /* Vznik noveho diamondu. */
            SbROAMMergeQueueDiamond * new_diamond =
              new SbROAMMergeQueueDiamond(right_neighbour, parent, NULL, NULL,
              computePriority(first_triangle));
            right_neighbour->diamond = new_diamond;
            parent->diamond = new_diamond;

            /* Vlozeni diamondu do fronty na rozdeleni. */
            merge_queue->add(new_diamond);
          }
        }
      }
    }
  }
}

void SoSimpleROAMTerrain::merge(SbROAMMergeQueueDiamond * diamond)
{
  /* Otcove, kteri vzniknou spojenim diamondu. */
  SbROAMSplitQueueTriangle * first_parent = NULL;
  SbROAMSplitQueueTriangle * second_parent = NULL;

  /* Spojeni prvni poloviny diamondu */
  halfMerge(diamond->first, diamond->second, first_parent);

  /* Uvolneni potomku z pameti. */
  delete diamond->first;
  delete diamond->second;

  /* Neni-li diamond degradovany. */
  if (diamond->third != NULL)
  {
    /* Spojeni druhe poloviny diamondu a propojeni polovin. */
    halfMerge(diamond->third, diamond->fourth, second_parent);
    second_parent->base = first_parent;
    first_parent->base = second_parent;

    /* Uvolneni potomku z pameti. */
    delete diamond->third;
    delete diamond->fourth;
  }

  /* Odstraneni stareho diamondu. */
  merge_queue->remove(diamond);
  delete diamond;
}

void SoSimpleROAMTerrain::mapSizeChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleROAMTerrain * instance =
    reinterpret_cast<SoSimpleROAMTerrain *>(_instance);
  instance->map_size = instance->mapSize.getValue();
}

void SoSimpleROAMTerrain::pixelErrorChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleROAMTerrain * instance =
    reinterpret_cast<SoSimpleROAMTerrain *>(_instance);
  instance->pixel_error = instance->pixelError.getValue();
}

void SoSimpleROAMTerrain::triangleCountChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleROAMTerrain * instance =
    reinterpret_cast<SoSimpleROAMTerrain *>(_instance);
  instance->triangle_count = instance->triangleCount.getValue();
}

void SoSimpleROAMTerrain::frustrumCullingChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleROAMTerrain * instance =
    reinterpret_cast<SoSimpleROAMTerrain *>(_instance);
  instance->is_frustrum_culling = instance->frustrumCulling.getValue();
}

void SoSimpleROAMTerrain::freezeChangedCB(void * _instance,
  SoSensor * sensor)
{
  /* Aktualizace vnitrni hodnoty pole. */
  SoSimpleROAMTerrain * instance =
    reinterpret_cast<SoSimpleROAMTerrain *>(_instance);
  instance->is_freeze = instance->freeze.getValue();
}

/******************************************************************************
* SoSimpleROAMTerrain - private
******************************************************************************/

SoSimpleROAMTerrain::~SoSimpleROAMTerrain()
{
  /* Uvolneni internich struktur. */
  delete[] triangle_tree;
  for (int I = 1; I <= split_queue->size(); ++I)
  {
     delete (*split_queue)[I];

  }
  delete split_queue;
  for (int J = 1; J <= merge_queue->size(); ++J)
  {
     delete (*merge_queue)[J];
  }
  delete merge_queue;
}
