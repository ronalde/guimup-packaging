/*
 *  qm_mpdCom.cpp
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

#include "qm_mpdcom.h"

qm_mpdCom::qm_mpdCom()
{
    if (objectName().isEmpty())
        setObjectName("mpdCom");
    conn =             NULL;
    config =           NULL;
    current_songinfo = NULL;
    b_reload =         true;
    b_autodetect =     false;
    b_mpdconf_found =  false;
    b_connecting =     false;
    b_statcheck_busy = false;
    b_dbase_updating = false;
    b_no_volume =      true;
    b_remote_server =  false;
    plist_lenght =     -10;
    current_playlist = -10;
    current_songID =   -10;
    current_songpos =  -10;
    current_status =   -10;
    current_volume =   -10;
    server_port =      -10;
    mpdconf_path =     "";
    server_name =      "";
    server_password =  "";

    statusLoop = new QTimer(this);
    connect(statusLoop, SIGNAL(timeout()), this, SLOT(status_check()));
    reconnectLoop = new QTimer(this);
    connect(reconnectLoop, SIGNAL(timeout()), this, SLOT(mpd_reconnect()));
}

// set a pointer to the parent's config
void qm_mpdCom::set_config(qm_config *conf)
{
    config = conf;
}


void qm_mpdCom::configure()
{
    b_autodetect = false;
    b_mpdconf_found = false;

    switch (config->profile)
    {
        case 0:
        {
            b_autodetect = true;
            // reset autodetected
            config->MPD_port_0 = 0;
            config->MPD_host_0 = "";
            config->MPD_password_0 = "";
        }
        case 1:
        {
            server_port = config->MPD_port_1;
            server_name = config->MPD_host_1;
            server_password = config->MPD_password_1;
            // just in case...
            server_password = server_password.left(server_password.lastIndexOf("@"));
            break;
        }
        case 2:
        {
            server_port = config->MPD_port_2;
            server_name = config->MPD_host_2;
            server_password = config->MPD_password_2;
            // just in case...
            server_password = server_password.left(server_password.lastIndexOf("@"));
            break;
        }
        case 3:
        {
            server_port = config->MPD_port_3;
            server_name = config->MPD_host_3;
            server_password = config->MPD_password_3;
            // just in case...
            server_password = server_password.left(server_password.lastIndexOf("@"));
            break;
        }
        case 4:
        {
            server_port = config->MPD_port_4;
            server_name = config->MPD_host_4;
            server_password = config->MPD_password_4;
            // just in case...
            server_password = server_password.left(server_password.lastIndexOf("@"));
            break;
        }
        default:
            break;
    }

    if (b_autodetect)
    {
        QFile file;

        mpdconf_path = get_from_etcdefaultmpd();

        file.setFileName(mpdconf_path);
        if (mpdconf_path.isEmpty() || !file.exists())
        {
            find_config_files(true);
        }
        else
            b_mpdconf_found = true;


        if (b_mpdconf_found)
        {
            server_port = get_port();
            if (!(server_port > 1024))
                server_port = 6600;
            server_name = get_host();
            if (server_name.isEmpty())
                server_name = "localhost";
            server_password = get_password();
        }
        else
        {
            server_port = 6600;
            server_name = "localhost";
            server_password = "";
            mpdconf_path = "";
        }
        config->MPD_port_0 = server_port;
        config->MPD_host_0 = server_name;
        config->MPD_password_0 = server_password;
    }

    // (in)validate server_name as an IP
    QRegExp rx("[0-9]{1,3}(.[0-9]{1,3}){3,3}");
    QRegExpValidator v(rx, 0);
    int pos = 0;

    if ( v.validate(server_name, pos) == 2 && server_name != "127.0.0.1" && server_name != "0.0.0.0")
    {
        printf ("Remote server address\n");
        b_remote_server = true;
    }
    else
    {
        if (server_name.contains("socket"))
            printf ("Local socket server address\n");
        else
            printf ("Local default server address\n");
        b_remote_server = false;
    }
}


bool qm_mpdCom::set_outputs(qm_outputList outputlist)
{
    if (conn == NULL)
        return false;

    bool wasplaying = false;
    if (current_status == MPD_STATE_PLAY)
    {
        wasplaying = true;
        pause();
    }

    bool b_success = true;

    QListIterator<qm_output> i(outputlist);
    while (i.hasNext())
    {
        qm_output output = i.next();
        if (output.enabled)
            mpd_send_enable_output(conn, output.id);
        else
            mpd_send_disable_output(conn, output.id);
        mpd_response_finish(conn);
        if (!error_check("set_outputs"))
            b_success = false;
    }

    if (wasplaying)
        resume();

    return b_success;
}


qm_outputList qm_mpdCom::get_outputs()
{
    qm_outputList outputlist;
    // get current output devices
    if (conn == NULL)
        return outputlist;

    mpd_output *output;
    mpd_send_outputs(conn);
    if (error_check("get_outputs"))
    {
        while ((output = mpd_recv_output(conn)))
        {
            qm_output qoutput;
            qoutput.id  = mpd_output_get_id(output);
            qoutput.enabled  = mpd_output_get_enabled(output);
            qoutput.name = QString::fromUtf8(mpd_output_get_name(output));
            outputlist.push_back(qoutput);
            mpd_output_free(output);
        }
    }
    mpd_response_finish(conn);

    return outputlist;
}


QString qm_mpdCom::get_musicPath()
{
    QString result = "";
    bool b_success = false;
    // 'config' command only works with socket connection
    if (config->mpd_config_allowed && config->mpd_socket_conn)
    {
        mpd_send_command(conn, "config", NULL);
        struct mpd_pair *pair = mpd_recv_pair(conn);
        if (pair != NULL)
        {
            result = pair->value;
            printf ("MPD's music dir : %s\n",(const char*)result.toUtf8() );
            b_success = true;
        }
        else
        {
            printf ("MPD's music dir : 'config' command failed!\n");
        }
        mpd_return_pair(conn, pair);
        mpd_response_finish(conn);
        mpd_connection_clear_error(conn);
    }

    if (!b_success)
    {
        if (b_autodetect)
        {
            if (b_mpdconf_found)
            {
                printf ("MPD's music dir : looking in %s\n", (const char*)mpdconf_path.toUtf8());
                result = get_string("music_directory");
                if (!result.isEmpty())
                    printf ("MPD's music dir : %s\n", (const char*)result.toUtf8());
            }
        }
        else
        {
            mpdconf_path = get_from_etcdefaultmpd();

            if (mpdconf_path.isEmpty())
            {
                find_config_files(false);
                if (b_mpdconf_found)
                {
                    printf ("MPD's music dir : looking in %s\n", (const char*)mpdconf_path.toUtf8());
                    result = get_string("music_directory");
                    if (!result.isEmpty())
                        printf ("MPD's music dir : %s\n", (const char*)result.toUtf8());
                }
            }
            else
            {
                b_mpdconf_found = true;
                printf ("MPD's music dir : looking in %s\n", (const char*)mpdconf_path.toUtf8());
                result = get_string("music_directory");
                if (!result.isEmpty())
                        printf ("MPD's music dir : %s\n", (const char*)result.toUtf8());
            }
        }
    }

    return result;
}


QString qm_mpdCom::get_host()
{
    QString result = get_string("bind_to_address");
    return result;
}


int qm_mpdCom::get_port()
{
    QString get = get_string("port")
                  ;
    return get.toInt();
}


QString qm_mpdCom::get_password()
{
    QString result = get_string("password");
    result = result.left(result.lastIndexOf("@"));
    return result;
}


QString qm_mpdCom::get_string(QString key)
{
    if (!b_mpdconf_found)
        return "";

    QFile file(mpdconf_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed(); // trim trailing & leading spaces
        if ( !line.startsWith("#") && line.contains(key, Qt::CaseInsensitive) )
        {
            QString result = line.section('"', 1, 1);
            return result.simplified();
            break;
        }
    }

    file.close();
    return "";
}


QString qm_mpdCom::get_from_etcdefaultmpd()
{
    QFile file;
    QString line = "";

    file.setFileName("/etc/default/mpd");
    if ( !file.exists() )
    {
        printf ("MPD's config : Tried /etc/default/mpd but it does not exist\n");
        return line;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printf ("MPD's config : Tried /etc/default/mpd but it is not readable\n");
        return line;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        line = in.readLine().trimmed(); // trim trailing & leading spaces
        if ( !line.startsWith("#") && line.contains("MPDCONF") )
        {
            line = line.right(line.size() - (line.lastIndexOf("MPDCONF=")+8));
            break;
        }
    }
    file.close();

    if (line.isEmpty())
        printf ("MPD's config : Tried /etc/default/mpd but MPDCONF is not set\n");
    else
        printf ("MPD's config found in /etc/default/mpd : %s\n", (const char*)line.toUtf8());

    return line;
}


void qm_mpdCom::update_all()
{
    if (conn == NULL)
    {
        b_dbase_updating = false;
        emit sgnl_update_ready();
        return;
    }

    if (config->mpd_update_allowed)
    {
        b_dbase_updating = true;
        current_playlist++; // prevent listupdate
        mpd_send_update(conn, (const char*)"/");
        mpd_response_finish(conn);
        if (error_check("update_all"))
            printf("Updating MPD database ... \n");
        else
        {
            printf("Updating MPD database : error!\n");
            b_dbase_updating = false;
        }
    }
    else
    {
        b_dbase_updating = false;
        emit sgnl_update_ready();
        printf ("Updating MPD database : not allowed\n");
        emit sgnl_update_ready();
    }
}


bool qm_mpdCom::mpd_connect()
{
    // disconnect if connected
    if (conn != NULL)
        mpd_disconnect(false);

    b_connecting = true;

    // no server, no go.
    if (server_name.isEmpty() && b_autodetect) // DEBUG : remove "&& b_autodetect"?
    {
        server_name = "localhost";
        printf ("No host name : using \"localhost\"\n");
    }

    // port number must be > 1024
    if (!(server_port > 1024))
    {
        printf ("Invalid port : %i [using 6600]\n", server_port);
        server_port = 6600;
    }

    // if local connection
    if (!b_remote_server)
    {
        if (!is_mpd_running())
        {
            printf ("MPD does not appear to be running\n");
            if (config->startMPD_onstart)
            {
                    printf ("Starting MPD, as configured : ");
                    QProcess *proc;
                    proc->startDetached(config->onstart_mpd_command);

                    // allow MPD to get ready
                    int sleepcount = 0;
                    bool success = false;
                    while (sleepcount < 6)
                    {
                        if (!is_mpd_running())
                            usleep(100000);
                        else
                        {
                            success = true;
                            break;
                        }
                        sleepcount++;
                    }

                    if (success)
                        printf ("succeeded!\n");
                    else
                    {
                        printf ("failed!\n");
                        emit sgnl_connected(false);
                        return false;
                    }
            }
            else
            {
                printf ("Not starting MPD, as configured\n");
                emit sgnl_connected(false);
                return false;
            }
        }
        else
            printf ("MPD is already running\n");
    }

    conn = mpd_connection_new( server_name.toUtf8(), server_port ,30000);

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

    // If connection failed try 4 more times at 0.5 sec intervals
    // (in case MPD is starting up)
    usleep(500000);
    int count = 1;
    while (conn == NULL && count < 5)
    {
        // wait 250 msec
        usleep(500000);

        // try again
        conn = mpd_connection_new( server_name.toUtf8(), server_port ,30000);
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
        printf ("Connection failed (tried %i times)\n", count);
        b_connecting = false;
        emit sgnl_connected(false);
        return false;
    }
    else
        printf ("Connected to MPD on try %i\n", count);

    bool b_status_allowed = false;
    char *command;
    mpd_pair *pair;

    // First let's tickle MPD
    mpd_send_allowed_commands (conn);

    while ( (pair = mpd_recv_command_pair(conn)) != NULL)
    {
        command = (char*)pair->value;
        if (!strcmp(command, "status"))
        {
            b_status_allowed = true;
            mpd_return_pair(conn, pair);
            break;
        }
        else
            mpd_return_pair(conn, pair);
    }

    mpd_response_finish(conn);


    if (!server_password.isEmpty())
    {
        if (!b_status_allowed) // password may be required
        {
            mpd_send_password(conn, server_password.toUtf8());
            mpd_response_finish(conn);
            if ( mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS )
            {
                printf ("Error : %s\n", mpd_connection_get_error_message(conn));
                mpd_connection_clear_error(conn);
                show_messagebox(tr("Provided password refused by MPD"), tr("Check out the 'Connect' tab in the 'settings' window."));
            }
            else
                printf("Password accepted\n");
        }
        else
            printf("Password provided but not used\n");
    }

    config->reset_temps();

    printf("Disallowed MPD commands : ");
    mpd_send_disallowed_commands (conn);
    int i = 0;
    while ( (pair = mpd_recv_command_pair(conn)) != NULL)
    {
        command = (char*)pair->value;
        printf(" %s ", command);
        mpd_return_pair(conn, pair);
        i++;
    }

    if (i == 0)
        printf("none reported\n");
    else
        printf("\n");

    mpd_send_allowed_commands(conn);
    while ( (pair = mpd_recv_command_pair(conn)) != NULL)
    {
        command = (char*)pair->value;
        // printf("command : %s\n", command);
        if (!strcmp(command, "config"))
            config->mpd_config_allowed = true;
        if (!strcmp(command, "status"))
            b_status_allowed = true;
        if (!strcmp(command, "outputs"))
            config->mpd_op_listall_allowed = true;
        if (!strcmp(command, "disableoutput"))
            config->mpd_op_disble_allowed = true;
        if (!strcmp(command, "enableoutput"))
            config->mpd_op_enable_allowed = true;
        if (!strcmp(command, "update"))
            config->mpd_update_allowed = true;
        if (!strcmp(command, "deleteid"))
            config->mpd_deleteid_allowed = true;
        if (!strcmp(command, "rm"))
            config->mpd_rm_allowed = true;
        if (!strcmp(command, "save"))
            config->mpd_save_allowed = true;
        if (!strcmp(command, "single"))
            config->mpd_single_allowed = true;
        if (!strcmp(command, "consume"))
            config->mpd_consume_allowed = true;
        if (!strcmp(command, "crossfade"))
            config->mpd_xfade_allowed = true;
        if (!strcmp(command, "repeat"))
            config->mpd_repeat_allowed = true;
        if (!strcmp(command, "rescan"))
            config->mpd_rescan_allowed = true;
        if (!strcmp(command, "random"))
            config->mpd_random_allowed = true;
        if (!strcmp(command, "shuffle"))
            config->mpd_shuffle_allowed = true;
        if (!strcmp(command, "clear"))
            config->mpd_clear_allowed = true;
        if (!strcmp(command, "stats"))
            config->mpd_stats_allowed = true;
        if (!strcmp(command, "replay_gain_mode")) // set
            config->mpd_setrpgain_allowed = true;
        //if (!strcmp(command, "replay_gain_status")) // get
        //	config->mpd_getrpgain_allowed = true;
        mpd_return_pair(conn, pair);
    }

    if (!b_status_allowed)
    {
        mpd_disconnect(true);
        printf ("No permission to get MPD's status. Is a (different) Password required?. Connection closed.\n");
        show_messagebox(tr("No permission to get MPD's status"), tr("Possibly a (different) password is required. Check out the 'Connect' tab in the 'settings' window."));
        return false;
    }

    // we're in business
    b_connecting = false;
    if (error_check("mpd_connect"))
        printf ("MPD errors: none reported\n");

    if ( server_name.contains("socket") )
        config->mpd_socket_conn = true;

    // First get the musicpath
    switch (config->profile)
    {
        case 0:
        {
             config->mpd_musicpath = get_musicPath();
             break;
        }
        case 1:
        {
            config->mpd_musicpath = config->MPD_musicdir_1;
            break;
        }
        case 2:
        {
            config->mpd_musicpath = config->MPD_musicdir_2;
            break;
        }
        case 3:
        {
            config->mpd_musicpath = config->MPD_musicdir_3;
            break;
        }
        case 4:
        {
            config->mpd_musicpath = config->MPD_musicdir_4;
            break;
        }
        default:
        {
            config->mpd_musicpath = get_musicPath();
            break;
        }
    }

    // Next verify the musicpath
    if (!config->mpd_musicpath.isEmpty())
    {
        if (!config->mpd_musicpath.endsWith("/"))
            config->mpd_musicpath.append("/");

        QDir dir(config->mpd_musicpath);
        if (!dir.exists())
        {
            printf ("MPD's music dir : does not exist\n");
            config->mpd_musicpath_status = 3; //"Music dir does not exist!";
        }
        else
        if (!dir.isReadable())
        {
            printf ("MPD's music dir : appears to be read-only\n");
            config->mpd_musicpath_status = 2; // "Music dir is read-only!";
        }
        else
        {
            printf ("MPD's music dir : appears to be accessible\n");
            config->mpd_musicpath_status = 1; // "Music dir is accessible to quimup";
        }
    }
    else
    {
        config->mpd_musicpath_status = 0; //"Music dir could not be determined";
        printf ("MPD's music dir : could not be determined\n");
    }

    // Next signal connected
    emit sgnl_connected(true);

    status_check();
    // poll MPD every 200 msec
    statusLoop->start(200);

    return true;
}


bool qm_mpdCom::mpd_reconnect()
{
    // previous loop has not finished
    if (b_connecting)
        return true;

    // try to connect
    b_connecting = true;

    conn = mpd_connection_new( server_name.toUtf8(), server_port ,30000);

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

    // login if there is a password
    if (!server_password.isEmpty())
    {
        mpd_send_password(conn, server_password.toUtf8());
        mpd_response_finish(conn);
    }

    // connected!
    printf ("Reconnected\n");
    reconnectLoop->stop();
    b_connecting = false;
    emit sgnl_connected(true);
    status_check();
    // poll MPD every 200 msec
    statusLoop->start(200);
    return false;
}


void qm_mpdCom::prev()
{
    if (conn == NULL)
        return;
    mpd_send_previous(conn);
    mpd_response_finish(conn);
    error_check("prev");
}

void qm_mpdCom::stop()
{
    if (conn == NULL)
        return;
    mpd_send_stop(conn);
    mpd_response_finish(conn);
    error_check("stop");
}

void qm_mpdCom::play(bool reload)
{
    if (conn == NULL)
        return;
    if (reload)
        b_reload = true;
    mpd_send_play(conn);
    mpd_response_finish(conn);
    error_check("play");
}

void qm_mpdCom::play_this(int ID)
{
    if (conn == NULL)
        return;
    mpd_send_play_id(conn, ID);
    mpd_response_finish(conn);
    error_check("play_this");
}

void qm_mpdCom::resume()
{
    if (conn == NULL)
        return;
    mpd_send_pause(conn, 0);
    mpd_response_finish(conn);
    error_check("resume");
}

void qm_mpdCom::pause()
{
    if (conn == NULL)
        return;
    mpd_send_pause(conn, 1);
    mpd_response_finish(conn);
    error_check("pause");
}

void qm_mpdCom::next()
{
    if (conn == NULL)
        return;
    mpd_send_next(conn);
    mpd_response_finish(conn);
    error_check("next");
}

void qm_mpdCom::set_random(bool status)
{
    if (conn == NULL)
        return;
    mpd_send_random(conn, status);
    mpd_response_finish(conn);
    error_check("set_random");
}

void qm_mpdCom::set_repeat(bool status)
{
    if (conn == NULL)
        return;
    mpd_send_repeat(conn, status);
    mpd_response_finish(conn);
    error_check("set_repeat");
}


void qm_mpdCom::set_single(bool status)
{
    if (conn == NULL)
        return;
    mpd_send_single(conn, status);
    mpd_response_finish(conn);
    error_check("set_single");
}


void qm_mpdCom::set_consume(bool status)
{
    if (conn == NULL)
        return;
    mpd_send_consume(conn, status);
    mpd_response_finish(conn);
    error_check("set_consume");
}

void qm_mpdCom::volume_up(int i)
{
    if (conn == NULL || b_no_volume)
        return;
    int vol = current_volume + i;
    if (vol > 100)
        vol = 100;
    mpd_send_set_volume(conn, vol);
    mpd_response_finish(conn);
    error_check("volume_up");
}

void qm_mpdCom::volume_down(int i)
{
    if (conn == NULL || b_no_volume)
        return;
    int vol = current_volume - i;
    if (vol < 0)
        vol = 0;
    mpd_send_set_volume(conn, vol);
    mpd_response_finish(conn);
    error_check("volume_down");
}

void qm_mpdCom::set_volume(int vol)
{
    if (conn == NULL || b_no_volume)
        return;
    mpd_send_set_volume(conn, vol);
    mpd_response_finish(conn);
    error_check("set_volume");
}

void qm_mpdCom::set_seek(int percent)
{
    if (conn == NULL)
        return;
    mpd_send_seek_pos(conn, current_songpos, percent);
    mpd_response_finish(conn);
    error_check("set_seek");
}


bool qm_mpdCom::error_check(QString action)
{
    if (conn == NULL)
    {
        mpd_disconnect(false);
        emit sgnl_connected(false);
        printf ("Error : on \"" + action.toUtf8() + "\": %s\n", mpd_connection_get_error_message(conn));
        printf ("Reconnecting...\n");
        // try to reconnect every 2 seconds
        reconnectLoop->start(2000);
        return false;
    }

    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
    {
        switch (mpd_connection_get_error(conn))
        {
            case MPD_ERROR_OOM:
            {
                printf ("MPD: Out of memory\n");
                break;
            }
                case MPD_ERROR_ARGUMENT:
            {
                printf ("MPD: Function called with an unrecognized or invalid argument\n");
                break;
            }
                case MPD_ERROR_STATE:
            {
                printf ("MPD: Called function is not available\n");
                break;
            }
                case MPD_ERROR_TIMEOUT:
            {
                printf ("MPD: Timeout\n");
                break;
            }
                case MPD_ERROR_SYSTEM:
            {
                printf ("MPD: System error\n");
                break;
            }

            case MPD_ERROR_RESOLVER:
            {
                printf ("MPD: Unknown host\n");
                break;
            }

            case MPD_ERROR_MALFORMED:
            {
                printf ("MPD: Malformed response received\n");
                break;
            }

            case MPD_ERROR_CLOSED:
            {
                printf ("MPD: Connection closed\n");
                break;
            }
                default:
                printf ("MPD: Returned an error code\n");
        }
        // If it is a non-fatal error (no permission etc.).
        if (mpd_connection_get_error(conn) != MPD_ERROR_CLOSED)
        {
            // A "broken pipe" error means the connection is lost (which really is quite fatal)
            // "reset by peer" error means the user killed mpd (which is also quite fatal)
            QString errormsg = mpd_connection_get_error_message(conn);
            printf ("Non fatal error : on \"" + action.toUtf8() + "\": %s\n", mpd_connection_get_error_message(conn));
            if ( !errormsg.contains("Broken pipe", Qt::CaseInsensitive) && !errormsg.contains("reset by peer", Qt::CaseInsensitive) )
            {

                mpd_connection_clear_error(conn);
                mpd_response_finish(conn);
                return true;
            }
            // else
            // no return. proceed as 'fatal'
        }
        else
            // A fatal error occurred: disconnnect & reconnect
            printf ("Fatal error : on \"" + action.toUtf8() + "\": %s\n", mpd_connection_get_error_message(conn));

        printf ("Reconnecting...\n");
        mpd_disconnect(false);
        emit sgnl_connected(false);
        // reconnect every 2 seconds
        reconnectLoop->start(2000);
        return false;
    }

    // No problems
    mpd_connection_clear_error(conn);
    return true;
}


void qm_mpdCom::set_xfade(int secs)
{
    if (conn == NULL)
        return ;
    mpd_send_crossfade(conn, secs);
    mpd_response_finish(conn);
    error_check("set_xfade");
}


bool qm_mpdCom::status_check()
{
    // return "false" terminates the loop
    if (!error_check("Starting status_check"))
        return false;

    // "true" keeps the loop going
    if (b_statcheck_busy)
        return true;

    b_statcheck_busy = true;

    // Get the current server status
    mpd_send_status(conn);

    mpd_status * servStatus = mpd_recv_status(conn);

    mpd_response_finish(conn);

    if (!error_check("mpd_recv_status") || servStatus == NULL)
    {
        b_statcheck_busy = false;
        if (servStatus != NULL && conn != NULL)
        {
            mpd_status_free(servStatus);
            mpd_response_finish(conn);
        }
        return true;
    }

    // server has not found a supported mixer
    if (mpd_status_get_volume(servStatus) == -1)
    {
        if (!b_no_volume)
        {
            printf ("MPD : No supported mixer found\n");
            b_no_volume = true;
        }
    }
    else
        b_no_volume = false;

    // database update was requested && has finished
    if (b_dbase_updating && mpd_status_get_update_id(servStatus) == 0)
    {
        get_statistics();
        b_dbase_updating = false;
        printf ("Update/Rescan : ready\n");
        current_playlist = -1; // trigger list update
        current_songID = -1;   // trigger reloading the song
        emit sgnl_update_ready();
    }

    // playlist was updated
    if (current_playlist < (int)mpd_status_get_queue_version(servStatus) )
    {
        current_playlist = mpd_status_get_queue_version(servStatus);
        printf ("Playlist : updated\n");
        get_playlist();
        // reload the song (streams use this to update song info)
        b_reload = true;
    }

    struct mpd_song *song = NULL;
    // the song changed, or new playlist
    if( (current_songID != mpd_status_get_song_id(servStatus))  || b_reload)
    {
        b_reload = false;
        // get the new song.
        mpd_entity * entity = NULL;
        mpd_send_current_song(conn);
        entity = mpd_recv_entity(conn);
        mpd_response_finish(conn);

        // empty list, last has finished or first hasn't started
        if (entity == NULL || !error_check("status_check : mpd_send_current_song"))
        {
            current_songpos = -1;
            current_songID = -1;
            song = NULL;
        }
        else
        {
            current_songpos = mpd_status_get_song_pos(servStatus);
            current_songID = mpd_status_get_song_id(servStatus);
            song = mpd_song_dup (mpd_entity_get_song(entity));
        }

        // free memory
        if (entity != NULL)
            mpd_entity_free(entity);

        // collect & send the song info
        get_songInfo_from(current_songinfo, song);
        emit sgnl_new_song();
    }

    // send the status info
    emit sgnl_status_info(servStatus);

    current_status = mpd_status_get_state(servStatus);
    current_volume = mpd_status_get_volume(servStatus);

    // free memory
    if (servStatus != NULL)
        mpd_status_free(servStatus);
    if (song != NULL)
        mpd_song_free (song);

    // done
    b_statcheck_busy = false;
    return true;
}

void qm_mpdCom::get_songInfo_from(qm_songInfo *sInfo, mpd_song *theSong)
{

    if (theSong == NULL)
        sInfo->nosong = true;
    else
    {
        sInfo->nosong = false;

        sInfo->file = QString::fromUtf8(mpd_song_get_uri(theSong));

        if (sInfo->file.startsWith("http://"))
        {
            sInfo->type = TP_STREAM;
            sInfo->time = 0;
        }
        else
        {
            if (sInfo->file.startsWith("/")) // URL relative to MPD music dir doesn't have a leading '/'
                sInfo->type = TP_SONGX;
            else
                sInfo->type = TP_SONG;

            sInfo->time = mpd_song_get_duration(theSong);
        }

        sInfo->songID  = mpd_song_get_id(theSong);
        sInfo->songNr  = mpd_song_get_pos(theSong);

        sInfo->name  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_NAME, 0));
        sInfo->artist  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_ARTIST, 0));
        sInfo->title  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_TITLE, 0));
        sInfo->album  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_ALBUM, 0));
        sInfo->track  = fix_trackformat(QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_TRACK, 0)) );
        sInfo->disc  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_DISC, 0));
        sInfo->date  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_DATE, 0));
        // convert xxxx-xx-xx format to xxxx
        sInfo->date = sInfo->date.left(4);
        sInfo->genre  = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_GENRE, 0));
        sInfo->comment = QString::fromUtf8(mpd_song_get_tag(theSong, MPD_TAG_COMMENT, 0));
    }
}

QString qm_mpdCom::get_version()
{
    QString version = "";
    if (conn == NULL)
        return version;

    version += QString::number(mpd_connection_get_server_version(conn)[0]) + ".";
    version += QString::number(mpd_connection_get_server_version(conn)[1]) ;

    if ( mpd_connection_get_server_version(conn)[0] == 0 && mpd_connection_get_server_version(conn)[1] < 16)
        show_messagebox( tr("Connected to MPD") + " " + version, "\n" + tr("MPD 0.16 or above is required.") + "\n" + tr("Some functions will not work!") );

    return version;
}


void qm_mpdCom::get_playlist()
{
    qm_songinfo newPlayList;
    plist_lenght = 0;

    if (conn == NULL)
    {
        emit sgnl_plistupdate(newPlayList, -10);
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
            qm_songInfo *newSongInfo = new qm_songInfo();
            get_songInfo_from(newSongInfo, song);
            newPlayList.push_back(*newSongInfo);
            plist_lenght++;
        }
        mpd_entity_free(entity);
    }
    mpd_response_finish(conn);
    error_check("get_playlist");
    if (song != NULL)
        mpd_song_free (song);

    int changedID = get_listchanger();
    // signal the library window
    emit sgnl_plistupdate(newPlayList, changedID);
    newPlayList.clear();
}


bool qm_mpdCom::execute_single_command(qm_mpd_command command, bool resetlist)
{
    bool result = false;
    if (conn == NULL)
        return result;
    /*
       the mpd_run commands include an error_check and return FALSE on non-fatal
       errors such as "No such song" (e.g. when trying to add a non-audio file)
    */
    switch (command.cmd)
    {
        case CMD_ADD:
        {
            current_playlist++; // prevent list update
            if (mpd_run_add(conn, (const char*)command.file.toUtf8()))
            {
                plist_lenght++;
                result = true;
            }
            break;
        }

        case CMD_INS:
        {
            // add
            current_playlist++; // prevent list update
            if (mpd_run_add(conn, (const char*)command.file.toUtf8()))
            {
                plist_lenght++;
                result = true;
                // move into position
                current_playlist++; // prevent list update
                mpd_run_move(conn, plist_lenght-1, command.moveTo);
            }
            break;
        }
    } // <- switch

    mpd_response_finish(conn);
    mpd_connection_clear_error(conn);
    // No error_check: we silently ignore txt files and such
    if (resetlist)
        current_playlist = -1;
    return result;
}


