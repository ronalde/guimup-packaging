/*
 *  gm_mpdcom.cc
 *  GUIMUP mpd communicator class
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

#include "gm_mpdcom.h"

gm_mpdCom::gm_mpdCom()
{
    conn = NULL;
    b_reload = true;
	b_connecting = false;
    b_mpdconf_found = false;
    b_statCheckBusy = false;
    b_dbaseUpdating = false;
    b_no_volume = false;
    mpdconf_path = "";
    plist_lenght = 0;
    current_playlist = -1;
    current_status = -10;
    current_volume = -10;
	serverPort = 0;
    serverName = "";
    serverPassword = "";
}


void gm_mpdCom::configure()
{
    bool autodetect = false;
    b_mpdconf_found = false;

	// reset autodetected
    config->MPD_port_0 = 0;
	config->MPD_host_0 = "";
	config->MPD_password_0 = "";
	config->MPD_configpath_0 = "";
	
    switch (config->MPD_profile)
    {
    case 0:
        {
            autodetect = true;
        }
    case 1:
        {
            serverPort = config->MPD_port_1;
            serverName = config->MPD_host_1;
            serverPassword = config->MPD_password_1;
            mpdconf_path = config->MPD_configpath_1;
            break;
        }
    case 2:
        {
            serverPort = config->MPD_port_2;
            serverName = config->MPD_host_2;
            serverPassword = config->MPD_password_2;
            mpdconf_path = config->MPD_configpath_2;
            break;
        }
    case 3:
        {
            serverPort = config->MPD_port_3;
            serverName = config->MPD_host_3;
            serverPassword = config->MPD_password_3;
            mpdconf_path = config->MPD_configpath_3;
            break;
        }
    default:
        break;
    }
    
    if (autodetect)
    {
		ustring homeDir = getenv("HOME");
		if (!str_has_suffix(homeDir,"/"))
		    homeDir += "/";
		
		// try ~/.mpdconf
		mpdconf_path = homeDir + ".mpdconf";
		b_mpdconf_found = Glib::file_test(mpdconf_path.data(),Glib::FILE_TEST_EXISTS);
		if (!b_mpdconf_found)
		{	
			// else try ~/.mpd/mpd.conf
		    mpdconf_path = homeDir + "/.mpd/mpd.conf";
			b_mpdconf_found = Glib::file_test(mpdconf_path.data(),Glib::FILE_TEST_EXISTS);
		    if (!b_mpdconf_found)
			{   
				// else try /etc/mpd.conf
				mpdconf_path = "/etc/mpd.conf";
				b_mpdconf_found = Glib::file_test(mpdconf_path.data(),Glib::FILE_TEST_EXISTS);
			}
		}
	
		if (b_mpdconf_found)
		{
			cout << "Using " << mpdconf_path.data() << " (auto-detected)" << endl;
			config->MPD_configpath_0 = mpdconf_path;
		}
		else
		{
			cout << "Could not locate MPD's config file" << endl;
			cout << "Is MPD installed?" << endl;
			config->MPD_configpath_0 = "?";
		}
	}
    else // not autodetect
    {
        if (!mpdconf_path.empty())
        {
			b_mpdconf_found = Glib::file_test(mpdconf_path.data(),Glib::FILE_TEST_EXISTS);
            if (b_mpdconf_found)
				cout << "Using " << mpdconf_path.data() << " (as configured)" << endl;
            else
                cout << "Configured MPD config file does not exist: " << mpdconf_path.data() << endl;
        }
        else
            cout << "No MPD config file configured! Some features will not be available" << endl;
    }
    
    
    if (autodetect)
    {
        if (b_mpdconf_found)
        {
            serverPort = get_port();
            serverName = get_host();
            if (serverName.empty())
                serverName = "localhost";
            serverPassword = get_password();
        }
        else
        {
            serverPort = 6600;
            serverName = "localhost";
            serverPassword = "";
        }

        config->MPD_port_0 = serverPort;
        config->MPD_host_0 = serverName;
        config->MPD_password_0 = serverPassword;
    }
    
    ustring musicpath = "";
    if (b_mpdconf_found)
    {
        musicpath = get_musicPath();
        if (musicpath.empty())
        {
            cout << "Music path not found in MPD's config file" << endl;
        }
        else
        {		
			if (!Glib::file_test(musicpath.data(),Glib::FILE_TEST_EXISTS))
            {
                cout << "Music path found in MPD's config, but it does not exist!" << endl;
                musicpath = "";
            }
        }
    }    
    config->MPD_Musicpath = musicpath;

    ustring plistpath = "";
    if (b_mpdconf_found)
    {
        plistpath = get_playlistPath();
        if (plistpath.empty())
        {
            cout << "Playlist path not found in MPD's config file" << endl;
        }
        else
        {		

			if (!Glib::file_test(plistpath.data(),Glib::FILE_TEST_EXISTS))
            {
                cout << "Playlist path found in MPD's config, but it does not exist!" << endl;
                plistpath = "";
            }
        }
    }    
    config->MPD_Playlistpath = plistpath;
	
    
    if (musicpath.empty())
        config->mpd_local_features = false;
    else
        config->mpd_local_features = true;
}


bool gm_mpdCom::mpd_connect()
{
    if (conn != NULL)
		mpd_disconnect(false);

	if (!is_mpd_running())
	{
		if (config->StartMPD_onStart)
		{
			cout << "Starting MPD (as configured)" << endl;
			system ( (config->MPD_onStart_command).data() );
			// wait 500 msec
        	usleep(500000);
			if (!is_mpd_running())
			{
				cout << "Start command failed. Trying \"/usr/bin/mpd\"..." << endl;
				system ( "/usr/bin/mpd" );
				usleep(500000);
				if (!is_mpd_running())
				{
					cout << "Could not start MPD. Is it installed?" << endl;
					return false;
				}
			}
		}
		else
		{
			cout << "Cannot connect: MPD is not running" << endl;
			return false;
		}		
	}
    
    // no server, no go.
    if (serverName.empty())
    {
        serverName = "localhost";
        cout << "No host name: using \"localhost\"" << endl;
    }
    
    // port number must be > 1024
    if (!(serverPort > 1024))
    {
        cout << "Invalid port: " <<  serverPort << ": [using 6600]" << endl;
        serverPort = 6600;
    }

	
    conn = mpd_connection_new(serverName.data(), serverPort,30000);
    
    if ( mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS )
    {
        mpd_connection_clear_error(conn);
        if (conn != NULL)
        {
            mpd_connection_free(conn);
            conn = NULL;
        }
    }
    
    if (conn != NULL)
        mpd_response_finish(conn);
        
        
    // Try 4 more times at 0.2 sec intervals
    // (in case MPD is starting up)
    int count = 1;
    while (conn == NULL && count < 5)
    {
        // wait 200 msec
        usleep(200000);
        
        // try again
        conn = mpd_connection_new( serverName.data(), serverPort ,30000);
        if ( mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS )
        {
            mpd_connection_clear_error(conn);
            if (conn != NULL)
            {
                mpd_connection_free(conn);
                conn = NULL;
            }
        }
        
        if (conn != NULL)
            mpd_response_finish(conn);
            
        count++;
    }
    
    if (conn == NULL) // Still no luck
    {
        cout << "Connection failed (tried " << count << " times). Is MPD running?" << endl;
		sgnl_connected.emit(false);
        return false;
    }
    else
    {
        cout << "Connected to MPD on try " << count << endl;
    }
	
	// Let's tickle MPD
	bool b_status_allowed = false;
	ustring command;
    mpd_pair *mp;
    mpd_send_allowed_commands (conn);
	while ( (mp = mpd_recv_command_pair(conn)) != NULL)
    {
        command = (char*)mp->value;
        if (command.compare("status") == 0) 
		{
            b_status_allowed = true;
			mpd_return_pair(conn, mp);
			break;
		}
		mpd_return_pair(conn, mp);
	}
	mpd_response_finish(conn);
	
    if (!serverPassword.empty())
    {
		if (b_status_allowed)
		 cout << "Password provided but not used" << endl;
		else
		{
		    mpd_send_password(conn, serverPassword.data());
		    mpd_response_finish(conn);
		    if ( mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS )
		    {
				ustring errormsg = mpd_connection_get_error_message(conn);
				if (errormsg.empty())
					errormsg = "(unknown)";
		        cout << "Error sending password: " << errormsg.data() << endl;
		        mpd_connection_clear_error(conn);
		    }
		    else
		        cout << "Password was accepted" << endl;
		}
    }
    
    mpd_send_allowed_commands (conn);
    while ( (mp = mpd_recv_command_pair(conn)) != NULL)
    {
        command = (char*)mp->value;
        if (command.compare("status") == 0) 
            b_status_allowed = true;			// statusCheck()
        if (command.compare("enableoutput") == 0)
            config->mpd_outputs_allowed = true; // set_outputs()
        if (command.compare("update") == 0)
            config->mpd_update_allowed = true;  // update_all(), rescan_folder()
        if (command.compare("deleteid") == 0)
            config->mpd_deleteid_allowed = true;// CMD_DEL
        if (command.compare("rm") == 0)
            config->mpd_rm_allowed = true;		// CMD_DPL
        if (command.compare("save") == 0)
            config->mpd_save_allowed = true;	// save_playlist()
        if (command.compare("single") == 0)
            config->mpd_single_allowed = true; 	// set_single()
        if (command.compare("consume") == 0)
            config->mpd_consume_allowed = true; // set_consume()
        if (command.compare("crossfade") == 0)
            config->mpd_xfade_allowed = true;	// set_xfade()
		 if (command.compare("commands") == 0)
            config->mpd_rpgain_allowed = true;	// set_replaygain()
        if (command.compare("repeat") == 0)
            config->mpd_repeat_allowed = true;	// set_repeat()
        if (command.compare("rescan") == 0)
            config->mpd_rescan_allowed = true;	// rescan_folder()
        if (command.compare("random") == 0)
            config->mpd_random_allowed = true;	// set_random()
        if (command.compare("shuffle") == 0)
            config->mpd_shuffle_allowed = true;	// shuffle_list()
		if (command.compare("clear") == 0)
            config->mpd_clear_allowed = true;	// clear_list()
		if (command.compare("stats") == 0)
            config->mpd_stats_allowed = true;	// get_statistics()
        mpd_return_pair(conn, mp);
    }
	mpd_response_finish(conn);
	
    if (!b_status_allowed)
    {
        mpd_disconnect(true);
        cout << "Can't get MPD's status (wrong or missing password?)" << endl;
		cout << "Connection closed." << endl;
		return false;
    }
	
    // else: we're in business!
    
	if (serverName.find("socket") != Glib::ustring::npos) // npos: -1
		config->mpd_socket_conn = true;
	
    sgnl_connected.emit(true);
    
    statusCheck();

    // poll mpd every 200 msec
	statusLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::statusCheck), 200);
    
    return true;
}


bool gm_mpdCom::mpd_reconnect()
{
    if (b_connecting)
        return true;
        
    // try to connect
    b_connecting = true;
    
    conn = mpd_connection_new( serverName.data(), serverPort ,30000);
    
    if ( mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS )
    {
        mpd_connection_clear_error(conn);
        if (conn != NULL)
        {
            mpd_connection_free(conn);
            conn = NULL;
        }
    }
    
    if (conn == NULL)
    {
        b_connecting = false;
        return true;
    }
    
    mpd_response_finish(conn);
    
    // login 
    if (!serverPassword.empty())
    {
        mpd_send_password(conn, serverPassword.data());
        mpd_response_finish(conn);
    }
    
    // connected!
    cout << "Reconnected!" << endl;
	if (reconnectLoop.connected())
		reconnectLoop.disconnect();
    b_connecting = false;
	
    sgnl_connected.emit(true);
    statusCheck();
    // poll mpd every 200 msec
	statusLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::statusCheck), 200);

    return false;
}


ustring gm_mpdCom::get_playlistPath()
{
    ustring result = get_string("playlist_directory");
	if (!result.empty())
	{
		if (result.rfind("/") != result.length()-1)
		result += "/";
	}
    return result;
}


ustring gm_mpdCom::get_host()
{
    ustring result = get_string("bind_to_address");
    return result;
}

int gm_mpdCom::get_port()
{
	int result = 0;
    ustring get = get_string("port");
	std::string str_int = get.data();
	std::istringstream input(str_int);
	input >> result;
	return result;
}


void gm_mpdCom::set_config(gm_Config *conf)
{
    config = conf;
}


void gm_mpdCom::set_sInfo(gm_songInfo *sInf)
{
    current_sInfo = sInf;
}


ustring gm_mpdCom::get_password()
{
	ustring key  = "password";
    ustring result = "";
    if (!b_mpdconf_found)
        return result;

    std::string s_line;

    std::ifstream conffile (mpdconf_path.data());
    if (conffile.is_open())
    {
        while (! conffile.eof() )
        {
            getline (conffile, s_line);
            // skip empty lines and comments
            if (s_line.empty() || s_line.find("#") == 0)
                  continue;
            if (s_line.find(key) == 0)
            {
                // substr refuses to do this in one step
                s_line = s_line.substr(s_line.find("\"")+1, s_line.length());
                s_line = s_line.substr(0, s_line.rfind("@"));
                result = s_line;
                break;
            }
        }
        conffile.close();
    }
    return result;
}


void gm_mpdCom::mpd_disconnect(bool sendsignal)
{
	if (statusLoop.connected())
		statusLoop.disconnect();
	
	if (reconnectLoop.connected())
		reconnectLoop.disconnect();
	
	if (conn != NULL)
	{
		mpd_connection_clear_error(conn);
		mpd_response_finish(conn);
		mpd_connection_free(conn);
        conn = NULL;
	}

	current_playlist = -1;
    plist_lenght = 0;
    current_status = -1;
	
	if (sendsignal)
	{
		current_sInfo->reset();
		config->mpd_socket_conn = false;
		config->mpd_outputs_allowed = false;
    	config->mpd_update_allowed = false;
    	config->mpd_deleteid_allowed = false;
    	config->mpd_save_allowed = false;
    	config->mpd_single_allowed = false;
    	config->mpd_consume_allowed = false;
    	config->mpd_xfade_allowed = false;
		config->mpd_rpgain_allowed = false;
    	config->mpd_repeat_allowed = false;
    	config->mpd_random_allowed = false;
    	config->mpd_rescan_allowed = false;
    	config->mpd_shuffle_allowed = false;
    	config->mpd_rm_allowed = false;
		config->mpd_stats_allowed = false;
		config->mpd_clear_allowed = false;
		config->mpd_local_features = false;
		
		sgnl_connected.emit(false);
	}
}


ustring gm_mpdCom::get_string(ustring key)
{
	ustring result = "";
	if (!b_mpdconf_found)
		return result;
	
    std::string s_line;

    std::ifstream conffile (mpdconf_path.data());
    if (conffile.is_open())
    {
        while (! conffile.eof() )
        {
            getline (conffile, s_line);
            // skip empty lines and comments
            if (s_line.empty() || s_line.find("#") == 0)
                  continue;
			if (s_line.find(key) == 0)
			{
				// substr refuses to do this in one step
				s_line = s_line.substr(s_line.find("\"")+1, s_line.length());
				result = s_line.substr(0, s_line.rfind("\""));
				//result = s_line;
				break;
			}
        }
        conffile.close();
    }

	return result;
}


ustring gm_mpdCom::get_musicPath()
{
    ustring result = get_string("music_directory");
	if (!result.empty())
	{
		if (result.rfind("/") != result.length()-1)
		result += "/";
	}
    return result;
}


bool gm_mpdCom::statusCheck()
{

    // FALSE terminates the loop
    if (!errorCheck("Starting statusCheck"))
        return false;
 
    // TRUE keeps the loop going
    if (b_statCheckBusy)
        return true;
        
    b_statCheckBusy = true;
    
    // Get the current server status
    mpd_send_status(conn);
	mpd_status *servStatus = mpd_recv_status(conn);
    mpd_response_finish(conn);

    if (!errorCheck("mpd_recv_status") || servStatus == NULL)
    {
        b_statCheckBusy = false;
        if (servStatus != NULL && conn != NULL)
        {
            mpd_status_free(servStatus);
            mpd_response_finish(conn);
        }
        return true;
    }
 
    // no supported mixer
    if (mpd_status_get_volume(servStatus) == -1)
    {
        if (!b_no_volume)
        {
            cout << "MPD: No supported mixer found" << endl;
            b_no_volume = true;
        }
    }
    else
        b_no_volume = false;
        
    // database update request has finished
    if (b_dbaseUpdating && mpd_status_get_update_id(servStatus) == 0)
    {
        b_dbaseUpdating = false;
        sgnl_update_ready.emit();
		get_statistics();
    }
    
    // playlist was updated
    if (current_playlist < (int)mpd_status_get_queue_version(servStatus) )
    {
        current_playlist = mpd_status_get_queue_version(servStatus);
        get_playlist();
        // reload the song (streams use this to update song info)
        b_reload = true;
    }

    struct mpd_song *song = NULL;
    // the song changed, or new playlist
    if( (current_sInfo->songID != mpd_status_get_song_id(servStatus))  || b_reload)
    {
        b_reload = false;
        // get the new song.
        mpd_entity * entity = NULL;
        mpd_send_current_song(conn);
        entity = mpd_recv_entity(conn);
        mpd_response_finish(conn);
        
        // empty list, last has finished or first hasn't started
        if (entity != NULL && errorCheck("statuscheck: mpd_send_current_song"))
            song = mpd_song_dup (mpd_entity_get_song(entity));
        
        if (entity != NULL)
            mpd_entity_free(entity);
            
        get_songInfo_from(current_sInfo, song);
        sgnl_newSong.emit();
    }
    
    sgnl_statusInfo.emit(servStatus);
    
    current_status = mpd_status_get_state(servStatus);
    current_volume = mpd_status_get_volume(servStatus);
    
    if (servStatus != NULL)
        mpd_status_free(servStatus);
    if (song != NULL)
        mpd_song_free (song);

    b_statCheckBusy = false;
    return true;
}


bool gm_mpdCom::errorCheck(ustring action)
{
    if (conn == NULL)
    {
        mpd_disconnect(false);
		sgnl_connected.emit(false);
        cout << "Error on " << action.data() << "\": no connection" << endl;
        cout << "Reconnecting..." << endl;
        reconnectLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::mpd_reconnect), 2000);
        return false;
    }
   
    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
    {

		switch (mpd_connection_get_error(conn))
    	{
        	case MPD_ERROR_OOM:
        	{
				cout << "Out of memory" << endl;
				break;
        	}

			case MPD_ERROR_ARGUMENT:
        	{
				cout << "MPD: Function called with an unrecognized or invalid argument" << endl;
				break;
        	}

			case MPD_ERROR_STATE:
        	{
				cout << "MPD: Called function is not available" << endl;
				break;
        	}

			case MPD_ERROR_TIMEOUT:
        	{
				cout << "MPD: Timeout" << endl;
				break;
        	}

			case MPD_ERROR_SYSTEM:
        	{
				cout << "MPD: System error" << endl;
				break;
        	}
				
			case MPD_ERROR_RESOLVER:
        	{
				cout << "MPD: Unknown host" << endl;
				break;
        	}
				
			case MPD_ERROR_MALFORMED:
        	{
				cout << "MPD: Malformed response received" << endl;
				break;
        	}
				
			case MPD_ERROR_CLOSED:
        	{
				cout << "MPD: Connection closed" << endl;
				break;
        	}

			default:
				cout << "MPD: Returned an error code" << endl;
		}

			
		ustring errormsg = mpd_connection_get_error_message(conn);
		if (errormsg.empty())
			errormsg = "(unknown)";
		
        // non-fatal error (no permission etc.).
        if (mpd_connection_get_error(conn) != MPD_ERROR_CLOSED)
        {
			// A "broken pipe" error means the connection is lost (which really is quite fatal)
			
			if (errormsg.compare("Broken pipe") != 0  && errormsg.compare("Connection reset by peer") != 0)
			{
        		cout << "Non fatal error on \"" << action.data() << "\": " << errormsg.data() << endl;
				mpd_connection_clear_error(conn);
				mpd_response_finish(conn);
				return true;
			}
			// else continue as 'fatal'
        }
        
        // A fatal error occurred: disconnnect & reconnect
		mpd_connection_clear_error(conn);
		mpd_disconnect(false);
		sgnl_connected.emit(false);
        cout << "Fatal error on \"" << action.data() << "\": " << errormsg.data() << endl;
		cout << "Reconnecting..." << endl;
        reconnectLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::mpd_reconnect), 2000);
        return false;
    }

    // No problems
    mpd_connection_clear_error(conn);
    return true;
}



void gm_mpdCom::get_songInfo_from(gm_songInfo *sInfo, mpd_song *theSong)
{
	if (sInfo == NULL)
		return;
	
	sInfo->reset();
	
	if (theSong == NULL)
		return;
	
    sInfo->nosong = false;

	if (mpd_song_get_uri(theSong) != NULL)
    	sInfo->file_mpd = mpd_song_get_uri(theSong);

	if (str_has_prefix(sInfo->file_mpd.lowercase(),"http:"))
    {
        sInfo->type = TP_STREAM;
        sInfo->time = 0;
		// path, from root, inc the last "/"
		sInfo->file_dir = (sInfo->file_mpd).substr(0, ((sInfo->file_mpd).rfind("/"))+1);
		// filename only
		sInfo->file_name = (sInfo->file_mpd).substr((sInfo->file_mpd).rfind("/")+1, (sInfo->file_mpd).length());
    }
    else
    {
		if (str_has_prefix(sInfo->file_mpd, "/")) // path from MPD database doesn't have have a leading '/'
		{
			sInfo->type = TP_SONGX;
			// path, from root, inc the last "/"
			sInfo->file_dir = (sInfo->file_mpd).substr(0, ((sInfo->file_mpd).rfind("/"))+1);
			// filename only
			sInfo->file_name = (sInfo->file_mpd).substr((sInfo->file_mpd).rfind("/")+1, (sInfo->file_mpd).length());
		}		
		else
		{
			sInfo->type = TP_SONG;

			// path, from root, inc the last "/"
			sInfo->file_dir = config->MPD_Musicpath + (sInfo->file_mpd).substr(0, ((sInfo->file_mpd).rfind("/"))+1);
			// filename only
			sInfo->file_name = (sInfo->file_mpd).substr((sInfo->file_mpd).rfind("/")+1, (sInfo->file_mpd).length());
		}

		if (mpd_song_get_duration(theSong) > 0)	
           	sInfo->time = mpd_song_get_duration(theSong);
     }

    sInfo->songID = mpd_song_get_id(theSong);
    sInfo->songNr = mpd_song_get_pos(theSong);
    if (mpd_song_get_tag(theSong, MPD_TAG_NAME, 0) != NULL)
       	sInfo->name = mpd_song_get_tag(theSong, MPD_TAG_NAME, 0);
	if (mpd_song_get_tag(theSong, MPD_TAG_ARTIST, 0) != NULL)
       	sInfo->artist = mpd_song_get_tag(theSong, MPD_TAG_ARTIST, 0);
	if (mpd_song_get_tag(theSong, MPD_TAG_TITLE, 0) != NULL)
       	sInfo->title = mpd_song_get_tag(theSong, MPD_TAG_TITLE, 0);
	if (mpd_song_get_tag(theSong, MPD_TAG_ALBUM, 0) != NULL)
       	sInfo->album = mpd_song_get_tag(theSong, MPD_TAG_ALBUM, 0);
	if (mpd_song_get_tag(theSong, MPD_TAG_TRACK, 0) != NULL)
		sInfo->track = fixTrackformat(mpd_song_get_tag(theSong, MPD_TAG_TRACK, 0));
	if (mpd_song_get_tag(theSong, MPD_TAG_DATE, 0) != NULL)
       	sInfo->year = mpd_song_get_tag(theSong, MPD_TAG_DATE, 0);
       sInfo->year = sInfo->year.substr(0, 4); // convert yyyy-mm-dd to yyyy
	if (mpd_song_get_tag(theSong, MPD_TAG_GENRE, 0))
       	sInfo->genre  = mpd_song_get_tag(theSong, MPD_TAG_GENRE, 0);
	if (mpd_song_get_tag(theSong, MPD_TAG_COMMENT, 0) != NULL)
       	sInfo->comment = mpd_song_get_tag(theSong, MPD_TAG_COMMENT, 0);
}

ustring gm_mpdCom::fixTrackformat(ustring fixthis)
{
	// fix xx/xx format
	fixthis =  fixthis.substr(0, fixthis.find("/"));
	// add leading zero
	if (fixthis.length() == 1)
		fixthis = "0" + fixthis;
	
	return fixthis;
}

void gm_mpdCom::set_seek(int percent)
{
    if (conn == NULL)
        return;
    mpd_send_seek_pos(conn, current_sInfo->songNr, percent);
    mpd_response_finish(conn);
    errorCheck("set_seek");
}


void gm_mpdCom::set_volume(int vol)
{
    if (conn == NULL || b_no_volume)
        return;
    mpd_send_set_volume(conn, vol);
    mpd_response_finish(conn);
    errorCheck("set_volume");
}

void gm_mpdCom::volume_up(int i)
{
    if (conn == NULL || b_no_volume)
        return;
    int vol = current_volume + i;
    if (vol > 100)
        vol = 100;
    mpd_send_set_volume(conn, vol);
    mpd_response_finish(conn);
    errorCheck("volume_up");
}

void gm_mpdCom::volume_down(int i)
{
    if (conn == NULL || b_no_volume)
        return;
    int vol = current_volume - i;
    if (vol < 0)
        vol = 0;
    mpd_send_set_volume(conn, vol);
    mpd_response_finish(conn);
    errorCheck("volume_down");
}

void gm_mpdCom::prev()
{
    if (conn == NULL)
        return;
    mpd_send_previous(conn);
    mpd_response_finish(conn);
    errorCheck("mpd_sendPrevCommand");
}

void gm_mpdCom::stop()
{
    if (conn == NULL)
        return;
    mpd_send_stop(conn);
    mpd_response_finish(conn);
    errorCheck("mpd_sendStopCommand");
}

void gm_mpdCom::play(bool reload)
{
    if (conn == NULL)
        return;
    if (reload)
        b_reload = true;
    mpd_send_play(conn);
    mpd_response_finish(conn);
    errorCheck("mpd_sendPlayCommand");
}

void gm_mpdCom::resume()
{
    if (conn == NULL)
        return;
    mpd_send_pause(conn, 0);
    mpd_response_finish(conn);
    errorCheck("mpd_sendPauseCommand_0");
}

void gm_mpdCom::pause()
{
    if (conn == NULL)
        return;
    mpd_send_pause(conn, 1);
    mpd_response_finish(conn);
    errorCheck("mpd_sendPauseCommand_1");
}

void gm_mpdCom::next()
{
    if (conn == NULL)
        return;
    mpd_send_next(conn);
    mpd_response_finish(conn);
    errorCheck("mpd_sendNextCommand");
}


void gm_mpdCom::rescan_folder(ustring file)
{
	if (conn == NULL)
    {
        b_dbaseUpdating = false;
        sgnl_update_ready.emit();
        return;
    }
	
    if (b_dbaseUpdating)
        return;
	
    b_dbaseUpdating = TRUE;       
    if (config->mpd_rescan_allowed)
	{
		sgnl_rescan_started.emit();
        mpd_send_rescan( conn, (const char*)file.c_str() );
	}
    else // second  best
	{
        if (config->mpd_update_allowed)
		{
			sgnl_rescan_started.emit();
            mpd_send_update( conn, (const char*)file.c_str() );
		}
		else
		{
			b_dbaseUpdating = false;
        	sgnl_update_ready.emit();
       		return;
		}
	}
           
    mpd_response_finish(conn);
    if (errorCheck("rescan_folder"))
		 //trigger reloading the song (playlist is updated anyway)
        current_sInfo->reset();
	else
		cout << "Error on database update" << endl;
}

void gm_mpdCom::update_all()
{
    if (conn == NULL || !config->mpd_update_allowed)
    {
        b_dbaseUpdating = false;
        sgnl_update_ready.emit();
        return;
    }
	
	if (b_dbaseUpdating)
		return;
	
    b_dbaseUpdating = TRUE;
    mpd_send_update(conn, (char*)"/");
    mpd_response_finish(conn);
    errorCheck("update_all");
    // when ready: signal from statusloop
}


void gm_mpdCom::set_xfade(int secs)
{
    if (conn == NULL || !config->mpd_xfade_allowed)
        return ;
    mpd_send_crossfade(conn, secs);
    mpd_response_finish(conn);
    errorCheck("set_xfade");
}


int gm_mpdCom::get_xfade()
{
    if (conn == NULL)
        return 0;
        
    mpd_send_status(conn);
    mpd_status * servStatus = mpd_recv_status(conn);
    mpd_response_finish(conn);
    
    if (!errorCheck("get_xfade") || servStatus == NULL)
    {
        if (servStatus != NULL)
        {
            mpd_status_free(servStatus);
            mpd_response_finish(conn);
        }
        return 0;
    }
    else
    {
        int i = mpd_status_get_crossfade(servStatus);
        mpd_status_free(servStatus);
        mpd_response_finish(conn);
        return i;
    }
}


void gm_mpdCom::set_random(bool status)
{
    if (conn == NULL || !config->mpd_random_allowed)
        return;
    mpd_send_random(conn, status);
    mpd_response_finish(conn);
    errorCheck("set_random");
}

void gm_mpdCom::set_repeat(bool status)
{
    if (conn == NULL || !config->mpd_repeat_allowed)
        return;
    mpd_send_repeat(conn, status);
    mpd_response_finish(conn);
    errorCheck("set_repeat");
}


void gm_mpdCom::set_single(bool status)
{
    if (conn == NULL || !config->mpd_single_allowed)
        return;
    mpd_send_single(conn, status);
    mpd_response_finish(conn);
    errorCheck("set_single");
}


void gm_mpdCom::set_consume(bool status)
{
    if (conn == NULL || !config->mpd_consume_allowed)
        return;
    mpd_send_consume(conn, status);
    mpd_response_finish(conn);
    errorCheck("set_consume");
}


void gm_mpdCom::get_statistics()
{
    if (conn == NULL || !config->mpd_stats_allowed)
    {
		sgnl_dbase_statistics.emit(-1,0,0);
        return;
    }
    
    mpd_stats *sData;
    mpd_send_stats(conn);
    sData = mpd_recv_stats(conn);
    mpd_response_finish(conn);
    if (!errorCheck("mpd_recv_stats") || sData == NULL)
    {
        sgnl_dbase_statistics.emit(-1,0,0);
        if (sData != NULL)
            mpd_stats_free(sData);
        return;
    }
                          
	sgnl_dbase_statistics.emit(mpd_stats_get_number_of_artists(sData), mpd_stats_get_number_of_albums(sData), mpd_stats_get_number_of_songs(sData));
    
    if (sData != NULL)
        mpd_stats_free(sData);
}


void gm_mpdCom::get_playlist()
{
    gm_songList newPlayList;
    plist_lenght = 0;
    
    if (conn == NULL)
    {
        sgnl_plistupdate.emit(newPlayList, -10);
        return;
    }
	
    struct mpd_song *song = NULL;
    mpd_entity * entity;
    mpd_send_list_queue_meta(conn);
    while((entity = mpd_recv_entity(conn)))
    {
        if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG)
        {
            song = mpd_song_dup (mpd_entity_get_song(entity));
            gm_songInfo *newSongInfo = new gm_songInfo();
			get_songInfo_from(newSongInfo, song);
            newPlayList.push_back(*newSongInfo);
            plist_lenght++;
        }
        mpd_entity_free(entity);
    }
    mpd_response_finish(conn);
    errorCheck("get_playlist");
    if (song != NULL)
        mpd_song_free (song);
        
    int changedID = get_listChanger();

    sgnl_plistupdate.emit(newPlayList, changedID);
    newPlayList.clear();
}


// which song (ID) caused the playlist to update?
int gm_mpdCom::get_listChanger()
{
    int id = -1;
    if (conn == NULL)
        return id;
        
    mpd_entity *entity = NULL;
    mpd_send_queue_changes_meta(conn, current_playlist-1);
    while (( entity = mpd_recv_entity(conn)) != NULL)
    {
        // get the first that comes along
        if(mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG)
            id = mpd_song_get_id( mpd_entity_get_song(entity) );
        mpd_entity_free(entity);
        if (id != -1)
            break;
    }
    mpd_response_finish(conn);
    errorCheck("get_listChanger");
    return id;
}


void gm_mpdCom::play_this(int ID)
{
    if (conn == NULL)
        return;
    mpd_send_play_id(conn, ID);
    mpd_response_finish(conn);
    errorCheck("play_this");
}


// cmdlist must start with the highest songnr!
void gm_mpdCom::execute_cmds(gm_commandList cmdlist, bool b_reset_list)
{
    if(cmdlist.empty() ||conn == NULL)
        return;
	
    mpd_command_list_begin(conn, true);
    if (!errorCheck("mpd_sendCommandListBegin"))
    {
        mpd_response_finish(conn);
        cmdlist.clear();
        return;
    }

	bool b_reset_song = false;
	
    std::list<gm_cmdStruct>::iterator iter;
    for (iter = cmdlist.begin(); iter != cmdlist.end(); ++iter)
    {
        switch (iter->cmd)
        {
        case CMD_DPL: // delete playlist
            {
				if (config->mpd_rm_allowed)
                	mpd_send_rm(conn, iter->file.data());
                break;
            }
            
        case CMD_ADD:
            {
                current_playlist++; // prevent list update
                if (mpd_send_add(conn, iter->file.data()))
				{
					plist_lenght++;
					b_reset_song = true;
				}
                break;
            }
            
        case CMD_INS:
            {
                // add
                current_playlist++;
                if (mpd_send_add(conn, iter->file.data()))
				{
					plist_lenght++;
					b_reset_song = true;
					// move into position
					current_playlist++; // prevent list update
					mpd_send_move(conn, plist_lenght-1, iter->moveTo);					
				}
                break;
            }
            
        case CMD_MOV: // move ID to pos
            {
                current_playlist++;
                if (mpd_send_move_id(conn, iter->songid, iter->moveTo))
					b_reset_song = true;
                break;
            }
            
        case CMD_POS: // move from pos to pos
            {
          		current_playlist++;
				if (mpd_send_move(conn, iter->songnr, iter->moveTo))
				    b_reset_song = true;
            	break;
            }
            
        case CMD_DEL:
            {
                current_playlist++;
				if (config->mpd_deleteid_allowed)
				{
		            if (mpd_send_delete_id(conn, iter->songid))
					{
						plist_lenght--;
						b_reset_song = true;
					}
				}
                break;
            }
            
        default:
            break;
        } // <- switch
    }
    mpd_command_list_end(conn);
    mpd_response_finish(conn);
    errorCheck("execute_cmds");
    
    cmdlist.clear();
    
	if (b_reset_song)
	{
		current_sInfo->songID = -1;
		current_sInfo->songNr = -1;
		current_status = -1;
	}
	
	if (b_reset_list)
			current_playlist = -1;
}


void gm_mpdCom::clear_list()
{
    if (conn == NULL || plist_lenght == 0 || !config->mpd_clear_allowed)
        return;
    mpd_send_clear(conn);
    mpd_response_finish(conn);
    errorCheck("clear_list");
	current_sInfo->songID = -1;
	current_sInfo->songNr = -1;
	current_status = -1;
    current_playlist = -1;
}


int gm_mpdCom::execute_single_command(gm_cmdStruct command, bool resetlist)
{
	int results = 0;

	if (conn == NULL)
		return results;
	/* 
	   the mpd_run_ commands include an errorcheck and return FALSE on non-fatal
	   errors such as "No such song" (e.g. when trying to add a txt file)
	*/
	switch (command.cmd)
    {
	    case CMD_ADD:
            {
                current_playlist++; // prevent list update
                if (mpd_run_add(conn, command.file.data()))
				{
						plist_lenght++;
						results++;
				}
                break;
            }
            
        case CMD_INS:
            {
                // add
                current_playlist++; // prevent list update
                if (mpd_run_add(conn, command.file.data()))
				{
						plist_lenght++;
						results++;
						// move into position
						current_playlist++; // prevent list update
						mpd_run_move(conn, plist_lenght-1, command.moveTo);
				}
                break;
            }         
        default:
            break;
	} // <- switch

	mpd_response_finish(conn);
	// No errorcheck: we silently ignore txt files and such
	if (resetlist)
		current_playlist = -1;
	return results;
}


