/*
* qm_mpdcom.h
*  QUIMUP MPD communicator class
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

#ifndef QM_MPDCOM_H
#define QM_MPDCOM_H

#include <unistd.h> 		// for usleep
#include <sys/stat.h>		// is_mpd_running()
#include <mpd/client.h>
#include <stdlib.h>
#include <QObject>
#include <QTimer>
#include <QList>
#include <QListIterator>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QProcess>
#include "qm_config.h"
#include "qm_songinfo.h"
#include "qm_itemlist.h"
#include "qm_commandlist.h"


typedef struct qm_output
{
	int id;
	bool enabled;
	QString name;
}
qm_output;

typedef QList<qm_output> qm_outputList;
typedef QList<int> qm_IDlist;

class qm_mpdCom : public QObject
{
	Q_OBJECT
	
public:
	qm_mpdCom();
	virtual ~qm_mpdCom ();
	
public slots:
	bool mpd_connect();
	void mpd_disconnect(bool sendsignal = true);
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
	void set_volume(int);
	void volume_up(int);
	void volume_down(int);
	void set_seek(int);
	void update_all();
	QString get_version();
	qm_outputList get_outputs();
	bool set_outputs(qm_outputList);
	void set_xfade(int);
	void set_config(qm_config*);
	void set_replaygain_mode(int);
	void configure();
	int get_listchanger();
	int execute_cmds(qm_commandList, bool);
	bool execute_single_command(qm_mpd_command, bool);
	void clear_list();
	void shuffle_list();
	void get_statistics();
	void reset_current_song();
	void show_messagebox(QString, QString);
	void set_sInfo(qm_songInfo*);
	QString save_playlist(QString);
	QString save_selection(QStringList, QString);
	QString get_album_year(QString, QString);
	qm_itemList get_albumlist(bool);
	qm_itemList get_album_searchlist(QString);
	qm_itemList get_albums_for_artist(QString);
	qm_itemList get_artistlist();
	qm_itemList get_artist_searchlist(QString);
	qm_itemList get_playlistlist();
	qm_itemList get_directorylist(QString);
	qm_itemList get_songlist(QString, QString, QString);
	qm_itemList get_song_searchlist(QString);
	qm_itemList get_genrelist(QString);
	qm_itemList get_genre_artistlist(QString findgenre);
	qm_itemList get_playlist_itemlist(QString);
	qm_itemList get_yearlist();
	qm_itemList get_dirsearchlist(QString);
	
signals:
	void sgnl_status(int, QString);
	void sgnl_status_info(mpd_status *);
	void sgnl_new_song();
	void sgnl_connected(bool);
	void sgnl_plistupdate(qm_songinfo, int);
	void sgnl_dbase_statistics(QString);
	void sgnl_update_ready();
	void sgnl_plistsaved();
	
protected:

private slots:
	bool status_check();
	bool mpd_reconnect();
	
private:
	//  functions
	void get_playlist();
	void get_songInfo_from(qm_songInfo *, mpd_song *);
	QString get_from_etcdefaultmpd();
	void find_config_files(bool);
	QString get_musicPath();
	QString get_password();
	QString get_host();
	int get_port();
	QString get_string(QString);
	bool error_check(QString);
	QString fix_trackformat (QString);
	bool is_mpd_running();
	void get_allsubdirs(QString, QString);
	//  variables
	mpd_connection *conn;
	QTimer *statusLoop;
	QTimer *reconnectLoop;
	qm_songInfo *current_songinfo;
	qm_config *config;
	qm_itemList dirsearchlist;
	
	bool
	b_statcheck_busy,
	b_mpdconf_found,
	b_dbase_updating,
	b_connecting,
	b_reload,
	b_no_volume,
	b_remote_server,
	b_autodetect;
	
	int
	current_playlist,
	current_songpos,
	current_songID,
	current_status,
	server_port,
	current_volume,
	plist_lenght;
	
	QString
	server_name,
	server_password,
	mpdconf_path;
};

#endif // QM_MPDCOM_H