// cmdlist must start with the highest songnr!
int qm_mpdCom::execute_cmds(qm_commandList cmdlist, bool resetlist)
{
    if(cmdlist.empty() ||conn == NULL)
        return 0;

    int results = 0;
    mpd_command_list_begin(conn, true);

    QListIterator<qm_mpd_command> itr(cmdlist);
    while (itr.hasNext())
    {
        qm_mpd_command curCommand = itr.next();

        switch (curCommand.cmd)
        {
            case CMD_DPL: // delete stored playlist
            {
                if ( mpd_send_rm(conn, (const char*)curCommand.file.toUtf8()) )
                    results++;
                break;
            }

            case CMD_ADD:
            {
                current_playlist++; // prevent list update
                if (mpd_send_add(conn, (const char*)curCommand.file.toUtf8()) )
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
                if (mpd_send_add(conn, (const char*)curCommand.file.toUtf8()))
                {
                    plist_lenght++;
                    results++;
                    // move into position
                    current_playlist++; // prevent list update
                    mpd_send_move(conn, plist_lenght-1, curCommand.moveTo);
                }
                break;
            }

            case CMD_MOV: // move ID to pos
            {
                current_playlist++; // prevent list update
                if (mpd_send_move_id(conn, curCommand.songid, curCommand.moveTo))
                    results++;
                break;
            }

            case CMD_POS: // move from pos to pos
            {
                current_playlist++; // prevent list update
                if (mpd_send_move(conn, curCommand.songnr, curCommand.moveTo))
                    results++;
                break;
            }

            case CMD_DEL:
            {
                current_playlist++; // prevent list update
                if (mpd_send_delete_id(conn, curCommand.songid))
                {
                    results++;
                    plist_lenght--;
                }
                break;
            }

            case CMD_SCN:
            {
                b_dbase_updating = true;
                current_playlist++; // prevent list update
                if (mpd_send_command(conn, "rescan", (const char*)curCommand.file.toUtf8(), NULL))
                {
                    results++;
                }
                break;
            }

            case CMD_UPD:
            {
                b_dbase_updating = true;
                current_playlist++; // prevent list update
                if (mpd_send_command(conn, "update", (const char*)curCommand.file.toUtf8(), NULL))
                {
                    results++;
                }
                break;
            }

            default:
                break;
        } // end switch
    }
    mpd_command_list_end(conn);
    mpd_response_finish(conn);
    error_check("execute_cmds");

    cmdlist.clear();

    if (results > 0)
    {
        current_songID = -1;
        current_songpos = -1;
        current_status = -1;
        if (resetlist)
            current_playlist = -1;
    }
    return results;
}


