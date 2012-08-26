/*
 *  gm_config.h
 *  GUIMUP configuration class
 *  (c) 2008-2012 Johan Spee
 *
 *  This file is part of Guimup
 *
 *  GUIMUP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GUIMUP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#ifndef GM_CONFIG_H
#define GM_CONFIG_H

//#include <stdlib.h>
#include <gtkmm/main.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <iostream>
    using std::cout;
    using std::endl;
#include <fstream>	// read file
#include <list>


struct confitem
{
    ustring key;
    ustring value;
};

class gm_Config
{
  public:

    gm_Config();
    virtual ~gm_Config ();

    bool get(ustring, ustring &);
    bool get(ustring, int &);
    bool get(ustring, bool &);
    void set(ustring, ustring);
    void set(ustring, int);
    void set(ustring, bool);
    bool save_config();
    bool load_config();
		
	// server
	bool QuitMPD_onQuit;
	ustring MPD_onQuit_command;
	bool StartMPD_onStart;
	ustring MPD_onStart_command;
	bool AutoConnect;
	int MPD_profile;		
	int MPD_port_1;
	ustring MPD_name_1;
    ustring MPD_host_1;
    ustring MPD_password_1;
    ustring MPD_configpath_1;
	int MPD_port_2;
	ustring	MPD_name_2;
    ustring MPD_host_2;
    ustring MPD_password_2;
    ustring MPD_configpath_2;
	int MPD_port_3;
	ustring	MPD_name_3;
    ustring MPD_host_3;
    ustring MPD_password_3;
    ustring MPD_configpath_3;
							// not saved ->
	int MPD_port_0;
    ustring MPD_host_0;
    ustring MPD_password_0;
    ustring MPD_configpath_0;
	ustring MPD_Musicpath;
	ustring MPD_Playlistpath;
	bool mpd_socket_conn;
	bool mpd_outputs_allowed;
    bool mpd_update_allowed;
    bool mpd_deleteid_allowed;
    bool mpd_save_allowed;
    bool mpd_single_allowed;
    bool mpd_consume_allowed;
    bool mpd_xfade_allowed;
	bool mpd_rpgain_allowed;
    bool mpd_repeat_allowed;
    bool mpd_random_allowed;
    bool mpd_rescan_allowed;
    bool mpd_shuffle_allowed;
    bool mpd_rm_allowed;
	bool mpd_stats_allowed;	
	bool mpd_clear_allowed;	
	bool mpd_local_features;
							// <- not saved
	// player window
	ustring Tag_Editor;
	ustring Image_Viewer;
	ustring File_Manager;
	int Scroller_Delay;
	int PlayerWindow_Xpos;
	int PlayerWindow_Ypos;
	int color_hue;
	int color_saturation;
	int color_value;
	bool PlayerWindow_Max;
	bool use_TrayIcon;
	bool use_TimeRemaining;
	bool disable_Albumart;
	// browser window
	int browserWindow_Xpos;
	int browserWindow_Ypos;
	int browserWindow_W;
	int browserWindow_H;
	int browserWindow_panePos;
	int browser_LibraryMode;
	int browser_SearchMode;
	ustring browser_SearchString;
	int browser_SelectMode;
	bool pList_mark_played;
	bool pList_new_startplaying;	
	bool pList_fixed_columns;
	bool lib_fixed_columns;
	bool lib_ignore_leading_the;
	bool lib_sort_albums_byear;
	int pList_artist_width;
	int pList_title_width;
	int pList_albumn_width;
	int lib_column0_width;
	int lib_column1_width;	
	// settings window
	int SettingsWindow_Xpos;
	int SettingsWindow_Ypos;
	// font-sizes
	int Scroller_Fontsize;
	int TrackInfo_Fontsize;
	int Time_Fontsize;
	int Album_Fontsize;
	int browser_Fontsize;
	int xfade_time;
	// general
	bool show_ToolTips;

  private:

    ustring path_file;
	std::list <confitem> items ;
};


#endif // GM_CONFIG_H
