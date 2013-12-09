/*
*  qm_trayicon.h
*  QUIMUP MPD system-tray-icon class
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

#ifndef QM_TRAYICON_H
#define QM_TRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QWheelEvent>
#include <QObject>
#include "qm_widget_ids.h"
#include "qm_mpdcom.h"

class qm_trayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	qm_trayIcon(QWidget *);
	void set_tip(QString);
	void set_mode(int);
	void set_connected(qm_mpdCom*, bool);
	virtual ~qm_trayIcon();
	
public slots:


signals:
	void clicked();
	void signal_tray_menu(int);
	
private:
	bool event(QEvent *);
	bool b_mpd_connected;
	qm_mpdCom *mpdCom;
	
	QAction
	*a_previous,
	*a_playpause,
	*a_stop,
	*a_next,
	*a_showhide,
	*a_quit;
	
	QColor m_prevColor;
	
	QPixmap
	pxb_st_noconn,
	pxb_st_paused,
	pxb_st_playing,
	pxb_st_stopped,
	pxb_st_idle,
	pxb_prev,
	pxb_stop,
	pxb_play,
	pxb_pause,
	pxb_next,
	pxb_exit;
	
private slots:
	void action(QSystemTrayIcon::ActivationReason);
	void on_prev_action();
	void on_stop_action();
	void on_play_action();
	void on_next_action();
	void on_quit_action();
	
};
#endif // QM_TRAYICON_H
