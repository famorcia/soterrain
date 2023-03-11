#ifndef SB_ROAM_PRIMITIVES_H
#define SB_ROAM_PRIMITIVES_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Z�ladn�datov�prvky algoritmu ROAM.
/// \file SbROAMPrimitives.h
/// \author Radek Barto�- xbarto33
/// \date 16.09.2005
///
/// Algoritmus ROAM ve sv�statick��sti vyu��jako z�ladn�datovou
/// strukturu bin�n�strom trojheln�. Pro tyto trojheln�y je zde
/// definov�a t�a ::SbROAMTriangle. V dynamick��sti jsou pak poteba
/// dv�prioritn�fronty. Prvn�z nich je prioritn�fronta trojheln� na
/// rozd�en� Obsahuje trojheln�y t�y ::SbROAMSplitQueueTriangle.
/// Druhou pak prioritn�fronta trojheln� na spojen� Ta obsahuje diamondy
/// (�veice trojheln�) t�y ::SbROAMMergeQueueDiamond.
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
#include <Inventor/SbVec3f.h>
#include <Inventor/SbHeap.h>

// lokalni includy
#include <debug.h>

/** Trojheln� bin�n�o stromu trojheln� algoritmu ROAM.
Z�ladn�datovou strukturou algoritmu ROAM je bin�n�strom trojheln�, kter
obsahuje pr��tyto struktury reprezentuj��jeden trojheln�
v triangulaci. */
struct SbROAMTriangle
{
  public:
    /* Metody. */
    /** Konstruktor.
    Vytvo�instanci t�y ::SbROAMTriangle a inicializuje jeho atributy.
    Parametry \p first, \p second a \p apex jsou indexy vrchol trojheln�u
    v seznamu koordin� a parametr \p level je �slo rovn�trojheln�u
    v bin�n� stromu trojheln�.
    \param first Prvn�(lev) vrchol pepony trojheln�u.
    \param second Druh (prav) vrchol pepony trojheln�u.
    \param apex Prav (nad peponou) vrchol trojheln�u.
    \param level �ove� na kter�se trojheln� nach��v bin�n� stromu
       trojheln�. */
    SbROAMTriangle(int first = -1, int second = -1, int apex = -1,
      int level = 0);
    /* Datove polozky. */
    /// Lev vrchol pepony.
    int first;
    /// Prav vrchol pepony.
    int second;
    /// Vrchol nad peponou.
    int apex;
    /// �ove�trojheln�u ve stromu.
    int level;
    /// Chyba nezavisl�na bodu pohledu.
    float error;
    /// Polom� kuloplochy ohrani�j��trojheln� a jeho potomky.
    float radius;
};

/// Trojheln�y s touto prioritou jsou zobrazeny vdy.
const float PRIORITY_MAX = 1e38f;
/// Trojheln�y s touto prioritou se nezobrazuj�(teoreticky).
const float PRIORITY_MIN = 0.0f;

struct SbROAMMergeQueueDiamond;
class SbROAMSplitQueue;

/** Reprezentace trojheln� algoritmu ROAM ve front�
T�a reprezentuje jeden trojheln� v triangulaci. Roziuje trojheln�
z bin�n�o stromu trojheln� o informace o sousedech a priorit�ve front� */
struct SbROAMSplitQueueTriangle
{
  public:
    /* Metody. */
    /** Konstruktor.
    Vytvo�instanci t�y trojheln�u \p triangle pro prioritn�frontu
    trojheln� na rozd�en�a inicializuje jeho ukazatele na sousedy
    \p left, \p right a \p base. Prioritu nastav�na hodnotu \p priority.
    \param triangle Trojheln� v bin�n� stromu trojheln�.
    \param priority Priorita trojheln�u ve front�
    \param left Lev soused trojheln�u.
    \param right Prav soused trojheln�u.
    \param base Z�ladnov soused trojheln�u.
    \param diamond Diamant, do kter�o trojheln� pat� */
    SbROAMSplitQueueTriangle(SbROAMTriangle * triangle,
      float priority = PRIORITY_MIN,
      SbROAMSplitQueueTriangle * left = NULL,
      SbROAMSplitQueueTriangle * right = NULL,
      SbROAMSplitQueueTriangle * base = NULL,
      SbROAMMergeQueueDiamond * diamond = NULL);
    /** Z�k��priority trojheln�u ve front�
    Vr��hodnotu priority trojheln�u ve front�trojheln� na rozd�en�
    \return Priorita trojheln�u ve front�trojheln� na rozd�en� */
    inline float getPriority() const;
    /** Nastaven�priority trojheln�u ve front�
    Nastav�prioritu trojheln�u ve fronte \p split_queue na novou hodnotu
    \p priority.
    \param split_queue Fronta, v n�se trojheln� nach��
    \param priority Nov�priorita trojheln�u. */
    inline void setPriority(SbROAMSplitQueue * split_queue, const float
      priority);
    /** Nastaven�priority trojheln�u.
    Nastav�prioritu trojheln�u na novou hodnotu \p priority, ale
    neaktualizuje jeho pozici ve front�
    \param priority Nov�priorita trojheln�u. */
    inline void setPriority(const float priority);
    /* Datove polozky. */
    /// Trojheln� v bin�n� stromu trojheln�.
    SbROAMTriangle * triangle;
    /// Soused nad levou odv�nou.
    SbROAMSplitQueueTriangle * left;
    /// Soused nad pravou odv�nou.
    SbROAMSplitQueueTriangle * right;
    /// Soused pod peponou.
    SbROAMSplitQueueTriangle * base;
    /// Diamant, pod kter trojheln� pat�
    SbROAMMergeQueueDiamond * diamond;
  private:
    /// Priorita trojheln�u ve front�trojheln� na rozd�en�
    float priority;
    /// Index trojheln�u ve front�trojheln� na rozd�en�
    int index;
  /** Prioritn�fronta trojheln� na rozd�en�me pistupovat k priv�n�
  atributm trojheln�u. */
  friend class SbROAMSplitQueue;
};

