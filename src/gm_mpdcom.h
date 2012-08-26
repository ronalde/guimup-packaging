/*
 *  gm_mpdcom.h
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

#ifndef GM_MPDCOM_H
#define GM_MPDCOM_H

#include <gtkmm/main.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <glibmm/stringutils.h>
#include <fstream>
#include <iostream>
    using std::cout;
    using std::endl;
#include <list>
#include <mpd/client.h>
#include <ctime>
#include <dirent.h>			// is_mpd_running()
#include <sys/stat.h>		// is_mpd_running()
#include "gm_songinfo.h"
#include "gm_config.h"
#include "gm_itemlist.h"
#include "gm_commandlist.h"


typedef struct gm_mpd_version
{
	int primary;
	int secundary;
}
gm_mpd_version;

typedef struct gm_output
{
	int id;
	bool enabled;
	ustring name;
}
gm_output;

typedef std::vector<gm_output> gm_outputList;

typedef std::vector<int> gm_IDlist;



class gm_mpdCom {

  public:
//  functions
    gm_mpdCom();
    virtual ~gm_mpdCom ();

    bool mpd_connect();
	void prev();
	void stop();
	void play(bool reload = false);
	void pause();
	void resume();
	void next();
	void play_this(int);
	void set_random(bool);
	void set_repeat(bool);
	void set_single(bool);
	void set_consume(bool);
	int get_xfade();
	int get_replaygain();
	void set_replaygain(int);
	void set_xfade(int);
	void set_volume(int);
	void volume_up(int);
	void volume_down(int);
	void set_seek(int);
	void rescan_folder(ustring);
	void update_all();
	gm_mpd_version get_version();
	gm_outputList get_outputs();
	void set_outputs(gm_outputList);
	gm_IDlist get_IDlist();
	void set_config(gm_Config*);
	void set_sInfo(gm_songInfo*);
	void configure();
	int get_listChanger();
	void execute_cmds(gm_commandList, bool);
	int execute_single_command(gm_cmdStruct, bool);
	void clear_list();
	void shuffle_list();
	void get_statistics();
	void get_playlist();
	bool save_playlist(ustring);
	bool save_selection(std::vector<ustring>, ustring);
	ustring get_album_year(ustring, ustring);
	gm_itemList get_albumlist(bool);
	gm_itemList get_album_searchlist(ustring);
	gm_itemList get_albums_for_artist(ustring);
	gm_itemList get_artistlist();
	gm_itemList get_artist_searchlist(ustring);
	gm_itemList get_playlistlist();
	gm_itemList get_folderlist(ustring);
	gm_itemList get_songlist(ustring, ustring, ustring);
	gm_itemList get_song_searchlist(ustring);
	gm_itemList get_genrelist(ustring);
	gm_itemList get_genre_artistlist(ustring findgenre);
	gm_itemList get_playlist_itemlist(ustring);
	gm_itemList get_yearlist();
	void mpd_disconnect(bool sendsignal = true);

    sigc::signal<void, mpd_status *> sgnl_statusInfo;
    sigc::signal<void> sgnl_newSong;
    sigc::signal<void, bool> sgnl_connected;		
	sigc::signal<void, gm_songList, int> sgnl_plistupdate;
	sigc::signal<void, int,int,int> sgnl_dbase_statistics;
	sigc::signal<void> sgnl_rescan_started;	
	sigc::signal<void> sgnl_update_ready;
	sigc::signal<void> sgnl_plistsaved;
		
  protected:

  private:
// functions
	bool statusCheck();
	bool mpd_reconnect();
	void get_songInfo_from(gm_songInfo *, mpd_song *);
	ustring get_musicPath();
	ustring get_playlistPath();
	ustring get_password();
	ustring get_host();
	int get_port();
	ustring get_string(ustring);
	bool errorCheck(ustring);
	ustring fixTrackformat (ustring);
	std::string into_string(int);
	// ustring time_to_string(time_t);
	bool is_mpd_running();
		
// variables
	mpd_connection *conn;
	sigc::connection statusLoop;
    sigc::connection reconnectLoop;
	gm_songInfo *current_sInfo;
	gm_Config *config;
		
	bool
		b_statCheckBusy,
		b_mpdconf_found,
		b_dbaseUpdating,
		b_reload,
		b_connecting,
		b_no_volume;
	
	int
		current_playlist,
		current_status,
		serverPort,
		current_volume,
		plist_lenght;
	
	ustring
		serverName,
		serverPassword,
		mpdconf_path;
};

#endif // GM_MPDCOM_H
