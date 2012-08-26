/*
 *  gm_mpdcom.cc
 *  GUIMUP mpd communicator class
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

#include "gm_mpdcom.h"

gm_mpdCom::gm_mpdCom()
{
	conn = NULL;
	b_reload = true;
	b_mpdconf_found = false;
	b_connecting = false;
	b_statCheckBusy = false;
	b_dbaseUpdating = false;
	mpdconf_path = "";
	current_playlist = -1;
	current_songID = -1;
	current_songNum = -1;
	current_status = -1;
	current_volume = 0;
	
    // first try ~/.mpdconf
    ustring homeDir = getenv("HOME");
    if (homeDir.rfind("/") != homeDir.length()-1)
        homeDir += "/";
        mpdconf_path = homeDir + ".mpdconf";
    std::ifstream conffile (mpdconf_path.data());
	
    if (!conffile.is_open())
    {	// next try /etc/mpd.conf
        mpdconf_path = "/etc/mpd.conf";
        std::ifstream conffile (mpdconf_path.data());
        if (conffile.is_open())
		{
            b_mpdconf_found = true;
			conffile.close();
		}
    }
    else
	{
        b_mpdconf_found = true;
		conffile.close();
	}
	
	if (b_mpdconf_found)
	{
		cout << "MPD: using " << mpdconf_path.data() << endl;
		cout << "MPD: user is " << get_user() << endl;
	}
	else
	{
		cout << "Could not locate MPD's config file" << endl;
		cout << "Is MPD installed?" << endl;
	}
}

void gm_mpdCom::setOutputs(outdev_list oDevices)
{
	if (conn == NULL)
		return;
	
    std::list<mpd_OutputEntity>::iterator cit;
    for ( cit = oDevices.begin(); cit != oDevices.end(); ++cit )
    {
    	mpd_OutputEntity curDev = *cit;
    	if (curDev.enabled)
    		mpd_sendEnableOutputCommand(conn, curDev.id);
    	else
			mpd_sendDisableOutputCommand(conn, curDev.id);
    	mpd_finishCommand(conn);
    	errorCheck("mpd_sendDisableOutputCommand");
    }
}


outdev_list gm_mpdCom::getOutputs()
{
    outdev_list oDevices;
    // get current output devices
    if (conn == NULL)
		return oDevices;
	
    mpd_OutputEntity * otity = NULL;
    mpd_sendOutputsCommand(conn);
    if (errorCheck("mpd_sendOutputsCommand"))
    {
        while ((otity = mpd_getNextOutput(conn)))
        oDevices.push_back(*otity);
    }
    mpd_finishCommand(conn);
	
    if (otity != NULL)
        mpd_freeOutputElement(otity);
	
	return oDevices;	
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

void gm_mpdCom::updatefile(ustring file)
{
    if (conn == NULL)
        return;
	
    mpd_sendUpdateCommand(conn, (char*)file.c_str());
    mpd_finishCommand(conn);
    if (errorCheck("mpd_sendUpdateCommand"))
	{
		//trigger reloading the song
		current_songID = -1;
		cout << "MPD database update: OK" << endl;
	}
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
                // substr refuses to do this in one step, so take two steps:
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

ustring gm_mpdCom::get_tagEncoding()
{
    ustring result = get_string("id3v1_encoding");
    return result;
}

ustring gm_mpdCom::get_user()
{
    ustring result = get_string("user");
    return result;
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
				// substr refuses to do this in one step, so take two steps:
				s_line = s_line.substr(s_line.find("\"")+1, s_line.length());
				s_line = s_line.substr(0, s_line.rfind("\""));
				result = s_line;
				break;
			}
        }
        conffile.close();
    }
	return result;
}


bool gm_mpdCom::mpd_connect(ustring host, int port, ustring pswd, bool usem)
{
	// disconnect if connected
	if (conn != NULL)
	{
		statusLoop.disconnect();
		current_playlist = -1;
		current_songID = -1;
		current_songNum = -1;
		current_status = -1;
		mpd_finishCommand(conn);
        mpd_closeConnection(conn);
        conn = NULL;
	}
	
	if (usem)
	{
		serverPort = port;
		serverName = host;
		serverPassword = pswd;
	}
	else
	{
		serverPort = get_port();
		serverName = get_host();
		serverPassword = get_password();
	}

    b_connecting = true;
	
    // no server, no go.
    if (serverName.empty())
    {
        serverName = "localhost";
        cout << "No host name: using \"localhost\"" << endl;
    }

    // port number must be > 1024
    if (serverPort <= 1024)
    {
        serverPort = 6600;
        cout << "Invalid port #: using 6600" << endl;
    }

	conn = mpd_newConnection(serverName.data(), serverPort ,2);

	if (conn->error)
	{
		mpd_clearError(conn);
		if (conn != NULL)
		{
			mpd_closeConnection(conn);
			conn = NULL;
		}
	}
	
	if (conn != NULL)
		mpd_finishCommand(conn);
	

    // If connection failed try 4 more tmes at 0.2 sec intervals
    // (in case mpd is starting up)
    int count = 1;
    while (conn == NULL && count < 5)
    {
        // wait 200 msec
        usleep(200000);
		
		// try again
		conn = mpd_newConnection(serverName.data(), serverPort ,2);
		if (conn->error)
		{
			mpd_clearError(conn);
			if (conn != NULL)
			{
				mpd_closeConnection(conn);
				conn = NULL;
			}
		}
	
		if (conn != NULL)
			mpd_finishCommand(conn);

        count++;
    }
	
    if (conn == NULL) // Still no luck
    {
		cout << "Connection failed (tried " << count << " times)" << endl;
		cout << "Is MPD running?" << endl;
        b_connecting = false;
        return false;
    }
    else
        cout << "Connected to MPD on try " << count << endl;

    // Let's tickle MPD
    mpd_sendStatusCommand(conn);
    mpd_finishCommand(conn);
    mpd_Status * servStatus = mpd_getStatus(conn);	
	mpd_finishCommand(conn);	
    if (conn->error) // Password required?
    {
		if (servStatus != NULL)
		{
        	mpd_freeStatus(servStatus);
			mpd_finishCommand(conn);
		}
		mpd_finishCommand(conn);
        mpd_clearError(conn);
		mpd_finishCommand(conn);
		
        if (!serverPassword.empty())
        {
            mpd_sendPasswordCommand(conn, serverPassword.data());
            mpd_finishCommand(conn);
            if (conn->error)
            {
				cout << "Error: " << conn->errorStr << endl;
                mpd_clearError(conn);
				mpd_finishCommand(conn);
                mpd_closeConnection(conn);
                conn = NULL;
                b_connecting = false;
                return false;
            }
        }
        else
        {
            mpd_closeConnection(conn);
            conn = NULL;
            cout << "No response: Password required?" << endl;
            b_connecting = false;
            return false;
        }
    }
    else
    {
		if (servStatus != NULL)
		{
        	mpd_freeStatus(servStatus);
			mpd_finishCommand(conn);
		}
		
        if (!serverPassword.empty())
		{
			serverPassword = ""; // so server_reconnect won't use it
            cout << "Password provided, but not used" << endl;
		}
        else
            cout << "No password required" << endl;
    }
    // we're in business
    b_connecting = false;
	statusCheck();
	// poll mpd every 333 msec
	statusLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::statusCheck), 333);
	signal_host_port_pwd.emit(serverName, serverPort, serverPassword);
    return true;
}


bool gm_mpdCom::mpd_reconnect()
{
	// previous loop has not finished
    if (b_connecting)
		return true;

    // try to connect
	b_connecting = true;
	
	conn = mpd_newConnection(serverName.data(), serverPort ,2);

	if (conn->error)
	{
		mpd_clearError(conn);
		if (conn != NULL)
		{
			mpd_closeConnection(conn);
			conn = NULL;
		}
	}
	
	if (conn == NULL)
	{
		b_connecting = false;
		return true;
	}
	else
		mpd_finishCommand(conn);	
	
    // login if there is a password
    if (!serverPassword.empty())
    {
		mpd_sendPasswordCommand(conn, serverPassword.data());
		if (conn->error)
		{
			cout << "Error: " << conn->errorStr << endl;
			mpd_clearError(conn);
			mpd_finishCommand(conn);
			mpd_closeConnection(conn);
			conn = NULL;
			b_connecting = false;
			return false;
		}
		mpd_finishCommand(conn);
	}
	
	// connected!
	cout << "Reconnected" << endl;
	reconnectLoop.disconnect();
	b_connecting = false;
	current_playlist = -1;
	current_songID = -1;
	current_songNum = -1;
	current_status = -1;
	statusCheck();
	// poll mpd every 333 msec
	statusLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::statusCheck), 333);
	signal_connected.emit(true);
	return false;
}


void gm_mpdCom::prev()
{
    if (conn == NULL)
    	return;
	mpd_sendPrevCommand(conn);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendPrevCommand");
}

void gm_mpdCom::stop()
{
    if (conn == NULL)
    	return;
	mpd_sendStopCommand(conn);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendPrevCommand");
}

void gm_mpdCom::play(bool reload)
{
    if (conn == NULL)
    	return;
	if (reload)
		b_reload = true;
	mpd_sendPlayCommand(conn, MPD_PLAY_AT_BEGINNING);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendPlayCommand");
}

void gm_mpdCom::resume()
{
    if (conn == NULL)
    	return;
	mpd_sendPauseCommand(conn, 0);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendPauseCommand 0");
}

void gm_mpdCom::pause()
{
    if (conn == NULL)
    	return;
	mpd_sendPauseCommand(conn, 1);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendPauseCommand 1");
}

void gm_mpdCom::next()
{
    if (conn == NULL)
    	return;
	mpd_sendNextCommand(conn);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendNextCommand");
}

void gm_mpdCom::set_random(bool status)
{
    if (conn == NULL)
    	return;
	mpd_sendRandomCommand(conn, status);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendRandomCommand");
}

void gm_mpdCom::set_repeat(bool status)
{
    if (conn == NULL)
    	return;
	mpd_sendRepeatCommand(conn, status);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendRepeatCommand");
}

void gm_mpdCom::volume_up(int i)
{
	if (conn == NULL)
		return;
	int vol = current_volume + i;
	if (vol > 100)
		vol = 100;
	mpd_sendSetvolCommand(conn, vol);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendSetvolCommand");
}

void gm_mpdCom::volume_down(int i)
{
	if (conn == NULL)
		return;
	int vol = current_volume - i;
	if (vol < 0)
		vol = 0;
	mpd_sendSetvolCommand(conn, vol);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendSetvolCommand");
}

void gm_mpdCom::set_volume(int vol)
{
    if (conn == NULL)
    	return;
	mpd_sendSetvolCommand(conn, vol);
	mpd_finishCommand(conn);
	errorCheck("mpd_sendSetvolCommand");
}

void gm_mpdCom::set_seek(int percent)
{
    if (conn == NULL)
    	return;
    mpd_sendSeekCommand(conn, current_songNum, percent);
    mpd_finishCommand(conn);
    errorCheck("mpd_sendSeekCommand");
}


bool gm_mpdCom::errorCheck(ustring action)
{
    if (conn == NULL)
    {
		statusLoop.disconnect();
		signal_connected.emit(false);
        cout << "Error on \"" << action << "\": No connection" << endl;
		cout << "Reconnecting..." << endl;
		signal_status.emit(-1, "Reconnecting...");
		mpd_reconnect();
		// still no luck: try to reconnect every 2 seconds
		if (conn == NULL);
		{
			reconnectLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::mpd_reconnect), 2000);
		}
        return false;
    }

    if (conn->error)
    {
        // If it is a non-fatal error (no permission etc.).
        if (conn->error == MPD_ERROR_ACK)
        {
            cout << "Non fatal error on \"" << action << "\": " << conn->errorStr << endl;
			cout << "Continue..." << endl;
            mpd_clearError(conn);
			mpd_finishCommand(conn);
            return true;
        }

        // A fatal error occurred: disconnnect & reconnect
		statusLoop.disconnect();
		signal_connected.emit(false);
		cout << "Fatal error on \"" << action << "\": " << conn->errorStr << endl;
		cout << "Reconnecting..." << endl;
		signal_status.emit(-1, "Error. Reconnecting...");;
        mpd_clearError(conn);
		mpd_finishCommand(conn);
        mpd_closeConnection(conn);
        conn = NULL;
		mpd_reconnect();
		// still no luck: reconnect every 2 seconds
		if (conn == NULL);
			reconnectLoop = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_mpdCom::mpd_reconnect), 2000);
        return false;
    }
	
	// No problems
    mpd_clearError(conn);
    return true;
}


bool gm_mpdCom::statusCheck()
{
	// return "false" terminates the loop
	if (!errorCheck("Starting statusCheck"))
		return false;
	
	// return "true" keeps the loop going
	if (b_statCheckBusy)
		return true;
	
	b_statCheckBusy = true;

	// Get the current server status
    mpd_sendStatusCommand(conn);
    mpd_Status * servStatus = mpd_getStatus(conn);
    mpd_finishCommand(conn);
    if (!errorCheck("mpd_sendStatsCommand") || servStatus == NULL)
    {
        b_statCheckBusy = false;
		if (servStatus != NULL)
		{
        	mpd_freeStatus(servStatus);
			mpd_finishCommand(conn);
		}
        return true;
    }

    // server has not found a supported mixer
    if (servStatus->volume == MPD_STATUS_NO_VOLUME)
    {
		signal_status.emit(MPD_STATUS_STATE_UNKNOWN, "No supported mixer found");
    	b_statCheckBusy = false;
		if (servStatus != NULL)
		{
        	mpd_freeStatus(servStatus);
			mpd_finishCommand(conn);
		}
        return true;
    }
	
	// playlist was updated
	if (current_playlist < servStatus->playlist)
    {
		cout << "playlist updated" << endl;
		current_playlist = servStatus->playlist;
		// reload the song (streams use this to update)
		b_reload = true;
	}
	// if started from the playlist
	if (servStatus->state == MPD_STATUS_STATE_PLAY
		&& current_status == MPD_STATUS_STATE_UNKNOWN)
	b_reload = true;
	// the song changed, or was restarted
    if((current_songID != servStatus->songid) || b_reload)
    {
		b_reload = false;
		// get the new song.
		mpd_InfoEntity * ntity = NULL;
		mpd_sendCurrentSongCommand(conn);
		ntity = mpd_getNextInfoEntity(conn);
		mpd_finishCommand(conn);
		
		// empty list, last has finished or first hasn't started
		if (ntity == NULL || !errorCheck("mpd_getNextInfoEntity"))
		{
			// Normally it already is, but ...
			if (servStatus->state != MPD_STATUS_STATE_STOP)
			{
				mpd_sendStopCommand(conn);
            	mpd_finishCommand(conn);
				errorCheck("mpd_sendStopCommand");
			}
			// send MPD_STATUS_STATE_UNKNOWN to the player
			servStatus->state = MPD_STATUS_STATE_UNKNOWN;
			servStatus->totalTime = 0;
			current_songNum = -1;
			current_songID = 0; // In fact, it already is
			theSong = NULL;
		}
		else
		{
			current_songNum = servStatus->song;
			current_songID = servStatus->songid;
			theSong = mpd_songDup (ntity->info.song);
		}

		// free memory
		if (ntity != NULL)
			mpd_freeInfoEntity(ntity);
		
		// collect & send the song info		
		songInfo newSong = get_songInfo_from(theSong);
		signal_songInfo.emit(newSong);	
    }
		
	// collect & send the status info
	statInfo newInfo;
	if (theSong != NULL)
		newInfo.mpd_status = servStatus->state;
	else
		newInfo.mpd_status = MPD_STATUS_STATE_UNKNOWN;
	
	current_status = newInfo.mpd_status;
	
	newInfo.mpd_time = servStatus->elapsedTime;
	newInfo.mpd_totalTime = servStatus->totalTime;
	newInfo.mpd_kHz = servStatus->sampleRate;
	newInfo.mpd_kbps = servStatus->bitRate;
	newInfo.mpd_volume = servStatus->volume;
	newInfo.mpd_random = servStatus->random;
	newInfo.mpd_repeat = servStatus->repeat;
	signal_statusInfo.emit(newInfo);
	
	current_volume = servStatus->volume;
	
	// free memory
	if (servStatus != NULL)
	{
     	mpd_freeStatus(servStatus);
		mpd_finishCommand(conn);
	}

	// done
    b_statCheckBusy = false;
	return true;
}

songInfo gm_mpdCom::get_songInfo_from(mpd_Song *theSong)
{
    songInfo newSong;
    if (theSong == NULL)
        newSong.nosong = true;
    else
    {
        newSong.nosong = false;
		
        if (theSong->pos >= 0)
            newSong.songNr = theSong->pos;
        else
            newSong.songNr = -1;
		
		if (theSong->time >= 0)
            newSong.time = theSong->time;
        else
            newSong.time = 0;
		
        if (theSong->file != NULL)
            newSong.file = theSong->file;
        else
            newSong.file = "";
		
        if (theSong->name != NULL)
            newSong.name = theSong->name;
        else
            newSong.name = "";
		
        if (theSong->artist != NULL)
            newSong.artist = theSong->artist;
        else
            newSong.artist = "";
		
        if (theSong->title != NULL)
            newSong.title = theSong->title;
        else
            newSong.title = "";
		
        if (theSong->album != NULL)
            newSong.album = theSong->album;
        else
            newSong.album = "";
		
        if (theSong->track != NULL)
            newSong.track = theSong->track;
        else
            newSong.track = "";
		
        if (theSong->disc != NULL)
            newSong.disc = theSong->disc;
        else
            newSong.disc = "";
		
        if (theSong->date != NULL)
            newSong.date = theSong->date;
        else
            newSong.date = "";
		
        if (theSong->genre != NULL)
            newSong.genre = theSong->genre;
        else
            newSong.genre = "";
		
        if (theSong->composer != NULL)
            newSong.composer = theSong->composer;
        else
            newSong.composer = "";
		
        if (theSong->performer != NULL)
            newSong.performer = theSong->performer;
        else
            newSong.performer = "";
		
        if (theSong->comment != NULL)
            newSong.comment = theSong->comment;
        else
            newSong.comment = "";
    }
    return newSong;
}

ustring gm_mpdCom::get_version()
{
	ustring	version = "";
	if (conn == NULL)
		return version;
	
    version += into_string(conn->version[0]) + "." + into_string(conn->version[1]);
    // version += into_string(conn->version[2]); // this reports the API version, which differs from the main MPD version
	return version;
}

std::string gm_mpdCom::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}

gm_mpdCom::~gm_mpdCom()
{

}
