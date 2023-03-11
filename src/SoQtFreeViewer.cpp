///////////////////////////////////////////////////////////////////////////////
//  SoTerrain
///////////////////////////////////////////////////////////////////////////////
/// Okno pro voln pohyb ve scen�pomoc�myi a kl�esnice.
/// \file SoQtFreeViewer.cpp
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

#include <SoQtFreeViewer.h>

SOQT_OBJECT_SOURCE(SoQtFreeViewer);

/******************************************************************************
* SoQtFreeViewer - public
******************************************************************************/

SoQtFreeViewer::SoQtFreeViewer(QWidget * parent, const char * name,
  SbBool embed):
  inherited(parent, name, embed, BROWSER, TRUE),
  sensitivity(1.0f), invert(FALSE)
{
  constructorCommon(TRUE);
}

SoQtFreeViewer::~SoQtFreeViewer()
{

}

float SoQtFreeViewer::getMouseSensitivity() const
{
  /* Vraceni citlivosti mysi. */
  return sensitivity;
}

void SoQtFreeViewer::setMouseSensitivity(const float _sensitivity)
{
  /* Nastaveni citlivosti mysi. */
  sensitivity = _sensitivity;
}

SbBool SoQtFreeViewer::getInvertMouse() const
{
  /* Ziskani priznaku invertovane mysi. */
  return invert;
}

void SoQtFreeViewer::setInvertMouse(const SbBool enabled)
{
  /* Nastaveni priznaku invertovane mysi. */
  invert = enabled;
}


/******************************************************************************
* SoQtFreeViewer - protected
******************************************************************************/

SoQtFreeViewer::SoQtFreeViewer(QWidget * parent, const char * const name,
  SbBool embed, SbBool build):
  inherited(parent, name, embed, BROWSER, build),
  sensitivity(1.0f), invert(FALSE)
{
  constructorCommon(build);
}

SbBool SoQtFreeViewer::processSoEvent(const SoEvent * const event)
{
  SbBool result = FALSE;

  /* Zjisteni typu udalosti a zavolani prislusne obsluzne rutiny. */
  if (event->isOfType(SoLocation2Event::getClassTypeId()))
  {
    result = processSoLocation2Event(reinterpret_cast<const SoLocation2Event *>
      (event));
  }
  else if (event->isOfType(SoKeyboardEvent::getClassTypeId()))
  {
    result = processSoKeyboardEvent(reinterpret_cast<const SoKeyboardEvent *>
      (event));
  }
  else if (event->isOfType(SoMouseButtonEvent::getClassTypeId()))
  {
    result = processSoMouseButtonEvent(reinterpret_cast<const SoMouseButtonEvent *>
      (event));
  }

  if (!result)
  {
    setViewing(FALSE);
    result = inherited::processSoEvent(event);
    setViewing(TRUE);
  }

  return result;
}

SbBool SoQtFreeViewer::processSoLocation2Event(const SoLocation2Event *
  const _event)
{
  SoCamera * camera = getCamera();

  /* Zjisteni zmeny pozice kurzoru. */
  SbVec2s position = getCursorPosition();
  static SbVec2s old_position = position;
  SbVec2s diff = old_position - position;

  float rotation_x = sensitivity * 0.001f * diff[1];
  rotation_x = invert ? -rotation_x : rotation_x;
  float rotation_z = sensitivity * 0.001f * diff[0];

  /* Rotace v X ose. */
  camera->orientation = SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), rotation_x) *
    camera->orientation.getValue();

  /* Rotace v Z ose. */
  camera->orientation = camera->orientation.getValue() *
    SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), rotation_z);

  old_position = getPosition() + getCenter();
  setCursorPosition(old_position);
  return TRUE;
}

