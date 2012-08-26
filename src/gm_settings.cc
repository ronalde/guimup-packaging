/*
 *  gm_settings.cc
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

#include "gm_settings.h"
#include "gm_settings_pix.h"

gm_settings::gm_settings()
{
	mpdCom = NULL;
	config = NULL;
	b_connected = false;
	cnt_label.set_text("Not Connected");
	cnt_image.set(pxb_con_dis);

	Glib::RefPtr<Gdk::Pixbuf> pxb;
    std::vector< Glib::RefPtr<Gdk::Pixbuf> > window_icons;
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon16, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon24, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
    window_icons.push_back(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon48, false);
    window_icons.push_back(pxb);
    set_icon_list(window_icons);
	
	pxb_con_ok  = Gdk::Pixbuf::create_from_inline(-1, con_ok, false);
	pxb_con_dis = Gdk::Pixbuf::create_from_inline(-1, con_dis, false);
    set_title("Guimup Settings");
    set_border_width(4);
    set_size_request(320, -1); 
    set_resizable(false);

	// widgets
	mainGrid.set_row_spacing(6);	
	theButtonBox.set_layout(Gtk::BUTTONBOX_END);
	theButtonBox.set_spacing(8);
		but_Close.set_label("Close");
		but_Close.set_size_request(90, -1);
		but_Close.set_tooltip_text("Just closes the window. Does not apply any changes");
		but_Apply.set_size_request(90, -1);	
		but_Apply.set_label("Apply");
		but_Apply.set_tooltip_text("Applies and saves all changes. Does not close the window");
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_check, false);
		img_but_apply.set(pxb);
		but_Apply.set_image(img_but_apply);
		pxb = Gdk::Pixbuf::create_from_inline(-1, tr_redcross, false);
		img_but_close.set(pxb);
		but_Close.set_image(img_but_close);
	cnt_grid_main.set_border_width(6);
		cnt_grid_main.set_row_spacing(12);
		cnt_grid_main.set_column_spacing(12);
		cnt_cb_autocon.set_label("Autoconnect");
		cnt_cb_autocon.set_hexpand(true);
		cnt_cb_autocon.set_tooltip_text("Automatically connect with MPD when starting up");
		cnt_fr_profiles.set_label("Connection profiles");
		cnt_fr_profiles.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	// start setup profiles combo ->
		Gtk::TreeRow row;
		cb_profile.set_tooltip_text("Preconfigured profiles to connect with MPD");
		cb_profile.set_size_request(160,-1);
		comboTreeModel = Gtk::ListStore::create(cbColums);
		cb_profile.set_model(comboTreeModel);

		pxb = Gdk::Pixbuf::create_from_inline(-1, st_conprofile, false);
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "auto detected ";
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "profile 1 ";
		row[cbColums.col_icon] = pxb;
	
		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "profile 2 ";
		row[cbColums.col_icon] = pxb;

		row = *(comboTreeModel->append());
		row[cbColums.col_name] = "profile 3 ";
		row[cbColums.col_icon] = pxb;
	
		cb_profile.pack_start(cbColums.col_icon, false);
		cb_profile.pack_start(cbColums.col_name, true);
		cb_profile.set_active(0);
		cb_profile.set_focus_on_click(false);
	// <- profiles combo
		cnt_grid_profiles.set_row_spacing(8);
		cnt_grid_profiles.set_column_spacing(6);
		cnt_grid_profiles.set_border_width(4);	
		cnt_et_name.set_size_request(150, -1);
	    cnt_lb_name.set_text("Description");
		cnt_lb_name.set_alignment(0, 0.5);
		cnt_et_host.set_size_request(150, -1);
	    cnt_lb_host.set_text("Host");
		cnt_lb_host.set_alignment(0, 0.5);
		cnt_et_port.set_size_request(150, -1);
	    cnt_lb_port.set_text("Port");
		cnt_lb_port.set_alignment(0, 0.5);
		cnt_et_pswd.set_size_request(150, -1);
	    cnt_lb_pswd.set_text("Password");
		cnt_lb_pswd.set_alignment(0, 0.5);
		cnt_et_conf.set_size_request(150, -1);
	    cnt_lb_conf.set_text("MPD's config");
		cnt_lb_conf.set_alignment(0, 0.5);
		cnt_btConnect.set_label("Apply & Connect");
		cnt_btConnect.set_margin_left(56);
		cnt_btConnect.set_margin_right(56);
		cnt_btConnect.set_tooltip_text("Save the selected profile and use it to (re)connect with MPD");
		cnt_image.set_size_request(32, 32);
		cnt_label.set_alignment(0.5, 0.5);
		cnt_label.set_margin_top(10);
	srv_grid_main.set_border_width(6);
		srv_grid_main.set_row_spacing(6);
		srv_fr_rpgain.set_label("Replay gain");
		srv_fr_rpgain.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		srv_grid_rpgain.set_border_width(2);
		srv_grid_rpgain.set_column_spacing(4);
		srv_spb_xfade.set_digits(0);
		srv_spb_xfade.set_range(0, 10);
		srv_spb_xfade.set_increments(1, 1);
		srv_spb_xfade.set_tooltip_text("Crossfade time from 1 to 10 seconds. (0 is off)");
        srv_lb_xfade.set_text("Time (seconds)");
		srv_fr_xfade.set_label("Cross fade");
		srv_fr_xfade.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		srv_grid_xfade.set_border_width(2);
		srv_grid_xfade.set_column_spacing(8);
		srv_rbt_off.add_label("Off");
		srv_rbt_off.set_hexpand(true);
		Gtk::RadioButtonGroup group = srv_rbt_off.get_group();
		srv_rbt_trk.add_label("Track");
		srv_rbt_trk.set_hexpand(true);
		srv_rbt_trk.set_group(group);
		srv_rbt_alb.add_label("Album");
		srv_rbt_alb.set_hexpand(true);
		srv_rbt_alb.set_group(group);
		srv_rbt_aut.add_label("Auto");
		srv_rbt_aut.set_hexpand(true);
		srv_rbt_aut.set_group(group);
		srv_fr_output.set_label("Output devices");
		srv_fr_output.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		srv_grid_outputs.set_border_width(2);
		srv_grid_outputs.set_row_spacing(4);
		srv_et_startcmd.set_size_request(150, -1);
		srv_et_killcmd.set_size_request(150, -1);
		srv_fr_command.set_label("Command MPD");
		srv_fr_command.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		srv_grid_command.set_row_spacing(6);
		srv_grid_command.set_column_spacing(4);
		srv_grid_command.set_border_width(2);
		srv_cb_start.set_label("Start with Guimup: ");
		srv_et_startcmd.set_tooltip_text("Usually \"mpd\" or \"/usr/bin/mpd\"");
		srv_cb_kill.set_label(  "Quit with Guimup: ");
		srv_et_killcmd.set_tooltip_text("Usually \"mpd --kill\" or \"killall mpd\"");
	cli_grid_main.set_border_width(6);
		cli_grid_main.set_row_spacing(4);
		cli_cb_systray.set_label("Use system tray icon");
		cli_cb_systray.set_hexpand(true);
		cli_cb_systray.set_tooltip_text("Control the program from the system tray (restart to apply)");
		cli_cb_ttips.set_label("Show tooltips");
		cli_cb_ttips.set_tooltip_text("This is a tooltip");
		cli_cb_noart.set_label("Disable albumart");
		cli_cb_noart.set_tooltip_text("Never look for images in the song's folder");
		cli_fr_lib.set_label("Library");
		cli_fr_lib.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		cli_grid_lib.set_row_spacing(4);
		cli_grid_lib.set_border_width(2);
		cli_cb_nothe.set_label("Ignore leading \"the\" when sorting artists");
		cli_cb_nothe.set_tooltip_text("e.g. \"the Beatles\" will be sorted under B");
		cli_cb_byear.set_label("In 'Artist' mode sort albums by year");
		cli_cb_byear.set_tooltip_text("When an artist is expanded the albums can be sorted by name or by year");
		cli_cb_libfixed.set_label("Use fixed column widths");
		cli_cb_libfixed.set_tooltip_text("Adjust widths automatically to the content or keep fixed widths");
		cli_fr_plist.set_label("Playlist");
		cli_fr_plist.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		cli_grid_plist.set_row_spacing(4);
		cli_grid_plist.set_border_width(2);
		cli_cb_start.set_label("Start first track in a new list");
		cli_cb_start.set_tooltip_text("Automatically start playing the first track when a new list is created");
        cli_cb_mark.set_label("Mark songs that have played");
		cli_cb_mark.set_tooltip_text("Songs that have started playing will be greyed-out (A bit slow with very long lists)");
		cli_cb_plistfixed.set_label("Use fixed column widths");
		cli_cb_plistfixed.set_tooltip_text("Adjust widths automatically to the content or keep fixed widths");
		cli_fr_extprogs.set_label("External programs");
		cli_fr_extprogs.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);	
		cli_grid_extprogs.set_row_spacing(4);
		cli_grid_extprogs.set_column_spacing(6);
		cli_grid_extprogs.set_border_width(4);
		cli_et_imgview.set_size_request(120, -1);
		cli_et_imgview.set_tooltip_text("Program to view images such as 'eog' or 'gwenview'"); 
		cli_lb_imgview.set_text("Image viewer");
		cli_lb_imgview.set_alignment(0, 0.5);
		cli_et_tagedit.set_size_request(120, -1);
		cli_et_tagedit.set_tooltip_text("Program to edit audio tags such as 'easytag' or 'kid3'");
		cli_lb_tagedit.set_text("Tag editor");
		cli_lb_tagedit.set_alignment(0, 0.5);
		cli_et_fileman.set_size_request(120, -1);
		cli_et_fileman.set_tooltip_text("Program to manage files such as 'nautilus' or 'dolphin'");
		cli_lb_fileman.set_text("File manager");
		cli_lb_fileman.set_alignment(0, 0.5);
	stl_grid_main.set_border_width(6);
		stl_grid_main.set_row_spacing(6);
		stl_fr_fonts.set_label("Font sizes");
		stl_fr_fonts.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		stl_grid_fonts.set_row_spacing(4);
		stl_grid_fonts.set_column_spacing(6);
		stl_grid_fonts.set_border_width(4);
		stl_bt_fontreset.set_label("Reset");
		stl_bt_fontreset.set_size_request(90, -1);
		stl_bt_fontreset.set_margin_left(4);
		stl_spb_title.set_digits(0);
		stl_spb_title.set_range(6, 16);
		stl_spb_title.set_increments(1, 1);
		stl_lb_title.set_text("Artist : Title");
		stl_lb_title.set_hexpand(true);
		stl_lb_title.set_alignment(0, 0.5);
		stl_spb_trackinfo.set_digits(0);
		stl_spb_trackinfo.set_range(6, 14);
		stl_spb_trackinfo.set_increments(1, 1);
		stl_lb_trackinfo.set_text("Track tech info");
		stl_lb_trackinfo.set_alignment(0, 0.5);
		stl_spb_time.set_digits(0);
		stl_spb_time.set_range(6, 14);
		stl_spb_time.set_increments(1, 1);
		stl_lb_time.set_text("Playtime / total time");
		stl_lb_time.set_alignment(0, 0.5);
		stl_spb_album.set_digits(0);
		stl_spb_album.set_range(6, 16);
		stl_spb_album.set_increments(1, 1);	
		stl_lb_album.set_text("Album info & comment");
		stl_lb_album.set_alignment(0, 0.5);
		stl_spb_library.set_digits(0);
		stl_spb_library.set_range(6, 16);
		stl_spb_library.set_increments(1, 1);
		stl_lb_library.set_text("Library & playlist");
		stl_lb_library.set_alignment(0, 0.5);
		stl_lb_library.set_hexpand(true);
		stl_fr_colors.set_label("Player colors");
		stl_fr_colors.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
		stl_grid_colors.set_row_spacing(0);
		stl_grid_colors.set_border_width(4);
		stl_lb_hue.set_markup("<small>Hue</small>");
		stl_lb_hue.set_alignment(0, 0.5);
		stl_lb_hue.set_hexpand(true);
		stl_scl_hue = new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL);
		stl_scl_hue->set_draw_value(false);
		stl_scl_hue->set_range (0, 360);
		stl_lb_sat.set_markup("<small>Saturation</small>");
		stl_lb_sat.set_alignment(0, 0.5);
		stl_scl_sat = new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL);
		stl_scl_sat->set_draw_value(false);
		stl_scl_sat->set_range (0, 100);
		stl_lb_val.set_markup("<small>Value</small>");
		stl_lb_val.set_alignment(0, 0.5);
		stl_scl_val = new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL);
		stl_scl_val->set_range (0, 100);
		stl_scl_val->set_draw_value(false);
		stl_grid_preview.set_row_spacing(0);
		stl_fr_preview.set_margin_top(6);
		stl_lb_titleinfo.set_markup("<b>☆ artist : title ☆</b>");
		stl_lb_titleinfo.set_size_request(-1, 30);
		stl_lb_titleinfo.set_hexpand(true);
		stl_lb_albuminfo.set_markup("<small><b>album</b> year comment</small>");
		stl_lb_albuminfo.set_size_request(-1, 30);
		stl_grid_previewreset.set_valign(Gtk::ALIGN_END);
		stl_bt_colreset.set_margin_left(4);
		stl_bt_colreset.set_size_request(90, -1);
		stl_bt_colreset.set_label("Reset");
	abt_grid_main.set_border_width(6);
		abt_grid_main.set_row_spacing(6);
		pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
		abt_image.set(pxb);
		abt_label.set_markup("<b>Guimup 0.3.2</b>");
		abt_label.set_alignment(0, 0.5);
		abt_scrollwin.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
		abt_scrollwin.set_shadow_type(Gtk::SHADOW_NONE);
		abt_scrollwin.set_vexpand(true);
		abt_scrollwin.set_hexpand(true);
		ustring s_about = "&#10;<small><b>G</b>tk <b>U</b>ser <b>I</b>nterface for the <b>MU</b>sic <b>P</b>layer daemon</small>&#10;&#10;";
        s_about += "Copyright 2012 Johan Spee&#10;guimup@coonsden.com&#10;&#10;";
        s_about += "License&#10;";
        s_about += "<small>This program is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but without any warrenty; without even the implied warranty of merchantability or fitness for a particular purpose. See the GNU General Public License for details.</small>&#10;&#10;";
       	s_about += "<span color='#1e5070'><b>TIPS</b></span>&#10;&#10;";
        s_about += "Right-clicking the player brings up a menu where you can start <span color='#1e5070'>external programs</span> to view albumart, edit audio tags etc. Preferred programs can be set here, in the Client tab. Note that Guimup must be able to access MPD's music directory (and files).&#10;&#10;";
        s_about += "Clicking the time counter toggles the <span color='#1e5070'>time mode</span>, i.e. <i>time elapsed</i> and <i>time remaining</i>.&#10;&#10;";
        s_about += "Clicking the middle mouse-button on the system-tray icon toggles <span color='#1e5070'>play / pause.</span>&#10;&#10;";
        s_about += "The <span color='#1e5070'>volume</span> can be controlled with the mouse wheel when the cursor is on the system-tray icon - and by using the slider in the player.&#10;&#10;";
        s_about += "Pressing the spacebar in the playlist scrolls the <span color='#1e5070'>current song</span> into view.&#10;&#10;";
        s_about += "A <span color='#1e5070'>scrolling title</span> speeds up (quite a bit) when the mouse cursor is moved over it.&#10;&#10;";
        s_about += "<span color='#1e5070'>Album-art</span> must be located in the same directory with the music file. The program looks for a filename that matches <i>*albumart*, *folder*, *front*, </i> or <i>*cover* (in that order).</i>&#10;&#10;";
        s_about += "<span color='#1e5070'>Selecting</span> an item in the library tree will disable the selection of subitems. A selected artist's albums, for instance, can not be selected because the artist already includes them.&#10;&#10;";
        s_about += "Guimup will accept <span color='#1e5070'>external files</span> (files that are not in MPD's database) if it is connected to MPD through a socket (both 'bind_to_address' in MPD's configuration-file and 'host' in Guimup's connection-profile must point to <i>~/.mpd/socket</i>). Files can be dropped on the playlist.&#10;&#10;";
		s_about += "In addition external files can be added on the commannd line (or using \"Open with\" in the file manager). Run '<i>guimup -h</i>' for an overview of the use of <span color='#1e5070'>command line parameters</span>&#10;&#10;";
		s_about += "When files are modified without changing the timestamp a simple <span color='#1e5070'>update</span> of the database will not pick up those changes. Try 'Rescan tags' from the library's context-menu instead.&#10;&#10;";
		s_about	+= "To view <span color='#1e5070'>album-art</span> or open files in <span color='#1e5070'>external programs</span> (such as a tag editor) both MPD and Guimup must use the SAME \"music directory\".&#10;";
		s_about	+= "This directory is set in MPD's configuration-file: make sure Guimup's connection-profile points to the configuration-file that MPD is actually using. You can set MPD's configuration-file with the MPDCONF parameter in /etc/default/mpd.&#10;&#10;";
		s_about	+= "Note that the music directory is not accessable to Guimup when MPD runs on a remote computer.&#10;&#10;&#10;";

	
		s_about += "<b>Enjoy!&#10;&#10;</b>";
		abt_text.set_justify(Gtk::JUSTIFY_CENTER);
		abt_text.set_size_request(240,-1);
		abt_text.set_line_wrap(true);
		abt_text.set_markup(s_about);

	// Put widgets together
	add(mainGrid);
	mainGrid.attach(theNotebook, 0, 0, 1, 1);
	mainGrid.attach(theButtonBox, 0, 1, 1, 1);
	theButtonBox.pack_start(but_Close, false, false, 0);
	theButtonBox.pack_start(but_Apply, false, false, 0);
  	theNotebook.append_page(cnt_grid_main, " Connect ");
  	theNotebook.append_page(srv_grid_main, " Server ");
  	theNotebook.append_page(cli_grid_main, " Client ");
	theNotebook.append_page(stl_grid_main, " Style ");
  	theNotebook.append_page(abt_grid_main, " About ");

	cnt_grid_main.attach(cnt_cb_autocon, 0, 0, 1, 1);
	cnt_grid_main.attach(cnt_fr_profiles, 0, 1, 1, 1);
		cnt_fr_profiles.add(cnt_grid_profiles);
			cnt_grid_profiles.attach(cb_profile,0,0,1,1);
			cnt_grid_profiles.attach(cnt_et_name,0,1,1,1);
			cnt_grid_profiles.attach(cnt_lb_name,1,1,1,1);
			cnt_grid_profiles.attach(cnt_et_host,0,2,1,1);
			cnt_grid_profiles.attach(cnt_lb_host,1,2,1,1);
			cnt_grid_profiles.attach(cnt_et_port,0,3,1,1);
			cnt_grid_profiles.attach(cnt_lb_port,1,3,1,1);
			cnt_grid_profiles.attach(cnt_et_pswd,0,4,1,1);
			cnt_grid_profiles.attach(cnt_lb_pswd,1,4,1,1);
			cnt_grid_profiles.attach(cnt_et_conf,0,5,1,1);
			cnt_grid_profiles.attach(cnt_lb_conf,1,5,1,1);
	cnt_grid_main.attach(cnt_btConnect, 0, 2, 1, 1);
	cnt_grid_main.attach(cnt_label, 0, 3, 1, 1);
	cnt_grid_main.attach(cnt_image, 0, 4, 1, 1);

	srv_grid_main.attach(srv_fr_rpgain,0,0,1,1);
		srv_fr_rpgain.add(srv_grid_rpgain);
			srv_grid_rpgain.attach(srv_rbt_off,0,0,1,1);
			srv_grid_rpgain.attach(srv_rbt_trk,1,0,1,1);
			srv_grid_rpgain.attach(srv_rbt_alb,2,0,1,1);
			srv_grid_rpgain.attach(srv_rbt_aut,3,0,1,1);
	srv_grid_main.attach(srv_fr_xfade,0,1,1,1);
		srv_fr_xfade.add(srv_grid_xfade);
			srv_grid_xfade.attach(srv_spb_xfade,0,0,1,1);
			srv_grid_xfade.attach(srv_lb_xfade,1,0,1,1);
	srv_grid_main.attach(srv_fr_output,0,2,1,1);
		srv_fr_output.add(srv_grid_outputs);
			srv_grid_outputs.attach(srv_cb_out1,0,0,1,1);
			srv_grid_outputs.attach(srv_cb_out2,0,1,1,1);
			srv_grid_outputs.attach(srv_cb_out3,0,2,1,1);
			srv_grid_outputs.attach(srv_cb_out4,0,3,1,1);
	srv_grid_main.attach(srv_fr_command,0,3,1,1);
		srv_fr_command.add(srv_grid_command);
			srv_grid_command.attach(srv_cb_start,0,0,1,1);
			srv_grid_command.attach(srv_cb_kill,0,1,1,1);
			srv_grid_command.attach(srv_et_startcmd,1,0,1,1);
			srv_grid_command.attach(srv_et_killcmd,1,1,1,1);
	
	cli_grid_main.attach(cli_cb_systray,0,0,1,1);
	cli_grid_main.attach(cli_cb_ttips,0,1,1,1);
	cli_grid_main.attach(cli_cb_noart,0,2,1,1);
	cli_grid_main.attach(cli_fr_lib,0,3,1,1);
		cli_fr_lib.add(cli_grid_lib);
			cli_grid_lib.attach(cli_cb_byear,0,0,1,1);
			cli_grid_lib.attach(cli_cb_nothe,0,1,1,1);
		    cli_grid_lib.attach(cli_cb_libfixed,0,2,1,1);                              
	cli_grid_main.attach(cli_fr_plist,0,4,1,1);
		cli_fr_plist.add(cli_grid_plist);
			cli_grid_plist.attach(cli_cb_start,0,0,1,1);
			cli_grid_plist.attach(cli_cb_mark,0,1,1,1);
			cli_grid_plist.attach(cli_cb_plistfixed,0,2,1,1);
	cli_grid_main.attach(cli_fr_extprogs,0,5,1,1);
		cli_fr_extprogs.add(cli_grid_extprogs);
		cli_grid_extprogs.attach(cli_et_imgview,0,0,1,1);
		cli_grid_extprogs.attach(cli_lb_imgview,1,0,1,1);
		cli_grid_extprogs.attach(cli_et_tagedit,0,1,1,1);
		cli_grid_extprogs.attach(cli_lb_tagedit,1,1,1,1);
		cli_grid_extprogs.attach(cli_et_fileman,0,2,1,1);
		cli_grid_extprogs.attach(cli_lb_fileman,1,2,1,1);

	stl_grid_main.attach(stl_fr_fonts,0,0,1,1);
		stl_fr_fonts.add(stl_grid_fonts);
			stl_grid_fonts.attach(stl_spb_title,0,0,1,1);
			stl_grid_fonts.attach(stl_lb_title,1,0,1,1);
			stl_grid_fonts.attach(stl_spb_trackinfo,0,1,1,1);
			stl_grid_fonts.attach(stl_lb_trackinfo,1,1,1,1);
			stl_grid_fonts.attach(stl_spb_time,0,2,1,1);
			stl_grid_fonts.attach(stl_lb_time,1,2,1,1);
			stl_grid_fonts.attach(stl_spb_album,0,3,1,1);
			stl_grid_fonts.attach(stl_lb_album,1,3,1,1);
			stl_grid_fonts.attach(stl_spb_library,0,4,1,1);
			stl_grid_fonts.attach(stl_lb_library,1,4,1,1);
			stl_grid_fonts.attach(stl_bt_fontreset,2,4,1,1);
	stl_grid_main.attach(stl_fr_colors,0,1,1,1);
			stl_fr_colors.add(stl_grid_colors);
				stl_grid_colors.attach(stl_lb_hue,0,0,1,1);
				stl_grid_colors.attach(*stl_scl_hue,0,1,1,1);
				stl_grid_colors.attach(stl_lb_sat,0,2,1,1);
				stl_grid_colors.attach(*stl_scl_sat,0,3,1,1);
				stl_grid_colors.attach(stl_lb_val,0,4,1,1);
				stl_grid_colors.attach(*stl_scl_val,0,5,1,1);
			 	stl_grid_colors.attach(stl_grid_previewreset,1,5,1,1);
					stl_grid_previewreset.add(stl_bt_colreset);
			  	stl_grid_colors.attach(stl_fr_preview,0,6,2,1);
					stl_fr_preview.add(stl_grid_preview);
					stl_grid_preview.attach(stl_eb_titleinfo,0,0,1,1);
					stl_eb_titleinfo.add(stl_lb_titleinfo);
					stl_grid_preview.attach(stl_eb_albuminfo,0,1,1,1);
					stl_eb_albuminfo.add(stl_lb_albuminfo);

	abt_grid_main.attach(abt_image,0,0,1,1);
	abt_grid_main.attach(abt_label,1,0,1,1);
	abt_grid_main.attach(abt_fr_swin,0,1,2,1);
		abt_fr_swin.add(abt_scrollwin);
			abt_scrollwin.add(abt_text);
  	show_all_children();
	
	// widget signals
	cb_profile.signal_changed().connect(sigc::mem_fun(*this,
	     	 &gm_settings::on_profile_change));
  	but_Close.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_button_close) );
  	but_Apply.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_apply_all) );
	cnt_btConnect.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_button_connect) );
	stl_bt_fontreset.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_fonts_reset) );
	stl_scl_hue->signal_value_changed().connect(sigc::mem_fun(*this,
             &gm_settings::on_color_change) );
	stl_scl_sat->signal_value_changed().connect(sigc::mem_fun(*this,
             &gm_settings::on_color_change) );	
	stl_scl_val->signal_value_changed().connect(sigc::mem_fun(*this,
             &gm_settings::on_color_change) );
	stl_bt_colreset.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_colors_reset) );
}


void gm_settings::on_profile_change()
{
	int profl = cb_profile.get_active_row_number();
	
	if (profl == 0)
	{
		cnt_et_name.set_editable(false);
		cnt_et_port.set_editable(false);
		cnt_et_host.set_editable(false);
		cnt_et_pswd.set_editable(false);
		cnt_et_conf.set_editable(false);		
	}
	else
	{
		cnt_et_name.set_editable(true);
		cnt_et_port.set_editable(true);
		cnt_et_host.set_editable(true);
		cnt_et_pswd.set_editable(true);
		cnt_et_conf.set_editable(true);		
	}

	switch (profl)
	{
		case 0:
		{
			if (config->MPD_host_0.empty())
				cnt_et_name.set_text("Connect to detect");
			else
				cnt_et_name.set_text("Default if not detected");
			if (config->MPD_port_0 != 0)
				cnt_et_port.set_text(into_string (config->MPD_port_0));
			else
				cnt_et_port.set_text("");
			cnt_et_host.set_text(config->MPD_host_0);
			cnt_et_pswd.set_text(config->MPD_password_0);
			cnt_et_conf.set_text(config->MPD_configpath_0);
			break;
		}
		case 1:
		{
			cnt_et_name.set_text(config->MPD_name_1);
			if (config->MPD_port_1 != 0)
				cnt_et_port.set_text(into_string (config->MPD_port_1));
			else
				cnt_et_port.set_text("");
			cnt_et_host.set_text(config->MPD_host_1);
			cnt_et_pswd.set_text(config->MPD_password_1);
			cnt_et_conf.set_text(config->MPD_configpath_1);
			break;
		}
		case 2:
		{
			cnt_et_name.set_text(config->MPD_name_2);
			if (config->MPD_port_2 != 0)
				cnt_et_port.set_text(into_string (config->MPD_port_2));
			else
				cnt_et_port.set_text("");
			cnt_et_host.set_text(config->MPD_host_2);
			cnt_et_pswd.set_text(config->MPD_password_2);
			cnt_et_conf.set_text(config->MPD_configpath_2);
			break;
		}
		case 3:
		{
			cnt_et_name.set_text(config->MPD_name_3);
			if (config->MPD_port_3 != 0)
				cnt_et_port.set_text(into_string (config->MPD_port_3));
			else
				cnt_et_port.set_text("");
			cnt_et_host.set_text(config->MPD_host_3);
			cnt_et_pswd.set_text(config->MPD_password_3);
			cnt_et_conf.set_text(config->MPD_configpath_3);
			break;
		}	
	}	
}


void gm_settings::on_button_close()
{
	get_position(config->settingsWindow_Xpos, config->settingsWindow_Ypos);
	hide();
}


void gm_settings::set_tooltips()
{
	cnt_cb_autocon.set_has_tooltip(config->show_ToolTips);
	cb_profile.set_has_tooltip(config->show_ToolTips);
	cnt_btConnect.set_has_tooltip(config->show_ToolTips);
	cli_cb_ttips.set_has_tooltip(config->show_ToolTips);
	cli_cb_noart.set_has_tooltip(config->show_ToolTips);
	srv_et_startcmd.set_has_tooltip(config->show_ToolTips);
	srv_et_killcmd.set_has_tooltip(config->show_ToolTips);
	srv_spb_xfade.set_has_tooltip(config->show_ToolTips);
	cli_cb_systray.set_has_tooltip(config->show_ToolTips);
	cli_cb_nothe.set_has_tooltip(config->show_ToolTips);
	cli_cb_mark.set_has_tooltip(config->show_ToolTips);
	cli_cb_libfixed.set_has_tooltip(config->show_ToolTips);
	cli_cb_plistfixed.set_has_tooltip(config->show_ToolTips);
	cli_et_imgview.set_has_tooltip(config->show_ToolTips);
	cli_et_tagedit.set_has_tooltip(config->show_ToolTips);
	cli_et_fileman.set_has_tooltip(config->show_ToolTips);
	cli_cb_byear.set_has_tooltip(config->show_ToolTips);
	cli_cb_start.set_has_tooltip(config->show_ToolTips);
	but_Apply.set_has_tooltip(config->show_ToolTips);
	but_Close.set_has_tooltip(config->show_ToolTips);
}


void gm_settings::on_apply_all()
{
	if (config == NULL)
	{
		cout << "Settings: Can't save config file" << endl;
		return;
	}
	config->AutoConnect = cnt_cb_autocon.get_active();
	if (config->show_ToolTips != cli_cb_ttips.get_active())
	{
		config->show_ToolTips = cli_cb_ttips.get_active();
		set_tooltips();
		signal_settooltips.emit();
	}
	config->disable_Albumart = cli_cb_noart.get_active();
	config->use_TrayIcon = cli_cb_systray.get_active();

	if (config->mpd_xfade_allowed)
	{
		if (srv_spb_xfade.get_value() != xfade_time && b_connected)
		{

			xfade_time = srv_spb_xfade.get_value();
			mpdCom->set_xfade(xfade_time);
		}
	}

	if (config->mpd_rpgain_allowed)
	{
		int rpgain_checked = 0;
		if (srv_rbt_trk.get_active())
			rpgain_checked = 1;
		else
		if (srv_rbt_alb.get_active())
			rpgain_checked = 2;
		else
		if (srv_rbt_aut.get_active())
			rpgain_checked = 3;    
		if (rpgain_checked != rpgain_mode && b_connected)
		{
			rpgain_mode = rpgain_checked;
			mpdCom->set_replaygain(rpgain_mode);
		}
	}

	int prfl = cb_profile.get_active_row_number();
	config->MPD_profile = prfl;
	switch (prfl)
	{
		case 0:
			break;
		case 1:
		{
			config->MPD_name_1 = cnt_et_name.get_text();
			config->MPD_port_1 = atoi(cnt_et_port.get_text().data());
			config->MPD_host_1 = cnt_et_host.get_text();
			config->MPD_password_1 = cnt_et_pswd.get_text();
			config->MPD_configpath_1 = cnt_et_conf.get_text();
			break;
		}
		case 2:
		{
			config->MPD_name_2 = cnt_et_name.get_text();
			config->MPD_port_2 = atoi(cnt_et_port.get_text().data());
			config->MPD_host_2 = cnt_et_host.get_text();
			config->MPD_password_2 = cnt_et_pswd.get_text();
			config->MPD_configpath_2 = cnt_et_conf.get_text();
			break;
		}	
		case 3:
		{
			config->MPD_name_3 = cnt_et_name.get_text();
			config->MPD_port_3 = atoi(cnt_et_port.get_text().data());
			config->MPD_host_3 = cnt_et_host.get_text();
			config->MPD_password_3 = cnt_et_pswd.get_text();
			config->MPD_configpath_3 = cnt_et_conf.get_text();
			break;
		}
		default:
			break;
	}

	// server
	bool b_set_outputs = false;
	if (b_connected && config->mpd_outputs_allowed)
	{
		gm_outputList oDevices;
		gm_output addDev;
			// dev1
		if (srv_cb_out1.is_sensitive())
		{
			addDev.id = 0;
			addDev.enabled = srv_cb_out1.get_active();
			if (addDev.enabled != b_output1)
			{
				b_output1 = addDev.enabled;
				b_set_outputs = true;
			}
			oDevices.push_back(addDev);
		}
			// dev2
		if (srv_cb_out2.is_sensitive())
		{
			addDev.id = 1;
			addDev.enabled = srv_cb_out2.get_active();
			if (addDev.enabled != b_output2)
			{
				b_output2 = addDev.enabled;
				b_set_outputs = true;
			}
			oDevices.push_back(addDev);
		}
			// dev3
		if (srv_cb_out3.is_sensitive())
		{
			addDev.id = 2;
			addDev.enabled = srv_cb_out3.get_active();
			if (addDev.enabled != b_output3)
			{
				b_output3 = addDev.enabled;
				b_set_outputs = true;
			}
			oDevices.push_back(addDev);
		}
			// dev4
		if (srv_cb_out4.is_sensitive())
		{
			addDev.id = 3;
			addDev.enabled = srv_cb_out4.get_active();
			if (addDev.enabled != b_output4)
			{
				b_output4 = addDev.enabled;
				b_set_outputs = true;
			}
			oDevices.push_back(addDev);
		}
		if (b_set_outputs)
			mpdCom->set_outputs(oDevices);
	}
	
	config->MPD_onStart_command = srv_et_startcmd.get_text();
	config->MPD_onQuit_command = srv_et_killcmd.get_text();

	config->QuitMPD_onQuit = srv_cb_kill.get_active();
	config->StartMPD_onStart = srv_cb_start.get_active();
	// fonts
	bool b_fonts_changed = false;
	if (config->Scroller_Fontsize != stl_spb_title.get_value())
	{
		b_fonts_changed = true;
		config->Scroller_Fontsize = stl_spb_title.get_value();
	}
	if (config->TrackInfo_Fontsize != stl_spb_trackinfo.get_value())
	{
		b_fonts_changed = true;
		config->TrackInfo_Fontsize = stl_spb_trackinfo.get_value();
	}
	if (config->Time_Fontsize != stl_spb_time.get_value())
	{
		b_fonts_changed = true;
		config->Time_Fontsize = stl_spb_time.get_value();
	}
	if (config->Album_Fontsize != stl_spb_album.get_value())
	{
		b_fonts_changed = true;
		config->Album_Fontsize = stl_spb_album.get_value();
	}
	if (config->browser_Fontsize != stl_spb_library.get_value())
	{
		b_fonts_changed = true;
		config->browser_Fontsize = stl_spb_library.get_value();
	}
	if (b_fonts_changed)
		signal_applyfonts.emit();
	// colors
	bool b_colors_changed = false;
	if 	(stl_scl_sat->get_value() != config->color_saturation)
	{
		config->color_saturation = stl_scl_sat->get_value();
		b_colors_changed = true;
 	}
	if 	(stl_scl_hue->get_value() != config->color_hue)
	{
		config->color_hue = stl_scl_hue->get_value();
		b_colors_changed = true;
 	}
	if 	(stl_scl_val->get_value() != config->color_value)
	{
		config->color_value = stl_scl_val->get_value();
		b_colors_changed = true;
 	}
	if (b_colors_changed)
		signal_applycolors.emit();

	config->lib_sort_albums_byear = cli_cb_byear.get_active();
    config->lib_ignore_leading_the = cli_cb_nothe.get_active();
	config->pList_new_startplaying = cli_cb_start.get_active();
	
	if (config->pList_mark_played != cli_cb_mark.get_active())
	{
    	config->pList_mark_played = cli_cb_mark.get_active();
		signal_setmarkplayed.emit();
	}
	
	if (b_lib_fixed != cli_cb_libfixed.get_active())
	{
		b_lib_fixed = cli_cb_libfixed.get_active();
		config->lib_fixed_columns = b_lib_fixed;
		signal_lib_setfixed.emit();
	}
	
	if (b_pList_fixed != cli_cb_plistfixed.get_active())
	{
		b_pList_fixed = cli_cb_plistfixed.get_active();
		config->pList_fixed_columns = b_pList_fixed;
		signal_plist_setfixed.emit();
	}

	config->Image_Viewer = cli_et_imgview.get_text();
	config->Tag_Editor = cli_et_tagedit.get_text();	
	config->File_Manager = cli_et_fileman.get_text();
 	// save config to disk in player
	signal_savesettings.emit();
}


void gm_settings::on_button_connect()
{
	int prfl = cb_profile.get_active_row_number();
	config->MPD_profile = prfl;

	switch (prfl)
	{
		case 0:
			break;
		case 1:
		{
			config->MPD_name_1 = cnt_et_name.get_text();
			config->MPD_port_1 = atoi(cnt_et_port.get_text().data());
			config->MPD_host_1 = cnt_et_host.get_text();
			config->MPD_password_1 = cnt_et_pswd.get_text();
			config->MPD_configpath_1 = cnt_et_conf.get_text();
			break;
		}
		case 2:
		{
			config->MPD_name_2 = cnt_et_name.get_text();
			config->MPD_port_2 = atoi(cnt_et_port.get_text().data());
			config->MPD_host_2 = cnt_et_host.get_text();
			config->MPD_password_2 = cnt_et_pswd.get_text();
			config->MPD_configpath_2 = cnt_et_conf.get_text();
			break;
		}	
		case 3:
		{
			config->MPD_name_3 = cnt_et_name.get_text();
			config->MPD_port_3 = atoi(cnt_et_port.get_text().data());
			config->MPD_host_3 = cnt_et_host.get_text();
			config->MPD_password_3 = cnt_et_pswd.get_text();
			config->MPD_configpath_3 = cnt_et_conf.get_text();
			break;
		}
		default:
			break;
	}

	cout << "Connecting" << endl;
	mpdCom->mpd_disconnect(true);
	mpdCom->configure();
	mpdCom->mpd_connect();

	if (config->MPD_profile == 0)
	{
			if (config->MPD_host_0.empty())
				cnt_et_name.set_text("Connect to detect");
			else
				cnt_et_name.set_text("Default if not detected");
			if (config->MPD_port_0 != 0)
				cnt_et_port.set_text(into_string (config->MPD_port_0));
			else
				cnt_et_port.set_text("");
			cnt_et_host.set_text(config->MPD_host_0);
			cnt_et_pswd.set_text(config->MPD_password_0);
			cnt_et_conf.set_text(config->MPD_configpath_0);
 	}
}


void gm_settings::on_fonts_reset()
{
	stl_spb_title.set_value(11);
	stl_spb_trackinfo.set_value(9);
	stl_spb_time.set_value(9);
	stl_spb_album.set_value(9);
	stl_spb_library.set_value(10);
}


void gm_settings::load_config()
{
/*CONNECT*/
	// autoconnect
	cnt_cb_autocon.set_active(config->AutoConnect);
	// profiles
	cb_profile.set_active(config->MPD_profile);
	on_profile_change();