class SbROAMMergeQueue;

/** Uzel pedstavuj��diamant algoritmu ROAM ve front�
T�a pedstavuje diamant, tj. �yi trojhelniky, kter�lze slou�t
na dva trojheln�y spojen�podstavou. T�to slou�n� mohou vzniknout
dal�diamanty. */
struct SbROAMMergeQueueDiamond
{
  public:
    /* Metody. */
    /** Konstruktor.
    Vytvo�instanci diamantu pro prioritn�frontu diamant na spojen�    z trojheln� \p first, \p second, \p third a \p fourth a nastav�    prioritu tohoto diamantu na hodnotu \p priority.
    \param first Prvn�trojheln� diamantu.
    \param second Druh trojheln� diamantu.
    \param third Tet�trojheln� diamantu.
    \param fourth �vrt trojheln� diamantu.
    \param priority Priorita diamantu ve front�diamant. */
    SbROAMMergeQueueDiamond(SbROAMSplitQueueTriangle * first = NULL,
      SbROAMSplitQueueTriangle * second = NULL,
      SbROAMSplitQueueTriangle * third = NULL,
      SbROAMSplitQueueTriangle * fourth = NULL,
      float priority = PRIORITY_MAX);
    /** Z�k��priority diamantu ve front�
    Vr��hodnotu priority diamantu ve front�diamant na spojen�
    \return Priorita diamantu ve front�diamant na spojen� */
    inline float getPriority() const;
    /** Nastaven�priority diamantu ve front�
    Nastav�hodnotu priority diamantu ve front�\p merge_queue na novou
    hodnotu \p priority.
    \param merge_queue Fronta, v n�se diamant nach��
    \param priority Nov�priorita diamantu. */
    inline void setPriority(SbROAMMergeQueue * merge_queue, const float
      priority);
    /** Nastaven�priority diamantu.
    Nastav�hodnotu priority diamantu na novou hodnotu \p priority, ale
    neaktualizuje jeho pozici ve front�
    \param priority Nov�priorita diamantu. */
    inline void setPriority(const float priority);
    /* Datove polozky. */
    /// Prvn�trojheln� diamantu.
    SbROAMSplitQueueTriangle * first;
    /// Druh trojheln� diamantu.
    SbROAMSplitQueueTriangle * second;
    /// Tet�trojheln� diamantu.
    SbROAMSplitQueueTriangle * third;
    /// �vrt trojheln� diamantu.
    SbROAMSplitQueueTriangle * fourth;
  private:
    /// Priorita diamantu ve front�diamant na spojen�
    float priority;
    /// Index diamantu ve front�diamant na spojen�
    int index;
  /** Prioritn�fronta diamant na spojen�me pistupovat k priv�n�
  atributm diamantu. */
  friend class SbROAMMergeQueue;
};

#include <roam/SbROAMSplitQueue.h>
#include <roam/SbROAMMergeQueue.h>

/******************************************************************************
* SbROAMSplitQueueTriangle - public
******************************************************************************/

inline float SbROAMSplitQueueTriangle::getPriority() const
{
  return -priority; // hledame maximum, takze opacna hodnota priority
}

inline void SbROAMSplitQueueTriangle::setPriority(SbROAMSplitQueue *
  split_queue, const float _priority)
{
  /* Nastaveni nove priority a opraveni fronty. */
  priority = -_priority;
  split_queue->newWeight(this, index);
}

inline void SbROAMSplitQueueTriangle::setPriority(const float _priority)
{
  /* Nastaveni nove priority. */
  priority = -_priority;
}

/******************************************************************************
* SbROAMMergeQueueDiamond - public
******************************************************************************/

inline float SbROAMMergeQueueDiamond::getPriority() const
{
  return priority; // vraceni priority
}

inline void SbROAMMergeQueueDiamond::setPriority(SbROAMMergeQueue * merge_queue,
  const float _priority)
{
  /* Nastaveni nove priority a opraveni fronty. */
  priority = _priority;
  merge_queue->newWeight(this, index);
}

inline void SbROAMMergeQueueDiamond::setPriority(const float _priority)
{
  /* Nastaveni nove priority. */
  priority = _priority;
}

#endif
