///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// T�a profileru algoritm.
/// \file PrProfiler.cpp
/// \author Radek Barto�- xbarto33
/// \date 26.06.2005
///
/// Tato statick�t�a slou�pro m�en�doby str�en�vykon��� rznch
/// �st�algoritm. Lze ji pou� bu�p�o nebo za pomoc�maker. Nejprve je
/// nutno prov�t inicializaci t�y pomoc�metody PrProfiler::initProfiler()
/// resp. makra PR_INIT_PROFILER(), posl�e lze m�it dobu str�enou v n�ak�
/// seku k�u pomoc�PrProfiler::startProfile() a PrProfiler::stopProfile()
/// resp. PR_START_PROFILE() a PR_STOP_PROFILE(). Tyto p�azy maj�tak�svou
/// objektovou verzi. Nakonec je mono vytisknout vsledky m�en�do souboru
/// p�azy PrProfiler::printResults() resp. PR_PRINT_RESULTS().
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
#ifdef PROFILE

#include <profiler/PrProfiler.h>

/* Cas potrebny na zjisteni frekvence procesoru. */
const PrTimeStamp PR_WAIT_TIME = 100000;

/* Zjisteni casoveho razitka. */
#ifdef __GNUC__
  __inline PrTimeStamp getTimeStamp()
  {
    register PrTimeStamp edxeax;
    __asm __volatile__( "rdtsc" : "=A" (edxeax));
    return edxeax;
  }
#else
  inline PrTimeStamp getTimeStamp()
  {
    timeval time;
    gettimeofday(&time, NULL);
    return PrTimeStamp(time.tv_sec) * 1000000 + PrTimeStamp(time.tv_usec);
  }
#endif

/******************************************************************************
* PrResult - public
******************************************************************************/

PrResult::PrResult():
  start(false), alg_id(PrProfiler::NULL_ID), object(NULL), time_stamp(0)
{
  // nic
}

PrResult::PrResult(bool _start, const int _alg_id, const void * _object,
  PrTimeStamp _time_stamp):
  start(_start), alg_id(_alg_id), object(_object), time_stamp(_time_stamp)
{
  // nic
}

/******************************************************************************
* PrProfiler - public
******************************************************************************/

/* Inicializace statickych konstant. */
const int PrProfiler::NULL_ID = -1;
const PrResult PrProfiler::NULL_RESULT = PrResult();

void PrProfiler::initProfiler()
{
  /* Inicializace bufferu. */
  result_buffer.clear();
  result_buffer.reserve(1024 * 1024);

  /* Zjisteni poctu tiku CPU za mirkosekundu. */
  #ifdef __GNUC__
    #if 0
      PrTimeStamp start = getTimeStamp();
      usleep(PR_WAIT_TIME);
      PrTimeStamp end = getTimeStamp();
      ticks_per_usec = (end - start) / PR_WAIT_TIME;
    #endif

    timeval tmp;
    PrTimeStamp end_time;
    PrTimeStamp current_time;

    PrTimeStamp start = getTimeStamp();
    gettimeofday(&tmp, NULL);
    end_time = PrTimeStamp(tmp.tv_sec) * 1000000 + PrTimeStamp(tmp.tv_usec)
     + PR_WAIT_TIME;
    do
    {
      gettimeofday(&tmp, NULL);
      current_time = PrTimeStamp(tmp.tv_sec) * 1000000 +
        PrTimeStamp(tmp.tv_usec);
    }
    while (current_time < end_time);
    PrTimeStamp end = getTimeStamp();
    ticks_per_usec = (end - start) / PR_WAIT_TIME;
  #else
    ticks_per_usec = 1;
  #endif

  start_time_stamp = getTimeStamp();
}

void PrProfiler::startProfile(const int alg_id)
{
  /* Volani obecnejsi verze metody. */
  startProfile(alg_id, NULL);
};

void PrProfiler::startProfile(const int alg_id, const void * object)
{
  PrTimeStamp start_time = getTimeStamp();

 //Debug(start_time_stamp);

  result_buffer.push_back(PrResult());
  PrResult & new_result = result_buffer[result_buffer.size() - 1];
  new_result.start = true;
  new_result.alg_id = alg_id;
  new_result.object = object;

  PrTimeStamp stop_time = getTimeStamp();
  start_time_stamp += stop_time - start_time;
  new_result.time_stamp = stop_time - start_time_stamp;
};

void PrProfiler::stopProfile(const int alg_id)
{
  /* Volani obecnejsi verze metody. */
  stopProfile(alg_id, NULL);
};

void PrProfiler::stopProfile(const int alg_id, const void * object)
{
  PrTimeStamp start_time = getTimeStamp();

  //Debug(start_time_stamp);

  result_buffer.push_back(PrResult());
  PrResult & new_result = result_buffer[result_buffer.size() - 1];
  new_result.start = false;
  new_result.alg_id = alg_id;
  new_result.object = object;
  new_result.time_stamp = start_time - start_time_stamp;

  PrTimeStamp stop_time = getTimeStamp();
  start_time_stamp += stop_time - start_time;
};


void PrProfiler::clearBuffer()
{
  result_buffer.clear(); // vyprazdneni bufferu
}

const int PrProfiler::getAlgId(const char * alg_name)
{
  /* Ulozeni noveho nazvu algoritmu a vraceni jeho ID. */
  alg_names.push_back(alg_name);
  return alg_names.size() - 1;
}

const char * PrProfiler::getAlgName(const int alg_id)
{
  // neni-li alg_id registrovano vraceni prazdneho retezce
  if ((alg_id < 0) && (alg_id >= static_cast<int>(alg_names.size())))
  {
    return "";
  }

  return alg_names[alg_id]; // vraceni nazvu algoritmu podle jeho ID
}

const PrTimeStamp PrProfiler::getTicksPerUsec()
{
  return ticks_per_usec;
}

long PrProfiler::getResultCount()
{
  return result_buffer.size(); // zjisteni poctu hodnot v bufferu
}

const PrResult & PrProfiler::getResult(long index)
{
  // je-li prekrocena velikost bufferu
  if (index >= static_cast<int>(result_buffer.size()))
  {
    return NULL_RESULT; // chybna hodnota
  }

  return result_buffer[index]; // vraceni vysledku mereni
}

bool PrProfiler::printResults(const char * filename)
{
  if (result_buffer.size() <= 0) // neni-li co tisknout
  {
    return false;
  }

  std::ofstream file(filename, std::ios::app); // otevreni logu

  if (file.bad()) // neodarilo-li se log otevrit
  {
    return false;
  }

  /* Tisk jednotlivych namerenych hodnot. */
  int result_count = result_buffer.size();
  for (int I = 0; I < result_count; ++I)
  {
    int alg_id = result_buffer[I].alg_id;
    file << result_buffer[I].start << ";" << alg_id << ";" <<
    result_buffer[I].object << ";" << getAlgName(alg_id) << ";" <<
    result_buffer[I].time_stamp / ticks_per_usec << std::endl;
  }

  /* Uspesny konec. */
  file.close();
  return true;
}

/******************************************************************************
* PrProfiler - protected
******************************************************************************/

/* Inicializace statickych promennych. */
std::vector<PrResult> PrProfiler::result_buffer;
std::vector<const char *> PrProfiler::alg_names;
PrTimeStamp PrProfiler::ticks_per_usec = 1;
PrTimeStamp PrProfiler::start_time_stamp = 0;

#endif
