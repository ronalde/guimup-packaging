/*
 *  gm_config.cc
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

#include "gm_config.h"


gm_Config::gm_Config()
{
	// set up the path to the config file
    ustring homeDir = getenv("HOME");
	if (homeDir.rfind("/") != homeDir.length()-1)
		homeDir += "/";
    path_file = homeDir + ".guimup.conf";

	bool b_no_config = false;
	
	// check if the config file exists
	if (!check_config_file())
	{
		b_no_config = true;
	}
	else
	{
		cout << "Config: using " << path_file << endl;
		if (!load_config())
		{
			cout << "Config: file could not be opened." << endl;
			b_no_config = true;
		}
	}
	
	// server related default values
	if (b_no_config | !get("QuitMPD_onQuit", QuitMPD_onQuit))
		QuitMPD_onQuit = false;
	if (b_no_config | !get("MPD_onQuit_command", MPD_onQuit_command))
		MPD_onQuit_command  =   "mpd --kill";
	if (b_no_config | !get("StartMPD_onStart", StartMPD_onStart))
		StartMPD_onStart = true;
	if (b_no_config | !get("MPD_onStart_command", MPD_onStart_command))
		MPD_onStart_command = "mpd";
	if (b_no_config | !get("AutoConnect",AutoConnect))
		AutoConnect = true;
	if (b_no_config | !get("OverrideMPDconf", OverrideMPDconf))
		OverrideMPDconf = false;
	if (b_no_config | !get("MPD_Host", MPD_Host))
		MPD_Host = "localhost";
	if (b_no_config | !get("MPD_Port", MPD_Port))
		MPD_Port =  6600;
	if (b_no_config | !get("MPD_Password", MPD_Password))
		MPD_Password = "";
	if (b_no_config | !get("MPD_MusicPath", MPD_MusicPath))
		MPD_MusicPath = "";
	if (b_no_config | !get("MPD_PlaylistPath", MPD_PlaylistPath))
		MPD_PlaylistPath = "";
	// 'player' related default 
	if (b_no_config | !get("Tag_Editor", Tag_Editor))
		Tag_Editor = "easytag";
	if (b_no_config | !get("Art_Viewer", Art_Viewer))
		Art_Viewer = "eog";	
	if (b_no_config | !get("AlbumArt_File", AlbumArt_File))
		AlbumArt_File = "";
	if (b_no_config | !get("Scroller_Delay", Scroller_Delay))
		Scroller_Delay = 60;
	if (b_no_config | !get("PlayerWindow_Xpos", PlayerWindow_Xpos))
		PlayerWindow_Xpos = 300;
	if (b_no_config | !get("PlayerWindow_Ypos", PlayerWindow_Ypos))
		PlayerWindow_Ypos = 200;
	if (b_no_config | !get("PlayerWindow_Max", PlayerWindow_Max))
		PlayerWindow_Max = true;
	if (b_no_config | !get("use_TrayIcon", use_TrayIcon))
		use_TrayIcon = true;
	if (b_no_config | !get("use_TimeRemaining", use_TimeRemaining))
		use_TimeRemaining = false;
	if (b_no_config | !get("toggle_Player", toggle_Player))
		toggle_Player = true;
	// 'library window' default values
	if (b_no_config | !get("libraryWindow_Xpos", libraryWindow_Xpos))
		libraryWindow_Xpos = 300;
	if (b_no_config | !get("libraryWindow_Ypos", libraryWindow_Ypos))
		libraryWindow_Ypos = 200;
	if (b_no_config | !get("libraryWindow_W", libraryWindow_W))
		libraryWindow_W = 600;
	if (b_no_config | !get("libraryWindow_H", libraryWindow_H))
		libraryWindow_H = 360;
	if (b_no_config | !get("libraryWindow_panePos", libraryWindow_panePos))
		libraryWindow_panePos = 160;
	if (b_no_config | !get("library_DBmode", library_DBmode))
		library_DBmode = 0;
	if (b_no_config | !get("toggle_Library", toggle_Library))
		toggle_Library = true;	
	// 'settings window' default values
	if (b_no_config | !get("SettingsWindow_Xpos", SettingsWindow_Xpos))
		SettingsWindow_Xpos = 300;
	if (b_no_config | !get("SettingsWindow_Ypos", SettingsWindow_Ypos))
		SettingsWindow_Ypos = 200;
	// default font & sizes
	if (b_no_config | !get("Font_Family", Font_Family))
		Font_Family = "Sans, DejaVu";
	if (b_no_config | !get("Scroller_Fontsize", Scroller_Fontsize))
		Scroller_Fontsize = 11;
	if (b_no_config | !get("TrackInfo_Fontsize", TrackInfo_Fontsize))
		TrackInfo_Fontsize = 9;
	if (b_no_config | !get("Time_Fontsize", Time_Fontsize))
		Time_Fontsize = 11;
	if (b_no_config | !get("Album_Fontsize", Album_Fontsize))
		Album_Fontsize = 10;
	if (b_no_config | !get("library_Fontsize", library_Fontsize))
		library_Fontsize = 11;
	// general
	if (b_no_config | !get("show_ToolTips", show_ToolTips))
		show_ToolTips = true;
	
	if (b_no_config)
	{
		if (save_config())
			cout << "Config: file successfully created." << endl;
		else
			cout << "Config: file could not be created."<< endl;
	}

}


// Check if the config file can be read
bool gm_Config::check_config_file()
{
	std::ifstream conffile (path_file.data());
  	if (conffile.is_open())
    {
   		conffile.close();
		return true;
    }
	else
		return false;
}


// Add a ustring item
void gm_Config::set(ustring key, ustring value)
{
	bool keyfound = false;
	
  	std::list<confitem>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ((*it).key == key)
		{
			(*it).value = value;
			keyfound = true;
			break;
		}
	}
	
	if (!keyfound) // add it
    {
		confitem item;
		item.key   = key;
		item.value = value;
		items.push_back(item);
    }	
}

// Add a int item
void gm_Config::set(ustring key, int value)
{
	bool keyfound = false;

	// convert int to string
	std::string str_value;
	std::ostringstream out;
	out << (int)value;
	str_value = out.str();
	
  	std::list<confitem>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ((*it).key == key)
		{
			(*it).value = str_value;
			keyfound = true;
			break;
		}
	}
	
	if (!keyfound) // add it
    {
		confitem item;
		item.key   = key;
		item.value = str_value;
		items.push_back(item);
    }
}

// Add a string item
void gm_Config::set(const ustring key, bool value)
{
	bool keyfound = false;
	
  	std::list<confitem>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ((*it).key == key)
		{
			if (!value)
				(*it).value = "false";
			else
				(*it).value = "true";
			keyfound = true;
			break;
		}
	}
	
	if (!keyfound) // add it
    {
		confitem item;
		item.key   = key;
		if (value)
			item.value = "true";
		else
			item.value = "false";

		items.push_back(item);
    }	
}

// Load the config file from disk
bool gm_Config::load_config()
{
    bool result = true;
	std::string s_line;
	ustring u_line;
	std::ifstream conffile (path_file.data());
  	if (conffile.is_open())
    {
	    while (! conffile.eof() )
   		{
   			getline (conffile,s_line);
			u_line = s_line;
			// skip empty lines and comments
			if (u_line.empty() || u_line.find("#") == 0)
				  continue;
			ustring key = u_line.substr(0, u_line.find("="));
			ustring value = u_line.substr(u_line.find("=")+1, u_line.length());
			if (value.empty())
				   continue; // use default
			// remove tailing spaces and linefeeds (not if value.empty()!)
			while (value.rfind("\n") == value.length()-1 || value.rfind(" ") == value.length()-1)
				value = value.substr(0, value.length()-1);

			set(key, value);
		}
   		conffile.close();
    }
    else
    {
        cout << "Config: failed to read from \"" << path_file << "\"" << endl;
        result = false;
    }
    return result;
}


// Save the options in a config file
bool gm_Config::save_config()
{
	// server related default values
	set("QuitMPD_onQuit", QuitMPD_onQuit);
	set("MPD_onQuit_command", MPD_onQuit_command);
	set("StartMPD_onStart", StartMPD_onStart);
	set("MPD_onStart_command", MPD_onStart_command);
	set("AutoConnect",AutoConnect);
	set("OverrideMPDconf", OverrideMPDconf);
	set("MPD_Host", MPD_Host);
	set("MPD_Port", MPD_Port);
	set("MPD_Password", MPD_Password);
	set("MPD_MusicPath", MPD_MusicPath);
	set("MPD_PlaylistPath", MPD_PlaylistPath);
	// 'player' related default 
	set("Tag_Editor", Tag_Editor);
	set("Art_Viewer", Art_Viewer);
	set("AlbumArt_File", AlbumArt_File);
	set("Scroller_Delay", Scroller_Delay);
	set("PlayerWindow_Xpos", PlayerWindow_Xpos);
	set("PlayerWindow_Ypos", PlayerWindow_Ypos);
	set("PlayerWindow_Max", PlayerWindow_Max);
	set("use_TrayIcon", use_TrayIcon);
	set("use_TimeRemaining", use_TimeRemaining);
	set("toggle_Player", toggle_Player);
	// 'library window' default values
	set("libraryWindow_Xpos", libraryWindow_Xpos);
	set("libraryWindow_Ypos", libraryWindow_Ypos);
	set("libraryWindow_W", libraryWindow_W);
	set("libraryWindow_H", libraryWindow_H);
	set("libraryWindow_panePos", libraryWindow_panePos);
	set("library_DBmode", library_DBmode);
	set("toggle_Library", toggle_Library);
	// 'settings window' default values
	set("SettingsWindow_Xpos", SettingsWindow_Xpos);
	set("SettingsWindow_Ypos", SettingsWindow_Ypos);
	// default font & sizes
	set("Font_Family", Font_Family);
	set("Scroller_Fontsize", Scroller_Fontsize);
	set("TrackInfo_Fontsize", TrackInfo_Fontsize);
	set("Time_Fontsize", Time_Fontsize);
	set("Album_Fontsize", Album_Fontsize);
	set("library_Fontsize", library_Fontsize);
	// general
	set("show_ToolTips", show_ToolTips);
	
	bool result = true;
	std::ofstream conffile (path_file.data());
	if (conffile.is_open())
	{
		conffile << "# Guimup configuration settings\n";
		conffile << "# Delete this file to restore defaults\n";
		conffile << "\n";		
		std::list<confitem>::iterator it;	
		for (it = items.begin(); it != items.end(); ++it)
		{
			ustring setting = (*it).key + "=" + (*it).value.data() + "\n";
			conffile << setting.data();
		}
        conffile.close();
    }
    else
    {
        cout << "Config: failed to write to \"" << path_file << "\"" << endl;
        result = false;
    }
    return result;
}


// get the ustring value of "key"
bool gm_Config::get(ustring key, ustring &theString)
{
	ustring result;
	bool found = false;
  	std::list<confitem>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ((*it).key == key)
		{
			result = (*it).value;
			found = true;
			break;
		}
	}
	if (!found)
	{
		return false;
	}
	else
	{
		theString = result;
		return true;
	}
}

// Get the int value of "key"
bool gm_Config::get(ustring key, int &theInt)
{
	int result;
	bool found = false;
  	std::list<confitem>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ((*it).key == key)
		{
			std::string str_int = ((*it).value).data();
			std::istringstream input(str_int);
			input >> result;
			found = true;
			break;
		}
	}
	if (!found)
	{
		return false;
	}
	else
	{
		theInt = result;
		return true;
	}
}

// Get the boolean value of "key"
bool gm_Config::get(ustring key, bool &theBool)
{
	bool result = false;
	bool found = false;
  	std::list<confitem>::iterator it;
	for (it = items.begin(); it != items.end(); ++it)
	{
		if ((*it).key == key)
		{
			if ((*it).value == "true")
				result = true;
			else
				result = false;
			found = true;
			break;
		}
	}
	if (!found)
	{
		return false;
	}
	else
	{
		theBool = result;
		return true;
	}
}


gm_Config::~gm_Config()
{

}
