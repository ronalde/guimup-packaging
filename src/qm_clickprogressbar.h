/*
*  qm_clickprogressbar.h
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

#ifndef QM_CPROGRESSB
#define QM_CPROGRESSB

#include <QSlider>
#include <qevent.h>

class qm_clickprogressbar : public QSlider
{
	Q_OBJECT

public:
	qm_clickprogressbar(QWidget *parent = 0);
	virtual ~qm_clickprogressbar();
	
protected:
	void keyPressEvent ( QKeyEvent * e);
	void keyReleaseEvent ( QKeyEvent * e);
	void mouseReleaseEvent ( QMouseEvent *e );
	void mousePressEvent ( QMouseEvent *e );	

signals:
	void clicked();
	void clicked(QMouseEvent *e);
	void pressed();
	void pressed(QMouseEvent *e);
	void scrolled(int);
};

#endif // QM_CPROGRESSB
