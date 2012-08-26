/*
 *  gm_player.h
 *  GUIMUP main player window
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

#ifndef GM_PLAYER_H
#define GM_PLAYER_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/fixed.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/settings.h> 	// sys-font
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include <glibmm/convert.h>     // filename_from_uri
#include <glibmm/spawn.h>       // spawn-async
#include <iostream>
    using std::cout;
    using std::endl;
#include <errno.h> 				// go_find_art()
#include <dirent.h> 			// go_find_art()
#include "gm_slider.h"
#include "gm_scroller.h"
#include "gm_config.h"
#include "gm_mpdcom.h"
#include "gm_browser.h"
#include "gm_settings.h"
#include "gm_trayicon.h"
#include "gm_progressbar.h"
#include "gm_fixedlabel.h"
#include "gm_songinfo.h"

enum // ID # for widgets
{
    ID_prev, // previous track button
    ID_stop, // stop button
    ID_play, // play/pause button
    ID_next, // next track button
    ID_libr, // playlist button
    ID_mmax, // mini/maxi mode button
    ID_sets, // settings button
};

class gm_Player : public Gtk::Window
{
public:
  gm_Player();
  virtual ~gm_Player();

	void on_open_with_request(std::vector<ustring>, bool);
	void activate_from_remote();
	
private:
	gm_browser *browserWindow;
	gm_settings *settingsWindow;
//  widgets
    Gtk::Fixed fx_main;
        Gtk::EventBox eb_lcd_bg;
        Gtk::Fixed fx_lcdisplay;
			Gtk::EventBox eb_scroller;
            gm_Scroller titleScroller;
            Gtk::Label lb_type, lb_kbps, lb_khz, lb_time;
			Gtk::EventBox eb_time;
	gm_Progressbar pb_timeprogress;
	Gtk::EventBox eb_center;
    Gtk::Grid gr_center;
		Gtk::Image img_aArt;
		gm_FixedLabel fxl_albuminfo;

	Gtk::Grid gr_buttonrow;
		Gtk::Grid gr_volslider;
		gm_Slider gsl_volume;
		Gtk::Button bt_prev, bt_stop, bt_play, bt_next, bt_plst, bt_sizr, bt_conf;

//  variables
    gm_mpdCom *mpdCom;
    gm_Config *config;
	gm_songInfo *current_sInfo;

    sigc::connection minmaxtimer;

	int
        current_status,
        current_volume,
		current_art_type,
		current_playtime,
        windowH, windowH_min, windowH_max,
        btrowPos, btrowPos_min, btrowPos_max,
        alphacycler,
        aArt_H, aArt_W, aArt_row,
		kbps_count_down;

    guint8  *ptr_xfad,
            *ptr_orig,
            *ptr_xfadpix,
            *ptr_origpix;

    bool
        b_minmax_xfade,
        b_minmax_busy,
        b_stream,
		b_connected,
		b_playerWindow_hidden,
		b_show_settingsWindow,
		b_show_browserWindow,
		b_nosong,
		b_skip_khz,
		b_no_art_dir,
		b_use_trayicon;

    Glib::RefPtr<Gdk::Pixbuf>
        pxb_albmart,
        pxb_xfading,
        pxb_nopic,
        pxb_onair,
        pxb_st_noconn,
        pxb_st_paused,
        pxb_st_playing,
        pxb_st_stopped,
		pxb_st_idle;

    Gtk::Image
		/* buttons */
        img_mn_config,
        img_mn_next,
        img_mn_pause,
        img_mn_play,
        img_mn_playlist,
        img_mn_prev,
        img_mn_sizer,
        img_mn_stop,
		/* context menu */
		img_men_view,
		img_men_browse,
		img_men_edit,
		img_men_reload,
		/* tray menu */
		img_tr_prev,
		img_tr_next,
		img_tr_stop,
		img_tr_play,
		img_tr_pause,
		img_tr_quit;

	Gtk::Menu* rClickMenu;
	Gtk::ImageMenuItem
		*imi_reload,
		*imi_viewer,
		*imi_fileman,
		*imi_tagedit;
	
	Glib::RefPtr<gm_trayIcon> trayIcon;
	Gtk::Menu *tIconMenu;
	Gtk::ImageMenuItem *mnItem_play;
	Gtk::ImageMenuItem *mnItem_pause;
	
	Gdk::RGBA 
		albuminfo_bg_color,
		albuminfo_fg_color,
		titleinfo_fg_color,
		titleinfo_bg_color;
	
	Glib::RefPtr< Gdk::Pixbuf> pxbfader;
        
//  functions
    void init_widgets();
    void init_signals();
    void init_vars();
    void set_albumart(int);
    bool go_find_art(ustring &, ustring);
    void set_status(int, ustring mesg = "");
	void set_colors();
    void on_hide();
    void on_newSong();
    void on_newStatus(mpd_status *);
    bool on_timeClicked(GdkEventButton*);
    bool on_playerRClicked(GdkEventButton*);
    void on_connected(bool connected);
    bool on_progressClicked(GdkEventButton*);
	bool on_volumeClicked(GdkEventButton*);
	void set_tooltips();
    void save_settings();
    void on_updatefile(ustring);
    void on_signal(int);
	void on_menu_Art();
	void on_menu_Edit();
	void on_menu_Reload();
	void on_menu_Browse();
    void set_fonts();
    bool maxiMice();
    bool miniMice();
    ustring escapeString(ustring);
    ustring into_string(int);
    ustring into_time(int);
	bool on_enter_scroller(GdkEventCrossing*);
	bool on_leave_scroller(GdkEventCrossing*);
	bool on_trayIcon_clicked(GdkEventButton*);
 	bool on_tray_or_vol_Scrolled(GdkEventScroll*);
    void create_trayIcon();
	void create_rClickmenu();
    void on_trayIcon_popup (guint, guint32);
    void toggle_hide_show();
    bool on_delete_event(GdkEventAny*);
};

#endif //  GM_PLAYER_H
