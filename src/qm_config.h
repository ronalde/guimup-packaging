/*
*  qm_config.h
*  QUIMUP configuration class
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

#ifndef GM_CONFIG_H
#define GM_CONFIG_H

#include <stdio.h>
#include <QSettings>

class qm_config
{
public:
	qm_config();
	virtual ~qm_config ();
	
	QString version; 
	// server
	bool quitMPD_onquit;
	QString onquit_mpd_command;
	bool startMPD_onstart;
	QString onstart_mpd_command;
	bool auto_connect;
	int profile;
	
	QString MPD_host_0;
	int MPD_port_0;
	QString MPD_password_0;
	
	QString Profilename_1;
	QString MPD_host_1;
	int MPD_port_1;
	QString MPD_password_1;
    QString MPD_musicdir_1;
	
	QString Profilename_2;
	QString MPD_host_2;
	int MPD_port_2;
	QString MPD_password_2;
    QString MPD_musicdir_2;
	
	QString Profilename_3;
	QString MPD_host_3;
	int MPD_port_3;
	QString MPD_password_3;
    QString MPD_musicdir_3;
	
	QString Profilename_4;
	QString MPD_host_4;
	int MPD_port_4;
	QString MPD_password_4;
    QString MPD_musicdir_4;
	
	QString mpd_musicpath;  	// not saved
	int mpd_musicpath_status;  	// not saved
	bool mpd_op_listall_allowed;// not saved
	bool mpd_op_disble_allowed; // not saved
	bool mpd_op_enable_allowed; // not saved	
	bool mpd_update_allowed; 	// not saved
	bool mpd_deleteid_allowed; 	// not saved
	bool mpd_save_allowed;  	// not saved
	bool mpd_single_allowed; 	// not saved
	bool mpd_consume_allowed; 	// not saved
	bool mpd_xfade_allowed;  	// not saved
	bool mpd_repeat_allowed; 	// not saved
	bool mpd_random_allowed; 	// not saved
	bool mpd_rescan_allowed; 	// not saved
	bool mpd_shuffle_allowed; 	// not saved
	bool mpd_rm_allowed;  		// not saved
	bool mpd_config_allowed;  	// not saved
	bool mpd_socket_conn;  		// not saved
	bool mpd_setrpgain_allowed; // not saved
	//bool mpd_getrpgain_allowed; // not saved
    bool mpd_stats_allowed;  	// not saved
	bool mpd_clear_allowed;  	// not saved
	bool xfade_is_on;  			// not saved
	
	int  xfade_time;
	
	// player'
	QString tag_editor;
	QString art_viewer;
	QString file_manager;
	QString rpgain_mode;
	int scroller_delay;
	int player_X;
	int player_Y;
	bool player_maxmode;
	bool use_trayicon;
	bool use_timeleft;
	// browser
	int browser_X;
	int browser_Y;
	int browser_W;
	int browser_H;
	int browser_L_splitter;
	int browser_R_splitter;
	int browser_libmode;
	int browser_searchmode;
	int browser_selectmode;
	int lib_max_col0;
	int lib_max_col1;
	int lib_min_col0;
	int lib_min_col1;
	int plist_max_col2;
	int plist_max_col3;
	int plist_min_col2;
	int plist_min_col3;
	bool lib_auto_colwidth;
	bool plist_auto_colwidth;	
	bool sort_albums_year;
	bool ignore_leading_the;
	bool disable_albumart;
	bool auto_playfirst;
	bool mark_played;
	QString search_string;
	QString select_string;
	// settings
	int settings_X;
	int settings_Y;
	int settings_W;
	int settings_H;
	QString font_family;
	int maintitle_fontsize;
	int codecinfo_fontsize;
	int time_fontsize;
	int album_fontsize;
	int liblist_fontsize;
	int comment_fontsize;
	QString color_fg;
	QString color_bg;
	bool rb_sysdef_checked;
	bool rb_qmcool_checked;
	bool rb_custom_checked;
	bool show_tooltips;
	bool color_browser;
	bool color_albuminfo;
	
	//  functions
	void  save_config();
	void  load_config();
	void  reset_temps();
};


#endif // QM_CONFIG_H