/*SERVER*/
	/* replaygain mode, cross fade & output devices 
	depend on b_connected: */
	set_connected(b_connected);
	// start / quit mpd
	srv_et_startcmd.set_text(config->MPD_onStart_command);
	srv_et_killcmd.set_text(config->MPD_onQuit_command);
	srv_cb_kill.set_active(config->QuitMPD_onQuit);
	srv_cb_start.set_active(config->StartMPD_onStart);

/*CLIENT*/	
	cli_cb_systray.set_active(config->use_TrayIcon);
	cli_cb_ttips.set_active(config->show_ToolTips);
	cli_cb_noart.set_active(config->disable_Albumart);
	cli_cb_byear.set_active(config->lib_sort_albums_byear);
    cli_cb_nothe.set_active(config->lib_ignore_leading_the);
	cli_cb_start.set_active(config->pList_new_startplaying);
    cli_cb_mark.set_active(config->pList_mark_played);
	b_pList_fixed = config->pList_fixed_columns;
	cli_cb_plistfixed.set_active(b_pList_fixed);
	b_lib_fixed = config->lib_fixed_columns;
	cli_cb_libfixed.set_active(b_lib_fixed);
	
	cli_et_imgview.set_text(config->Image_Viewer);
	cli_et_tagedit.set_text(config->Tag_Editor);
	cli_et_fileman.set_text(config->File_Manager);

