#ifndef SO_SIMPLE_ROAM_TERRAIN_H
#define SO_SIMPLE_ROAM_TERRAIN_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Ter  rendered by the ROAM algorithm.
/// \file SoSimpleROAMTerrain.h
/// \author Radek Barton - xbarto33
/// \date 25.08.2005
///
/// The scene graph node represents the ter rendered by the ROAM algorithm. For
/// the node is used as a node with coordinates of type \p SoCoordinate3
/// content map and set its dimensions.
/// The size of the map must be equal to the page size of 2^n + 1, where n is a positive integer.
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

// Coin includes
#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoTextureEnabledElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/bundles/SoMaterialBundle.h>

// OpenGL includes
#if defined(__WIN32__) || defined(_WIN32)
  #include <windows.h>
#endif
#include <GL/gl.h>

// standard includes
#include <iostream>

// local includes
#include <roam/SbROAMPrimitives.h>
#include <roam/SbROAMSplitQueue.h>
#include <roam/SbROAMMergeQueue.h>
#include <profiler/PrProfiler.h>
#include <debug.h>

/**
 * Ter  rendered by the ROAM algorithm.
 * The sc y graph node represents the meter rendered by the ROAM algorithm.
 * To use the node, add a node with coordinates of the type
 * \p SoCoordinate3 to the current content map and set its dimensions.
 * The size of the map must be a vertices of side size 2^n + 1, where n is a positive integer.
 */
