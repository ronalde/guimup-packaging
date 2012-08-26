/*
 *  gm_player.h
 *  GUIMUP main player window
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

#ifndef GM_PLAYER_H
#define GM_PLAYER_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/fixed.h>
#include <gtkmm/button.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/image.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/stock.h>
#include <gtkmm/iconfactory.h>
#include <gtkmm/tooltip.h>
#include <gtkmm/toggleaction.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <stdlib.h>
#include <iostream>
    using std::cout;
    using std::endl;
#include <string>
#include <sstream>
#include <errno.h>
#include <dirent.h>
#include "gm_slider.h"
#include "gm_scroller.h"
#include "gm_config.h"
#include "gm_mpdcom.h"
#include "gm_library.h"
#include "gm_settings.h"
#include "gm_trayicon.h"


enum // ID # for widgets
{
    ID_prev, // previous track button
    ID_stop, // stop button
    ID_play, // play/pause button
    ID_next, // next track button
    ID_plst, // playlist button
    ID_mmax, // mini/maxi mode button
    ID_sets, // settings button
    ID_volm, // volume slider
};


class gm_Player : public Gtk::Window
{
public:
  gm_Player();
  virtual ~gm_Player();

private:
//  tray icon functions & vars
    void tIcon_create();
    void tIcon_on_popup (guint button, guint32 time);
    void tIcon_toggle_hide();
    bool on_delete_event(GdkEventAny*);
	void on_set_playertoggle();
	void on_set_libtoggle();
	void on_library_hide();

    Glib::RefPtr<gm_trayIcon> trayIcon;
    Glib::RefPtr<Gtk::UIManager> tIcon_UIManager;
    Glib::RefPtr<Gtk::ActionGroup> tIcon_ActionGroup;
    Glib::RefPtr<Gtk::ToggleAction> ToggleLibrary;
    Glib::RefPtr<Gtk::ToggleAction> TogglePlayer;

//  widgets
	Gtk::EventBox eb_all;				// for context menu popup
    Gtk::Fixed fx_main;
        Gtk::EventBox eb_background;	// for bg color only
        Gtk::Frame fr_lcdisplay;
        Gtk::VBox vb_lcdisplay;
			Gtk::EventBox eb_scroller;  // for mouse-over effect
            gm_Scroller titleScroller;
            Gtk::HBox hb_trackdata;
                Gtk::Label lb_type, lb_kbps, lb_khz, lb_time, lb_totaltime;
                Gtk::EventBox eb_time;  // for time +/- toggle
        Gtk::ProgressBar pb_timeprogress;
        Gtk::HBox hb_center;
            Gtk::Image img_aArt;
            Gtk::VBox vb_center_right;
                Gtk::Label lb_album;
                Gtk::Label lb_year;
				Gtk::Label lb_comment;
                gm_slider hsc_volume;
        Gtk::HBox hb_buttonrow;
            Gtk::Image img_statusled;
            Gtk::Button bt_prev, bt_stop, bt_play, bt_next, bt_plst, bt_sizr, bt_conf;

//  variables
    gm_mpdCom mpdCom;
    gm_Config config;
    gm_library libraryWindow;
    gm_settings settingsWindow;

    sigc::connection minmaxtimer;

    ustring
        mpd_music_path,
		mpd_plist_path,
        current_art_path,
		serverName,
		serverPassword,
		file_to_update,
		fullpath;
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
        b_repeat,
        b_random,
		b_single,
		b_consume,
        b_minmax_xfade,
        b_minmax_busy,
        b_mpdSetsVolume,
        b_stream,
		b_connected,
		b_playerWindow_hidden,
		b_settingsWindow_hidden,
		b_libraryWindow_hidden,
		b_nosong,
		b_useTrayIcon;

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
		img_men_art,
		img_men_edit,
		img_men_update,
		img_men_browse;

    Gtk::Menu rClickMenu;
        
//  functions
    void init_widgets();
    void init_signals();
    void init_vars();
    void set_albumart(ustring path);
    bool go_find_art(ustring &, const char*);
    void set_status(int, ustring mesg = "");
    void on_hide();
    void on_get_config();
    void on_newSong(songInfo);
    void on_newStatus(statInfo);
    bool on_timeClicked(GdkEventButton*);
    bool on_playerRClicked(GdkEventButton*);
    void on_connectsignal(bool connected);
    bool on_progressClicked(GdkEventButton*);
    void on_connectrequest(ustring, int, ustring, bool);
    void on_settingssaved();
    void on_updatefile(ustring file);
    void on_signal(int);
	void on_signal_host_port_pwd(ustring, int, ustring);
	void on_menu_Art();
	void on_menu_Edit();
	void on_menu_Reload();
	void on_menu_Browse();
    void set_fonts();
	void get_mpd_paths();
    bool maxiMice();
    bool miniMice();
    ustring escapeString(ustring str);
    std::string into_string(int);
    ustring into_time(int);
	bool on_enter_scroller(GdkEventCrossing*);
	bool on_leave_scroller(GdkEventCrossing*);
	bool on_trayClicked(GdkEventButton*);
 	bool on_tray_or_vol_Scrolled(GdkEventScroll*);

};

#endif //  GM_PLAYER_H
