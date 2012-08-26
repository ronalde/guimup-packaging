/*
 *  gm_config.cc
 *  GUIMUP configuration class
 *  (c) 2008 Johan Spee
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
	
	// server related default values
	set_bool("QuitMPD_onQuit",			false);
	set_string("MPD_onQuit_command",	"mpd --kill");
	set_bool("StartMPD_onStart",		true);
	set_string("MPD_onStart_command",	"mpd");
	set_bool("AutoConnect",				true);
	set_bool("OverrideMPDconf",			false);
	set_string("MPD_Host",				"localhost");
	set_int("MPD_Port",					6600);
	set_string("MPD_Password",			"");
	set_string("MPD_MusicPath",			"");
	set_string("MPD_PlaylistPath",		"");
	// 'player' related default values
	set_string("AlbumArt_File",			"unameit");
	set_int("Scroller_Delay",			60);
	set_int("PlayerWindow_Xpos",		300);
	set_int("PlayerWindow_Ypos",		200);
	set_bool("PlayerWindow_Max",		true);
	set_bool("use_TrayIcon",	 		true);
	set_bool("use_TimeRemaining",		false);
	// 'tracks window' default values
	set_int("TracksWindow_Xpos",		300);
	set_int("TracksWindow_Ypos",		200);
	set_int("TracksWindow_W",			600);
	set_int("TracksWindow_H",			360);
	set_int("TracksWindow_panePos",		160);
	set_int("Tracks_DBmode",			0);
	// 'settings window' default values
	set_int("SettingsWindow_Xpos",		300);
	set_int("SettingsWindow_Ypos",		200);
	// default font & sizes
	set_string("Font_Family",			"Sans, Helvetica");	
	set_int("Scroller_Fontsize",		12);
	set_int("TrackInfo_Fontsize",		10);
	set_int("Time_Fontsize",			12);
	set_int("Album_Fontsize",			11);
	set_int("Tracks_Fontsize",			12);
	// tooltips
	set_bool("show_ToolTips",			true);
	// check if the config file exists
	if (!check_config_file())
	{
		if (!save_config())
			cout << "Config: file not found and could not be created." << endl;
		else
			cout << "Config: file not found, but was created."<< endl;
	}
	else
		cout << "Config: using " << path_file << endl;
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
void gm_Config::set_string(ustring key, ustring value)
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
void gm_Config::set_int(ustring key, int value)
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
void gm_Config::set_bool(const ustring key, bool value)
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

			set_string(key, value);
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
ustring gm_Config::get_string(ustring key)
{
	ustring result = "";
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
		cout << "Config: string \"" << key << "\" is unknown" << endl;
    return result;
}

// Get the int value of "key"
int gm_Config::get_int(ustring key)
{
	int result = -1;
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
		cout << "Config: int \"" << key << "\" is unknown" << endl;
    return result;
}

// Get the boolean value of "key"
bool gm_Config::get_bool(ustring key)
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
		cout << "Config: bool \"" << key << "\" is unknown" << endl;
    return result;
}


gm_Config::~gm_Config()
{

}
