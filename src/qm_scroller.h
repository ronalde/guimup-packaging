/*
*  qm_Scroller.h
*  QUIMUP graphical text scroller
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

#ifndef QM_SCROLLER_H
#define QM_SCROLLER_H

#include <QLabel>
#include <QFont>
#include <QColor>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QEvent>

class qm_Scroller : public QLabel
{
	Q_OBJECT
	
public:
	//  functions
	qm_Scroller(QWidget *parent = 0);
	virtual ~qm_Scroller();
	
	void set_title( QString artist, QString title = "" );
	void set_fast(bool);
	void set_delay( int );
	void setPalette(QPalette);
	void setGeometry(QRect);
	void pause(bool);
	void setFont(const QFont & );
	
protected:

private slots:
	void scrollerstep();
	
private:
	//  functions
	void start_scroll();
	void render();
	void enterEvent ( QEvent * event );
	void leaveEvent ( QEvent * event );
	
	//  variables
	QString new_string;
	QColor  fg_color,
	        bg_color;
	QFont   the_font;
	QLabel *virtual_label;
	bool
		b_pause,
		b_scrolling,
		b_busy,
		b_fast;
	int
		scrollpos,
		scrollstep,
		steptime,
		W_display,
		W_text,
		H_display,
		cR, cG, cB;
	
	QImage
		QImg_fulltext,
		QImg_display;
	
	QTimer *steploop;
};

#endif //  QM_SCROLLER_H