class SoSimpleROAMTerrain : public SoShape
{
  SO_NODE_HEADER(SoSimpleROAMTerrain);
  public:
    /** Run-time initialization.
    You must call this method before creating any instance of it
    ::SoSimpleROAMTerrain.*/
    static void initClass();

/** Constructor.
    Creating an instance of ::SoSimpleROAMTerrain, initializes both priority queues. */
    SoSimpleROAMTerrain();
    /* Field. */
    /// The size (height and width) of the current map.
    SoSFInt32 mapSize;
    /// Display error in pixels.
    SoSFInt32 pixelError;
    /// Maximum number of triangles in triangulation.
    SoSFInt32 triangleCount;
    /// P nak oezer   out of field of vision.
    SoSFBool frustumCulling;
    /// The image is "frozen" on the renderer.
    SoSFBool freeze;
  protected:
/** Render the current triangulation.
    Based on the input data in the map, the position and distance of the camera and distance
    properties in the first step builds a trihedral binotree, then
    and so on in long chains - it expands and connects triangles and diamonds in
    priority fronts so as to achieve minimal triangulation. Next, draw this triangulation
    \param action An object containing information about the sc y graph. */
    virtual void GLRender(SoGLRenderAction * action);

/** Generated trigonal metalmaps.
    Create a list of triangles on the highest level from the input map
    detail for collision detection purposes and similar
    \param action An object containing information about the sc y graph. */
    virtual void generatePrimitives(SoAction * action);
    /** Input boundary layer.
    It measures the dimensions and the center of the cross that bounds the whole area.
    \param action An object containing information about the sc y graph.
    \param box Computed cross border.
    \param center Calculated center of the bounding box. */
    virtual void computeBBox(SoAction * action, SbBox3f & box,
      SbVec3f & center);
    /** Inicializace bin�n�o stromu trojheln�.
    Rekurzivn�inicializuje vechny trojheln�y bin�n�o stromu trojheln�,
    vypo�t�jejich norm�y (pro plon�nebo vpo�t plynul�o st�ov��.
    \param index Index trojheln�u, kter inicializovat.
    \param triangle_tree Ukazatel na koen bin�n�o stromu trojheln�. */
    void initTriangle(SbROAMTriangle * triangle_tree, const int index);
    /** Test na viditelnost trojheln�u.
    Otestuje trojheln� definovan body \p first, \p second a \p apex proti
    pohledov�u t�esu. Nenach�i-li se �n z bodu v pohledu kamery, vr��    \p FALSE. Pou��zjednodueny, ale do tech rozm� roz�eny
    Cohen-Shuterland algoritmus, a proto me vr�it \p TRUE i kdy je
    trojheln� mimo pohled kamery.
    \param first Prvn�vrchol testovan�o trojheln�u.
    \param second Druh vrchol testovan�o trojheln�u.
    \param apex Tet�vrchol testovan�o trojheln�u.
    \return Vr��\p false nen�li trojheln� ur�t�v pohledu kamery, je-li
    v pohledu nebo nejde-li rozhodnout vrat�\p TRUE. */
    inline SbBool isInViewVolume(const SbVec3f first, const SbVec3f
      second, const SbVec3f apex) const;
    /** Vpo�t priority trojheln�u v triangulaci.
    Na z�lad�pozice a orientace kamery vypo�e prioritu trojheln�u
    \p triangle v triangulaci.
    \param triangle Trojheln�, jeho priorita se m�spo�tat.
    \return Vr��vypo�enou prioritu trojheln�u. */
    inline float computePriority(const SbROAMTriangle * triangle) const;
    /** Vm�a sousedn�o trojheln�u.
    Zjist� kterm sousedem je trojheln� \p old_neighbour, a zam��ho za
    nov trojheln� \p new_neighbour.
    \param triangle Trojheln�, u kter�o vm�u prov�t.
    \param old_neighbour Pvodn�soused trojheln�u.
    \param new_neighbour Nov soused trojheln�u. */
    inline void reconnectNeighbour(SbROAMSplitQueueTriangle * triangle,
      SbROAMSplitQueueTriangle * old_neighbour,
      SbROAMSplitQueueTriangle * new_neighbour);
    /** Z�ladn�rozd�en�trojheln�u.
    Vyjme trojheln� \p parent z prioritn�fronty trojheln�
    na rozd�en� Nalezne jeho potomky v bin�n� stromu trojheln�,
    naalokuje jejich reprezentace v prioritn�front�a vlo�je do n�
    Propoj�oba potomky mezi sebou a s okoln�i trojheln�y krom�b�ov�o
    souseda otce. Aby bylo toto mono prov�t zp�n� vr��oba potomky
    \p left_child a \p right_child. Reprezentaci otce neuvol�je,
    protoe je tak�teba.
    \param parent Otec, kter se m�rozd�it.
    \param left_child Lev potomek rozd�en�o otce.
    \param right_child Prav potomek rozd�en�o otce. */
    void simpleSplit(SbROAMSplitQueueTriangle * parent,
      SbROAMSplitQueueTriangle *& left_child,
      SbROAMSplitQueueTriangle *& right_child);
    /** Obecn�rozd�en�trojheln�u.
    Kompletn�rozd��trojheln� \p parent z prioritn�fronty trojheln�
    na rozd�en� Pro vlastn�rozd�en�vol�::simpleSplit. Na rozd�en�    trojheln�y napoj�vledek rozd�en�b�ov�o souseda, kter me bt
    d�e d�en rekurzivn� Rozd�en�o otce dealokuje.
    \param parent Otec, kter se m�rozd�it.
    \param left_child Lev potomek rozd�en�o otce.
    \param right_child Prav potomek rozd�en�o otce. */
    void forceSplit(SbROAMSplitQueueTriangle * parent,
      SbROAMSplitQueueTriangle *& left_child,
      SbROAMSplitQueueTriangle *& right_child);
    /** Spojen�poloviny diamant.
    Spoj�dva trojheln�y \p left_child a \p right_child diamantu do jednoho
    rodi�vsk�o trojheln�u \p parent. Me vytvoit nov�diamanty.
    \param left_child Lev trojheln� z diamatu.
    \param right_child Prav trojheln� z diamantu.
    \param parent Vsledn rodi�vsk trojheln�. */
    void halfMerge(SbROAMSplitQueueTriangle * left_child,
      SbROAMSplitQueueTriangle * right_child,
      SbROAMSplitQueueTriangle *& parent);
    /** Spojen�diamantu.
    Spoj�diamant \p diamond z prioritn�fronty diamant na spojen� a
    vytvo�tak dva trojheln�y z pvodn�h �y v prioritn�front�    trojheln� na rozd�en�
    \param diamond Diamant z prioritn�fronty na rozd�en� */
    void merge(SbROAMMergeQueueDiamond * diamond);
    /* Callbacky. */
    /** Callback zm�y pole \p ::mapSize.
    Pi zm��hodnoty pole \p ::mapSize nastav�jeho intern�reprezentaci novou
    hodnotu.
    \param instance Ukazatel na instanci t�y \p SoSimpleROAMTerrain.
    \param sensor Senzor, kter callback vyvolal. */
    static void mapSizeChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p ::pixelError.
    Pi zm��hodnoty pole \p ::pixelError nastav�jeho intern�reprezentaci
    novou hodnotu.
    \param instance Ukazatel na instanci t�y \p SoSimpleROAMTerrain.
    \param sensor Senzor, kter callback vyvolal. */
    static void pixelErrorChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p ::triangleCount.
    Pi zm��hodnoty pole \p ::triangleCount nastav�jeho intern�reprezentaci
    novou hodnotu.
    \param instance Ukazatel na instanci t�y \p SoSimpleROAMTerrain.
    \param sensor Senzor, kter callback vyvolal. */
    static void triangleCountChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p ::frustumCulling.
    Pi zm��hodnoty pole \p ::frustumCulling nastav�jeho intern�    reprezentaci novou hodnotu.
    \param instance Ukazatel na instanci t�y \p SoSimpleROAMTerrain.
    \param sensor Senzor, kter callback vyvolal. */
    static void frustumCullingChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p ::freeze.
    Pi zm��hodnoty pole \p ::freeze nastav�jeho intern�reprezentaci novou
    hodnotu.
    \param instance Ukazatel na instanci t�y \p SoSimpleROAMTerrain.
    \param sensor Senzor, kter callback vyvolal. */
    static void freezeChangedCB(void * instance, SoSensor * sensor);
    /* Zkratky elementu. */
    /// Body vykov�mapy.
    const SbVec3f * coords;
    /// Texturov�souadnice pro body vkov�mapy.
    const SbVec2f * texture_coords;
    /// Norm�y.
    const SbVec3f * normals;
    /// Pohledov�t�eso.
    const SbViewVolume * view_volume;
    /// Vykreslovac�okno.
    const SbViewportRegion * viewport_region;
    /// Roviny pohledoveho telesa
    SbPlane planes[6];
    /* Datove polozky. */
    /// Bin�n�strom trojheln�.
    SbROAMTriangle * triangle_tree;
    /// Velikost bin�n�o stromu trojheln�.
    int tree_size;
    /// Po�t rovn�bin�n�o stromu trojheln�.
    int level;
    /// Konstanta udavaj��po�t pixel na jeden radi� zorn�o pole.
    float lambda;
    /// Fronta trojheln� na rozd�en�
    SbROAMSplitQueue * split_queue;
    /// Fronta diamant pro spojen�
    SbROAMMergeQueue * merge_queue;
    /// P�nak pouit�textury.
    SbBool is_texture;
    /// P�nak pouit�norm�.
    SbBool is_normals;
    /* Interni pole. */
    /// Velikost (vka i �ka) vkov�mapy ter�u.
    int map_size;
    /// Chyba zobrazen�v pixelech.
    int pixel_error;
    /// Maxim�n�po�t trojheln� v triangulaci.
    int triangle_count;
    /// P�nak oez���ter�u mimo zorn�pole.
    SbBool is_frustum_culling;
    /// P�nak "zmrazen� vykreslov��ter�u.
    SbBool is_freeze;
    /* Sensory. */
    /// Senzor pole \p ::mapSize.
    SoFieldSensor * map_size_sensor;
    /// Senzor pole \p ::pixelError.
    SoFieldSensor * pixel_error_sensor;
    /// Senzor pole \p ::triangleCount.
    SoFieldSensor * triangle_count_sensor;
    /// Senzor pole \p ::frustumCulling.
    SoFieldSensor * frustum_culling_sensor;
    /// Senzor pole \p ::freeze.
    SoFieldSensor * freeze_sensor;
    /* Konstanty. */
    /// Vchoz�hodnota pro chybu triangulace v pixelech.
    static const int DEFAULT_PIXEL_ERROR;
    /// Vchoz�hodnota pro maxim�n�po�t trojheln� v triangulaci.
    static const int DEFAULT_TRIANGLE_COUNT;
  private:
    /* Metody */
    /** Destruktor.
    Zprivatizov�, aby se zabr�ilo ruen�uzlu, nebo Inventor uvol�je pam�    ve vlastn�reii. */
    virtual ~SoSimpleROAMTerrain();
};

#endif
