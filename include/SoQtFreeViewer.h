#ifndef SO_QT_FREE_VIEWER_H
#define SO_QT_FREE_VIEWER_H

///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Prohl��pro voln pohyb ve sc��pomoc�myi a kl�esnice.
/// \file SoQtFreeViewer.h
/// \author Radek Barto�- xbarto33
/// \date 06.03.2006
///
/// Prohl��pro voln pohyb ve sc��pomoc�myi a kl�esnice.
/// Zobraz�okno se sc�ou, ve kter�je mono se pohybovat pomoc�myi a
/// kl�esnice. Kurzor myi je oknem zachycen a skryt, take je mono ot�et
/// pohledem do vech sm� dokola. Pozici kamery lze ovl�at t�ito kl�esami:
/// <DL>
///   <DT>W</DT><DD>posun kamery kupedu</DD>
///   <DT>S</DT><DD>posun kamery dozadu</DD>
///   <DT>A</DT><DD>posun kamery doleva</DD>
///   <DT>D</DT><DD>posun kamery doprava</DD>
///   <DT>Q</DT><DD>posun kamery nahoru</DD>
///   <DT>E</DT><DD>posun kamery dol</DD>
///   <DT>LEFT</DT><DD>nato�n�kamery doleva</DD>
///   <DT>RIGHT</DT><DD>nato�n�kamery doprava</DD>
///   <DT>UP</DT><DD>nato�n�kamery nahoru</DD>
///   <DT>DOWN</DT><DD>nato�n�kamery dol</DD>
///   <DT>PGUP</DT><DD>naklon��kamery doleva</DD>
///   <DT>PGDOWN</DT><DD>naklon��kamery doprava</DD>
/// </DL>
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

#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtCursor.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>

#include <qcursor.h>
#include <qwidget.h>
#include <qpoint.h>
#include <qsize.h>

#include <debug.h>

