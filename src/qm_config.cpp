/*
*  qm_config.cpp
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

#include "qm_config.h"


qm_config::qm_config()
{
    load_config();
}

void qm_config::reset_temps()
{
    mpd_musicpath = "";
    mpd_musicpath_status = 0;
	mpd_socket_conn = false;
	mpd_op_listall_allowed = false;
	mpd_op_disble_allowed = false;
	mpd_op_enable_allowed = false;
    mpd_update_allowed = false;
    mpd_deleteid_allowed = false;
    mpd_save_allowed = false;
    mpd_single_allowed = false;
    mpd_consume_allowed = false;
    mpd_xfade_allowed = false;
    mpd_repeat_allowed = false;
    mpd_random_allowed = false;
    mpd_rescan_allowed = false;
    mpd_shuffle_allowed = false;
    mpd_rm_allowed = false;
    mpd_config_allowed = false;
    mpd_setrpgain_allowed = false;
    mpd_stats_allowed = false;
	mpd_clear_allowed = false;
	xfade_is_on = false;
}

// Save the options in a config file
void qm_config::save_config()
{
    QSettings sts("coonsden", "quimup");
	sts.setValue("version",            version);
    // server related values
    sts.setValue("quitMPD_onquit",     quitMPD_onquit);
    sts.setValue("onquit_mpd_command", onquit_mpd_command);
    sts.setValue("startMPD_onstart",   startMPD_onstart);
    sts.setValue("onstart_mpd_command",onstart_mpd_command);
    sts.setValue("auto_connect",       auto_connect);
    
    sts.setValue("profile",            profile);
    
    sts.setValue("MPD_host_0",         MPD_host_0);
    sts.setValue("MPD_port_0",         MPD_port_0);
    sts.setValue("MPD_password_0",     MPD_password_0);
    
    sts.setValue("Profilename_1",      Profilename_1);
    sts.setValue("MPD_host_1",         MPD_host_1);
    sts.setValue("MPD_port_1",         MPD_port_1);
    sts.setValue("MPD_password_1",     MPD_password_1);
    sts.setValue("MPD_musicdir_1",     MPD_musicdir_1);
    
    sts.setValue("Profilename_2",      Profilename_2);
    sts.setValue("MPD_host_2",         MPD_host_2);
    sts.setValue("MPD_port_2",         MPD_port_2);
    sts.setValue("MPD_password_2",     MPD_password_2);
    sts.setValue("MPD_musicdir_2",     MPD_musicdir_2);
    
    sts.setValue("Profilename_3",      Profilename_3);
    sts.setValue("MPD_host_3",         MPD_host_3);
    sts.setValue("MPD_port_3",         MPD_port_3);
    sts.setValue("MPD_password_3",     MPD_password_3);
    sts.setValue("MPD_musicdir_3",     MPD_musicdir_3);
    
    sts.setValue("Profilename_4",      Profilename_4);
    sts.setValue("MPD_host_4",         MPD_host_4);
    sts.setValue("MPD_port_4",         MPD_port_4);
    sts.setValue("MPD_password_4",     MPD_password_4);
    sts.setValue("MPD_musicdir_4",     MPD_musicdir_4);
    
    sts.setValue("xfade_time",         xfade_time);
    
    // 'player' related values
    sts.setValue("scroller_delay",     scroller_delay);
    sts.setValue("player_X",        player_X);
    sts.setValue("player_Y",        player_Y);
    sts.setValue("player_maxmode",  player_maxmode);
    sts.setValue("use_trayicon",       use_trayicon);
    sts.setValue("use_timeleft",       use_timeleft);
    sts.setValue("tag_editor",         tag_editor);
    sts.setValue("art_viewer",         art_viewer);
    sts.setValue("file_manager",       file_manager);
    sts.setValue("rpgain_mode",        rpgain_mode);
    // 'browser window' values
    sts.setValue("browser_X",        browser_X);
    sts.setValue("browser_Y",        browser_Y);
    sts.setValue("browser_W",        browser_W);
    sts.setValue("browser_H",        browser_H);
    sts.setValue("browser_L_splitter",  browser_L_splitter);
    sts.setValue("browser_R_splitter",  browser_R_splitter);
    sts.setValue("browser_libmode",     browser_libmode);
    sts.setValue("browser_searchmode",  browser_searchmode);
    sts.setValue("browser_selectmode",  browser_selectmode);
	sts.setValue("lib_max_col0",        lib_max_col0);
	sts.setValue("lib_max_col1",        lib_max_col1);
	sts.setValue("lib_min_col0",        lib_min_col0);
	sts.setValue("lib_min_col1",        lib_min_col1);
	sts.setValue("plist_max_col2",      plist_max_col2);
	sts.setValue("plist_max_col3",      plist_max_col3);
	sts.setValue("plist_min_col2",      plist_min_col2);
	sts.setValue("plist_min_col3",      plist_min_col3);
    sts.setValue("sort_albums_year",	sort_albums_year);
    sts.setValue("lib_auto_colwidth",	lib_auto_colwidth);	
    sts.setValue("plist_auto_colwidth",	plist_auto_colwidth);	
    sts.setValue("ignore_leading_the",  ignore_leading_the);
    sts.setValue("disable_albumart",    disable_albumart);
    sts.setValue("search_string",		search_string);
    sts.setValue("auto_playfirst",		auto_playfirst);
	sts.setValue("mark_played",			mark_played);
    // 'settings window' values
    sts.setValue("settings_X",      settings_X);
    sts.setValue("settings_Y",      settings_Y);
    // font sizes
    sts.setValue("maintitle_fontsize", maintitle_fontsize);
    sts.setValue("codecinfo_fontsize", codecinfo_fontsize );
    sts.setValue("time_fontsize",      time_fontsize);
    sts.setValue("album_fontsize",     album_fontsize);
    sts.setValue("liblist_fontsize",   liblist_fontsize);
    sts.setValue("comment_fontsize",   comment_fontsize);
    sts.setValue("color_fg",           color_fg);
    sts.setValue("color_bg",           color_bg);
    sts.setValue("rb_sysdef_checked",  rb_sysdef_checked);
    sts.setValue("rb_qmcool_checked",  rb_qmcool_checked);
    sts.setValue("rb_custom_checked",  rb_custom_checked);
    sts.setValue("color_browser",      color_browser);
	sts.setValue("color_albuminfo",    color_albuminfo);
    
    // tooltips
    sts.setValue("show_tooltips",      show_tooltips);
    sts.sync();
    if (sts.status() == QSettings::NoError)
        printf ("Saving settings : OK\n");
    else
        printf ("Saving settings : error!\n");
}

// Get the options from the config file
void qm_config::load_config()
{
    QSettings sts("coonsden", "quimup");
    version       	    = sts.value("version",			    "0.0" ).toString();  
    // server related values
    quitMPD_onquit    	= sts.value("quitMPD_onquit",		false ).toBool();
    onquit_mpd_command  = sts.value("onquit_mpd_command",	"mpd --kill").toString();
    startMPD_onstart    = sts.value("startMPD_onstart",		false  ).toBool();
    onstart_mpd_command = sts.value("onstart_mpd_command",	"mpd"  ).toString();
    auto_connect     	= sts.value("auto_connect",			true   ).toBool();
    
    profile    			= sts.value("profile",				0   ).toInt();
    
    MPD_host_0       	= sts.value("MPD_host_0",			"localhost" ).toString();
    MPD_port_0     		= sts.value("MPD_port_0",			6600  ).toInt();
    MPD_password_0   	= sts.value("MPD_password_0",		""   ).toString();
    
    Profilename_1    	= sts.value("Profilename_1",		"Profile 1").toString();
    MPD_host_1       	= sts.value("MPD_host_1",			"localhost" ).toString();
    MPD_port_1     		= sts.value("MPD_port_1",			6600 ).toInt();
    MPD_password_1   	= sts.value("MPD_password_1",		""   ).toString();
    MPD_musicdir_1   	= sts.value("MPD_musicdir_1",		""   ).toString();
    
    Profilename_2    	= sts.value("Profilename_2",		"Profile 2").toString();
    MPD_host_2       	= sts.value("MPD_host_2",			"localhost" ).toString();
    MPD_port_2     		= sts.value("MPD_port_2",			6600 ).toInt();
    MPD_password_2   	= sts.value("MPD_password_2",		""   ).toString();
    MPD_musicdir_2   	= sts.value("MPD_musicdir_2",		""   ).toString();
    
    Profilename_3    	= sts.value("Profilename_3",		"Profile 3").toString();
    MPD_host_3       	= sts.value("MPD_host_3",			"localhost" ).toString();
    MPD_port_3     		= sts.value("MPD_port_3",			6600 ).toInt();
    MPD_password_3   	= sts.value("MPD_password_3",		""   ).toString();
    MPD_musicdir_3   	= sts.value("MPD_musicdir_3",		""   ).toString();
    
    Profilename_4    	= sts.value("Profilename_4",		"Profile 4").toString();
    MPD_host_4       	= sts.value("MPD_host_4",			"localhost" ).toString();
    MPD_port_4     		= sts.value("MPD_port_4",			6600 ).toInt();
    MPD_password_4   	= sts.value("MPD_password_4",		""   ).toString();
    MPD_musicdir_4   	= sts.value("MPD_musicdir_4",		""   ).toString();
    
    xfade_time   		= sts.value("xfade_time",			5    ).toInt();
    // 'player' related values
    scroller_delay    	= sts.value("scroller_delay",		60   ).toInt();
    player_X     	= sts.value("player_X",			300  ).toInt();
    if (player_X < 0)
        player_X = 0;
    player_Y     	= sts.value("player_Y",			200   ).toInt();
    if (player_Y < 0)
        player_Y = 0;
    player_maxmode   = sts.value("player_maxmode",	true  ).toBool();
    use_trayicon     	= sts.value("use_trayicon",			true  ).toBool();
    use_timeleft    	= sts.value("use_timeleft",			false  ).toBool();
    tag_editor    		= sts.value("tag_editor",			"kid3" ).toString();
    art_viewer    		= sts.value("art_viewer",			"gwenview" ).toString();
    file_manager    	= sts.value("file_manager",			"dolphin" ).toString();
    rpgain_mode    		= sts.value("rpgain_mode",			"album" ).toString();
    // 'browser window' values
    browser_X     	= sts.value("browser_X",         300   ).toInt();
    if (browser_X < 0)
        browser_X = 0;
    browser_Y     	= sts.value("browser_Y",         200   ).toInt();
    if (browser_Y < 0)
        browser_Y = 0;
    browser_W      	= sts.value("browser_W",         600   ).toInt();
    browser_H      	= sts.value("browser_H",         360   ).toInt();
    browser_L_splitter  = sts.value("browser_L_splitter",   240   ).toInt();
    browser_R_splitter  = sts.value("browser_R_splitter",   340   ).toInt();
    browser_libmode     = sts.value("browser_libmode",      0     ).toInt();
    browser_searchmode  = sts.value("browser_searchmode",   0     ).toInt();
    browser_selectmode  = sts.value("browser_selectmode",   0     ).toInt();
    lib_max_col0        = sts.value("lib_max_col0",         250   ).toInt();
    lib_max_col1        = sts.value("lib_max_col1",         250   ).toInt();
    lib_min_col0        = sts.value("lib_min_col0",         150   ).toInt();
    lib_min_col1        = sts.value("lib_min_col1",         150   ).toInt();
    plist_max_col2      = sts.value("plist_max_col2",       250   ).toInt();
    plist_max_col3      = sts.value("plist_max_col3",       250   ).toInt();
    plist_min_col2      = sts.value("plist_min_col2",       150   ).toInt();
    plist_min_col3      = sts.value("plist_min_col3",       150   ).toInt();
    sort_albums_year   	= sts.value("sort_albums_year",		true  ).toBool();
    lib_auto_colwidth   = sts.value("lib_auto_colwidth",	true  ).toBool();	
    plist_auto_colwidth = sts.value("plist_auto_colwidth",	true  ).toBool();	
    ignore_leading_the  = sts.value("ignore_leading_the",	true  ).toBool();
    disable_albumart    = sts.value("disable_albumart",		false ).toBool();
    auto_playfirst   	= sts.value("auto_playfirst",		true  ).toBool();
	mark_played     	= sts.value("mark_played",			true  ).toBool();
    search_string    	= sts.value("search_string",		""    ).toString();
    // 'settings window' values
    settings_X  		= sts.value("settings_X",		300  ).toInt();
    if (settings_X < 0)
        settings_X = 0;
    settings_Y  = sts.value("settings_Y",				200  ).toInt();
    if (settings_Y < 0)
        settings_Y = 0;
    // default font sizes
    maintitle_fontsize  = sts.value("maintitle_fontsize",	10  ).toInt();
    codecinfo_fontsize  = sts.value("codecinfo_fontsize",	8   ).toInt();
    time_fontsize     	= sts.value("time_fontsize",		8   ).toInt();
    album_fontsize     	= sts.value("album_fontsize",		8   ).toInt();
    liblist_fontsize    = sts.value("liblist_fontsize",		9   ).toInt();
    comment_fontsize    = sts.value("comment_fontsize",		8   ).toInt();
    color_fg      		= sts.value("color_fg",				"#e4f3fc" ).toString();
    color_bg      		= sts.value("color_bg",				"#505f72" ).toString();
    rb_sysdef_checked 	= sts.value("rb_sysdef_checked",	false  ).toBool();
    rb_qmcool_checked 	= sts.value("rb_qmcool_checked",	true   ).toBool();
    rb_custom_checked 	= sts.value("rb_custom_checked",	false  ).toBool();
    color_browser  		= sts.value("color_browser",		true   ).toBool();
    color_albuminfo  	= sts.value("color_albuminfo",		true   ).toBool();	
    // tooltips
    show_tooltips     	= sts.value("show_tooltips",		true   ).toBool();
    sts.sync();
    if (sts.status() == QSettings::NoError)
        printf ("Getting settings : OK\n");
    else
        printf ("Error getting settings. Using defaults.\n");
}

qm_config::~qm_config()
{}
