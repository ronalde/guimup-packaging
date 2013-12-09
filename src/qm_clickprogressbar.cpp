/*
*  qm_clickprogressbar.cpp
*  QUIMUP clickable progressbar class
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

#include "qm_clickprogressbar.h"
#include <qevent.h>

qm_clickprogressbar::qm_clickprogressbar (QWidget *parent)
{
    setParent(parent);
}

void qm_clickprogressbar::mouseReleaseEvent (QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        emit clicked();
        emit clicked(e);
    }
}

void qm_clickprogressbar::mousePressEvent (QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        emit pressed();
        emit pressed(e);
    }
}

void qm_clickprogressbar::keyPressEvent (QKeyEvent * e)
{
	e->ignore();
}

void qm_clickprogressbar::keyReleaseEvent (QKeyEvent * e)
{
	e->ignore();
}

qm_clickprogressbar::~qm_clickprogressbar()
{}
