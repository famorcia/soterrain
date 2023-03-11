#ifndef PR_PROFILER_H
#define PR_PROFILER_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// T�a profileru algoritm.
/// file PrProfiler.h
/// \author Radek Barton - xbarto33
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

#include <cstdlib>
#include <vector>
#include <fstream>
#ifdef __GNUC__
  #include <sys/time.h>
#else
  #include <time.h>
#endif

#include <iostream>

#include <debug.h>

#ifdef PROFILE

/// Typ pro uloen��su - platformn�z�isl.
#ifdef __GNUC__
  typedef unsigned long long PrTimeStamp;
#else
  typedef int64_t PrTimeStamp;
#endif

/** Jeden z�nam m�en�
Tato t�a slou�pro ukl���jednotlivch z�nam profilov��do pam�i
a n�ledn�vytisknut�do zadan�o souboru. */
struct PrResult
{
  public:
    /** Konstruktor.
    Vytvo�instanci t�y ::PrProfiler s nulovmi hodnotami. */
    PrResult();
    /** Konstruktor.
    Vytvo�instanci t�y ::PrProfiler s hodnotami \p start, \p alg_id,
    \p object a \p time_stamp.
    \param start P�nak za�tku profilov��seku k�u.
    \param alg_id ID algoritmu piazen�o seku k�u.
    \param object Instance objektu p�luej��o z�namu.
    \param time_stamp �sov�raz�ko z�namu. */
    PrResult(bool start, const int alg_id, const void * object,
      PrTimeStamp time_stamp);
    /// P�nak za�tku profilov��seku k�u.
    bool start;
    /// ID algoritmu piazen�o seku k�u.
    int alg_id;
    /// Instance objektu p�luej��o z�namu.
    const void * object;
    /// �sov�raz�ko z�namu.
    PrTimeStamp time_stamp;
};

/** T�a profileru algoritm.
Tato statick�t�a slou�pro m�en�doby str�en�vykon��� rznch
�st�algoritm. Lze ji pou� bu�p�o nebo za pomoc�maker. Nejprve je
nutno prov�t inicializaci t�y pomoc�metody PrProfiler::initProfiler()
resp. makra PR_INIT_PROFILER(), posl�e lze m�it dobu str�enou v n�ak�
seku k�u pomoc�PrProfiler::startProfile() a PrProfiler::stopProfile()
resp. PR_START_PROFILE() a PR_STOP_PROFILE(). Tyto p�azy maj�tak�svou
objektovou verzi. Nakonec je mono vytisknout vsledky m�en�do souboru
p�azy PrProfiler::printResults() resp. PR_PRINT_RESULTS(). */
class PrProfiler
{
  public:
    /* Metody. */
    /** Inicializace profileru.
    Zjist�takt procesoru, inicializuje buffer pro zapisov��nam�ench
    vsledk, inicializuje syst� pro generov��ID algoritm. */
    static void initProfiler();
    /** Vytvo�z�nam za�tku algoritmu.
    Spust�m�en��su b�u algoritmu s ID  \p alg_id.
    \param alg_id ID sput��o algoritmu. */
    static void startProfile(const int alg_id);
    /** Vytvo�z�nam za�tku algoritmu (objektov�verze).
    Spust�m�en��su b�u algoritmu s ID \p alg_id v objektu \p object.
    \param alg_id ID sput��o algoritmu.
    \param object Instance objektu algoritmu. */
    static void startProfile(const int alg_id, const void * object);
    /** Vytvo�z�nam konce algoritmu.
    Ukon� m�en��su b�u agoritmu s ID \p alg_id.
    \param alg_id ID sput��o algoritmu. */
    static void stopProfile(const int alg_id);
    /** Vytvo�z�nam konce algoritmu (objektov�verze).
    Ukon� m�en��su b�u agoritmu s ID \p alg_id na objektu \p object.
    \param alg_id ID sput��o algoritmu.
    \param object Instance objektu algoritmu. */
    static void stopProfile(const int alg_id, const void * object);
    /** Vypr�dn�buffer z�nam m�en�
    Vy�st�vsledky m�en�doby trv��algoritm. */
    static void clearBuffer();
    /** Vr��ID algoritmu.
    Piad�n�vu algoritmu \p alg_name jedine��ID a vrat�jej.
    \param alg_name N�ev algoritmu.
    \return ID algoritmu. */
    static const int getAlgId(const char * alg_name);
    /** Vr��n�ev algoritmu.
    Vr��n�ev algoritmu s ID \p alg_id. Pokud agoritmus s danm ID
    nenexistuje vr��\p NULL.
    \param alg_id ID algoritmu.
    \return N�ev algoritmu. */
    static const char * getAlgName(const int alg_id);
    /** Vr��po�t �sovch jednotek za mikrosekundu.
    Vr��po�t takt �sov�o raz�ka, kter�je ve struktue ::PrResult, za
    jednu mikrosekundu. Po vyd�en��sov�o raz�ka touto hodnotou lze z�kat
    jeho hodnotu v mikrosekund�h.
    \return Po�t �sovch jednotek za mikrosekundu. */
    static const PrTimeStamp getTicksPerUsec();
    /** Vr��po�t nam�ench hodnot.
    Vr��po�t nam�ench hodnot v bufferu, kter�byly doposud nam�eny.
    \return Po�t nam�ench hodnot. */
    static long getResultCount();
    /** Vr��vsledek m�en�
    Vr��nam�enou hodnotu na indexu \p index. Pekro�-li index po�t
    nam�ench vsledk, vr��PrProfiler::NULL_RESULT. �sov�raz�ko vyd�en�    hodnotou vr�enou z funkce PrProfiler::getTicksPerUsec() ud��po�t
    mikrosekund od po�tku m�en�definovan�o inicializac�profileru.
    \param index Index vsledku m�en�
    \return Vsledek m�en� */
    static const PrResult & getResult(long index);
    /** Vytiskne vsledky m�en�
    Vytiskne vsledky na konec souboru \p filename. Pokud se tisk poda�vr��    \p true, jinak \p false.
    \param filename Soubor kam vsledky tisknout.
    \return P�nak sp��o tisku vsledk. */
    static bool printResults(const char * filename);
    /* Konstanty. */
    /// Hodnota neplatn�o ID.
    static const int NULL_ID;
    /// Vysledek m�en�s pr�dnou hodnotou.
    static const PrResult NULL_RESULT;
  protected:
    /* Promenne. */
    /// Vektor vsledk m�en�
    static std::vector<PrResult> result_buffer;
    /// Vektor n�v algoritm.
    static std::vector<const char *> alg_names;
    /// Po�t takt CPU za mikrosekundu.
    static PrTimeStamp ticks_per_usec;
    /// �sov�raz�ko pi za�tku m�en�
    static PrTimeStamp start_time_stamp;
};

