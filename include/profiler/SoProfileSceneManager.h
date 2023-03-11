#ifndef SO_PROFILE_SCENE_MANAGER_H
#define SO_PROFILE_SCENE_MANAGER_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// T�a roziuj��t�u SoSceneManager o monosti profilov��
/// \file SoProfileSceneManager.h
/// \author Radek Barto�- xbarto33
/// \date 05.07.2005
///
/// Tento manaer sc�y umo�je zjistit jakou dobu str�il procesor po�ta�
/// vykreslov�� sc�y. Pro pouit�je teba nahradit vchoz�manaer sc�y
/// t�to a inicializovat profilovac�knihovnu ve t��::PrProfiler.
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

#include <Inventor/SoSceneManager.h>
#include <Inventor/SbTime.h>

#include <profiler/PrProfiler.h>

#ifdef PROFILE

/** T�a roziuj��t�u SoSceneManager o monosti profilov��
Tento manaer sc�y umo�je zjistit jakou dobu str�il procesor po�ta�
vykreslov�� sc�y. Pro pouit�je teba nahradit vchoz�manaer sc�y
t�to a inicializovat profilovac�knihovnu ve t��::PrProfiler. */
class SoProfileSceneManager: public SoSceneManager
{
  public:
    /** Konstruktor.
    Nastav�vychoz�instance t� ::SoGLRenderAction a ::SoHandleEventAction. */
    SoProfileSceneManager();
    /** Destruktor.
    Zru�instanci t�y ::SoProfileSceneManager. */
    virtual ~SoProfileSceneManager();
    /** Vykresl�graf sceny.
    M�li \p clearwindow hodnotu \p TRUE, nejd�e vymae buffer pro vykreslov��
    M�li \p clearzbuffer hodnotu \p TRUE, ped vykreslov�� vymae z-buffer.
    Oba tyto parametry by za norm�n�h okolnost�m�y bt \p TRUE, ale ve
    speci�n�h p�adech mohou bt nastaveny na \p FALSE kvli optimalizaci.
    (Nap. pi vykreslov��pomoc��r nen�poteba pou� z-buffer. */
    virtual void render(const SbBool clearwindow = TRUE,
      const SbBool clearzbuffer = TRUE);
    /** Zjit��doby trv��posledn�o vykreslen�
    Vr��dobu, kterou trvalo posledn�vykreslen�grafu sceny. */
    virtual SbTime getLastRenderTime() const;
  protected:
    /// Po�t jednotek �su profilovac�knihovny za mikrosekundu.
    PrTimeStamp ticks_per_usec;
    /// Doba vykreslov��posledn�o sn�ku.
    SbTime last_render_time;
};

#else

class SoProfileSceneManager: public SoSceneManager
{
  // nic
};

#endif // PROFILE
#endif // SO_PROFILE_SCENE_MANAGER_H