void qm_mpdCom::clear_list()
{
    if (conn == NULL)
        return;
    mpd_send_clear(conn);
    mpd_response_finish(conn);
    error_check("clear_list");
    current_songID = -1;
    current_songpos = -1;
    current_status = -1;
    current_playlist = -1;
}


void qm_mpdCom::shuffle_list()
{
    if (plist_lenght < 2 || conn == NULL)
        return;

    mpd_run_shuffle(conn);
    mpd_response_finish(conn);
    error_check("shuffle_list 1");

    // move current song to the top
    if (current_songID != -1)
    {
        current_playlist++; // prevent reloading
        mpd_send_move_id(conn, current_songID, 0);
        mpd_response_finish(conn);
        error_check("shuffle_list 2");
    }
    // trigger reloading the list
    current_playlist = -1;
}


void qm_mpdCom::get_statistics()
{
    if (conn == NULL)
    {
        emit sgnl_dbase_statistics(tr("Data not available"));
        return;
    }

    mpd_stats *sData;
    mpd_send_stats(conn);
    sData = mpd_recv_stats(conn);
    mpd_response_finish(conn);
    if (!error_check("mpd_recv_stats") || sData == NULL)
    {
        emit sgnl_dbase_statistics(tr("Data not available"));
        if (sData != NULL)
            mpd_stats_free(sData);
        return;
    }

    QString statistics =  "<i>" + tr("Artists") + "</i> " + QString::number(mpd_stats_get_number_of_artists(sData)) +
                          "<i> &middot; " + tr("Albums") + "</i> "  + QString::number(mpd_stats_get_number_of_albums(sData)) +
                          "<i> &middot; " + tr("Songs")  + "</i> "  + QString::number(mpd_stats_get_number_of_songs(sData)) ;

    emit sgnl_dbase_statistics(statistics);

    if (sData != NULL)
        mpd_stats_free(sData);
}


