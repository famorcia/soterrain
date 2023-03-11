#ifndef SO_SIMPLE_GEO_MIPMAP_TERRAIN_H
#define SO_SIMPLE_GEO_MIPMAP_TERRAIN_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Ter  rendered by the Geo Mip-Mapping algorithm.
/// \file SoSimpleGeoMipmapTerrain.h
/// \author Radek Barton - xbarto33
/// \date 29.01.2006
///
/// The sc y graph node represents the ter rendered by the Geo algorithm
/// Mip-Mapping. To use the te node, the node should be a node with coordinates of the type
/// SoCoordinate3 contains the terrain map and set its dimensions a
/// sizes of tiles generated from this texture map The size of the texture map must
/// bt vertices of side size 2^n + 1, where n is a positive integer.
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

// OpenInventor includy
#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoShape.h>
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

// OpenGL includy
#if defined(__WIN32__) || defined(_WIN32)
  #include <windows.h>
#endif
#include <GL/gl.h>

// ostatni includy
#include <assert.h>

// lokalni includy
#include <geomipmapping/SbGeoMipmapPrimitives.h>
#include <profiler/PrProfiler.h>
#include <debug.h>

/** Ter� vykreslovan algoritmem Geo Mip-Mapping.
Uzel grafu sc�y reprezentuj��ter� vykreslovan algoritmem Geo
Mip-Mapping. Pro pouit�je teba uzlu pedadit uzel s koordin�y typu
\p SoCordinate3 obsahuj��vkovou mapu ter�u a nastavit jej�rozm�y a
rozm�y dladic generovanch z t�o vkov�mapy. Velikost vkov�mapy mus�bt �vercov�o velikosti strany 2^n + 1, kde n je cel�kladn��slo. */
class SoSimpleGeoMipmapTerrain : public SoShape
{
  SO_NODE_HEADER(SoSimpleGeoMipmapTerrain);
  public:
    /* Metody */
    /** Run-time inicializace t�y.
    Tuto metodu je teba zavolat ped vytvoen� jak�oliv instance t�y
    ::SoSimpleGeoMipmapTerrain.*/
    static void initClass();
    /** Konstruktor.
    Vytvo�instanci t�y ::SoSimpleGeoMipmapTerrain. */
    SoSimpleGeoMipmapTerrain();
    /* Pole. */
    /// Velikost strany vstupn�vkov�mapy.
    SoSFInt32 mapSize;
    /// Velikost strany dladic.
    SoSFInt32 tileSize;
    /// Chyba zobrazen�v pixelech.
    SoSFInt32 pixelError;
    /// P�nak oez���pohledovm t�esem.
    SoSFBool frustumCulling;
    /// P�nak "zmrazen� vykreslov��ter�u.
    SoSFBool freeze;
  protected:
    /* Metody */
    /** Vykreslen�ter�u.
    Pi prvn� sput��vybuduje ze vstupn�vkov�mapy kvadrantov strom
    dladic. N�ledn�a i pi dal�h sput��h zaktualizuje vybranou rove�    detail dladic ve stromu podle pozice a nato�n�kamery a vybran�rovn�    vykresl�
    \param action Objekt nesouc�informace o grafu sc�y. */
    virtual void GLRender(SoGLRenderAction * action);
    /** Vytvoen�vech trojheln� vkov�mapy.
    Ze vstupn�vkov�mapy vytvo�seznam trojheln� na nejvy�rovni
    detail pro �ly detekce koliz�a podobn�
    \param action Objekt nesouc�informace o grafu sc�y. */
    virtual void generatePrimitives(SoAction * action);
    /** Vpo�t ohrani�n�ter�u.
    Vypo�e rozm�y a sted kv�ru, kter ohrani�je cel ter�.
    \param action Objekt nesouc�informace o grafu sc�y.
    \param box Vypo�en kv�r ohrani�j��ter�.
    \param center Vypo�en sted kv�ru ohrani�j��o ter�. */
    virtual void computeBBox(SoAction * action, SbBox3f & box,
      SbVec3f & center);
    /** Inicializace kvadrantov�o stromu dladic.
    Rekurzivn�inicializuje strom dladic podle zadan�vkov�mapy a po�t�    statickou �st chybov�metriky. Parametr \p index slou�jako index dladice,
    kter�tvo�koen inicializovan�o podstromu stromu dladic. Parametr
    \p coord_box slou�k ped���rozsahu index bod vstupn�vkov�mapy.
    \param index Index koene inicializovan�o podstromu stromu dladic.
    \param coord_box Rozsah index bod vstupn�vkov�mapy.
    \return */
    SbBox3f initTree(const int index, SbBox2s coord_box);
    /** Inicializace jedn�dladice.
    Inicializuje jednu dladici \p tile kvadrantov�o stromu dladic v�tn�    jej�h rovn�detail body vstupn�vkov�mapy v rozsahu index
    \p coord_box.
    \param tile Dladice, kter�se m�inicializovat.
    \param coord_box Rozsah index vstupn�vkov�mapy na zem�dladice. */
    inline void initTile(SbGeoMipmapTile & tile, SbBox2s coord_box);
    /** Inicializace rovn�detail dladice.
    Inicializuje rove�detail \p level o velikosti strany \p level_size
    podle ji zinicializovan�vy�rovn�detail dladice \p parent.
    \param level Inicializovan�rove�detail dladice.
    \param parent Ji inicializovan�vy�rove�detail dladice.
    \param level_size Velikost strany rovn�detail dladice. */
    inline void initLevel(SbGeoMipmapTileLevel & level,
      SbGeoMipmapTileLevel & parent, const int level_size);
    /** Pepo�t��kvadrantov�o stromu dladic.
    Podle vpo�u dynamick��sti chybov�metriky a podle pozice pohledov�o
    t�esa vybere u kad�dladice stromu p�lunou rove�detail. Toto
    prov��rekurzivn�od indexu podstromu \p index. Parametr \p render_parent
    slou�k propagaci informace zda se se vykresluje rodi�vsk�dladice koene
    podstromu.
    \param index Index koene podstromu stromu dladic.
    \param render_parent P�nak toho, e se rodi�koene podstromu vykresluje. */
    void recomputeTree(const int index, const SbBool render_parent);
    /** Vykreslen�kvadrantov�o stromu dladic.
    Rekurzivn�projde kvadrantovm stromem dladic podle indexu koene \p index
    a vykresl�jeho dladice s vybranou rovn�detail.
    \param action Objekt nesouc�informace o grafu sc�y.
    \param index Index koene podstromu stromu dladic. */
    void renderTree(SoAction * action, const int index);
    /** Vb� rovn�detail dladice.
    Podle chybov�metriky vybere spr�nou rove�detail dladice, kter�se m�    vykreslit.
    \param tile Dladice, u n�vb� prob��
    \return Index vybran�rovn�detail. */
    inline int pickLevel(const SbGeoMipmapTile & tile) const;
    /* Callbacky. */
    /** Callback zm�y pole \p mapSize.
    Pi zm��hodnoty pole \p mapSize nastav�jeho intern�reprezentaci novou
    hodnotu.
    \param instance Ukazatel na instanci t�y SoSimpleGeoMipmapTerrain.
    \param sensor Senzor, kter callback vyvolal */
    static void mapSizeChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p tileSize.
    Pi zm��hodnoty pole \p tileSize nastav�jeho intern�reprezentaci novou
    hodnotu.
    \param instance Ukazatel na instanci t�y SoSimpleGeoMipmapTerrain.
    \param sensor Senzor, kter callback vyvolal */
    static void tileSizeChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p pixelError.
    Pi zm��hodnoty pole \p pixelError nastav�jeho intern�reprezentaci
    novou hodnotu.
    \param instance Ukazatel na instanci t�y SoSimpleGeoMipmapTerrain.
    \param sensor Senzor, kter callback vyvolal */
    static void pixelErrorChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p frustumCulling.
    Pi zm��hodnoty pole \p frustumCulling nastav�jeho intern�reprezentaci
    novou hodnotu.
    \param instance Ukazatel na instanci t�y SoSimpleGeoMipmapTerrain.
    \param sensor Senzor, kter callback vyvolal */
    static void frustumCullingChangedCB(void * instance, SoSensor * sensor);
    /** Callback zm�y pole \p freeze.
    Pi zm��hodnoty pole \p freeze nastav�jeho intern�reprezentaci novou
    hodnotu.
    \param instance Ukazatel na instanci t�y SoSimpleGeoMipmapTerrain.
    \param sensor Senzor, kter callback vyvolal */
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
    /* Datove polozky. */
    /// Kvadrantov strom dladic.
    SbGeoMipmapTileTree * tile_tree;
    /// Konstanta pro vpo�t dynamick��sti chybov�metriky.
    float distance_const;
    /// P�nak pouit�textury.
    SbBool is_texture;
    /// P�nak pouit�morm�.
    SbBool is_normals;
    /* Interni pole. */
    /// Velikost strany vstupn�vkov�mapy.
    int map_size;
    /// Velikost strany dladic.
    int tile_size;
    /// Chyba zobrazen�v pixelech.
    int pixel_error;
    /// P�nak oez���pohledovm t�esem.
    SbBool is_frustum_culling;
    /// P�nak "zmrazen� vykreslov��ter�u.
    SbBool is_freeze;
    /* Sensory. */
    /// Senzor pole \p mapSize.
    SoFieldSensor * map_size_sensor;
    /// Senzor pole \p tileSize.
    SoFieldSensor * tile_size_sensor;
    /// Senzor pole \p pixelError.
    SoFieldSensor * pixel_error_sensor;
    /// Senzor pole \p frustumCulling.
    SoFieldSensor * frustum_culling_sensor;
    /// Senzor pole \p freeze.
    SoFieldSensor * freeze_sensor;
    /* Konstanty. */
    /// Vchoz�hodnota chyby zobrazen�v pixelech.
    static const int DEFAULT_PIXEL_ERROR;
  private:
    /* Metody */
    /** Destruktor.
    Zprivatizov�, aby se zabr�ilo ruen�uzlu, nebo Inventor uvol�je pam�    ve vlastn�reii. */
    virtual ~SoSimpleGeoMipmapTerrain();
};

#endif