/* Makra pro snadnejsi pouzivani tridy PrProfiler. Jsou funkcni pouze tehdy,
je-li definovana promenna preprocesoru PROFILE. */
/** Ur� za�tek profilov��
Vytvo�z�nam za�tku profilov��algoritmu s n�vem \p name. */
#define PR_START_PROFILE(name) \
  static int name##_id = PrProfiler::getAlgId(#name); \
  PrProfiler::startProfile(name##_id)
/** Ur� za�tek profilov��(objektov�verze).
Vytvo�z�nam za�tku profilov��algoritmu s n�vem \p name pat��o objektu
\p object. */
#define PR_START_OBJ_PROFILE(name, object) \
  static int name##_id = PrProfiler::getAlgId(#name); \
  PrProfiler::startProfile(name##_id, object)
/** Ur� konec profilov��
Vytvo�z�nam konce profilov��algoritmu s n�vem \p name. */
#define PR_STOP_PROFILE(name) PrProfiler::stopProfile(name##_id)
/** Ur� konec profilov��(objektov�verze).
Vytvo�z�nam konce profilov��algoritmu s n�vem \p name pat��o objektu
\p object. */
#define PR_STOP_OBJ_PROFILE(name, object) \
  PrProfiler::stopProfile(name##_id, object)
/** Inicializace profileru.
Incializace cel�o profileru, je nutno zavolat ped jeho pouit�,
nejl�e na za�tku programu. */
#define PR_INIT_PROFILER() PrProfiler::initProfiler()
/** Tisk vsledku profilov��
Vytiskne vsledky profilov��na konec souboru \p filename ve form�u
definovan� v PrProfiler::printResults(). */
#define PR_PRINT_RESULTS(filename) PrProfiler::printResults(filename)

#else // PROFILE

struct PrResult;
class PrProfiler;

#define PR_START_PROFILE(name)
#define PR_START_OBJ_PROFILE(name, object)
#define PR_STOP_PROFILE(name)
#define PR_STOP_OBJ_PROFILE(name, object)
#define PR_INIT_PROFILER()
#define PR_PRINT_RESULTS(filename)

#endif // PROFILE
#endif // PR_PROFILER_H