SbBool SoQtFreeViewer::processSoKeyboardEvent(const SoKeyboardEvent *
  const event)
{
  SoCamera * camera = getCamera();
  SbVec3f old_position = camera->position.getValue();
  SbRotation old_orientation = camera->orientation.getValue();

  if (SO_KEY_PRESS_EVENT(event, ESCAPE))
  {
    SoQt::exitMainLoop();
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, ENTER))
  {
    if (event->wasAltDown())
    {
      setFullScreen(!isFullScreen());
      return TRUE;
    }
  }
  else if (SO_KEY_PRESS_EVENT(event, A))
  {
    SbVec3f diff_position(-0.01f, 0.0f, 0.0f);
    old_orientation.multVec(diff_position, diff_position);
    camera->position = old_position + diff_position;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, D))
  {
    SbVec3f diff_position(0.01f, 0.0f, 0.0f);
    old_orientation.multVec(diff_position, diff_position);
    camera->position = old_position + diff_position;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, Q))
  {
    SbVec3f diff_position(0.0f, -0.01f, 0.0f);
    old_orientation.multVec(diff_position, diff_position);
    camera->position = old_position + diff_position;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, E))
  {
    SbVec3f diff_position(0.0f, 0.01f, 0.0f);
    old_orientation.multVec(diff_position, diff_position);
    camera->position = old_position + diff_position;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, W))
  {
    SbVec3f diff_position(0.0f, 0.0f, -0.01f);
    old_orientation.multVec(diff_position, diff_position);
    camera->position = old_position + diff_position;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, S))
  {
    SbVec3f diff_position(0.0f, 0.0f, 0.01f);
    old_orientation.multVec(diff_position, diff_position);
    camera->position = old_position + diff_position;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, UP_ARROW))
  {
    SbRotation diff_orientation = SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), 0.02f);
    camera->orientation = diff_orientation * old_orientation;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, DOWN_ARROW))
  {
    SbRotation diff_orientation = SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), -0.02f);
    camera->orientation = diff_orientation * old_orientation;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, LEFT_ARROW))
  {
    SbRotation diff_orientation = SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), 0.02f);
    camera->orientation =  old_orientation * diff_orientation;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, RIGHT_ARROW))
  {
    SbRotation diff_orientation = SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -0.02f);
    camera->orientation = old_orientation * diff_orientation;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, PAGE_UP))
  {
    SbRotation diff_orientation = SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), 0.02f);
    camera->orientation =  diff_orientation * old_orientation;
    return TRUE;
  }
  else if (SO_KEY_PRESS_EVENT(event, PAGE_DOWN))
  {
    SbRotation diff_orientation = SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), -0.02f);
    camera->orientation =  diff_orientation * old_orientation;
    return TRUE;
  }

  return FALSE;
}

SbBool SoQtFreeViewer::processSoMouseButtonEvent(const SoMouseButtonEvent *
  const event)
{
  return FALSE;
}

/******************************************************************************
* SoQtFreeViewer - private
******************************************************************************/

void SoQtFreeViewer::constructorCommon(SbBool buildNow)
{
  if (buildNow)
  {
    QWidget * widget = buildWidget(getParentWidget());
    setBaseWidget(widget);
  }

  setComponentCursor(SoQtCursor::getBlankCursor());
}

SbVec2s SoQtFreeViewer::getCenter() const
{
  /* Ziskani stredu okna relativne. */
  QWidget * widget = getParentWidget();
  return SbVec2s(widget->width() / 2, widget->height() / 2);
}

SbVec2s SoQtFreeViewer::getPosition() const
{
  /* Ziskani pocatku okna vuci obrazovce. */
  QWidget * widget = getParentWidget();
  QPoint position = widget->pos();
  return SbVec2s(position.x(), position.y());
}

void SoQtFreeViewer::setCursorPosition(const SbVec2s position)
{
  /* Nastaveni absolutni pozice kurzoru. */
  QCursor::setPos(position[0], position[1]);
}

SbVec2s SoQtFreeViewer::getCursorPosition()
{
  /* Ziskani absolutni pozice kurzoru. */
  QPoint position = QCursor::pos();
  return SbVec2s(position.x(), position.y());
}
