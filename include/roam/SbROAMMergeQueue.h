#ifndef SB_ROAM_MERGE_QUEUE_H
#define SB_ROAM_MERGE_QUEUE_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Prioritn�fronta diamant na spojen�
/// \file SbROAMMergeQueue.h
/// \author Radek Barto�- xbarto33
/// \date 25.08.2005
///
/// Do t�o prioritn�fronty se vkl�aj�diamanty, kter�lze spojit na dva
/// trojheln�y a t� dva trojheln�y z pvodn�h �y z triangulace odstranit.
/// Toto se prov��tehdy, pokud je teba triangulaci zjednoduit a to tak, e
/// se vyjme diamant s minim�n�prioritou, spoj�se a vloi se zp� p�adn�/// vznikl�nov�diamanty.
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

#include <Inventor/SbHeap.h>

struct SbROAMSplitQueueTriangle;
struct SbROAMMergeQueueDiamond;

/** Prioritn�fronta diamant na spojen�algoritmu ROAM.
Do t�o prioritn�fronty se vkl�aj�diamanty, kter�lze spojit na dva
trojheln�y a t� dva trojheln�y z pvodn�h �y z triangulace odstranit.
Toto se prov��tehdy, pokud je teba triangulaci zjednoduit a to tak, e
se vyjme diamant s minim�n�prioritou, spoj�se a vloi se zp� p�adn�vznikl�nov�diamanty. */
class SbROAMMergeQueue : protected SbHeap
{
  public:
    /* Metody. */
    /** Konstruktor.
    Vytvo�instanci t�y ::SbROAMMergeQueue o po�te��velikosti
    \p init_size poloek. Prioritn�fronta se sama podle poteb zv�uje.
    \param init_size Po�te��velikost prioritn�fronty trojheln�
    na rozd�en�  */
    SbROAMMergeQueue(const int init_size = 4096);
    /** Destruktor.
    Zru�instanci t�y ::SbROAMMergeQueue. */
    ~SbROAMMergeQueue();
    /** Vypr�dn�prioritn�frontu.
    Zavol�� t�o metody dojde k vypr�dn��prioritn�fronty diamant na
    spojen� */
    void emptyQueue();
    /** Vloen�diamantu do fronty.
    Vlo�diamant \p diamond do prioritn�fronty diamant na spojen�
    \param diamond Diamant, kter se m�do fronty vloit. */
    void add(SbROAMMergeQueueDiamond * diamond);
    /** Odstran�diamant z fronty.
    Odstran�diamant \p diamond z prioritn�fronty diamant.
    \param diamond Diamant, kter se m�z fronty odstranit. */
    void remove(SbROAMMergeQueueDiamond * diamond);
    /** Odstran�diamant z fronty podle jeho trojheln�u.
    Najde ve front�diamant na spojen�diamant, jen obsahuje trojheln�
    \p triangle, vyjme ho z fronty a vr��na n� ukazatel.
    \param triangle Trojheln�, pat��odstra�van�u diamantu.
    \return Ukazatel na odstran� diamant. */
    SbROAMMergeQueueDiamond * remove(SbROAMSplitQueueTriangle * triangle);
    /** Vyjme a vr��diamant z fronty.
    Odstran�z prioritn�fronty diamant na spojen�trojheln� s
    nejni�prioritou a vr��ukazatel na n�.
    \return Ukazatel na diamant s nejni�prioritou. */
    SbROAMMergeQueueDiamond * extractMin();
    /** Vr��diamant z fronty.
    Vr��ukazatel na diamant s nejni�prioritou ve front� Diamant ve front�    nad�e zst��
    \return Ukazatel na diamant s nejni�prioritou. */
    SbROAMMergeQueueDiamond * getMin();
    /** Vr��diamant z fronty podle indexu.
    Vr��ukazatel na diamant na indexu \p index v prioritn�front�diamant.
    \return Ukazatel na trojheln� na indexu \p index. */
    SbROAMMergeQueueDiamond * operator[](const int index);
    using SbHeap::size;
    using SbHeap::newWeight;
    using SbHeap::buildHeap;
  protected:
    /* Metody. */
    /** Vr��prioritu diamantu.
    Vr��prioritu diamantu \p diamond v prioritn�front�diamant na spojen�
    \param diamond Diamant, jeho priorita se m�z�kat.
    \return Priorita diamantu. */
    static float getDiamondPriority(void * diamond);
    /** Vr��index diamantu.
    Vr��index diamantu \p diamant v prioritn�front�diamant na spojen�
    \param diamond Diamant, jeho index se m�z�kat.
    \return Index diamantu. */
    static int getDiamondIndex(void * diamond);
    /** Nastav�index diamantu.
    Nastav�index diamantu \p diamond v prioritn�front�diamant na hodnotu
    \p index.
    \param diamond Diamant, u kter�o se m�index nastavit.
    \param index Nov index diamantu. */
    static void setDiamondIndex(void * diamond, int index);
    /* Datove polozky. */
    /// Struktura callback obsahuj��metody pro pr�i s diamanty.
    static SbHeapFuncs heap_funcs;
  private:
};

#endif
