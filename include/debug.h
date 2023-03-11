#ifndef DEBUG_H
#define DEBUG_H

////////////////////////////////////////////////////////////////////////////////
//  SoTerrain
////////////////////////////////////////////////////////////////////////////////
/// Obecn�makra pro ladic�vpisy.
/// \file debug.h
/// \author Radek Barto�- xbarto33
/// \date 19.04.2005
///
/// Makra slou��k pomocnm vpism b�em lad��programu.
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

// ostatni includy
#include <iostream>

/** Vpis ladic�zpr�y.
Zap� na standardn�vstup zpr�u \p message ve form�u vhodn� pro lad��
\param message Zpr�a pro ladic�vpis. Me bt zaps�a jako b� vraz nebo
(vzhledem k tomu, e to je makro) jako seznam identifik�or odd�ench
oper�orem <<. */
#ifdef DEBUG
  #define Debug(message) \
  { \
    std::cout << "DEBUG: " << "file " << __FILE__ << " at line " << __LINE__ \
      << " in " << __FUNCTION__ << ": " << #message << ": " << message << \
      std::endl; \
  }
#else
  #define Debug(message)
#endif

/** Zobrazen�obsahu asociativn�o pole.
Vyp� na standardn�vstup obsah asociativn�o pole \p map typu \p std::map
ve form�u vhod� pro lad��
\param map Asociativn�pole, jeho obsah se m�vypsat.
\param iterator_type Typ iter�oru, kter ma do asociativn�o pole \p map p�tup.
*/
#ifdef DEBUG
  #define DebugMap(map, iterator_type) \
  { \
    std::cout << "DEBUG: " << "file " << __FILE__ << " at line " << __LINE__ \
      << " in " << __FUNCTION__  << ": " << "polozky asociativniho pole " << \
      #map << ":" << std::endl; \
    t_size __tmp_index = 0; \
    for (iterator_type __tmp_it = map.begin(); __tmp_it != map.end(); \
      ++__tmp_it, ++__tmp_index) \
    { \
      std::cout << "\t" << __tmp_index + 1 << ". polozka: " << __tmp_it->first \
        << " hodnota: " << __tmp_it->second << std::endl; \
    } \
  }
#else
  #define DebugMap(map, iterator_type)
#endif

/** Zobrazeni obsahu vektoru.
Vyp� na standardn�vstup obsah vektoru typu \p std::vector ve form�u
vhod� pro lad��
\param vector Asociativn�pole, jeho obsah se m�vypsat.
\param iterator_type Typ iter�oru, kter ma do vectoru \p vector p�tup.
*/
#ifdef DEBUG
  #define DebugVec(vector, iterator_type) \
  { \
    std::cout << "DEBUG: " << "file " << __FILE__ << " at line " << __LINE__ \
      << " in " << __FUNCTION__  << ": " << "polozky vektoru " << #vector << ":" \
      << std::endl; \
    t_size __tmp_index = 0; \
    for (iterator_type __tmp_it = vector.begin(); __tmp_it != vector.end(); \
      ++__tmp_it, ++__tmp_index) \
    { \
      std::cout << "\t" << __tmp_index + 1 << ". polozka: " << *__tmp_it << \
      std::endl; \
    } \
  }
#else
  #define DebugVec(vector, iterator_type)
#endif

#endif
