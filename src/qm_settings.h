/*
*  qm_settings.h
*  QUIMUP settings window
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

#ifndef QM_SETTINGS_H
#define QM_SETTINGS_H

#include <QApplication>
#include <QMainWindow>
#include <QCloseEvent>
#include <QHideEvent>
#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QSettings>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include "qm_config.h"
#include "qm_mpdcom.h"
#include "qm_colorpicker.h"

class qm_settings : public QMainWindow
{
	Q_OBJECT

public:

	qm_settings();
	virtual ~qm_settings();
	
	qm_colorpicker *colorpicker;
	
public slots:
	void set_connected(qm_mpdCom*, bool);
	void set_config(qm_config*);
	
private slots:
	void cancel_changes();
	void apply_settings();
	void reset_fonsizes();
	void set_profile(int);
	void server_connect();
	void show_colorpicker();
	void on_new_customcolors(QColor, QColor);
	
private:
	QWidget *main_widget;
	QVBoxLayout *vbox_all;
	QHBoxLayout *hbox_buttons;
	QPushButton *bt_close;
	QPushButton *bt_apply;
	QTabWidget *tab_widget;
	
	QWidget *tab_con;
	QVBoxLayout *vbox_connect;
	QHBoxLayout *hbox_autoconn;
	QCheckBox *cb_autoConn;
	QComboBox *cm_profiles;
	QLabel *lb_profiles;
	QGroupBox *gbox_connect;
	QGridLayout *grid_connect;
	QLabel *lb_profilename;
	QLineEdit *le_profilename;
	QLabel *lb_passwd;
	QLineEdit *le_pass_text;
	QLabel *lb_port;
	QLineEdit *le_port_text;
	QLabel *lb_host;
	QLineEdit *le_host_text;
    QLabel *lb_musicdir;
    QLineEdit *le_musicdir_text;
	QPushButton *bt_connect;
	QSpacerItem *spacer_tab_con;
	QGroupBox *gbox_status;
	QGridLayout * grid_connect_status;
	QLabel *lb_conn_status_text;
	QLabel *lb_conn_icon;
	QLabel *lb_musicdir_status_text;
	QLabel *lb_musicdir_icon;
	
	QVBoxLayout *vbox_server;
	QWidget *tab_server;
	QGroupBox *gbox_xfade;
	QHBoxLayout *hbox_xfade;
	QLabel *lb_xfade;
	QSpinBox *sb_xfade;
	QGroupBox *gbox_replaygain;
	QHBoxLayout *hbox_replaygain;
	QRadioButton *rb_rpg_off;
	QRadioButton *rb_rpg_track;
	QRadioButton *rb_rpg_album;
	QRadioButton *rb_rpg_auto;
	QGroupBox *gbox_outputs;
	QVBoxLayout *vbox_devices;
	QCheckBox *cb_dev1;
	QCheckBox *cb_dev2;
	QCheckBox *cb_dev3;
	QCheckBox *cb_dev4;
	QCheckBox *cb_dev5;
	QGroupBox *gbox_manage_mpd;
	QGridLayout *grid_manage;
	QVBoxLayout *vbox_killmpd;
	QLabel *lb_killcomm;
	QLineEdit *le_killcommand;
	QVBoxLayout *vbox_startmpd;
	QCheckBox *cb_startserver;
	QLabel *lb_startcomm;
	QLineEdit *le_startcommand;
	QCheckBox *cb_killserver;
	QSpacerItem *spacer_tab_server;
	
	QVBoxLayout *vbox_client;
	QWidget *tab_client;
	QGroupBox *gbox_general;	
	QVBoxLayout *vbox_general;
	QCheckBox *cb_useSysTray;
	QCheckBox *cb_showTips;
	QCheckBox *cb_disable_aart;	
	QGroupBox *gbox_library;
	QVBoxLayout *vbox_library;
	QCheckBox *cb_sortwithyear;
	QCheckBox *cb_ignore_the;
	QCheckBox *cb_lib_auto_colwidth;
	QGroupBox *gbox_playlist;
	QVBoxLayout *vbox_playlist;
	QCheckBox *cb_auto_playfirst;
	QCheckBox *cb_mark_played;
	QCheckBox *cb_plist_auto_colwidth;
	QGroupBox *gbox_ext_progs;
	QGridLayout *grid_extprogs;
	QLineEdit *le_artviewer;
	QLineEdit *le_tageditor;
	QLineEdit *le_filebrowser;
	QLabel *lb_artviewer;
	QLabel *lb_tageditor;
	QLabel *lb_filebrowser;
	QSpacerItem *spacer_tab_client;
	
	QVBoxLayout *vbox_style;
	QWidget *tab_style;
	QGroupBox * gbox_fonts;
	QGridLayout *grid_fonts;
	QSpinBox *sb_titlefont;
	QSpinBox *sb_infofont;
	QSpinBox *sb_timefont;
	QSpinBox *sb_albumfont;
	QSpinBox *sb_commentfont;
	QSpinBox *sb_liblistfont;
	QLabel *lb_titlefont;
	QLabel *lb_infofont;
	QLabel * lb_timefont;
	QLabel *lb_albumfont;
	QLabel *lb_commentfont;
	QLabel *lb_liblistfont;
	QPushButton *bt_fonts_reset;
	QGroupBox *gbox_colors;
	QHBoxLayout *hbox_colors;
	QVBoxLayout *vbox_colorchoice;
	QRadioButton *rb_sysdef;
	QRadioButton *rb_qmcool;
	QRadioButton *rb_custom;
	QCheckBox *cb_color_browser;
	QCheckBox *cb_color_albinfo;
	QVBoxLayout *vbox_colorcustom;
	QSpacerItem *spacer_l;
	QSpacerItem *spacer_r;
	QPushButton *bt_cust_color;
	QVBoxLayout *vbox_colorlabels;
	QLabel *lb_custcolor_main;
	QLabel *lb_custcolor_album;
	QSpacerItem *spacer_tab_style;
	
	QVBoxLayout *vbox_about;
	QWidget *tb_about;
	QHBoxLayout *hbox_about_top;
	QLabel *lb_version;
	QLabel *px_about;
	QTextEdit *txt_about;
	
	bool b_mpd_connected,
	b_profile_readonly,
	dev1_wasChecked,
	dev2_wasChecked,
	dev3_wasChecked,
	dev4_wasChecked,
	dev5_wasChecked,
	b_new_custom_colors;
	
	qm_mpdCom *mpdCom;  // pointer to the parent mpdCom
	qm_config *config; // pointer to the parent config
	
	void setupUI();
	void retranslateUI();
	void get_outputs();
	void apply_fontsizes();
	void apply_colors();
	void set_outputs();
	void set_replaygain_mode();
	
protected:
	void closeEvent(QCloseEvent *);
	void hideEvent(QHideEvent *);
	void showEvent(QShowEvent *);
	
signals:
	void sgnl_fonts();
	void sgnl_colors();
	void sgnl_markplayed();
	void sgnl_plist_auto_cols();
	void sgnl_lib_auto_cols();
};

#endif // QM_SETTINGS_H
