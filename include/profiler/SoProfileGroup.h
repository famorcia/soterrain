#ifndef SO_PROFILE_GROUP_H
#define SO_PROFILE_GROUP_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// T�a roziuj��t�u SoGroup o monosti profilov��
/// \file SoProfileGroup.h
/// \author Radek Barton - xbarto33
/// \date 18.04.2006
///
///
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

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/SbTime.h>

#include <profiler/PrProfiler.h>

#ifdef PROFILE

/** T�a roziuj��t�u SoGroup o monosti profilov��

*/
class SoProfileGroup: public SoGroup
{
  SO_NODE_HEADER(SoProfileGroup);
  public:
    /* Metody. */
    /** Run-time inicializace t�y.
    Tuto metodu je teba zavolat ped vytvoen� jak�oliv instance t�y
    ::SoProfileGroup. */
    static void initClass();
    /** Konstruktor.

    */
    SoProfileGroup();
    /** Konstruktor.

    */
    SoProfileGroup(int numChildren);
    /**

    */
    virtual void GLRender(SoGLRenderAction * action);
    /** Zjit��doby trv��posledn�o vykreslen�
    Vr��dobu, kterou trvalo posledn�vykreslen�grafu sceny. */
    virtual SbTime getLastRenderTime() const;
  protected:
    /* Datove polozky. */
    /// Po�t jednotek �su profilovac�knihovny za mikrosekundu.
    PrTimeStamp ticks_per_usec;
    /// Doba vykreslov��posledn�o sn�ku.
    SbTime last_render_time;
  private:
    /* Metody. */
    /** Destruktor.

    */
    virtual ~SoProfileGroup();
};

#else

class SoProfileGroup: public SoGroup
{
  SO_NODE_HEADER(SoProfileGroup);
  public:
    static void initClass();
    SoProfileGroup();
    SoProfileGroup(int numChildren);
  private:
    virtual ~SoProfileGroup();
};

#endif // PROFILE
#endif // SO_PROFILE_SCENE_MANAGER_H
