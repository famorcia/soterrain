#ifndef DEBUG_H
#define DEBUG_H

////////////////////////////////////////////////////////////////////////////////
//  SoTerrain
////////////////////////////////////////////////////////////////////////////////
/// General macros for debug entries.
/// \file debug.h
/// \author Radek Barton - xbarto33
/// \date 19.04.2005
///
/// Macros with auxiliary entries during debugging.
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

#include <iostream>

/** Entry of debug messages.
Turn on the standard input of the \p message in a form suitable for debugging
\param message The message for the debug entry. I should be listed as a murderer or
(since this is a macro) as a list of department identifiers
operator <<. */
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

/** The content of the associative field is displayed.
Outputs the contents of an associative array \p map of type \p std::map to standard input
in a form suitable for harmony
\param map An associative field, its contents should be printed.
\param iterator_type The type of iterator that has access to the associative array \p map.
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

/** Display the contents of the vector.
Outputs to standard input the contents of a vector of type \p std::vector in the form
suitable for harmony
\param vector An associative array, its content to be written.
\param iterator_type The type of iterator that has a vector path to the \p vector.
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