QString qm_mpdCom::save_playlist(QString listname)
{
    if (conn == NULL)
        return "Error : MPD is not connected.";

    mpd_send_save(conn, listname.toUtf8() );
    mpd_response_finish(conn);

    emit sgnl_plistsaved();

    if (!error_check("save_playlist"))
        return "Error : " + QString::fromUtf8(mpd_connection_get_error_message(conn));
    else
        return "Playlist \"" + listname + "\" was successfully saved.";
}


QString qm_mpdCom::save_selection(QStringList tobeSaved, QString listname)
{
    if (conn == NULL)
        return "Error : MPD is not connected.";

    mpd_command_list_begin(conn, true);
    for (int i = 0; i < tobeSaved.size(); ++i)
    {
        mpd_send_playlist_add(conn, listname.toUtf8(), tobeSaved.at(i).toUtf8());
    }
    mpd_command_list_end(conn);
    mpd_response_finish(conn);

    emit sgnl_plistsaved();

    if (!error_check("save_selection"))
        return "Error : " + QString::fromUtf8(mpd_connection_get_error_message(conn));
    else
        return "Playlist \"" + listname + "\" was successfully saved.";
}


void qm_mpdCom::show_messagebox(QString message, QString info)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr(" "));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<b>" + message + " </b>");
    msgBox.setInformativeText(info);
    msgBox.exec();
}


