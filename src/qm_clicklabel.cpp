/*
*  qm_clicklabel.cpp
*  QUIMUP clickable label class
*  © 2008-2013 Johan Spee
*
*  This file is part of Quimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  QUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "qm_clicklabel.h"
#include <qevent.h>

clickLabel::clickLabel (QWidget *parent)
{
    setParent(parent);
}

clickLabel::~clickLabel()
{}


void clickLabel::mouseReleaseEvent (QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton )
    {
        emit clicked();
        emit clicked(e);
    }
}


void clickLabel::wheelEvent ( QWheelEvent * e)
{
    const int numSteps = (e->delta() / 8) / 15;
    emit scrolled(numSteps);
}


void clickLabel::keyReleaseEvent (QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        emit clicked();
    }
}
