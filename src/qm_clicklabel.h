/*
*  qm_clicklabel.h
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

#ifndef QM_CLICKLABEL
#define QM_CLICKLABEL

#include <qlabel.h>
#include <qevent.h>

class clickLabel : public QLabel
{
	Q_OBJECT

public:
	clickLabel( QWidget * parent = 0);
	virtual ~clickLabel();
	
signals:
	void clicked();
	void clicked(QMouseEvent *e);
	void scrolled(int);
	
protected:
	void mouseReleaseEvent ( QMouseEvent *e );
	void keyReleaseEvent ( QKeyEvent *e );
	void wheelEvent ( QWheelEvent * e);
};

#endif // QM_CLICKLABEL