/*STYLE*/
	// font sizes
	stl_spb_title.set_value(config->Scroller_Fontsize);
	stl_spb_trackinfo.set_value(config->TrackInfo_Fontsize);
	stl_spb_time.set_value(config->Time_Fontsize);
	stl_spb_album.set_value(config->Album_Fontsize);
	stl_spb_library.set_value(config->browser_Fontsize);

	stl_scl_hue->set_value(config->color_hue);
	stl_scl_sat->set_value(config->color_saturation);
	stl_scl_val->set_value(config->color_value);
	on_color_change();
} 


void gm_settings::on_colors_reset()
{
	stl_scl_sat->set_value(20); 
	stl_scl_hue->set_value(204);
	stl_scl_val->set_value(50);
	on_color_change();	
}


void gm_settings::on_color_change()
{
	// HSV
	double sat = stl_scl_sat->get_value()/100;
	double hue = stl_scl_hue->get_value();
	double value = (50 - stl_scl_val->get_value())/200; // -0.25 .. +0.25

	double val;
	
	val = (0.5 + value) * 0.92;
	titleinfo_bg_color.set_hsv(hue, sat, val),
	stl_eb_titleinfo.override_background_color(titleinfo_bg_color, Gtk::STATE_FLAG_NORMAL);
	
	val = (value * 0.08) + 0.96;
	titleinfo_fg_color.set_hsv(hue, sat/6, val); 
	stl_lb_titleinfo.override_color(titleinfo_fg_color, Gtk::STATE_FLAG_NORMAL);

	val = 0.6 + (0.5 - value) * 0.32;
	albuminfo_bg_color.set_hsv(hue, sat/4, val); 
	stl_eb_albuminfo.override_background_color(albuminfo_bg_color, Gtk::STATE_FLAG_NORMAL);

	val = (value * 0.80) + 0.20;
	albuminfo_fg_color.set_hsv(hue, sat/4, val);
	stl_lb_albuminfo.override_color(albuminfo_fg_color, Gtk::STATE_FLAG_NORMAL);
}


