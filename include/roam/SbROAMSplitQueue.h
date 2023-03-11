#ifndef SB_ROAM_SPLIT_QUEUE_H
#define SB_ROAM_SPLIT_QUEUE_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Prioritn�fronta trojheln� na rozd�en�
/// \file SbROAMSplitQueue.h
/// \author Radek Barton - xbarto33
/// \date 25.08.2005
///
/// Do t�o prioritn�fronty se vkl�aj�trojheln�y, kter�je teba zaadit
/// do triangulace. Pokud je teba triangulaci d�e zjem�vat, vyjme se z fronty
/// trojheln� s maxim�n�prioritou a vlo�se zp� jeho potomci.
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

/** Prioritn�fronta trojheln� na rozd�en�algoritmu ROAM.
Do t�o prioritn�fronty se vkl�aj�trojheln�y, kter�je teba zaadit
do triangulace. Pokud je teba triangulaci d�e zjem�vat, vyjme se z fronty
trojheln� s maxim�n�prioritou a vlo�se zp� jeho potomci. */
class SbROAMSplitQueue : protected SbHeap
{
  public:
    /* Metody. */
    /** Konstruktor.
    Vytvo�instanci t�y ::SbROAMSplitQueue o po�te��velikosti
    \p init_size poloek. Prioritn�fronta se sama podle poteb zv�uje.
    \param init_size Po�te��velikost prioritn�fronty trojheln�
    na rozd�en�  */
    SbROAMSplitQueue(const int init_size = 1024);
    /** Destruktor.
    Zru�instanci t�y ::SbROAMSplitQueue. */
    ~SbROAMSplitQueue();
    /** Vypr�dn�prioritn�frontu.
    Zavol�� t�o metody dojde k vypr�dn��prioritn�fronty trojheln�
    na rozd�en� */
    void emptyQueue();
    /** Vlo�trojheln� do fronty.
    Vlo�trojheln� bin�n�o stromu trojheln� \p triangle do prioritn�    fronty trojheln� na rozd�en�
    \param triangle Trojheln�, kter se m�do fronty vloit. */
    void add(SbROAMSplitQueueTriangle * triangle);
    /** Odstran�trojheln� z fronty.
    Odstran�trojheln� \p triangle z prioritn�fronty trojheln�.
    \param triangle Trojheln�, kter se m�z fronty odstranit. */
    void remove(SbROAMSplitQueueTriangle * triangle);
    /** Vyjme a vr��trojheln� z fronty.
    Odstran�z prioritn�fronty trojheln� na rozd�en�trojheln� s
    nejvy�prioritou a vr��ukazatel na n�.
    \return Ukazatel na trojheln� s nejvy�prioritou. */
    SbROAMSplitQueueTriangle * extractMax();
    /** Vr��trojheln� z fronty.
    Vr��ukazatel na trojheln� s nejvy�prioritou ve front� Trojheln�
    ve front�nad�e zst��
    \return Ukazatel na trojheln� s nejvy�prioritou. */
    SbROAMSplitQueueTriangle * getMax();
    /** Vr��trojheln� z fronty podle indexu.
    Vr��ukazatel na trojheln� na indexu \p index v prioritn�front�    trojheln�.
    \return Ukazatel na trojheln� na indexu \p index. */
    SbROAMSplitQueueTriangle * operator[](const int index);
    /* Importovane metody. */
    using SbHeap::size;
    using SbHeap::newWeight;
    using SbHeap::buildHeap;
 protected:
    /* Metody. */
    /** Vr��prioritu trojheln�u.
    Vr��prioritu trojheln�u \p triangle v prioritn�front�trojheln�
    na rozd�en�
    \param triangle Trojheln�, jeho priorita se m�z�kat.
    \return Priorita trojheln�u. */
    static float getTrianglePriority(void * triangle);
    /** Vr��index trojheln�u.
    Vr��index trojheln�u \p triangle v prioritn�front�trojheln�
    na rozd�en�
    \param triangle Trojheln�, jeho index se m�z�kat.
    \return Index trojheln�u. */
    static int getTriangleIndex(void * triangle);
    /** Nastav�index trojheln�u.
    Nastav�index trojheln�u \p triangle v prioritn�front�trojheln�
    na hodnotu \p index.
    \param triangle Trojheln�, u kter�o se m�index nastavit.
    \param index Nov index trojheln�u. */
    static void setTriangleIndex(void * triangle, int index);
    /* Datove polozky. */
    /// Struktura callback obsahuj��metody pro pr�i s trojheln�y.
    static SbHeapFuncs heap_funcs;
};

#endif