/** Prohl��pro voln pohyb ve sc��pomoc�myi a kl�esnice.
Zobraz�okno se sc�ou, ve kter�je mono se pohybovat pomoc�myi a
kl�esnice. Kurzor myi je oknem zachycen a skryt, take je mono ot�et
pohledem do vech sm� dokola. Pozici kamery lze ovl�at t�ito kl�esami:
<DL>
  <DT>W</DT><DD>posun kamery kupedu</DD>
  <DT>S</DT><DD>posun kamery dozadu</DD>
  <DT>A</DT><DD>posun kamery doleva</DD>
  <DT>D</DT><DD>posun kamery doprava</DD>
  <DT>Q</DT><DD>posun kamery nahoru</DD>
  <DT>E</DT><DD>posun kamery dol</DD>
  <DT>LEFT</DT><DD>nato�n�kamery doleva</DD>
  <DT>RIGHT</DT><DD>nato�n�kamery doprava</DD>
  <DT>UP</DT><DD>nato�n�kamery nahoru</DD>
  <DT>DOWN</DT><DD>nato�n�kamery dol</DD>
  <DT>PGUP</DT><DD>naklon��kamery doleva</DD>
  <DT>PGDOWN</DT><DD>naklon��kamery doprava</DD>
</DL>*/
class SoQtFreeViewer : public SoQtViewer
{
  SOQT_OBJECT_HEADER(SoQtFreeViewer, SoQtViewer);
  public:
    /** Konstruktor.
    Vytvo�instanci t�y ::SoQtFreeViewer jako potomka okna \p parent a
    nastav�jeho vnitn�jm�o na hodnotu \p name. V p�ad� e nen�specifikov�
    parametr \p name, je jm�o ur�no automaticky. Parametr \p emdbed slou�    k ur�n� zda m�bt prohl��vloen do sv�o rodi�, � zda m�vytvoit svj
    vlastn�shell.
    \param parent Ukazatel na rodi�vsk�okno.
    \param name Jm�o prohl��.
    \param embed P�nak vloen�prohl�� do sv�o rodi�. */
    SoQtFreeViewer(QWidget * parent = NULL, const char * name = NULL,
      SbBool embed = TRUE);
    /** Destruktor.
    Zru�instanci t�y ::SoQtFreeViewer. */
    ~SoQtFreeViewer();
    /** Vr��citlivost myi.
    Vr��nastavenou hodnotu citlivosti myi prohl��.
    \return Citlivost myi prohl��. */
    float getMouseSensitivity() const;
    /** Nastav�citlivost myi.
    Nastav�citlivost myi prohl�� na hodnotu \p sensitivity.
    \param sensitivity Nov�hodnota citlivosti myi prohl��. */
    void setMouseSensitivity(const float sensitivity);
    /** Vr��p�nak invertovan�myi.
    Vr��nastavenou hodnotu p�naku invertovan�myi prohl��.
    \return P�nak invertovan�myi. */
    SbBool getInvertMouse() const;
    /** Nastav�p�nak invertovan�myi.
    Nastav�p�nak invertovan�myi prohl�� na hodnotu \p enabled.
    \param enabled Nov�hodnota p�naku invertovan�myi. */
    void setInvertMouse(const SbBool enabled);
  protected:
    /* Metody. */
    /** Konstruktor.
    Vytvo�instanci t�y ::SoQtFreeViewer jako potomka okna \p parent a
    nastav�jeho vnitn�jm�o na hodnotu \p name. V p�ad� e nen�specifikov�
    parametr \p name, je jm�o ur�no automaticky. Parametr \p emdbed slou�    k ur�n� zda m�bt prohl��vloen do sv�o rodi�, � zda m�vytvoit svj
    vlastn�shell. Parametr \p build se pou��pro zajit��spr�n�o poad�    inicializace sou�st�prohl��.
    \param parent Ukazatel na rodi�vsk�okno.
    \param name Jm�o prohl��.
    \param embed P�nak vloen�prohl�� do sv�o rodi�.
    \param build P�nak vytvoen�okna prohl��. */
    SoQtFreeViewer(QWidget * parent, const char * const name, SbBool embed,
      SbBool build);
    /** Obecn callback ud�ost�
    Zpracuje poadavek na obslouen�ud�osti prohl�� a vr��\p TRUE
    v p�ad� e byla ud�ost obslouena. Ve skute�osti pouze zjist�typ
    ud�osti a obslouen�ponech�na p�lun callback za�en�
    \param event Objekt ud�osti.
    \return P�nak obslouen�ud�osti. */
    virtual SbBool processSoEvent(const SoEvent * const event);
    /** Callback ud�ost�pohybu myi.
    Zpracuje ud�ost pohybu myi prohl�� a vr��\p TRUE v p�ad� e byla
    ud�ost obslouena.
    \param event Objekt ud�osti.
    \return P�nak obslouen�ud�osti. */
    virtual SbBool processSoLocation2Event(const SoLocation2Event * const
      event);
    /** Callback ud�ost�kl�esnice.
    Zpracuje ud�ost stisku kl�esy prohl�� a vr��\p TRUE v p�ad� e
    byla ud�ost obslouena.
    \param event Objekt ud�osti.
    \return P�nak obslouen�ud�osti. */
    virtual SbBool processSoKeyboardEvent(const SoKeyboardEvent * const event);
    /** Callback ud�ost�stisku tla�tka myi.
    Zpracuje ud�ost stisku tla�tka myi prohl�� a vr��\p TRUE v p�ad�
    e byla ud�ost obslouena.
    \param event Objekt ud�osti.
    \return P�nak obslouen�ud�osti. */
    virtual SbBool processSoMouseButtonEvent(const SoMouseButtonEvent * const
      event);
    /* Datove polozky. */
    /// Cytlivost myi.
    float sensitivity;
    /// P�nak invertovan�myi.
    SbBool invert;
  private:
    /** Spole���st konstruktor.
    Provede inicializace prohl�� spole��jak pro priv�n� tak pro veejn
    konstruktor.
    \param buildNow P�nak vytvoen�okna prohl��. */
    void constructorCommon(SbBool buildNow);
    /** Vr��sted okna prohl��.
    Zjist�a vr��sted okna prohl�� v pixelech relativn�od po�tku okna.
    \return Sted okna prohl��. */
    SbVec2s getCenter() const;
    /** Vr��pozici okna prohl��.
    Zjist�a vr��pozici okna prohl�� v pixelech relativn�od po�tku rodi�
    okna prohl��.
    \return Pozice okna prohl��. */
    SbVec2s getPosition() const;
    /** Nastav�pozici kurzoru prohl��.
    Nastav�pozici kurzoru prohl�� na hodnotu \p position absolutn�od
    po�tku obrazovky.
    \param position Nov�pozice kurzoru prohl��. */
    void setCursorPosition(const SbVec2s position);
    /** Vr��pozici kurzoru prohl��.
    Vr��hodnotu pozice kurzoru prohl�� absolutn�od po�tku obrazovky.
    \return Pozice kurzoru prohl��.  */
    SbVec2s getCursorPosition();
};

#endif