void gm_mpdCom::shuffle_list()
{
    if (plist_lenght < 2 || conn == NULL || !config->mpd_shuffle_allowed)
        return;
        
    mpd_run_shuffle(conn);
    mpd_response_finish(conn);
    errorCheck("shuffle_list_1");
    
    // move current song to the top
    if (current_sInfo->songID != -1)
    {
        current_playlist++; // prevent reloading
        mpd_send_move_id(conn, current_sInfo->songID, 0);
        mpd_response_finish(conn);
        errorCheck("shuffle_list_2");
    }
    // trigger reloading the list
    current_playlist = -1;
}


gm_IDlist gm_mpdCom::get_IDlist()
{
    gm_IDlist idlist;
    
    if (conn == NULL)
        return idlist;
        
    struct mpd_song *song = NULL;
    mpd_entity * entity;
    mpd_send_list_queue_meta(conn);
    while((entity = mpd_recv_entity(conn)))
    {
        if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG)
        {
            song = mpd_song_dup (mpd_entity_get_song(entity));
            idlist.push_back(mpd_song_get_id(song));
        }
        mpd_entity_free(entity);
    }
    mpd_response_finish(conn);
    errorCheck("mpd_send_list_queue_meta");
    if (song != NULL)
        mpd_song_free (song); 
    return idlist;
}


