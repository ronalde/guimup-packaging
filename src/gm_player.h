/*
 *  gm_player.h
 *  GUIMUP main player window
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

#ifndef GM_PLAYER_H
#define GM_PLAYER_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/fixed.h>
#include <gtkmm/button.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/image.h>
#include <gtkmm/scale.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/statusicon.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/stock.h>
#include <gtkmm/iconfactory.h>
#include <gtkmm/tooltip.h>
#include <gdkmm/general.h> // for screen_width()
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <iostream>
    using std::cout;
    using std::endl;
#include <string>
#include <sstream>
#include <errno.h>
#include <dirent.h>
#include "gm_scroller.h"
#include "gm_config.h"
#include "gm_taginfo.h"
#include "gm_mpdcom.h"
#include "gm_tracks.h"
#include "gm_settings.h"

enum // ID # for widgets
{
    ID_prev, // previous track button
    ID_stop, // stop button
    ID_play, // play/pause button
    ID_next, // next track button
    ID_plst, // playlist button
    ID_mmax, // mini/maxi mode button
    ID_sets, // settings button
    ID_rept, // repeat toggle button
    ID_rand, // random toggle button
    ID_info, // song info button
    ID_volm, // volume slider
};


class gm_Player : public Gtk::Window
{
public:
  gm_Player();
  virtual ~gm_Player();

protected:
//  tray icon functions & vars
    void tIcon_create();
    void tIcon_blink();
    void tIcon_on_popup (guint button, guint32 time);
    void tIcon_toggle_hide();
    bool on_delete_event(GdkEventAny*);
    Glib::RefPtr<Gtk::StatusIcon> trayIcon;
    Glib::RefPtr<Gtk::UIManager> tIcon_UIManager;
    Glib::RefPtr<Gtk::ActionGroup> tIcon_ActionGroup;
    std::pair<int, int> win_pos;

private:
//  widgets
    Gtk::Fixed fx_main;
        Gtk::EventBox eb_background;
        Gtk::Frame fr_lcdisplay;
        Gtk::VBox vb_lcdisplay;
            gm_Scroller titleScroller;
            Gtk::HBox hb_trackdata;
                Gtk::Label lb_type, lb_kbps, lb_khz, lb_time, lb_totaltime;
                Gtk::EventBox eb_time;
        Gtk::ProgressBar pb_timeprogress;
        Gtk::HBox hb_center;
            Gtk::Image img_aArt;
            Gtk::VBox vb_center_right;
                Gtk::Label lb_album;
                Gtk::Label lb_year;
                Gtk::HBox hb_tbuttons;
                    Gtk::Button bt_info;
                    Gtk::ToggleButton  tbt_repeat;
                    Gtk::ToggleButton  tbt_random;
                Gtk::HScale hsc_volume;
        Gtk::HBox hb_buttonrow;
            Gtk::Image img_statusled;
            Gtk::Button bt_prev, bt_stop, bt_play, bt_next, bt_plst, bt_sizr, bt_conf;

//  variables
    gm_mpdCom mpdCom;
    gm_tagInfo tagInfo;
    gm_Tracks tracksWindow;
    gm_Config config;
    gm_settings settingsWindow;
    sigc::connection minmaxtimer;
    ustring
        mpd_music_path,
        current_art_path,
        custom_art_file,
		serverName,
		serverPassword;
    int
        current_status,
        current_volume,
        windowH,
        btrowPos,
        alphacycler,
        aArt_H, aArt_W, aArt_row,
		prev_fractint,
		serverPort;

    guint8  *ptr_xfad,
            *ptr_orig,
            *ptr_xfadpix,
            *ptr_origpix;

    double songTotalTime;

    bool
        b_winIsMax,
        b_repeat,
        b_random,
        b_trayicon,
        b_minmax_xfade,
        b_minmax_busy,
        b_mpdSetsVolume,
        b_timeremain,
        b_stream,
		b_connected;

    Glib::RefPtr<Gdk::Pixbuf>
        pxb_albmart,
        pxb_xfading,
        pxb_led_noconn,
        pxb_led_paused,
        pxb_led_playing,
        pxb_led_stopped,
        pxb_nopic,
        pxb_onair,
        pxb_st_noconn,
        pxb_st_paused,
        pxb_st_playing,
        pxb_st_stopped;

    Gtk::Image
        img_mn_config,
        img_mn_next,
        img_mn_pause,
        img_mn_play,
        img_mn_playlist,
        img_mn_prev,
        img_mn_sizer,
        img_mn_stop,
        img_mn_info_a,
        img_mn_info_b,
        img_tb_random,
        img_tb_repeat;

//  functions
    void init_widgets();
    void init_signals();
    void init_vars();
    void set_childwindows();
    void set_albumart(ustring path, bool stream = false);
    bool go_find_art(ustring &, const char*);
    void set_status(int, ustring mesg = "");
    void on_hide();
    void on_get_config();
    void on_newSong(songInfo);
    void on_newStatus(statInfo);
    bool on_timeClicked(GdkEventButton*);
    void on_connectsignal(bool connected);
    bool on_progressClicked(GdkEventButton*);
    void on_connectrequest(ustring, int, ustring, bool);
    void on_settingssaved();
    void on_signal(int);
	void on_signal_host_port_pwd(ustring, int, ustring);
    void set_fonts();
    bool maxiMice();
    bool miniMice();
    ustring escapeString(ustring str);
    std::string into_string(int);
    ustring into_time(int);
};

#endif //  GM_PLAYER_H