qm_itemList qm_mpdCom::get_yearlist()
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    QString year = "*^*";
    QString newyear;
    QString album = "*^*";
    QString newalbum;

    struct mpd_song *song;
    mpd_send_list_all_meta(conn, "/");
    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_DATE, 0) != NULL )
            newyear = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_DATE, 0));
        else
            newyear = "";

        // convert xxx-xx-xx format toi xxxx
        if (!newyear.isEmpty())
        newyear = newyear.left(4) + " ";

        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ALBUM, 0));

            if (album != newalbum || year != newyear)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ARTIST, 0));
                if (newyear.isEmpty())
                {
                    newItem.sorter = "0000" + newalbum + newItem.artist;
                    newItem.year = "?";
                }
                else
                {
                    newItem.sorter = newyear + newalbum + newItem.artist;
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
    error_check("get_yearlist");

    return itemlist;
}

qm_itemList qm_mpdCom::get_albumlist(bool sort_by_date)
{
    qm_itemList itemlist;


    if (conn == NULL)
        return itemlist;

    int nowtime = (QDateTime::currentDateTime()).toTime_t();
    int nowtime_length = QString::number(nowtime).length();

    QString album = "*^*";
    QString newalbum;
    QString artist = "*^*";
    QString newartist;
    struct mpd_song *song;
    mpd_send_list_all_meta(conn, "/");
    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
            newartist = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ARTIST, 0));
        else
            newartist = "";

        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ALBUM, 0));

            if (album != newalbum || artist != newartist)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = newartist;
                newItem.sorter = newalbum + newartist;
                if (sort_by_date)
                {
                    QDateTime time;
                    time.setTime_t(mpd_song_get_last_modified(song));
                    newItem.moddate = time.toString("yy/MM/dd  hh:mm");
                    // invert the sort order

                    int oldlast = nowtime - time.toTime_t();
                    QString sort  = QString::number( oldlast );

                    while (sort.length() < nowtime_length)
                        sort = "0" + sort;

                    newItem.sorter = sort + newItem.sorter;
                }
                itemlist.push_back(newItem);
                album = newalbum;
                artist = newartist;
            }
        }

        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) == NULL)
        {
            newalbum = "";

            if (artist != newartist)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = newartist;
                newItem.sorter = newalbum + newartist;
                if (sort_by_date)
                {
                    QDateTime time;
                    time.setTime_t(mpd_song_get_last_modified(song));
                    newItem.moddate = time.toString("yy/MM/dd  hh:mm");
                    // invert the sort order

                    int oldlast = nowtime - time.toTime_t();
                    QString sort  = QString::number( oldlast );

                    while (sort.length() < nowtime_length)
                        sort = "0" + sort;

                    newItem.sorter = sort + newItem.sorter;
                }
                itemlist.push_back(newItem);
                album = newalbum;
                artist = newartist;
            }
        }

        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    error_check("get_albumlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_album_searchlist(QString searchfor)
{
    if (searchfor == "*^*")
        return get_albumlist(false);

    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    QString album = "*^*";
    QString newalbum;
    QString artist = "*^*";
    QString newartist;
    struct mpd_song *song;
    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM, searchfor.toUtf8());
    mpd_search_commit(conn);

    while ((song = mpd_recv_song (conn)))
    {
        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
            newartist = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ARTIST, 0));
        else
            newartist = "";

        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ALBUM, 0));

            if (album != newalbum || artist != newartist)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = newartist;
                newItem.sorter = newalbum + newartist;
                itemlist.push_back(newItem);
                album = newalbum;
                artist = newartist;
            }
        }
        else
            if (searchfor.isEmpty())
            {
                newalbum = "";

                if (!album.isEmpty() || artist != newartist)
                {
                    qm_listitemInfo newItem;
                    newItem.type = TP_ALBUM;
                    newItem.album = newalbum;
                    newItem.artist = newartist;
                    newItem.sorter = newalbum + newartist;
                    itemlist.push_back(newItem);
                    album = newalbum;
                    artist = newartist;
                }
            }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    error_check("get_album_searchlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_albums_for_artist(QString artist)
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    QString album = "*^*";
    QString newalbum;
    struct mpd_song *song;
    mpd_search_db_songs(conn, true);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, artist.toUtf8());
    mpd_search_commit(conn);

    while ((song = mpd_recv_song (conn)))
    {

        if (mpd_song_get_tag (song, MPD_TAG_ALBUM, 0) != NULL )
        {
            newalbum = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ALBUM, 0));

            if (album != newalbum)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ALBUM;
                newItem.album = newalbum;
                newItem.artist = artist;
                newItem.sorter = newalbum;
                itemlist.push_back(newItem);
                album = newalbum;
            }
        }
        else
        {
            newalbum = "";
            if (!album.isEmpty())
            {
                qm_listitemInfo newItem;
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
    error_check("get_album_searchlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_artistlist()
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    QString artist = "*^*";
    QString newartist;
    struct mpd_song *song;
    mpd_send_list_all_meta(conn, "/");
    while ((song = mpd_recv_song (conn)))
    {

        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
        {
            newartist = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ARTIST, 0));

            if (artist != newartist)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.sorter = newartist;
                if (config->ignore_leading_the && (newItem.sorter).startsWith("the ", Qt::CaseInsensitive) )
                    newItem.sorter = (newItem.sorter).right((newItem.sorter).size() - 4);
                itemlist.push_back(newItem);
                artist = newartist;
            }
        }
        else
        {
            newartist = "";

            if (!artist.isEmpty())
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.sorter = newartist;
                if (config->ignore_leading_the && (newItem.sorter).startsWith("the ", Qt::CaseInsensitive) )
                    newItem.sorter = (newItem.sorter).right((newItem.sorter).size() - 4);
                itemlist.push_back(newItem);
                artist = newartist;
            }
        }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    error_check("get_artistlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_artist_searchlist(QString searchfor)
{
    if (searchfor == "*^*")
        return get_artistlist();

    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    QString artist = "*^*";
    QString newartist;
    struct mpd_song *song;
    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, searchfor.toUtf8());
    mpd_search_commit(conn);

    while ((song = mpd_recv_song (conn)))
    {

        if (mpd_song_get_tag (song, MPD_TAG_ARTIST, 0) != NULL )
        {
            newartist = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_ARTIST, 0));

            if (artist != newartist)
            {
                qm_listitemInfo newItem;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.sorter = newartist;
                if (config->ignore_leading_the && (newItem.sorter).startsWith("the ", Qt::CaseInsensitive) )
                    newItem.sorter = (newItem.sorter).right((newItem.sorter).size() - 4);
                itemlist.push_back(newItem);
                artist = newartist;
            }
        }
        else
            if (searchfor.isEmpty())
            {
                newartist = "";

                if (!artist.isEmpty())
                {
                    qm_listitemInfo newItem;
                    newItem.type = TP_ARTIST;
                    newItem.artist = newartist;
                    newItem.sorter = newartist;
                    if (config->ignore_leading_the && (newItem.sorter).startsWith("the ", Qt::CaseInsensitive) )
                        newItem.sorter = (newItem.sorter).right((newItem.sorter).size() - 4);
                    itemlist.push_back(newItem);
                    artist = newartist;
                }
            }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    error_check("get_album_searchlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_playlistlist()
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    struct mpd_entity *ent;
    mpd_send_list_meta (conn, "/");
    while ((ent = mpd_recv_entity (conn)))
    {
        if (mpd_entity_get_type (ent) == MPD_ENTITY_TYPE_PLAYLIST)
        {
            qm_listitemInfo newItem;
            newItem.type = TP_PLAYLIST;
            const struct mpd_playlist *playlist = mpd_entity_get_playlist(ent);
            QString path = QString::fromUtf8(mpd_playlist_get_path (playlist));
            newItem.file = path;
            // put only the list name in 'name'
            newItem.name = path.section("/",-1);
            newItem.sorter = newItem.name;
            itemlist.push_back(newItem);
        }
        mpd_entity_free (ent);
    }
    mpd_response_finish (conn);
    error_check("get_playlistlist");
    return itemlist;
}