bool gm_mpdCom::save_playlist(ustring listname)
{
    if (conn == NULL || !config->mpd_save_allowed)
        return false;
    
    mpd_send_save(conn, listname.data() );
    mpd_response_finish(conn);

    sgnl_plistsaved.emit();
    
    if (!errorCheck("save_playlist"))
    {
        return false;
    }
    else
        return true;     
}


bool gm_mpdCom::save_selection(std::vector<ustring> tobeSaved, ustring listname)
{
    if (conn == NULL)
        return false;
    
    mpd_command_list_begin(conn, true);
    for (int i = 0; i < tobeSaved.size(); ++i)
    {
        mpd_send_playlist_add(conn, listname.data(), tobeSaved.at(i).data());
    }
    mpd_command_list_end(conn);
    mpd_response_finish(conn);

    sgnl_plistsaved.emit();
    
    if (!errorCheck("save_selection"))
        return false;
    else
        return true; 
}


gm_mpd_version gm_mpdCom::get_version()
{
	gm_mpd_version version;

	if (conn == NULL)
	{
		version.primary = mpd_connection_get_server_version(conn)[0];
		version.secundary = mpd_connection_get_server_version(conn)[1];
		return version;
	}
	
	version.primary = mpd_connection_get_server_version(conn)[0];
	version.secundary = mpd_connection_get_server_version(conn)[1];

	return version;
}


