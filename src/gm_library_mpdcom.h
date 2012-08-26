/*
 * library_mpdCom.h
 *  GUIMUP mpd communicator class for the library window
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

#ifndef TR_MPDCOM_H
#define TR_MPDCOM_H

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
#include "gm_commandlist.h"
#include "gm_itemlist.h"

#include <sys/stat.h>
#include <fcntl.h>

class library_mpdCom {

  public:
//  functions
    library_mpdCom();
    virtual ~library_mpdCom ();
    bool mpd_connect(ustring, int, ustring);
	void mpd_disconnect();
    // mpd commands
    void play_this(int, bool stream = false);
    void data_update();
	void clear_list();
	void shuffle_list();
	bool save_list(ustring);
	void execute_cmds(gm_commandList, bool resetlist = true);
	void load_savedplaylist(ustring);
	int get_listChange();
	int get_version();
    void set_random(bool);
    void set_repeat(bool);
    void set_single(bool);
    void set_consume(bool);
	gm_songList get_IDlist();
	gm_itemList get_artistlist();
	gm_itemList get_playlistlist();
	gm_itemList get_albumlist(ustring);
	gm_itemList get_folderlist(ustring);
	gm_itemList get_songlist(ustring, ustring);
	gm_itemList get_songsbytitle(ustring);
	gm_itemList get_songsbygenre(ustring);
	gm_itemList get_genrelist();
	listItem get_item_from(ustring filename);
	ustring get_date(ustring, ustring);
    // mpd config
    ustring get_musicPath();
    ustring get_playlistPath();
//  variables

//  signals
    sigc::signal<void, gm_songList> signal_songList;
	sigc::signal<void, int, int, int> signal_stats;
	sigc::signal<void, int, int> signal_songchange;
	sigc::signal<void> signal_update_ready;
	// library must notify the playlist & datatree when dis/reconnected
	sigc::signal<void, bool> signal_connected;

  protected:

  private:
//  functions
    songInfo get_songInfo_from(mpd_Song *theSong);
    bool statusCheck();
    bool mpd_reconnect();
	int get_plistMax();
    bool errorCheck(ustring);
	void get_statistics();
	void get_playlist();
	ustring get_string(ustring key);
	ustring into_string(int in);
	ustring fixTrackformat (ustring);
				
//  variables
    mpd_Connection *conn;
    sigc::connection statusLoop;
	sigc::connection reconnectLoop;
	int plistlength, plistMax;

    bool
        b_statCheckBusy,
        b_mpdconf_found,
        b_dbaseUpdating,
        b_connecting,
		b_shuffle_busy;
    int
        current_status,
        current_playlist,
        current_songNum,
		current_songID,
        serverPort;

    ustring
        serverName,
        serverPassword,
        mpdconf_path;
};

#endif // TR_MPDCOM_H