qm_itemList qm_mpdCom::get_dirsearchlist(QString searchfor)
{
    dirsearchlist.clear();
    get_allsubdirs("", searchfor);
    return dirsearchlist;
}

void qm_mpdCom::get_allsubdirs(QString filePath, QString searchfor)
{
    qm_itemList itemlist;
    if (conn == NULL)
        return;

        struct mpd_entity * entity;
    mpd_send_list_meta(conn, filePath.toUtf8());
    while( (entity = mpd_recv_entity(conn)) != NULL )
    {
        if( mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_DIRECTORY )
        {
            qm_listitemInfo newItem;
            const struct mpd_directory *directory = mpd_entity_get_directory(entity);
            newItem.file = QString::fromUtf8(mpd_directory_get_path (directory));
            newItem.name = (newItem.file).section("/",-1);
            itemlist.push_back(newItem);
            if ((newItem.name).contains(searchfor, Qt::CaseInsensitive))
            {
                newItem.type = TP_DIRECTORY;
                newItem.sorter = newItem.name;
                dirsearchlist.push_back(newItem);
            }
        }
        mpd_entity_free(entity);
    } // end while
    mpd_response_finish (conn);
    error_check("get_directorysearchlist");


    std::list<qm_listitemInfo>::iterator itr;
    for (itr = itemlist.begin(); itr != itemlist.end(); ++itr)
    {
            get_allsubdirs(itr->file, searchfor);
            //itemlist.splice(itemlist.end(),sublist); // append sublist to itemlist
    }
}