void gm_settings::set_config(gm_Config *conf)
{
	config = conf;
	
	if (config == NULL)
		return;
	
	b_use_trayicon = config->use_TrayIcon;
	set_tooltips();
}


void gm_settings::set_mpdCom(gm_mpdCom *com)
{
	mpdCom = com;
	if (mpdCom != NULL)
		mpdCom->sgnl_connected.connect(sigc::mem_fun(*this, &gm_settings::set_connected));
}


void gm_settings::set_connected(bool iscon)
{
	if (iscon)
	{
		b_connected = true;;
		gm_mpd_version vrs = mpdCom->get_version();
		ustring version = "<b>Connected</b> to MPD " + into_string(vrs.primary) + "." + into_string(vrs.secundary);
		cnt_label.set_markup(version);
		cnt_image.set(pxb_con_ok);

		rpgain_mode = mpdCom->get_replaygain();
		if (rpgain_mode <= 0)
			srv_rbt_off.set_active();
		else
		if (rpgain_mode == 1)
			srv_rbt_trk.set_active();
		else
		if (rpgain_mode == 2)
			srv_rbt_alb.set_active();
		else
		if (rpgain_mode == 3)
			srv_rbt_aut.set_active();
		
		if (config->mpd_rpgain_allowed)
		{
			srv_rbt_off.set_sensitive(true);
			srv_rbt_trk.set_sensitive(true);
			srv_rbt_alb.set_sensitive(true);
			srv_rbt_aut.set_sensitive(true);
		}
		else
		{
			srv_rbt_off.set_sensitive(false);
			srv_rbt_trk.set_sensitive(false);
			srv_rbt_alb.set_sensitive(false);
			srv_rbt_aut.set_sensitive(false);
		}

		xfade_time = mpdCom->get_xfade(); 
		srv_spb_xfade.set_value(xfade_time);
		if (config->mpd_xfade_allowed)
			srv_spb_xfade.set_sensitive(true);
		else
			srv_spb_xfade.set_sensitive(false);
	}
	else
	{
		b_connected = false;
		cnt_label.set_markup("<b>Not Connected</b>");
		cnt_image.set(pxb_con_dis);
		srv_rbt_off.set_sensitive(false);
		srv_rbt_trk.set_sensitive(false);
		srv_rbt_alb.set_sensitive(false);
		srv_rbt_aut.set_sensitive(false);
		srv_spb_xfade.set_sensitive(false);
	}

	// output devices
	b_output1 = false;
	b_output2 = false;
	b_output3 = false;
	b_output4 = false;
	gm_outputList oDevices;
	if (b_connected) // else empty list
		oDevices = mpdCom->get_outputs();
    std::vector<gm_output>::iterator cit;
    gm_output curDev;
    	// dev1
    cit = oDevices.begin();
    if (cit != oDevices.end())
    {
		if (config->mpd_outputs_allowed)
    		srv_cb_out1.set_sensitive(true);
		else
			srv_cb_out1.set_sensitive(false);
    	curDev = *cit;
    	srv_cb_out1.set_label(curDev.name);
        if (curDev.enabled)
		{
        	srv_cb_out1.set_active(true);
			b_output1 = true;
		}
    	else
        	srv_cb_out1.set_active(false);
		++cit;
    }
    else
    {
    	srv_cb_out1.set_label("Is mpd connected?");
    	srv_cb_out1.set_active(false);
    	srv_cb_out1.set_sensitive(false);
    }
    	// dev2
    if (cit != oDevices.end())
    {
		if (config->mpd_outputs_allowed)
    		srv_cb_out2.set_sensitive(true);
		else
			srv_cb_out2.set_sensitive(false);
    	curDev = *cit;
    	srv_cb_out2.set_label(curDev.name);
        if (curDev.enabled)
		{
        	srv_cb_out2.set_active(true);
			b_output2 = true;
		}
    	else
        	srv_cb_out2.set_active(false);
		++cit;
    }
    else
    {
    	srv_cb_out2.set_label("*");
    	srv_cb_out2.set_active(false);
    	srv_cb_out2.set_sensitive(false);
    }
    	// dev3
    if (cit != oDevices.end())
    {
		if (config->mpd_outputs_allowed)
    		srv_cb_out3.set_sensitive(true);
		else
			srv_cb_out3.set_sensitive(false);
    	curDev = *cit;
    	srv_cb_out3.set_label(curDev.name);
        if (curDev.enabled)
		{
        	srv_cb_out3.set_active(true);
			b_output3 = true;
		}
    	else
        	srv_cb_out3.set_active(false);
		++cit;
    }
    else
    {
    	srv_cb_out3.set_label("*");
    	srv_cb_out3.set_active(false);
    	srv_cb_out3.set_sensitive(false);
    }
   		// dev4
    if (cit != oDevices.end())
    {
		if (config->mpd_outputs_allowed)
    		srv_cb_out4.set_sensitive(true);
		else
			srv_cb_out4.set_sensitive(false);
    	curDev = *cit;
    	srv_cb_out4.set_label(curDev.name);
        if (curDev.enabled)
		{
        	srv_cb_out4.set_active(true);
			b_output4 = true;
		}
    	else
        	srv_cb_out4.set_active(false);
		++cit;
    }
    else
    {
    	srv_cb_out4.set_label("*");
    	srv_cb_out4.set_active(false);
    	srv_cb_out4.set_sensitive(false);
    }
}


std::string gm_settings::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


bool gm_settings::on_delete_event(GdkEventAny* event)
{
	get_configs();
	hide();
    return true;
}


void gm_settings::get_configs()
{
	if (get_visible())
		get_position(config->settingsWindow_Xpos, config->settingsWindow_Ypos);
}


gm_settings::~gm_settings()
{
}
