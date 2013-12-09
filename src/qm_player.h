/*
*  qm_player.h
*  QUIMUP main player window
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

#ifndef QM_PLAYER_H
#define QM_PLAYER_H

#include <QApplication>
#include <QMainWindow>
#include <QCloseEvent>
#include <QHideEvent>
#include <QShowEvent>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QImage>
#include <QPixmap>
#include <QSettings>
#include <QTimer>
#include <QSignalMapper>
#include <QPalette>
#include <QColorGroup>
#include <QDir>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMenu>
#include <QWidgetAction>
#include <stdlib.h>
#include "qm_clicklabel.h"
#include "qm_clickprogressbar.h"
#include "qm_config.h"
#include "qm_mpdcom.h"
#include "qm_trayicon.h"
#include "qm_widget_ids.h"
#include "qm_scroller.h"
#include "qm_settings.h"
#include "qm_browser.h"
#include "qm_songinfo.h"


class qm_player : public QMainWindow
{
    Q_OBJECT
    
public:
    qm_player();
    virtual ~qm_player();
	
	qm_browser *browser_window; 
	
protected:
    void keyPressEvent(QKeyEvent *);
    
public slots:
    void on_shudown();
	void wakeup_call(bool);
    
private slots:
    void on_contextmenu();
    void on_connect_sgnl(bool);
    void set_status(int, QString s = "");
    void on_signal(int);
    void on_new_song();
    void on_new_status(mpd_status *);
    void on_tray_clicked();
    void lock_progress(QMouseEvent *e);
    void unlock_progress();
    void lock_progress();
    void minimice();
    void show_albumart();
    void open_directory();
    void edit_tags();
    void reload_tags();
    void set_colors();
    void set_fonts();
    void on_curentsong_deleted();
    void on_volumedial(int);
 	void dragLeaveEvent(QDragLeaveEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent *);
	
private:
    QWidget *main_widget;
    QVBoxLayout *vbox_all;
    
    QLabel *main_display;
    qm_Scroller *title_scroller;
    QLabel *lb_extension;
    QLabel *lb_kbps;
    QLabel *lb_kHz;
    clickLabel *lb_time;
    QSlider *ds_progress;
    QSpacerItem *spacer;
    
    QLabel *center_widget;
    QHBoxLayout *hbox_center;
    QLabel *lb_albumart;
    
    QVBoxLayout *vbox_cright;
    QLabel *lb_album;
    QLabel *lb_comment;
    
    QHBoxLayout *hbox_bottom;
    QPushButton *bt_prev;
    QPushButton *bt_stop;
    QPushButton *bt_playpause;
    QPushButton *bt_next;
    QPushButton *bt_browser;
    QSlider *vol_slider;
    QPushButton *bt_sizer;
    QPushButton *bt_setts;
    
    qm_settings * settings_window;
    QSignalMapper *button_mapper;
    qm_trayIcon *the_trayicon;
    QTimer *maximizer;
    QTimer *minimizer;
    qm_config *config;
    qm_mpdCom *mpdCom;
	qm_songInfo *current_songinfo;
    
    QString
    current_mpd_dir,
    current_art_path;
    
    int
	current_type,
    current_status,
    current_volume,
	current_songnr,
	kbps_count_down,
    alphacycler,
    albumart_H,
	albumart_W,
    song_total_time,
	song_previous_time,
    cR, cG, cB,
    window_H,
    window_H_max,
    window_W_max;
    
    bool
    b_repeat,
    b_random,
    b_single,
    b_consume,
    b_xfade,
    b_minmax_xfade,
    b_minmax_busy,
    b_mpd_sets_volume,
    b_stream,
    b_mpd_connected,
    b_settings_hidden,
    b_browser_hidden,
    b_nosong,
    b_lock_progress,
    b_really_close,
    b_use_trayicon,
    b_need_newSong,
    b_reshow_browser,
    b_reshow_settings,
	b_skip_khz;
    
    QPixmap
    pxb_albmart,
    pxb_xfading,
    pxb_led_noconn,
    pxb_led_paused,
    pxb_led_playing,
    pxb_led_idle,
    pxb_led_stopped;
    
    QImage
    img_nopic,
    img_stream,
    img_xfade_input;
    
    QIcon
    img_mn_pause,
    img_mn_play;
	
	QMenu * context_menu;  
    QAction
	*a_menuheader,
    *a_openaart,
    *a_edittags,
    *a_loadtags,
    *a_opendir;
    
    void setupUI();
    void retranslateUI();
    void init_vars();
    void init_widgets();
    QString into_time(int);
    void set_albumart(QString, int);
    bool go_find_art(QString &, QString);

protected:
    void closeEvent( QCloseEvent *);
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);
};

#endif // QM_PLAYER_H