qm_itemList qm_mpdCom::get_directorylist(QString filePath)
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    struct mpd_entity * entity;
    mpd_send_list_meta(conn, filePath.toUtf8());
    while( (entity = mpd_recv_entity(conn)) != NULL )
    {
        if( mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_DIRECTORY )
        {
            qm_listitemInfo newItem;
            const struct mpd_directory *directory = mpd_entity_get_directory(entity);
            QString path = QString::fromUtf8(mpd_directory_get_path (directory));
            newItem.file = path;
            newItem.name = path.section("/",-1);
            newItem.type = TP_DIRECTORY;
            newItem.sorter = "0" + newItem.name; // "0" so dirs come first
            itemlist.push_back(newItem);
        }
        else
            if(mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG)
            {
                qm_listitemInfo newItem;
                const struct mpd_song * song = mpd_entity_get_song (entity);
                newItem.file = QString::fromUtf8(mpd_song_get_uri(song));
                newItem.type = TP_SONG; // never a stream here
                newItem.time = mpd_song_get_duration(song);
                newItem.name = (newItem.file).section("/",-1);
                newItem.sorter = "1" + newItem.name; // "1" so files come next
                newItem.artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
                newItem.album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
                newItem.title = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
                newItem.track = fix_trackformat(QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TRACK, 0)) );
                newItem.genre = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_GENRE, 0));
                itemlist.push_back(newItem);
            }
        //else // FIXME: allow *cue and other playlists
        //    if(mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_PLAYLIST)
        //    {
        //    }

        mpd_entity_free(entity);
    } // end while
    mpd_response_finish (conn);
    error_check("get_directorylist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_songlist(QString album, QString artist, QString genre)
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    struct mpd_song *song;
    mpd_search_db_songs(conn, true);
    if (artist != "*^*")
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, artist.toUtf8());
    if (album != "*^*")
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM, album.toUtf8());
    if (genre != "*^*")
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_GENRE, genre.toUtf8());
    mpd_search_commit(conn);

    while ( (song = mpd_recv_song(conn)) )
    {
        qm_listitemInfo newItem;
        newItem.file = QString::fromUtf8(mpd_song_get_uri(song));
        newItem.type = TP_SONG; // never a stream here
        newItem.time = mpd_song_get_duration(song);
        newItem.artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
        newItem.album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
        newItem.title = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
        newItem.genre = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_GENRE, 0));
        newItem.sorter = newItem.artist + newItem.album;
        newItem.disc = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_DISC, 0));
        if (!newItem.disc.isEmpty())
            newItem.sorter += newItem.disc;
        else
            newItem.sorter += "00000000"; // no disc tag comes 1st
        if (mpd_song_get_tag(song, MPD_TAG_TRACK, 0))
            newItem.track = fix_trackformat(QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TRACK, 0)) );
        // 0-999 tracks should be enough
        if (newItem.track.size() == 0)
            newItem.sorter += "000";
        else
        if (newItem.track.size() == 1)
            newItem.sorter += "00" + newItem.track;
        else
        if (newItem.track.size() == 2)
            newItem.sorter += "0" + newItem.track;
        else
            newItem.sorter += newItem.track;
        //
        if (newItem.title.isEmpty())
            newItem.sorter += newItem.file;
        else
            newItem.sorter += newItem.title;

        itemlist.push_back(newItem);
        mpd_song_free(song);
    }
    mpd_response_finish(conn);
    error_check("get_songlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_song_searchlist(QString searchfor)
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    struct mpd_song *song;
    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_TITLE, searchfor.toUtf8());
    mpd_search_commit(conn);

    while ((song = mpd_recv_song (conn)))
    {

        if (mpd_song_get_tag (song, MPD_TAG_TITLE, 0) != NULL )
        {
            qm_listitemInfo newItem;
            newItem.file = QString::fromUtf8(mpd_song_get_uri(song));
            newItem.type = TP_SONG; // never a stream here
            newItem.time = mpd_song_get_duration(song);
            newItem.artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
            newItem.album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
            newItem.title = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
            newItem.genre = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_GENRE, 0));

            if (mpd_song_get_tag(song, MPD_TAG_TRACK, 0))
            {
                newItem.track = fix_trackformat(QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TRACK, 0)) );
                newItem.sorter = newItem.artist + newItem.album + newItem.track + newItem.title;
            }
            else
                newItem.sorter = newItem.artist + newItem.album + "00" + newItem.title;
            if (newItem.title.isEmpty())
                newItem.sorter = newItem.sorter + newItem.file;

            itemlist.push_back(newItem);
        }
        else
            if (searchfor.isEmpty())
            {
                qm_listitemInfo newItem;
                newItem.file = QString::fromUtf8(mpd_song_get_uri(song));
                newItem.type = TP_SONG; // never a stream here
                newItem.time = mpd_song_get_duration(song);
                newItem.artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
                newItem.album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
                newItem.title = "";
                newItem.genre = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_GENRE, 0));

                if (mpd_song_get_tag(song, MPD_TAG_TRACK, 0))
                {
                    newItem.track = fix_trackformat(QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TRACK, 0)) );
                    newItem.sorter = newItem.artist + newItem.album + newItem.track + newItem.title;
                }
                else
                    newItem.sorter = newItem.artist + newItem.album + "00" + newItem.title;
                if (newItem.title.isEmpty())
                    newItem.sorter = newItem.sorter + newItem.file;

                itemlist.push_back(newItem);
            }
        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    error_check("get_song_searchlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_genre_artistlist(QString searchfor)
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    QString artist = "*^*";
    QString newartist;
    QString album = "*^*";
    QString newalbum;
    struct mpd_song *song;
    mpd_search_db_songs(conn, true);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_GENRE, searchfor.toUtf8());
    mpd_search_commit(conn);

    while ((song = mpd_recv_song (conn)))
    {
        QString genre = QString::fromUtf8(mpd_song_get_tag (song, MPD_TAG_GENRE, 0));
        if ( genre == searchfor )
        {
            newalbum = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
            newartist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
            if (artist != newartist || album != newalbum)
            {
                qm_listitemInfo newItem;
                newItem.genre = searchfor;
                newItem.type = TP_ARTIST;
                newItem.artist = newartist;
                newItem.album = newalbum;
                newItem.sorter = newItem.artist + newItem.album;
                itemlist.push_back(newItem);
                artist =newartist;
                album = newalbum;
            }
        }

        mpd_song_free (song);
    }
    mpd_response_finish (conn);
    error_check("get_genre_artistlist");

    return itemlist;
}


qm_itemList qm_mpdCom::get_genrelist(QString searchfor)
{
    qm_itemList itemlist;
    if (conn == NULL)
        return itemlist;

    if (searchfor == "*^*") // get full list
    {
        QString newgenre;
        struct mpd_pair *pair;
        mpd_search_db_tags(conn, MPD_TAG_GENRE);
        mpd_search_commit(conn);

        while ((pair = mpd_recv_pair_tag (conn, MPD_TAG_GENRE)))
        {
            newgenre = QString::fromUtf8(pair->value);
            qm_listitemInfo newItem;
            newItem.genre = newgenre;
            newItem.sorter = newgenre;
            itemlist.push_back(newItem);
            mpd_return_pair(conn, pair);
        }

        mpd_response_finish (conn);
        error_check("get_genrelist full");
    }
    else
    {
        struct mpd_song *song;
        mpd_search_db_songs(conn, false);
        mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_GENRE, searchfor.toUtf8());
        mpd_search_commit(conn);

        while ((song = mpd_recv_song (conn)))
        {

            if (mpd_song_get_tag (song, MPD_TAG_GENRE, 0) != NULL )
            {
                qm_listitemInfo newItem;
                newItem.genre = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_GENRE, 0));
                newItem.sorter = newItem.genre; // not toLower() or libview mays show things like: Jazz jazz Jazz
                itemlist.push_back(newItem);
            }
            else
                if (searchfor.isEmpty())
                {
                    qm_listitemInfo newItem;
                    newItem.genre = "";
                    newItem.sorter = "0";
                    itemlist.push_back(newItem);
                }
            mpd_song_free (song);
        }
        mpd_response_finish (conn);
        error_check("get_genrelist search");
    }

    return itemlist;
}

QString qm_mpdCom::fix_trackformat(QString instring)
{
    // remove the x/xx format
    QString str = instring;
    str =  str.left(str.lastIndexOf("/"));
    // add leading zero, when required
    if (str.size() == 1)
        str = "0" + str;
    return str;
}


