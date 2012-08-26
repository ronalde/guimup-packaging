/*
 *  gm_config.h
 *  GUIMUP configuration class
 *  (c) 2008-2009 Johan Spee
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

#include <stdlib.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <iostream>
    using std::cout;
    using std::endl;
#include <string>
#include <fstream>
#include <sstream>
#include <list>


struct confitem
{
    ustring key;
    ustring value;
};

class gm_Config
{
  public:
//  functions
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

	// server related default values
	bool QuitMPD_onQuit;
	ustring MPD_onQuit_command;
	bool StartMPD_onStart;
	ustring MPD_onStart_command;
	bool AutoConnect;
	bool OverrideMPDconf;
	ustring MPD_Host;
	int MPD_Port;
	ustring MPD_Password;
	ustring MPD_MusicPath;
	ustring MPD_PlaylistPath;
	// 'player' related default values
	ustring Tag_Editor;
	ustring Art_Viewer;
	ustring AlbumArt_File;
	int Scroller_Delay;
	int PlayerWindow_Xpos;
	int PlayerWindow_Ypos;
	bool PlayerWindow_Max;
	bool use_TrayIcon;
	bool use_TimeRemaining;
	bool toggle_Player;
	// 'library window' default values
	int libraryWindow_Xpos;
	int libraryWindow_Ypos;
	int libraryWindow_W;
	int libraryWindow_H;
	int libraryWindow_panePos;
	int library_DBmode;
	bool toggle_Library;
	// 'settings window' default values
	int SettingsWindow_Xpos;
	int SettingsWindow_Ypos;
	// default font & sizes
	ustring Font_Family;
	int Scroller_Fontsize;
	int TrackInfo_Fontsize;
	int Time_Fontsize;
	int Album_Fontsize;
	int library_Fontsize;
	// general
	bool show_ToolTips;

  private:
//  functions
    bool  check_config_file();
//  variables
    ustring path_file;
	std::list <confitem> items ;
};


#endif // GM_CONFIG_H
