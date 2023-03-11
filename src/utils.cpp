
////////////////////////////////////////////////////////////////////////////////
//  SoTerrain
////////////////////////////////////////////////////////////////////////////////
/// Obecn�makra a funkce.
/// Hodnota zpracovan�o prarametru.
/// \file utils.cpp
/// \author Radek Barto�- xbarto33
/// \date 19.04.2005
///
/// Makra pro vemon��ly.
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

#include <utils.h>

char * optarg = NULL;

inline bool isshort(char * argument)
{
  return (argument[0] == '-') && (argument[1] != '-');
}

inline bool isvalue(char * argument)
{
  return (argument[0] != '-');
}

#ifndef HAVE_GETOP

int getopt(int argc, char * const argv[], const char * optstring)
{
  static int argument_index = 1;  // Pozice zpracovaneho parametru.
  static int option_index = 1; // Pozice zpracovavane volby v parametru.

  /* Kontrola zadaneho parametru a apripadne nulovani cyklu. */
  if (optstring == NULL)
  {
    optarg = NULL;
    argument_index = 1;
    option_index = 1;
    return -1;
  }

  /* Je-li co zpracovavat. */
  while (argument_index < argc)
  {
    char * argument = argv[argument_index]; // Zpracovavany parametr.
    char * value = NULL; // Potencialni hodnota zpracovan�o parametru.

    /* Zjisteni potencialni hodntoty parametru. */
    if ((argument_index  + 1) < argc)
    {
      if (isvalue(argv[argument_index + 1]))
      {
        value = argv[argument_index + 1];
      }
    }

    /* Je-li zpracovavany parametr seznam kratkch voleb. */
    if (isshort(argument))
    {
      int I = option_index;  // Index kratke volby.

      /* Cyklus pres vsechny kratke volby zpracovan�o parametru. */
      while (argument[I] != '\0')
      {
        char option = argument[I]; // Kratka volba.
        int J = 0; // Index definovane volby.

        /* Cyklus pres vsechny definovane volby. */
        while (optstring[J] != '\0')
        {
          char def_option = optstring[J]; // Definovana volba.

          /* Preskoceni symbolu pro pritomnost hodnoty. */
          if (def_option != ':')
          {
            /* Pasuje-li zpracovavana volba s definovanou. */
            if (option == def_option)
            {
              // Priznak hodnoty definovane volby.
              bool def_value = optstring[J + 1] == ':';

              /* Kontrola zda je kratka volba s hodnotou na konci seznamu. */
              if (def_value && (argument[I + 1] != '\0'))
              {
                fprintf(stderr, "Chyba: Volba s hodnotou musi byt na konci"
                 " seznamu voleb!\n");
                optarg = NULL;
                return -1;
              }

              /* Nastaveni pripadne hodnoty parametru. */
              optarg = def_value ? value : NULL;
              option_index++;
              return def_option;
            }
          }

          /* Na dalsi definovanou volbu. */
          J++;
        }

        /* Zadna volba v parametru nepasovala. */
        optarg = NULL;
        option_index++;
        return '?';
      }

      /* Ukonceni zpracovani seznamu voleb. */
      option_index = 1;
      argument_index++;
    }
    else
    {
      /* Preskoceni nekratkeho parametru. */
      option_index = 1;
      argument_index++;
    }
  }

  optarg = NULL;
  return -1;
}

#endif // HAVE_GETOPT