QString qm_mpdCom::get_album_year(QString album, QString artist)
{
    QString year = "";

    if (conn == NULL)
        return year;

    if (album.isEmpty() || artist.isEmpty())
        return year;

    struct mpd_pair *pair;
    mpd_search_db_tags(conn, MPD_TAG_DATE);
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ARTIST, artist.toUtf8());
    mpd_search_add_tag_constraint (conn, MPD_OPERATOR_DEFAULT, MPD_TAG_ALBUM,  album.toUtf8());
    mpd_search_commit(conn);

    while ((pair = mpd_recv_pair_tag (conn, MPD_TAG_DATE)))
    {
        // take the first that comes along
        year = QString::fromUtf8(pair->value);
        mpd_return_pair(conn, pair);
        if (!year.isEmpty())
            break;
    }

    mpd_response_finish (conn);
    error_check("get_album_year");

    // convert xxx-xx-xx format toi xxxx
    if (!year.isEmpty())
        year = year.left(4) + " ";

    return year;
}


void qm_mpdCom::reset_current_song()
{
    current_songID = -1;
}


qm_itemList qm_mpdCom::get_playlist_itemlist(QString playlist)
{
    qm_itemList itemlist;

    if (conn == NULL)
        return itemlist;

    struct mpd_song *song;
    mpd_send_list_playlist_meta(conn, playlist.toUtf8());

    while ((song = mpd_recv_song(conn)))
    {
        qm_listitemInfo newItem;
        newItem.file = QString::fromUtf8(mpd_song_get_uri(song));
        if (newItem.file.startsWith("http://"))
        {
            newItem.type = TP_STREAM;
            newItem.time = 0;
        }
        else
        {
            if (newItem.file.startsWith("/")) // URL relative to MPD music dir doesn't have a leading '/'
                newItem.type = TP_SONGX;
            else
                newItem.type = TP_SONG;
            newItem.time = mpd_song_get_duration(song);
        }
        newItem.artist = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
        newItem.album = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));
        newItem.title = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
        newItem.genre = QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_GENRE, 0));
        newItem.track = fix_trackformat(QString::fromUtf8(mpd_song_get_tag(song, MPD_TAG_TRACK, 0)) );

        itemlist.push_back(newItem);
        mpd_song_free (song);
    }
    mpd_response_finish(conn);
    error_check("get_playlist_itemlist");
    return itemlist;
}

// find out which song (ID) caused the playlist to update
int qm_mpdCom::get_listchanger()
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
    error_check("get_listchanger");
    return id;
}


void qm_mpdCom::set_replaygain_mode(int mode)
{
    if (conn == NULL)
        return;
    const char *rg_mode;

    switch (mode)
    {
        case 0:
        {
            rg_mode = "off";
            break;
        }
        case 1:
        {
            rg_mode = "track";
            break;
        }
        case 2:
        {
            rg_mode = "album";
            break;
        }
        case 3:
        {
            rg_mode = "auto";
            break;
        }
        default:
            return;
    }
    mpd_send_command(conn, "replay_gain_mode", rg_mode, NULL);
    mpd_response_finish(conn);
    error_check("set_replaygain_mode");
}


bool qm_mpdCom::is_mpd_running()
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

    return b_isrunning;
}


void qm_mpdCom::mpd_disconnect(bool sendsignal)
{
    // stop the status loop
    if (statusLoop->isActive())
        statusLoop->stop();
    // stop the reconnect loop
    if (reconnectLoop->isActive())
        reconnectLoop->stop();
    // disconnect if connected
    if (conn != NULL)
    {
        mpd_connection_clear_error(conn);
        mpd_response_finish(conn);
        mpd_connection_free(conn);
        conn = NULL;
    }

    current_playlist = -1;
    current_songID = -1;
    current_songpos = -1;
    current_status = -1;
    plist_lenght = 0;

    if (sendsignal)
    {
        current_songinfo->reset();
        config->reset_temps();

        emit sgnl_connected(false);
    }
}


void qm_mpdCom::set_sInfo(qm_songInfo *sInf)
{
    current_songinfo = sInf;
}


void qm_mpdCom::find_config_files(bool show_dialogs)
{
    printf ("Auto-detecting MPD's config file(s) : ");
    int config_count = 0;
    QString allConfigs;
    QString thispath = "/etc/mpd.conf";
    QString homeDir = QDir::homePath();
    if (!homeDir.endsWith("/"))
        homeDir.append("/");
    QFile file;
    file.setFileName(thispath);

    if ( file.exists() )
    {
        printf (" /etc/mpd.conf ");
        if (file.open(QIODevice::ReadOnly))
        {
            file.close();
            b_mpdconf_found = true;
            mpdconf_path = thispath;
            allConfigs = "*  /etc/mpd.conf";
            config_count++;
        }
        else
        {
            allConfigs = "*  /etc/mpd.conf" + tr(" (not readable!)");
            printf ("(not-readable!)");
        }
        config_count++;
    }

    thispath = homeDir + ".mpd/mpd.conf";
    file.setFileName(thispath);
    if ( file.exists() )
    {
        printf (" ~/.mpd/mpd.conf ");
        if (file.open(QIODevice::ReadOnly))
        {
            file.close();
            b_mpdconf_found = true;
            mpdconf_path = thispath;
            allConfigs.append("\n*  ~/.mpd/mpd.conf");
        }
        else
        {
            allConfigs = "\n*  ~/.mpd/mpd.conf " + tr(" (not readable!)");
            printf ("(not-readable!)");
        }
        config_count++;
    }

    thispath = homeDir + ".mpdconf";
    file.setFileName(thispath);
    if ( file.exists() )
    {
        printf (" ~/.mpd.conf ");
        if (file.open(QIODevice::ReadOnly))
        {
            file.close();
            b_mpdconf_found = true;
            mpdconf_path = thispath;
            allConfigs.append("\n* ~/.mpd.conf");
            }
        else
        {
            allConfigs = "\n*  ~/.mpd.conf " + tr(" (not readable!)");
            printf ("(not-readable!)");
        }
        config_count++;
    }

    if (config_count > 1)
    {
        if (b_mpdconf_found)
        {
            printf ("\nMultiple MPD configuration files found! Using : %s\n", (const char*)mpdconf_path.toUtf8());
            if (show_dialogs)
            show_messagebox(tr("Multiple MPD configuration files found!"),
                            allConfigs + "\n\n"
                            + tr("The default choice is : ") + mpdconf_path + "\n\n"
                            + tr("Please make sure that MPD is also using this. You can set MPD to use a specific configuration file in /etc/default/mpd.") );
        }
        else
        {
            printf ("\nMultiple MPD configuration files found, but none are readable!");
            if (show_dialogs)
            show_messagebox(tr("Multiple MPD configuration files found, but none are readable!"),
                        allConfigs + "\n\n"
                        + tr("Please create a readable configuration file that MPD is also using or create a custom connection profile in the 'Connect' tab of the settings window.") );

        }
    }
    else
    if (config_count == 1)
    {
        if (!b_mpdconf_found)
        {
            printf ("\nMPD configuration file found, but it is not readable!");
            if (show_dialogs)
            show_messagebox(tr("MPD configuration file found, but it is not readable!"),
                        allConfigs + "\n\n"
                        + tr("Please create a readable configuration file that MPD is also using or create a custom connection profile in the 'Connect' tab of the settings window.") );
        }
            printf ("\n");
    }
    else // (config_count == 0)
    {
        printf ("not found!\nIs MPD installed?\n");
        if (show_dialogs)
        show_messagebox(tr("MPD's configuration file was not found!"),
                        tr("Make sure MPD is installed and using a default configuration file or, especially when connecting to a remotely running MPD, create a custom connection profile in the 'Connect' tab of the settings window.") );
    }
}


qm_mpdCom::~qm_mpdCom()
{}
