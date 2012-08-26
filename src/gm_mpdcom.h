/*
 *  gm_mpdcom.h
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

#ifndef GM_MPDCOM_H
#define GM_MPDCOM_H

#include <stdlib.h>
#include <glibmm/main.h>
#include <sigc++/sigc++.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <iostream>
    using std::cout;
    using std::endl;
#include <fstream>
#include <sstream>
#include <list>
#include "libmpdclient.h"
#include "gm_songlist.h"
#include <sys/types.h>

typedef std::list<mpd_OutputEntity> outdev_list;

struct statInfo
{
    int mpd_status;
    int mpd_time;
    int mpd_totalTime;
    int mpd_kHz;
    int mpd_kbps;
    int mpd_volume;
    bool mpd_random;
    bool mpd_repeat;
    bool mpd_single;
    bool mpd_consume;
};


class gm_mpdCom {

  public:
//  functions
    gm_mpdCom();
    virtual ~gm_mpdCom ();

    virtual bool mpd_connect(ustring host = "",
                                 int port = 0 ,
                             ustring pswd = "",
                                bool usem = false);
    // mpd commands
    void prev();
    void stop();
    void play(bool reload = false);
    void pause();
    void resume();
    void next();
    void set_volume(int);
	void volume_up(int);
	void volume_down(int);
    void set_seek(int);
	void updatefile(ustring);
    // mpd config
    ustring get_musicPath();
    ustring get_playlistPath();
    ustring get_tagEncoding();
    ustring get_user();
    // other
    ustring get_version();
    outdev_list getOutputs();
    void setOutputs(outdev_list oDevices);
	void mpd_disconnect();
//  variables

//  signals
    sigc::signal<void, int, ustring> signal_status;
    sigc::signal<void, statInfo> signal_statusInfo;
    sigc::signal<void, songInfo> signal_songInfo;
    // Player must notify 'library' and 'settings' when dis/reconnected
    sigc::signal<void, bool> signal_connected;
	sigc::signal<void, ustring, int, ustring> signal_host_port_pwd;

  protected:

  private:
//  functions
    songInfo get_songInfo_from(mpd_Song *theSong);
    bool statusCheck();
    bool mpd_reconnect();
    ustring get_string(ustring);
    ustring get_host();
    int get_port();
    ustring get_password();
    bool errorCheck(ustring);
    std::string into_string(int in);
//  variables
    mpd_Connection *conn;
    sigc::connection statusLoop;
    sigc::connection reconnectLoop;
    mpd_Song *theSong;

    bool
        b_statCheckBusy,
        b_mpdconf_found,
        b_dbaseUpdating,
        b_connecting,
        b_reload,
		b_no_volume;
    int
        current_playlist,
        current_songNum,
        current_songID,
        current_status,
        serverPort,
		current_volume;
    ustring
        serverName,
        serverPassword,
        mpdconf_path;
};

#endif // GM_MPDCOM_H
