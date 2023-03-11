#ifndef UTILS_H
#define UTILS_H

////////////////////////////////////////////////////////////////////////////////
//  SoTerrain
////////////////////////////////////////////////////////////////////////////////
/// Obecn�makra a funkce.
/// Hodnota zpracovan�o prarametru.
/// \file utils.h
/// \author Radek Barton - xbarto33
/// \date 19.04.2005
///
/// Macros for bugs.
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

#include <stdio.h>

#include <debug.h>

/** Vpo�t dvojkov�o logaritmu celo�seln�o �sla.
Vypo�e logaritmus celo�seln�o �sla \p number se z�ladem logaritmu 2.
Vsledek je op� celo�seln a pedpokl��se, ze �slo \p number je n�terou
mocninou �sla 2.
\param number �slo, jeho logaritmus se m�spo�tat.
\return Vypo�en celo�seln logaritmus se z�ladem 2. */
inline int ilog2(int number)
{
  int tmp = number;
  int result = 0;
  while (tmp > 1)
  {
    result++;
    tmp >>= 1;
  }
  return result;
}

/// Hodnota zpracovan�o prarametru.
extern char * optarg;

#ifndef HAVE_GETOPT

/** Zpracov��parametr programu.
N�rada standardn�funkce \p getopt pro prosted�Microsoft Visual Studio.
\param argc Po�t parametr programu.
\param argv Pole parametr programu.
\param optstring �t�ec popisuj��zpracov�an�parametry.
\return Znak zpracovan�o parametru nebo \p -1, jsou-li vsechny parametry
zpracov�y. */
int getopt(int argc, char * const argv[], const char * optstring);

#endif // HAVE_GETOPT

#endif
