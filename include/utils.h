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

/** Computation of binary logarithm of integer powers.
Calculates the logarithm of the entire word \p number with the logarithm base of 2.
The result is again complete and assumes that the word \p number is an integer
to the power of 2.
\param number  slo, its logarithm to be calculated.
\return Calculated integer logarithm with base 2. */
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

/// The value processed by the parameter.
extern char * optarg;

#ifndef HAVE_GETOPT

/** Program processing parameter.
Like the standard \p getopt function for Microsoft Visual Studio.
\param argc Number of program parameters.
\param argv Program parameter array.
\param optstring describes the parameters to be processed.
\return Character processed by parameter or \p -1 if all parameters
processed. */
int getopt(int argc, char * const argv[], const char * optstring);

#endif // HAVE_GETOPT

#endif
