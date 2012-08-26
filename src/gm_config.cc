/*
 *  gm_config.cc
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

#include "gm_config.h"


gm_Config::gm_Config()
{
	// path to the config file
    ustring homeDir = getenv("HOME");
	if (homeDir.rfind("/") != homeDir.length()-1)
		homeDir += "/";
    path_file = homeDir + ".config/guimup";
	bool b_no_config = false;

	// ckeck if ~/.config/guimup exists
	if (!Glib::file_test(path_file.data(),Glib::FILE_TEST_EXISTS))
	{
		b_no_config = true;
		ustring command = "mkdir " + path_file;
		system(command.data());
		if(!Glib::file_test(path_file.data(),Glib::FILE_TEST_EXISTS))
			cout << "Faild to create ~/.config/guimup" << endl;
		else
			cout << "Created ~/.config/guimup" << endl;
		run_intro_dialog();		
	}

	path_file += "/guimup.conf";
	
	// check if config file exists
	if (!b_no_config && !Glib::file_test(path_file.data(),Glib::FILE_TEST_EXISTS))
	{	
		b_no_config = true;			
		run_intro_dialog();
	}

	if (!b_no_config)
	{
		cout << "Using " << path_file << endl;
		if (!load_config())
		{
			cout << "Config: file could not be opened." << endl;
			b_no_config = true;
		}
	}

	// not saved
	MPD_port_0 = 0;
	MPD_host_0 = "";
	MPD_password_0 = "";
	MPD_configpath_0 = "";
	MPD_Musicpath = "";
	MPD_Playlistpath = "";
	mpd_socket_conn = false;
	mpd_outputs_allowed = false;
    mpd_update_allowed = false;
    mpd_deleteid_allowed = false;
    mpd_save_allowed = false;
    mpd_single_allowed = false;
    mpd_consume_allowed = false;
    mpd_xfade_allowed = false;
	mpd_rpgain_allowed = false;
    mpd_repeat_allowed = false;
    mpd_random_allowed = false;
    mpd_rescan_allowed = false;
    mpd_shuffle_allowed = false;
    mpd_rm_allowed = false;
	mpd_stats_allowed = false;
	mpd_clear_allowed = false;
	mpd_local_features = false;
	
	// server
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
	if (b_no_config | !get("MPD_profile", MPD_profile))
		MPD_profile = 0;
	if (b_no_config | !get("MPD_port_1", MPD_port_1))
		MPD_port_1 = 6600;
    if (b_no_config ||!get("MPD_host_1", MPD_host_1))
		MPD_host_1 = "localhost";
	if (b_no_config ||!get("MPD_name_1", MPD_name_1))
		MPD_name_1 = "default";
    if (b_no_config ||!get("MPD_password_1", MPD_password_1))
		MPD_password_1 = "";
    if (b_no_config ||!get("MPD_configpath_1", MPD_configpath_1))
		MPD_configpath_1 = "/home/user/.mpd/mpd.conf";
	if (b_no_config | !get("MPD_port_2", MPD_port_2))
		MPD_port_2 = 6600;
    if (b_no_config ||!get("MPD_host_2", MPD_host_2))
		MPD_host_2 = "/home/user/.mpd/socket";
	if (b_no_config ||!get("MPD_name_2", MPD_name_2))
		MPD_name_2 = "socket";
    if (b_no_config ||!get("MPD_password_2", MPD_password_2))
		MPD_password_2 = "";
    if (b_no_config ||!get("MPD_configpath_2", MPD_configpath_2))
		MPD_configpath_2 = "/home/user/.mpd/mpd.conf";
	if (b_no_config | !get("MPD_port_3", MPD_port_3))
		MPD_port_3 = 0;
    if (b_no_config ||!get("MPD_host_3", MPD_host_3))
		MPD_host_3 = "";
	if (b_no_config ||!get("MPD_name_3", MPD_name_3))
		MPD_name_3 = "empty";
    if (b_no_config ||!get("MPD_password_3", MPD_password_3))
		MPD_password_3 = "";
    if (b_no_config ||!get("MPD_configpath_3", MPD_configpath_3))
		MPD_configpath_3 = ""; 
	// player 
	if (b_no_config | !get("Tag_Editor", Tag_Editor))
		Tag_Editor = "kid3-qt";
	if (b_no_config | !get("Image_Viewer", Image_Viewer))
		Image_Viewer = "eog";
	if (b_no_config | !get("File_Manager", File_Manager))
		File_Manager = "nautilus";	
	if (b_no_config | !get("Scroller_Delay", Scroller_Delay))
		Scroller_Delay = 60;
	if (b_no_config | !get("color_hue", color_hue))
		color_hue = 204;
	if (b_no_config | !get("color_saturation", color_saturation))
		color_saturation = 20;
	if (b_no_config | !get("color_value", color_value))
		color_value = 50;
	if (b_no_config | !get("playerWindow_Xpos", playerWindow_Xpos))
		playerWindow_Xpos = 100;
	if (b_no_config | !get("playerWindow_Ypos", playerWindow_Ypos))
		playerWindow_Ypos = 100;
	if (b_no_config | !get("playerWindow_Max", playerWindow_Max))
		playerWindow_Max = true;
	if (b_no_config | !get("use_TrayIcon", use_TrayIcon))
		use_TrayIcon = true;
	if (b_no_config | !get("use_TimeRemaining", use_TimeRemaining))
		use_TimeRemaining = false;
	if (b_no_config | !get("disable_Albumart", disable_Albumart))
		disable_Albumart = false;
	// browser
	if (b_no_config | !get("browserWindow_Xpos", browserWindow_Xpos))
		browserWindow_Xpos = 100;
	if (b_no_config | !get("browserWindow_Ypos", browserWindow_Ypos))
		browserWindow_Ypos = 100;
	if (b_no_config | !get("browserWindow_W", browserWindow_W))
		browserWindow_W = 600;
	if (b_no_config | !get("browserWindow_H", browserWindow_H))
		browserWindow_H = 400;
	if (b_no_config | !get("browserWindow_panePos", browserWindow_panePos))
		browserWindow_panePos = 300;
	if (b_no_config | !get("browser_LibraryMode", browser_LibraryMode))
		browser_LibraryMode = 0;
	if (b_no_config | !get("browser_SearchMode", browser_SearchMode))
		browser_SearchMode = 0;	
	if (b_no_config | !get("browser_SelectMode", browser_SelectMode))
		browser_SelectMode = 0;
	if (b_no_config | !get("browser_SearchString", browser_SearchString))
		browser_SearchString = "";	
	if (b_no_config | !get("lib_ignore_leading_the", lib_ignore_leading_the))
		lib_ignore_leading_the = true;
	if (b_no_config | !get("lib_sort_albums_byear", lib_sort_albums_byear))
		lib_sort_albums_byear = false;
	if (b_no_config | !get("pList_mark_played", pList_mark_played))
		pList_mark_played = true;
	if (b_no_config | !get("pList_new_startplaying", pList_new_startplaying))
		pList_new_startplaying = true;
	if (b_no_config | !get("pList_fixed_columns", pList_fixed_columns))
		pList_fixed_columns = true;
	if (b_no_config | !get("lib_fixed_columns", lib_fixed_columns))
		lib_fixed_columns = true;	
	if (b_no_config | !get("pList_artist_width", pList_artist_width))
		pList_artist_width = 100;
	if (b_no_config | !get("pList_title_width", pList_title_width))
		pList_title_width = 180;
	if (b_no_config | !get("pList_albumn_width", pList_albumn_width))
		pList_albumn_width = 140;
	if (b_no_config | !get("lib_column0_width", lib_column0_width))
		lib_column0_width = 120;
	if (b_no_config | !get("lib_column1_width", lib_column1_width))
		lib_column1_width = 120;	
	// settings
	if (b_no_config | !get("settingsWindow_Xpos", settingsWindow_Xpos))
		settingsWindow_Xpos = 100;
	if (b_no_config | !get("settingsWindow_Ypos", settingsWindow_Ypos))
		settingsWindow_Ypos = 100;
	// fontsizes
	if (b_no_config | !get("Scroller_Fontsize", Scroller_Fontsize))
		Scroller_Fontsize = 11;
	if (b_no_config | !get("TrackInfo_Fontsize", TrackInfo_Fontsize))
		TrackInfo_Fontsize = 9;
	if (b_no_config | !get("Time_Fontsize", Time_Fontsize))
		Time_Fontsize = 9;
	if (b_no_config | !get("Album_Fontsize", Album_Fontsize))
		Album_Fontsize = 9;
	if (b_no_config | !get("browser_Fontsize", browser_Fontsize))
		browser_Fontsize = 10;
	if (b_no_config | !get("xfade_time", xfade_time))
		xfade_time = 0;
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

// Add a bool item
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
		item.key = key;
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
	    while (!conffile.eof() )
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


bool gm_Config::save_config()
{
	// server
	set("QuitMPD_onQuit", QuitMPD_onQuit);
	set("MPD_onQuit_command", MPD_onQuit_command);
	set("StartMPD_onStart", StartMPD_onStart);
	set("MPD_onStart_command", MPD_onStart_command);
	set("AutoConnect",AutoConnect);
	set("MPD_profile", MPD_profile);
	set("MPD_port_1", MPD_port_1);
    set("MPD_host_1", MPD_host_1);
	set("MPD_name_1", MPD_name_1);
    set("MPD_password_1", MPD_password_1);
    set("MPD_configpath_1", MPD_configpath_1);
	set("MPD_port_2", MPD_port_2);
    set("MPD_host_2", MPD_host_2);
	set("MPD_name_2", MPD_name_2);
    set("MPD_password_2", MPD_password_2);
    set("MPD_configpath_2", MPD_configpath_2);
	set("MPD_port_3", MPD_port_3);
    set("MPD_host_3", MPD_host_3);
	set("MPD_name_3", MPD_name_3);
    set("MPD_password_3", MPD_password_3);
    set("MPD_configpath_3", MPD_configpath_3);
	// player
	set("Tag_Editor", Tag_Editor);
	set("Image_Viewer", Image_Viewer);
	set("File_Manager", File_Manager);
	set("Scroller_Delay", Scroller_Delay);
	set("color_hue", color_hue);
	set("color_saturation", color_saturation);
	set("color_value", color_value);	
	set("playerWindow_Xpos", playerWindow_Xpos);
	set("playerWindow_Ypos", playerWindow_Ypos);
	set("playerWindow_Max", playerWindow_Max);
	set("use_TrayIcon", use_TrayIcon);
	set("use_TimeRemaining", use_TimeRemaining);
	set("disable_Albumart", disable_Albumart);
	// browser
	set("browserWindow_Xpos", browserWindow_Xpos);
	set("browserWindow_Ypos", browserWindow_Ypos);
	set("browserWindow_W", browserWindow_W);
	set("browserWindow_H", browserWindow_H);
	set("browserWindow_panePos", browserWindow_panePos);
	set("browser_LibraryMode", browser_LibraryMode);
	set("browser_SearchMode", browser_SearchMode);
	set("browser_SelectMode", browser_SelectMode);
	set("browser_SearchString", browser_SearchString);
	set("lib_ignore_leading_the", lib_ignore_leading_the);
	set("lib_sort_albums_byear", lib_sort_albums_byear);
	set("pList_mark_played", pList_mark_played);
	set("pList_new_startplaying", pList_new_startplaying);
	set("pList_fixed_columns", pList_fixed_columns);
	set("lib_fixed_columns", lib_fixed_columns);
	set("pList_artist_width", pList_artist_width);
	set("pList_title_width", pList_title_width);
	set("pList_albumn_width", pList_albumn_width);
	set("lib_column0_width", lib_column0_width);
	set("lib_column1_width", lib_column1_width);
	// settings
	set("settingsWindow_Xpos", settingsWindow_Xpos);
	set("settingsWindow_Ypos", settingsWindow_Ypos);
	// fonsizes
	set("Scroller_Fontsize", Scroller_Fontsize);
	set("TrackInfo_Fontsize", TrackInfo_Fontsize);
	set("Time_Fontsize", Time_Fontsize);
	set("Album_Fontsize", Album_Fontsize);
	set("browser_Fontsize", browser_Fontsize);
	set("xfade_time", xfade_time);
	// general
	set("show_ToolTips", show_ToolTips);
	
	bool result = true;
	std::ofstream conffile (path_file.data());
	if (conffile.is_open())
	{
		conffile << "# Guimup configuration file\n";
		conffile << "# Delete to restore defaults\n";
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


void gm_Config::run_intro_dialog()
{
	Gtk::Dialog dlg("Welcome to Guimup", true);
	dlg.add_button("OK", Gtk::RESPONSE_CANCEL);
	dlg.set_default_response (Gtk::RESPONSE_CANCEL);

	ustring message = "&#10;<b>Please read this first!</b>&#10;&#10;&#10;";
	message	+= "To view <span color='#1e5070'><b>album-art</b></span> or open files in <span color='#1e5070'><b>external programs</b></span> (such as a tag editor) both MPD and Guimup must use the SAME \"music directory\".&#10;&#10;";
	message	+= "This directory is set in MPD's configuration-file: make sure Guimup's connection-profile points to the configuration-file that MPD is actually using (usually ~/.mpd/mpd.conf).&#10;&#10;";
	message	+= "Note that the music directory is not accessable to Guimup when MPD runs on a remote computer.&#10;&#10;&#10;";
	message	+= "Guimup will accept <span color='#1e5070'><b>external files</b></span> (files that are not in MPD's database) if it is connected to MPD through a socket: both 'bind_to_address' in MPD's configuration-file and 'host' in Guimup's connection-profile must point to <i>~/.mpd/socket</i>.&#10;&#10;";
	message	+= "External files can be passed as command-line arguments and can be dropped on the playlist.&#10;&#10;&#10;";
	message += "<i>See the \"about\" tab in the settings window for more tips.</i>&#10;&#10;&#10;";
	message += "<span color='#1e5070'><small>This message is only shown once.</small></span>";

	Gtk::Grid content;
	content.set_row_spacing(20);
	content.set_orientation(Gtk::ORIENTATION_VERTICAL);
	Gtk::Label label("", 0.0, 0.5);
	label.set_size_request(250, 250);
	label.set_line_wrap(true);
	label.set_use_markup(true);
	label.set_markup(message);
	dlg.get_content_area()->pack_start(content);
	content.set_border_width(10);
	content.attach(label,0,0,1,1);
	content.show();
	label.show();
	dlg.set_size_request(300, 300);

	dlg.run();	
}


gm_Config::~gm_Config()
{

}