gm_itemList gm_mpdCom::get_playlistlist()
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
        
    struct mpd_entity *ent;
    mpd_send_list_meta (conn, "/");
    while ((ent = mpd_recv_entity (conn)))
    {
        if (mpd_entity_get_type (ent) == MPD_ENTITY_TYPE_PLAYLIST)
        {
            gm_listItem newItem;
            newItem.type = TP_PLAYLIST;
            const struct mpd_playlist *playlist = mpd_entity_get_playlist(ent);
            ustring path = mpd_playlist_get_path(playlist);
            newItem.file = path;
            // put only the list name in 'name'
            newItem.name = path.substr(path.rfind("/")+1, path.length());
            newItem.sorter = newItem.name.lowercase();
            itemlist.push_back(newItem);
        }
        mpd_entity_free (ent);
    }
    mpd_response_finish (conn);
    errorCheck("get_playlistlist");
    return itemlist;
}


gm_itemList gm_mpdCom::get_artistlist()
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
        
    ustring artist = "~0~";
    ustring newartist;
    struct mpd_song *song;
    mpd_send_list_all_meta(conn, "/");
    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
        {
            newartist = mpd_song_get_tag (song, MPD_TAG_ARTIST, 0);
            
            if (artist != newartist)
            {
                gm_listItem newItem;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.sorter = newartist.lowercase();
                if (config->lib_ignore_leading_the && str_has_prefix(newItem.sorter,"the"))
                    newItem.sorter = (newItem.sorter).substr(4, (newItem.sorter).length());
                itemlist.push_back(newItem);
                artist = newartist;
            }
        }
        else
        {
            newartist = "";
            
            if (!artist.empty())
            {
                gm_listItem newItem;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.sorter = newartist.lowercase();
                if (config->lib_ignore_leading_the && str_has_prefix(newItem.sorter,"the"))
                    newItem.sorter = (newItem.sorter).substr(4, (newItem.sorter).length());
                itemlist.push_back(newItem);
                artist = newartist;
            }
        }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_artistlist");
    return itemlist;
}


