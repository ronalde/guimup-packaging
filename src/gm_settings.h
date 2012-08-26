/*
 *  gm_settings.h
 *  GUIMUP settings window
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

#ifndef GM_SETTINGS_H
#define GM_SETTINGS_H

#include <stdlib.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/image.h>
#include <gtkmm/alignment.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include "gm_mpdcom.h"
#include "gm_config.h"

struct st_posxy
{
    int x_pos;
    int y_pos;
};


class gm_settings : public Gtk::Window
{
public:
    gm_settings();
    virtual ~gm_settings();

    void set_stPos(st_posxy sets);
    st_posxy get_stPos();
    void set_mpdcom(gm_mpdCom*);
    void set_config(gm_Config *conf);
    void load_and_show();
//  signals
    sigc::signal<void, ustring, int, ustring, bool> signal_pleaseconnect;
    sigc::signal<void> signal_settingssaved;
	sigc::signal<void> signal_applyfonts;
protected:

private:
//  functions
    void on_button_cancel();
    void on_button_save();
    void on_button_connect();
    void on_fonts_reset();
	void on_fonts_apply();
    std::string into_string(int in);

//  widgets:
    Gtk::VBox mainVBox;
    Gtk::Notebook theNotebook;
        Gtk::VBox vbx_connect;
            Gtk::CheckButton cnt_cb_autocon;
            Gtk::CheckButton cnt_cb_override;
            Gtk::Frame cnt_fr_settings;
                Gtk::VBox cnt_framevbox;
                    Gtk::HBox cnt_hb_host;
                        Gtk::Entry cnt_et_host;
                        Gtk::Label cnt_lb_host;
                    Gtk::HBox cnt_hb_port;
                        Gtk::Entry cnt_et_port;
                        Gtk::Label cnt_lb_port;
                    Gtk::HBox cnt_hb_pswd;
                        Gtk::Entry cnt_et_pswd;
                        Gtk::Label cnt_lb_pswd;
            Gtk::HBox cnt_btcbox;
                Gtk::Button cnt_btConnect;
                Gtk::Alignment cnt_spacer;
            Gtk::HBox cnt_bottomhbox;
                Gtk::Label cnt_label;
                Gtk::Image cnt_image;
        Gtk::VBox vbx_server;
            Gtk::Frame srv_fr_output;
                Gtk::VBox srv_frout_vbox;
                    Gtk::CheckButton srv_cb_out1;
                    Gtk::CheckButton srv_cb_out2;
                    Gtk::CheckButton srv_cb_out3;
                    Gtk::CheckButton srv_cb_out4;
                    Gtk::CheckButton srv_cb_out5;
		    Gtk::HBox srv_hb_mpath;
                Gtk::Entry srv_et_mpath;
                Gtk::Label srv_lb_mpath;
            Gtk::HBox srv_hb_ppath;
                Gtk::Entry srv_et_ppath;
                Gtk::Label srv_lb_ppath;
            Gtk::Frame srv_fr_manage;
                Gtk::VBox srv_frmng_vbox;
                    Gtk::CheckButton srv_cb_launch;
                    Gtk::CheckButton srv_cb_kill;
        Gtk::VBox vbx_client;
            Gtk::Frame cli_fr_fonts;
                Gtk::VBox cli_framevbox;
                    Gtk::VBox cli_vb_host;
                        Gtk::HBox cli_hbx_title;
                            Gtk::SpinButton cli_spb_title;
                            Gtk::Label cli_lb_title;
                        Gtk::HBox cli_hbx_trackinfo;
                            Gtk::SpinButton cli_spb_trackinfo;
                            Gtk::Label cli_lb_trackinfo;
                        Gtk::HBox cli_hbx_time;
                            Gtk::SpinButton cli_spb_time;
                            Gtk::Label cli_lb_time;
                        Gtk::HBox cli_hbx_album;
                            Gtk::SpinButton cli_spb_album;
                            Gtk::Label cli_lb_album;
                        Gtk::HBox cli_hbx_tracks;
                            Gtk::SpinButton cli_spb_tracks;
                            Gtk::Label cli_lb_tracks;
						Gtk::HBox cli_hbx_fontbtns;
							Gtk::Button cli_bt_reset;
							Gtk::Button cli_bt_apply;
			Gtk::Frame cli_fr_restart;
				Gtk::VBox cli_vb_restart;
            		Gtk::CheckButton cli_cb_systray;
            		Gtk::CheckButton cli_cb_ttips;
            Gtk::HBox cli_hb_art;
                        Gtk::Entry cli_et_art;
                        Gtk::Label cli_lb_art;
        Gtk::VBox vbx_about;
            Gtk::HBox abt_hbox;
            Gtk::Image abt_image;
            Gtk::Label abt_label;
            Gtk::ScrolledWindow abt_scrollwin;
            Gtk::Label abt_text;
        Gtk::HButtonBox theButtonBox;
            Gtk::Button but_Cancel, but_Save;

//  variables
    gm_mpdCom *st_mpdCom; // pointer to the parent mpdCom
    gm_Config *st_config;
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_con_ok,
        pxb_con_dis;
};

#endif //  GM_SETTINGS_H
