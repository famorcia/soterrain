#ifndef SB_GEO_MIPMAP_PRIMITIVES_H
#define SB_GEO_MIPMAP_PRIMITIVES_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Align the data elements of the Geo Mip-mapping algorithm.
/// \file SbGeoMipmapPrimitives.h
/// \author Radek Barton - xbarto33
/// \date 30.01.2006
///
/// Zde jsou definov�y z�ladn�datov�typy algoritmu Geo Mip-Mapping.
/// Kvadrantov strom dladic ve t��::SbGeoMipmapTileTree rekurzivn�/// rozd�uje vkovou mapu ter�u na dladice t�y SbGeoMipmapTile. V jeho
/// horn�h patrech jsou dladice pouze virtu�n� tj. ty, kter�neobsahuj�/// vlastn�geometrii, ale jenom sv�potomky. Dladice na nejni�rovni
/// stromu obsahuj�pole jednotlivch rovn�detail dladice ve t��/// ::SbGeoMipmapTileLevel, jen u geometrii zahrnuj�
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

// Coin includy
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>

// lokalni includy
#include <debug.h>
#include <utils.h>

/** �ove�detail dladice algoritmu Geo Mip-Mapping.
Tato t�a obsahuje indexy vrchol geometrie rovn�detail jedn�dladice.
D�e je zde statick��st chybov�metriky platn�pro tuto rove� kter�se
pedpo�ta v dob�na�t��vstupn�vkov�mapy. */
struct SbGeoMipmapTileLevel
{
  public:
    /* Metody */
    /** Konstruktor.
    Inicializuje instanci t�y ::SbGeoMipmapTileLevel, aby obsahovala
    pole vrchol rovn�detail dladice \p vertices s celkovou statickou
    �st�chybov�metriky \p error.
    \param error Chyba statick��sti chybov�metriky.
    \param vertices Pole vrchol geometrie rovn�detail dladice. */
    SbGeoMipmapTileLevel(float error = 0.0f, int * vertices = NULL);
    /** Destruktor.
    Uvoln�pole vrchol rovn�detail dladice a deinicalizuje instanci
    t�y SbGeoMipmapTileLevel. */
    ~SbGeoMipmapTileLevel();
    /* Datove polozky */
    /// Statick��st chybov�metriky rovn�detail dladice.
    float error;
    /// Vrcholy geometrie rovn�detail dladice.
    int * vertices;
};

/** Dladice algoritmu Geo Mip-Mapping.
Z t�hto dladic se vytvo�kvadrantov strom dladic slou��pro vpo�t
oez���pohledovm t�esem. Pokud je ukazatel na rovn�datail dladice
roven \p NULL, jedn�se o virtu�n�dladici v horn�h rovn�h stromu. */
struct SbGeoMipmapTile
{
  public:
    /* Metody */
    /** Konstruktor.
    Vytvo�instanci t�y ::SbGeoMipmapTile a inicializuje jeho datov�    prvky na hodnoty \p levels, \p left, \p right, \p top, \p bottom, \p level,
    \p bounds a \p center.
    \param levels Ukazatel na pole rovn�detail dladice.
    \param left Ukazatel na lev�o souseda dladice.
    \param right Ukazatel na prav�o souseda dladice.
    \param top Ukazatel na horn�o souseda dladice.
    \param bottom Ukazatel na doln�o souseda dladice.
    \param level �slo rovn�detail, kter�se m�vykreslit.
    \param bounds Ohrani�n�vrchol dladice.
    \param center Sted ohrani�n�vrchol dladice. */
    SbGeoMipmapTile(SbGeoMipmapTileLevel * levels = NULL,
      SbGeoMipmapTile * left = NULL, SbGeoMipmapTile * right = NULL,
      SbGeoMipmapTile * top = NULL, SbGeoMipmapTile * bottom = NULL,
      int level = LEVEL_NONE, SbBox3f bounds = SbBox3f(),
      SbVec3f center = SbVec3f());
    /** Destruktor.
    Zru�instanci t�y ::SbGeoMipmapTile a uvoln�pole rovni detail
    dladice. */
    ~SbGeoMipmapTile();
    /* Datove polozky */
    /// Ukazatel na pole rovn�detail dladice.
    SbGeoMipmapTileLevel * levels;
    /// Ukazatel na lev�o souseda dladice.
    SbGeoMipmapTile * left;
    /// Ukazatel na prav�o souseda dladice.
    SbGeoMipmapTile * right;
    /// Ukazatel na horn�o souseda dladice.
    SbGeoMipmapTile * top;
    /// Ukazatel na doln�o souseda dladice.
    SbGeoMipmapTile * bottom;
    /// �slo rovn�detail, kter�se m�vykreslit.
    int level;
    /// Ohrani�n�vrchol dladice.
    SbBox3f bounds;
    /// Sted ohrani�n�vrchol dladice.
    SbVec3f center;
    /* Konstanty. */
    /// �slo rovn�detail dladice, kter�se nem�vykreslovat vbec.
    static const int LEVEL_NONE;
};

/** Strom dladic algoritmu Geo Mip-Mapping.
Kvadrantov strom dladic se vytvo�v prb�u na�t��vkov�mapy a napln�se dladicemi t�y ::SbGeoMipmapTile obsahuj��i jednotliv�rovn�detail
dladice t�y ::SbGeoMipmapTileLevel. */
struct SbGeoMipmapTileTree
{
  public:
    /* Metody. */
    /** Konstruktor.
    Vytvo�kvadrantov strom dladic algoritmu Geo Mip-Mapping t�y
    ::SbGeoMipmapTileTree obsahuj��o dladice o velikosti strany \p tile_size
    uspo�anch do �verce o stran�\p tile_count dladic.
    \param tile_count Po�t dladic na stranu vkov�mapy.
    \param tile_size Velikost strany jedn�dladice. */
    SbGeoMipmapTileTree(int tile_count, int tile_size);
    /** Destruktor.
    Zru�instanci t�y ::SbGeoMipmapTileTree a uvoln�pole velikost�rovn�    detail dladic i vechny dladice ve strom� */
    ~SbGeoMipmapTileTree();
    /* Datove polozky. */
    /// Po�t dladic na stranu vstupn�vkov�mapy.
    int tile_count;
    /// Velikost strany jedn�dladice.
    int tile_size;
    /// Vka kvadrantov�o stromu dladic.
    int tree_size;
    /// Index v poli dladic, kde za�n�nejni�patro stromu.
    int bottom_start;
    /// Po�t rovn�detail kad�dladice v nejni� patru stromu.
    int level_count;
    /// Pole velikost�stran pol�vrchol rovn�kad�dladice.
    int * level_sizes;
    /// Pole jednotlivch dladic kvadrantov�o stromu dladic.
    SbGeoMipmapTile * tiles;
  private:
    /** Kop�ovac�konstruktor.
    Zprivatizov�, aby se zabr�ilo vytv�en�kopi�kvadrantov�o stromu dladic.
    \param old_tree Pvodn�kvadrantov strom dladic. */
    SbGeoMipmapTileTree(const SbGeoMipmapTileTree & old_tree);
};

#endif