gm_itemList gm_mpdCom::get_artist_searchlist(ustring searchfor)
{
    if (searchfor == "~0~")
        return get_artistlist();
        
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
        
    ustring artist = "~0~";
    ustring newartist;
    struct mpd_song *song;
    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, searchfor.data());
    mpd_search_commit(conn);
    
    while ((song = mpd_recv_song (conn)))
    {
    
        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
        {
            newartist = mpd_song_get_tag (song, MPD_TAG_ARTIST, 0);
            
            if (artist != newartist)
            {
                gm_listItem newItem;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.sorter = newartist.lowercase();
                if (config->lib_ignore_leading_the && str_has_prefix(newItem.sorter,"the"))
                    newItem.sorter = (newItem.sorter).substr(4, (newItem.sorter).length());
                itemlist.push_back(newItem);
                artist = newartist;
            }
        }
        else
            if (searchfor.empty())
            {
                newartist = "";
                
                if (!artist.empty())
                {
                    gm_listItem newItem;
                    newItem.type = TP_ARTIST;
                    newItem.artist = newartist;
                    newItem.sorter = newartist.lowercase();
                	if (config->lib_ignore_leading_the && str_has_prefix(newItem.sorter,"the"))
                    	newItem.sorter = (newItem.sorter).substr(4, (newItem.sorter).length());
                    itemlist.push_back(newItem);
                    artist = newartist;
                }
            }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_album_searchlist"); 
    return itemlist;
}


