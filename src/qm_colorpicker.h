/*
*  qm_colorpicker.h
*  QUIMUP window to select custom colors
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

#ifndef QM_COLOR_PICKER
#define QM_COLOR_PICKER

#include <QMainWindow>
#include <QColor>
#include <QLabel>
#include <QGridLayout>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <stdio.h> // printf


class qm_colorpicker : public QMainWindow
{
	Q_OBJECT

public:
	qm_colorpicker(QWidget *parent = 0);
	~qm_colorpicker();
	
public slots:
	void set_colors(QColor, QColor);
	
private slots:
	void set_custom_fg();
	void set_custom_bg();
	void on_cancel();
	void on_ok();
	
private:
	QSlider *sl_H_front;
	QSlider *sl_S_front;
	QSlider *sl_V_front;
	
	QSlider *sl_H_back;
	QSlider *sl_S_back;
	QSlider *sl_V_back;
	
	QLabel *lb_cust_color;
	QColor  bgcolor,
	fgcolor;
	QPushButton
	*bt_cancel,
	*bt_ok;
signals:
	void sgnl_newcolors(QColor, QColor);
};

#endif // QM_COLOR_PICKER
