/*
 *  gm_settings.h
 *  GUIMUP settings window
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

#ifndef GM_SETTINGS_H
#define GM_SETTINGS_H

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/image.h>
#include <gtkmm/frame.h>
#include <gtkmm/scale.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/liststore.h>
#include <sigc++/sigc++.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
    using Glib::ustring;
#include "gm_mpdcom.h"
#include "gm_config.h"



class gm_settings : public Gtk::Window
{
public:
    gm_settings();
    virtual ~gm_settings();

    void set_mpdCom(gm_mpdCom*);
    void set_config(gm_Config*);
	void set_connected(bool);
    void load_config();
	void get_configs();
//  signals
    sigc::signal<void> signal_savesettings;
	sigc::signal<void> signal_applyfonts;
	sigc::signal<void> signal_settooltips;
	sigc::signal<void> signal_applycolors;
	sigc::signal<void> signal_lib_setfixed;
	sigc::signal<void> signal_plist_setfixed;
	sigc::signal<void> signal_setmarkplayed;
	
protected:

private:
	
//	combo boxes
	class comboColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

	comboColumns()
	{ add(col_name); add(col_icon);}
		Gtk::TreeModelColumn<Glib::ustring> col_name;
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > col_icon;
	};

	comboColumns cbColums;
	Glib::RefPtr<Gtk::ListStore> comboTreeModel;
	
//  functions
    void on_button_close();
    void on_apply_all();
    void on_button_connect();
    void on_fonts_reset();
	void on_profile_change();
	void set_tooltips();
	void on_color_change();
	void on_colors_reset();
	std::string into_string(int in);
	bool on_delete_event(GdkEventAny* event);

//  widgets:
    Gtk::Grid mainGrid;
    Gtk::Notebook theNotebook;
        Gtk::Grid cnt_grid_main;
            Gtk::CheckButton cnt_cb_autocon;
            Gtk::Frame cnt_fr_profiles;
                Gtk::Grid cnt_grid_profiles;
					Gtk::ComboBox cb_profile;
                    Gtk::Entry cnt_et_name;
                    Gtk::Label cnt_lb_name;
                    Gtk::Entry cnt_et_host;
                    Gtk::Label cnt_lb_host;
                    Gtk::Entry cnt_et_port;
                    Gtk::Label cnt_lb_port;
                    Gtk::Entry cnt_et_pswd;
                    Gtk::Label cnt_lb_pswd;
                    Gtk::Entry cnt_et_conf;
                    Gtk::Label cnt_lb_conf;
                   Gtk::Button cnt_btConnect;
               Gtk::Label cnt_label;
               Gtk::Image cnt_image;
        Gtk::Grid srv_grid_main;
			Gtk::Frame srv_fr_rpgain;
			Gtk::Grid srv_grid_rpgain;
					Gtk::RadioButton srv_rbt_off;
					Gtk::RadioButton srv_rbt_trk;
					Gtk::RadioButton srv_rbt_alb;
					Gtk::RadioButton srv_rbt_aut;
			Gtk::Frame srv_fr_xfade;
			Gtk::Grid srv_grid_xfade;
					Gtk::SpinButton srv_spb_xfade;
                    Gtk::Label srv_lb_xfade;
            Gtk::Frame srv_fr_output;
                Gtk::Grid srv_grid_outputs;
                    Gtk::CheckButton srv_cb_out1;
                    Gtk::CheckButton srv_cb_out2;
                    Gtk::CheckButton srv_cb_out3;
                    Gtk::CheckButton srv_cb_out4;
            Gtk::Frame srv_fr_command;
                Gtk::Grid srv_grid_command;
						Gtk::CheckButton srv_cb_start;
				        Gtk::Entry srv_et_startcmd;
						Gtk::CheckButton srv_cb_kill;
				        Gtk::Entry srv_et_killcmd;
        Gtk::Grid cli_grid_main;
			Gtk::CheckButton cli_cb_systray;
            Gtk::CheckButton cli_cb_ttips;
			Gtk::CheckButton cli_cb_noart;
			Gtk::Frame cli_fr_lib;
					Gtk::Grid cli_grid_lib; // FIXME
						Gtk::CheckButton cli_cb_byear;
            			Gtk::CheckButton cli_cb_nothe;
						Gtk::CheckButton cli_cb_libfixed;
			Gtk::Frame cli_fr_plist;
					Gtk::Grid cli_grid_plist; // FIXME
						Gtk::CheckButton cli_cb_start;
            			Gtk::CheckButton cli_cb_mark;
						Gtk::CheckButton cli_cb_plistfixed;
			Gtk::Frame cli_fr_extprogs;
					Gtk::Grid cli_grid_extprogs;
                        Gtk::Entry cli_et_imgview;
                        Gtk::Label cli_lb_imgview;
                        Gtk::Entry cli_et_tagedit;
                        Gtk::Label cli_lb_tagedit;
                        Gtk::Entry cli_et_fileman;
                        Gtk::Label cli_lb_fileman;
		Gtk::Grid stl_grid_main;
	            Gtk::Frame stl_fr_fonts;
                Gtk::Grid stl_grid_fonts;
                    Gtk::SpinButton stl_spb_title;
                    Gtk::Label stl_lb_title;
                    Gtk::SpinButton stl_spb_trackinfo;
                    Gtk::Label stl_lb_trackinfo;
                    Gtk::SpinButton stl_spb_time;
                    Gtk::Label stl_lb_time;
                    Gtk::SpinButton stl_spb_album;
                    Gtk::Label stl_lb_album;
                    Gtk::SpinButton stl_spb_library;
                    Gtk::Label stl_lb_library;
					Gtk::Button stl_bt_fontreset;
				Gtk::Frame stl_fr_colors;
				Gtk::Grid stl_grid_colors;
					Gtk::Label stl_lb_hue;
					Gtk::HScale stl_scl_hue;
					Gtk::Label stl_lb_sat;
					Gtk::HScale stl_scl_sat;
					Gtk::Label stl_lb_val;
					Gtk::HScale stl_scl_val;
				  Gtk::Frame stl_fr_preview;
					Gtk::Grid stl_grid_preview;
					Gtk::EventBox stl_eb_titleinfo; // bg color
					Gtk::Label stl_lb_titleinfo;
					Gtk::Label stl_lb_albuminfo;
					Gtk::EventBox stl_eb_albuminfo; // bg color
					Gtk::Grid stl_grid_previewreset;
						Gtk::Button stl_bt_colreset;
        Gtk::Grid abt_grid_main;
            Gtk::Image abt_image;
            Gtk::Label abt_label;
			Gtk::Frame abt_fr_swin;
            Gtk::ScrolledWindow abt_scrollwin;
            Gtk::Label abt_text;
        Gtk::HButtonBox theButtonBox;
            Gtk::Button but_Close, but_Apply;

//  variables
    gm_mpdCom *mpdCom;
    gm_Config *config;
    Glib::RefPtr<Gdk::Pixbuf>
        pxb_con_ok,
        pxb_con_dis;
	Gtk::Image
		img_but_apply,
        img_but_close;
	bool
		b_connected,
		b_use_trayicon,
		b_pList_fixed,
		b_lib_fixed,
		b_output1,
		b_output2,
		b_output3,
		b_output4;
	int
		xfade_time,
		rpgain_mode;
	Gdk::RGBA 
		albuminfo_fg_color,
		albuminfo_bg_color,
		titleinfo_bg_color,
		titleinfo_fg_color;
};

#endif //  GM_SETTINGS_H