gm_itemList gm_mpdCom::get_folderlist(ustring filePath)
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
	
    if (filePath == "~0~")
		filePath = "";
	
    struct mpd_entity * entity;
    mpd_send_list_meta(conn, filePath.data());
    while( (entity = mpd_recv_entity(conn)) != NULL )
    {
        if( mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_DIRECTORY )
        {
            gm_listItem newItem;
            const struct mpd_directory *directory = mpd_entity_get_directory(entity);
            ustring path = mpd_directory_get_path (directory);
            newItem.file = path;
			newItem.name = path.substr(path.rfind("/")+1, path.length());
            newItem.type = TP_FOLDER;
            newItem.sorter = "00" + newItem.name.lowercase(); // "00" so dirs come first
            itemlist.push_back(newItem);
        }
        else
            if(mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG)
            {
                gm_listItem newItem;
                const struct mpd_song * song = mpd_entity_get_song (entity);
				newItem.type = TP_SONG; // never a TP_STREAM or TP_SONGX here
				if (mpd_song_get_uri(song) != NULL)
				{
                	newItem.file = mpd_song_get_uri(song);
					newItem.name = (newItem.file).substr((newItem.file).rfind("/")+1, (newItem.file).length());
				}
				newItem.sorter = "11" + newItem.name.lowercase(); // "11" so files come next
				if (mpd_song_get_duration(song) > 0)
                	newItem.time = mpd_song_get_duration(song);
				if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
                	newItem.artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
				if (mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) != NULL)
                	newItem.album = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
				if (mpd_song_get_tag(song, MPD_TAG_TITLE, 0) != NULL)
                	newItem.title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
				if (mpd_song_get_tag(song, MPD_TAG_TRACK, 0) != NULL)
                	newItem.track = fixTrackformat(mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
                itemlist.push_back(newItem);
            }
        mpd_entity_free(entity);
    } // <- while
    mpd_response_finish (conn);
    errorCheck("get_folderlist");
    return itemlist;
}

/*
ustring gm_mpdCom::time_to_string(time_t time)
{
	struct tm * timeinfo = localtime (&time);
	
	ustring month = into_string(timeinfo->tm_mon + 1); // tm_mon: 0-11
	if (month.length() == 1)
		month = "0" + month;
	ustring day = into_string(timeinfo->tm_mday);
	if (day.length() == 1)
		day = "0" + day;
	ustring year = into_string(timeinfo->tm_year + 1900); // tm_year: years since 1900
	ustring hour = into_string(timeinfo->tm_hour);
	if (hour.length() == 1)
		hour = "0" + hour;
	ustring min = into_string(timeinfo->tm_min);
	if (min.length() == 1)
		min = "0" + min;
	
	ustring result = year +"/"+ month +"/"+ day + "  "+ hour + ":" + min;
	return result;
}
*/

gm_itemList gm_mpdCom::get_albumlist(bool sort_by_date)
{
    gm_itemList itemlist;
	
    if (conn == NULL)
        return itemlist;
	time_t nowtime;
  	time(&nowtime);
	int max_album_length = 0;
    ustring album = "~0~";
    ustring newalbum;
    ustring artist = "~0~";
    ustring newartist;
	
    struct mpd_song *song;
    mpd_send_list_all_meta(conn, "/");

    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
            newartist = mpd_song_get_tag (song, MPD_TAG_ARTIST, 0);
        else
            newartist = "";
            
        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = mpd_song_get_tag (song, MPD_TAG_ALBUM, 0);
            if (newalbum.length() > max_album_length)
					max_album_length = newalbum.length();
			
            if (album != newalbum || artist != newartist)
            {
				gm_listItem newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = newartist;
                if (sort_by_date)
                {
					newItem.set_sort_modtime();
                    time_t time = mpd_song_get_last_modified(song);
	                /* newItem.moddate = time_to_string(time); FIXME allow? */
					// invert the sort order: oldest last
					newItem.modtime = nowtime - time;
                }
				// else : see below
                itemlist.push_back(newItem);
                album = newalbum;
                artist = newartist;
            }
        }
		else
        {
            newalbum = "";
            
            if (artist != newartist)
            {
                gm_listItem newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = newartist;
                if (sort_by_date)
                {
					newItem.set_sort_modtime();
                    time_t time = mpd_song_get_last_modified(song);
	                /* newItem.moddate = time_to_string(time); FIXME allow? */
					// invert the sort order
					newItem.modtime = nowtime - time;
                }
				// else : see below
                itemlist.push_back(newItem);
                album = newalbum;
                artist = newartist;
            }
        }
        
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_albumlist");

	/* 	give all album-strings the same length in the sorter
		or album names can overlap with artist names   */
	if (!sort_by_date)
	{
		std::list<gm_listItem>::iterator iter;
		for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
		{
			ustring albumstring = iter->album;
			while (albumstring.length() < max_album_length)
				albumstring += "0";
			iter->sorter = (albumstring + iter->artist).lowercase();
		}
		
	}
    return itemlist;
}


gm_itemList gm_mpdCom::get_album_searchlist(ustring searchfor)
{
    if (searchfor == "~0~")
        return get_albumlist(false);
        
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
	
    int max_album_length = 0;   
    ustring album = "~0~";
    ustring newalbum;
    ustring artist = "~0~";
    ustring newartist;
    struct mpd_song *song;
    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM, searchfor.data());
    mpd_search_commit(conn);
    
    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
            newartist = mpd_song_get_tag (song, MPD_TAG_ARTIST, 0);
        else
            newartist = "";
            
        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = mpd_song_get_tag (song, MPD_TAG_ALBUM, 0);
			
            if (newalbum.length() > max_album_length)
				max_album_length = newalbum.length();
			
            if (album != newalbum || artist != newartist)
            {
                gm_listItem newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = newartist;
                itemlist.push_back(newItem);
                album = newalbum;
                artist = newartist;
            }
        }
        else
            if (searchfor.empty())
            {
                newalbum = "";
                
                if (!album.empty() || artist != newartist)
                {
                    gm_listItem newItem;
                    newItem.type = TP_ALBUM;
                    newItem.album = newalbum;
                    newItem.artist = newartist;
                    itemlist.push_back(newItem);
                    album = newalbum;
                    artist = newartist;
                }
            }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_album_searchlist");

	/* 	give all album-strings the same length in the sorter
		or album names can overlap with artist names   */
	std::list<gm_listItem>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		ustring albumstring = iter->album;
		while (albumstring.length() < max_album_length)
				albumstring += "0";
		iter->sorter = (albumstring + iter->artist).lowercase();
	}
    return itemlist;
}


std::string gm_mpdCom::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


gm_itemList gm_mpdCom::get_yearlist()
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
        
    ustring year = "~0~";
    ustring newyear;
    ustring album = "~0~";
    ustring newalbum;
	
    struct mpd_song *song;
    mpd_send_list_all_meta(conn, "/");
    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_DATE, 0) != NULL )
            newyear = mpd_song_get_tag (song, MPD_TAG_DATE, 0);
        else
            newyear = "";
			
		// convert xxx-xx-xx format to xxxx
		if (!newyear.empty())
			newyear =  newyear.substr(0, 4) + " ";
            
        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = mpd_song_get_tag (song, MPD_TAG_ALBUM, 0);
            
            if (album != newalbum || year != newyear)
            {
                gm_listItem newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
				if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL)
					newItem.artist = mpd_song_get_tag (song, MPD_TAG_ARTIST, 0);
				
				if (newyear.empty())
				{
					newItem.sorter = "0000" + newalbum.lowercase() + newItem.artist.lowercase();
				}
				else
				{
					newItem.sorter = newyear + newalbum.lowercase() + newItem.artist.lowercase();
					newItem.year = newyear;
				}
                itemlist.push_back(newItem);
                album = newalbum;
                year = newyear;
            }
        }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_yearlist");
	
    return itemlist;
}


gm_itemList gm_mpdCom::get_albums_for_artist(ustring artist)
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
        
    ustring album = "~0~";
    ustring newalbum;
    struct mpd_song *song;
    mpd_search_db_songs(conn, true);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, artist.data());
    mpd_search_commit(conn);
    
    while ((song = mpd_recv_song (conn)))
    {
    
        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = mpd_song_get_tag (song, MPD_TAG_ALBUM, 0);
            
            if (album != newalbum)
            {
                gm_listItem newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = artist;
                newItem.sorter = newalbum.lowercase();
                itemlist.push_back(newItem);
                album = newalbum;
            }
        }
        else
        {
            newalbum = "";
            if (!album.empty())
            {
                gm_listItem newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = artist;
                newItem.sorter = newalbum;
                itemlist.push_back(newItem);
                album = newalbum;
            }
        }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_albums_for_artist");
    
    return itemlist;
}


ustring gm_mpdCom::get_album_year(ustring album, ustring artist)
{
    ustring year = "";
    
    if (conn == NULL)
        return year;
        
    if (album.empty() || artist.empty())
        return year;
        
    struct mpd_pair * pair;
    mpd_search_db_tags(conn, MPD_TAG_DATE);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, artist.data());
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM,  album.data());
    mpd_search_commit(conn);
    
    while ((pair = mpd_recv_pair_tag (conn, MPD_TAG_DATE)))
    {
        // take the first that comes along
		if (pair->value != NULL)
        	year = pair->value;
		mpd_return_pair(conn, pair);
        if (!year.empty())
            break;
    }
    mpd_response_finish (conn);
    errorCheck("get_album_year");
    
	// convert xxx-xx-xx format to xxxx
	if (!year.empty())
		year =  year.substr(0, 4) + " ";
        
    return year;
}


gm_itemList gm_mpdCom::get_genre_artistlist(ustring searchfor)
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
    	    
    ustring artist = "~0~";
    ustring newartist;
    ustring album = "~0~";
    ustring newalbum;
	int max_name_length = 0;
    struct mpd_song *song;
    mpd_search_db_songs(conn, true);

    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_GENRE, searchfor.data());
    mpd_search_commit(conn);
    while ((song = mpd_recv_song (conn)))
    {
		if (mpd_song_get_tag (song, MPD_TAG_GENRE, 0))
		{
			ustring genre = mpd_song_get_tag (song, MPD_TAG_GENRE, 0);
		    if (genre == searchfor) 
		    {
				if (mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) != NULL)
		        	newalbum = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
				else
					newalbum = "";
				if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
		        	newartist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
				else
					newartist = "";
		        if (artist != newartist || album != newalbum)
		        {	
					if (newartist.length() > max_name_length)
						max_name_length = newartist.length();
		            gm_listItem newItem;
		            newItem.genre = searchfor;
		            newItem.type = TP_ARTIST;
		            newItem.artist = newartist;
		            newItem.album = newalbum;
		            itemlist.push_back(newItem);
		            artist = newartist;
		            album = newalbum;
		        }
			}
		}
		else
		if (searchfor.empty())
		{
			if (mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) != NULL)
		    	newalbum = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
			else
				newalbum = "";
			if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
		    	newartist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
			else
				newartist = "";
		    if (artist != newartist || album != newalbum)
		    {	
		        gm_listItem newItem;
		        newItem.type = TP_ARTIST;
		        newItem.artist = newartist;
		        newItem.album = newalbum;	        
		        itemlist.push_back(newItem);
		        artist = newartist;
		        album = newalbum;
		    }					
		}
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_genre_artistlist");

	/* 	give all artists-strings the same length in the sorter
		or album names can overlap with artist names   */
    std::list<gm_listItem>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
		ustring namestring = iter->artist;
		while (namestring.length() < max_name_length)
		 namestring += "0";
		iter->sorter = (namestring + iter->album).lowercase();
	}
	
    return itemlist;
}


gm_itemList gm_mpdCom::get_genrelist(ustring searchfor)
{
    gm_itemList itemlist;
    if (conn == NULL)
        return itemlist;
        
    if (searchfor == "~0~") // get full list
    {
        struct mpd_pair * pair;
        mpd_search_db_tags(conn, MPD_TAG_GENRE);
        mpd_search_commit(conn);
        
        while ((pair = mpd_recv_pair_tag (conn, MPD_TAG_GENRE)))
        {
			gm_listItem newItem;
			
			if (pair->value != NULL)
            	newItem.genre = pair->value;		
			newItem.type = TP_GENRE;
            newItem.sorter = newItem.genre.lowercase();
            itemlist.push_back(newItem);
			mpd_return_pair(conn, pair);
        }
        mpd_response_finish (conn);
        errorCheck("get_genrelist full");
    }
    else
    {
        struct mpd_song *song;
        mpd_search_db_songs(conn, false);
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_GENRE, searchfor.data());
        mpd_search_commit(conn);
        
        while ((song = mpd_recv_song (conn)))
        {
            if (mpd_song_get_tag (song, MPD_TAG_GENRE, 0) != NULL )
            {
				gm_listItem newItem;
               	newItem.genre = mpd_song_get_tag(song, MPD_TAG_GENRE, 0);
				newItem.type = TP_GENRE;
                newItem.sorter = newItem.genre; // not lowercase (keep Jazz & jazz)
                itemlist.push_back(newItem);
            }
            else
                if (searchfor.empty())
                {
                    gm_listItem newItem;
                    newItem.genre = "";
                    newItem.sorter = "0";
					newItem.type = TP_GENRE;
                    itemlist.push_back(newItem);
                }
            mpd_song_free (song);
        }
        mpd_response_finish (conn);
        errorCheck("get_genrelist search");
    }
    
    return itemlist;
}


gm_itemList gm_mpdCom::get_songlist(ustring album, ustring artist, ustring genre)
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
        
    struct mpd_song *song;
    mpd_search_db_songs(conn, true);
    if (artist != "~0~")
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, artist.data());
    if (album != "~0~")
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM, album.data());
    if (genre != "~0~")
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_GENRE, genre.data());
    mpd_search_commit(conn);
    
    while ( (song = mpd_recv_song(conn)) )
    {
        gm_listItem newItem;
		newItem.type = TP_SONG;
		if (mpd_song_get_uri(song) != NULL)
		{
           	newItem.file = mpd_song_get_uri(song);
			newItem.name = (newItem.file).substr((newItem.file).rfind("/")+1, (newItem.file).length());
		}
		if (mpd_song_get_duration(song) > 0)
        	newItem.time = mpd_song_get_duration(song);
		if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
        	newItem.artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
		if (mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) != NULL)
        	newItem.album = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
		if (mpd_song_get_tag(song, MPD_TAG_TITLE, 0) != NULL)
        	newItem.title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
		if (mpd_song_get_tag(song, MPD_TAG_GENRE, 0) != NULL)
        	newItem.genre = mpd_song_get_tag(song, MPD_TAG_GENRE, 0);
        if (mpd_song_get_tag(song, MPD_TAG_TRACK, 0) != NULL)
        {
            newItem.track = fixTrackformat(mpd_song_get_tag(song, MPD_TAG_TRACK, 0));
            newItem.sorter = (newItem.artist + newItem.album + newItem.track + newItem.title).lowercase();
        }
        else
		{
            newItem.sorter = (newItem.artist + newItem.album + "00" + newItem.title).lowercase();
		}
        if (newItem.title.empty())
            newItem.sorter = newItem.sorter + newItem.file;
            
        itemlist.push_back(newItem);
        mpd_song_free(song);
    }
    mpd_response_finish(conn);
    errorCheck("get_songlist");

    return itemlist;
}


gm_itemList gm_mpdCom::get_playlist_itemlist(ustring playlist)
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
     
    struct mpd_song *song;
    mpd_send_list_playlist_meta(conn, playlist.data());

    while ((song = mpd_recv_song(conn)))
    {
        gm_listItem newItem;
		
		if (mpd_song_get_uri(song))
		{
        	newItem.file = mpd_song_get_uri(song);
			newItem.name = (newItem.file).substr((newItem.file).rfind("/")+1, (newItem.file).length());
			if (str_has_prefix((newItem.file).lowercase(),"http:"))
		    {
		        newItem.type = TP_STREAM;
		    }
		    else
		    {
				if (str_has_prefix(newItem.file,"/"))
					newItem.type = TP_SONGX;
				else
					newItem.type = TP_SONG;
				if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
        			newItem.artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
				if ( mpd_song_get_tag(song, MPD_TAG_TITLE, 0) != NULL)
        			newItem.title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
		    }
		}
		else
		{
			newItem.type = TP_NOITEM;
			newItem.title = "U.P.O. (Unidentified Playlist Object)";
		}
		
        itemlist.push_back(newItem);
        mpd_song_free (song);
    }
    mpd_response_finish(conn);

    errorCheck("get_playlist_itemlist");
    return itemlist;
}


gm_itemList gm_mpdCom::get_song_searchlist(ustring searchfor)
{
    gm_itemList itemlist;
    
    if (conn == NULL)
        return itemlist;
    int max_title_length = 0; 
    struct mpd_song *song;
    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_TITLE, searchfor.data());
    mpd_search_commit(conn);
    
    while ((song = mpd_recv_song (conn)))
    {
    
        if (mpd_song_get_tag (song, MPD_TAG_TITLE, 0) != NULL )
        {
            gm_listItem newItem;
			if (mpd_song_get_uri(song))
			{
            	newItem.file = mpd_song_get_uri(song);
				newItem.name = (newItem.file).substr((newItem.file).rfind("/")+1, (newItem.file).length());
			}
            newItem.type = TP_SONG; // never a TP_STREAM or TP_SONGX here
			if (mpd_song_get_tag(song, MPD_TAG_TITLE, 0) != NULL)
	    		newItem.title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
			if (newItem.title.length() > max_title_length)
				max_title_length = newItem.title.length();
			if (mpd_song_get_duration(song) > 0)
		    	newItem.time = mpd_song_get_duration(song);
			if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
		    	newItem.artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
			if (mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) != NULL)
		    	newItem.album = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
			if (mpd_song_get_tag(song, MPD_TAG_GENRE, 0) != NULL)
		    	newItem.genre = mpd_song_get_tag(song, MPD_TAG_GENRE, 0);
            itemlist.push_back(newItem);
        }
        else
            if (searchfor.empty())
            {
                gm_listItem newItem;
				newItem.type = TP_SONG;
				if (mpd_song_get_uri(song) != NULL)
				{
           			newItem.file = mpd_song_get_uri(song);
					newItem.name = (newItem.file).substr((newItem.file).rfind("/")+1, (newItem.file).length());
				}
                if (mpd_song_get_duration(song) > 0)
		    		newItem.time = mpd_song_get_duration(song);
				if (mpd_song_get_tag(song, MPD_TAG_ARTIST, 0) != NULL)
					newItem.artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
				if (mpd_song_get_tag(song, MPD_TAG_ALBUM, 0) != NULL)
					newItem.album = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
				if (mpd_song_get_tag(song, MPD_TAG_GENRE, 0) != NULL)
					newItem.genre = mpd_song_get_tag(song, MPD_TAG_GENRE, 0);
                itemlist.push_back(newItem);
            }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    errorCheck("get_song_searchlist");

	/* 	give all title-strings the same length in the sorter
		or title names will overlap with artist names   */
    std::list<gm_listItem>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
    {
		ustring titlestring = iter->title;
		while (titlestring.length() < max_title_length)
			titlestring += "0";
		iter->sorter = (titlestring + iter->artist + iter->album).lowercase();
	}
	
    return itemlist;
}


void gm_mpdCom::set_outputs(gm_outputList outputlist)
{
    if (conn == NULL || !config->mpd_outputs_allowed)
        return;
        
    bool wasplaying = false;
    if (current_status == MPD_STATE_PLAY)
    {
        wasplaying = true;
        pause();
    }
    
    std::vector<gm_output>::iterator it;
    gm_output output;
    it = outputlist.begin();
    while (it != outputlist.end())
    {
    	output = *it;
        if (output.enabled)
            mpd_send_enable_output(conn, output.id);
        else
            mpd_send_disable_output(conn, output.id);
        mpd_response_finish(conn);
        errorCheck("set_outputs");
		++it;
	}
    
    if (wasplaying)
        resume();
}


gm_outputList gm_mpdCom::get_outputs()
{
    gm_outputList outputlist;

    if (conn == NULL)
        return outputlist;
        
    mpd_output *output;
    mpd_send_outputs(conn);
    if (errorCheck("get_outputs"))
    {
        while ((output = mpd_recv_output(conn)))
        {
            gm_output goutput;
            goutput.id  = mpd_output_get_id(output);
            goutput.enabled  = mpd_output_get_enabled(output);
            goutput.name = mpd_output_get_name(output);
            outputlist.push_back(goutput);
			mpd_output_free(output);
        }
    }
    mpd_response_finish(conn);
    
    return outputlist;
}


int gm_mpdCom::get_replaygain()
{
	int result = -1;
    ustring mode = "";
	
    if (conn == NULL)
        return result;
        
    if (!mpd_send_command(conn, "replay_gain_status", NULL))
        return result;
        
    if (mpd_pair *pair = mpd_recv_pair_named(conn, "replay_gain_mode"))
    {
        mode = pair->value;
		mode.lowercase();
		
		mpd_return_pair(conn, pair);
    }
    mpd_response_finish(conn);
	
	if (mode == "off")
	    result = 0;
	else
	if (mode == "track")
	    result = 1;
	else
	if (mode == "album")
	    result = 2;
	else
	if (mode == "auto")
	    result = 3;
	else
		result = -1;	

	return result;
}


void gm_mpdCom::set_replaygain(int mode)
{
    if (conn == NULL)
        return;
    const char *rg_mode;
    
    switch (mode)
    {
    case 0:
        rg_mode = "off";
        break;
    case 1:
        rg_mode = "track";
        break;
    case 2:
        rg_mode = "album";
        break;
    case 3:
        rg_mode = "auto";
        break;
    default:
        return;
    }
	
    mpd_send_command(conn, "replay_gain_mode", rg_mode, NULL);
    mpd_response_finish(conn);
	errorCheck("set_replaygain");
}


bool gm_mpdCom::is_mpd_running()
{	
	struct stat sts;
	bool b_isrunning = false;
	
	// try pidof
	if (stat("/bin/pidof", &sts) == 0)
	{
		if( system("pidof mpd > /dev/null") == 0)
		b_isrunning = true;
	}
	else
	// try pgrep
	if (stat("/usr/bin/pgrep", &sts) == 0)
	{
		if( system("pgrep mpd > /dev/null") == 0)
			b_isrunning = true;
	}

	if (b_isrunning)
		return true;
	else
	{
		DIR* dir;
		struct dirent* ent;
		char* endptr;
		char buf[512];
		const char* name = "mpd";
		const char* path = "/usr/bin/mpd";
		bool result = false;
	
		if (!(dir = opendir("/proc")))
		{
			system("killall mpd");
		   	cout << "Cannot determine if MPD is running: assuming it is not" << endl;
		   	return false;
		}

		while((ent = readdir(dir)) != NULL)
		{
		    // if endptr is not '\0', dir is not numeric: ignore it
		    long lpid = strtol(ent->d_name, &endptr, 10);
		    if (*endptr != '\0')
			{
		        continue;
		    }

		    // try to open the 'cmdline' file
		    snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
		    FILE* fl = fopen(buf, "r");
		    if (fl)
			{
		        if (fgets(buf, sizeof(buf), fl) != NULL)
				{
		            // check first token in file: program name
		            char* first = strtok(buf, " ");
		            if (strcmp(first, name) == 0 || strcmp(first, path) == 0)
					{
						fclose(fl);
						result = true;
						break;
		            }
		        }
		        fclose(fl);
		    }
		}

		closedir(dir);

		return result;
	}
}


gm_mpdCom::~gm_mpdCom()
{

}
