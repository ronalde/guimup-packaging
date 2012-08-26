/*
 *  gm_settings.cc
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

#include "gm_settings.h"
#include "gm_settings_pix.h"

gm_settings::gm_settings()
{
	st_mpdCom = NULL;
	st_config = NULL;
	// window properties
	Glib::RefPtr<Gdk::Pixbuf> pxb;
	pxb = Gdk::Pixbuf::create_from_inline(-1, mn_config, false);
	set_icon(pxb);
	pxb_con_ok  = Gdk::Pixbuf::create_from_inline(-1, con_ok, false);
	pxb_con_dis = Gdk::Pixbuf::create_from_inline(-1, con_dis, false);
    set_title("Guimup Settings");
    set_border_width(4);
    set_size_request(300, -1);
    set_resizable(false);
	// signals
  	but_Cancel.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_button_cancel) );
  	but_Save.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_button_save) );
	cnt_btConnect.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_button_connect) );
	cli_bt_reset.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_fonts_reset) );
	cli_bt_apply.signal_clicked().connect(sigc::mem_fun(*this,
             &gm_settings::on_fonts_apply) );
	// widgets
	mainVBox.set_spacing(6);	
	theButtonBox.set_layout(Gtk::BUTTONBOX_END);
	theButtonBox.set_spacing(8);
		but_Cancel.set_label("Cancel");
		but_Cancel.set_size_request(60, -1);	
		but_Save.set_size_request(60, -1);	
		but_Save.set_label("Save");	
	vbx_connect.set_border_width(6);
	vbx_connect.set_spacing(6);
		cnt_framevbox.set_border_width(4);
		cnt_framevbox.set_spacing(4);
		cnt_cb_autocon.set_label(" Autoconnect");
	cnt_cb_override.set_label(" Override MPD's config with:");
		cnt_hb_host.set_spacing(12);
		cnt_et_host.set_size_request(150, -1);
	    cnt_lb_host.set_text("Host");
		cnt_lb_host.set_alignment(0, 0.5);
		cnt_hb_port.set_spacing(12);
		cnt_et_port.set_size_request(150, -1);
	    cnt_lb_port.set_text("Port");
		cnt_lb_port.set_alignment(0, 0.5);
		cnt_hb_pswd.set_spacing(12);
		cnt_et_pswd.set_size_request(150, -1);
	    cnt_lb_pswd.set_text("Password");
		cnt_lb_pswd.set_alignment(0, 0.5);
		cnt_btcbox.set_border_width(0);
		cnt_btcbox.set_spacing(0);
		cnt_btConnect.set_label("Connect");
		cnt_btConnect.set_size_request(120, -1);
		cnt_bottomhbox.set_border_width(0);
		cnt_bottomhbox.set_spacing(8);
		cnt_image.set_size_request(32, 32);
		cnt_label.set_alignment(0, 0.5);
		cnt_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
	vbx_server.set_border_width(6);
	vbx_server.set_spacing(6);
		srv_fr_output.set_label("Output devices");
		srv_frout_vbox.set_border_width(4);
		srv_frout_vbox.set_spacing(4);
		srv_hb_mpath.set_spacing(12);
		srv_et_mpath.set_size_request(150, -1);
	    srv_lb_mpath.set_text("Music path");
		srv_lb_mpath.set_alignment(0, 0.5);
		srv_hb_ppath.set_spacing(12);
		srv_et_ppath.set_size_request(150, -1);
	    srv_lb_ppath.set_text("Playlist path");
		srv_lb_ppath.set_alignment(0, 0.5);
		srv_fr_manage.set_label("Manage MPD");
		srv_frmng_vbox.set_border_width(4);
		srv_frmng_vbox.set_spacing(4);
		srv_cb_launch.set_label(" Launch on startup");
		srv_cb_kill.set_label(  " Shut down on exit");
	vbx_client.set_border_width(6);
	vbx_client.set_spacing(6);
	cli_fr_fonts.set_label("Font sizes");
	cli_framevbox.set_border_width(4);
	cli_framevbox.set_spacing(4);
	cli_hbx_fontbtns.set_spacing(6);
	cli_bt_reset.set_label("Reset all");
	cli_bt_reset.set_size_request(120, -1);
	cli_bt_apply.set_label("Apply");
	cli_bt_apply.set_size_request(120, -1);
	cli_spb_title.set_digits(0);
	cli_spb_title.set_range(8, 16);
	cli_spb_title.set_increments(1, 1);
	cli_lb_title.set_text("Title scroller");
	cli_lb_title.set_alignment(0, 0.5);
	cli_spb_trackinfo.set_digits(0);
	cli_spb_trackinfo.set_range(8, 16);
	cli_spb_trackinfo.set_increments(1, 1);
	cli_lb_trackinfo.set_text("Track tech info");
	cli_lb_trackinfo.set_alignment(0, 0.5);
	cli_spb_time.set_digits(0);
	cli_spb_time.set_range(8, 16);
	cli_spb_time.set_increments(1, 1);
	cli_lb_time.set_text("Playtime & total time");
	cli_lb_time.set_alignment(0, 0.5);
	cli_spb_album.set_digits(0);
	cli_spb_album.set_range(8, 16);
	cli_spb_album.set_increments(1, 1);	
	cli_lb_album.set_text("Album name & year");
	cli_lb_album.set_alignment(0, 0.5);
	cli_spb_tracks.set_digits(0);
	cli_spb_tracks.set_range(8, 16);
	cli_spb_tracks.set_increments(1, 1);
	cli_lb_tracks.set_text("Data tree & playlist");
	cli_lb_tracks.set_alignment(0, 0.5);
	cli_fr_restart.set_label("Restart to apply");
	cli_vb_restart.set_border_width(4);
	cli_vb_restart.set_spacing(4);
	cli_cb_systray.set_label(" Use system tray");
	cli_cb_ttips.set_label(" Show tooltips");
	cli_et_art.set_size_request(120, -1);
	cli_lb_art.set_text("Custom album art");
	vbx_about.set_border_width(6);
		abt_hbox.set_size_request(-1,40);
		abt_hbox.set_spacing(8);
		pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
		abt_image.set(pxb);
		abt_label.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
		abt_label.set_markup("<b>Guimup 0.1.4</b>");
		abt_label.set_alignment(0, 0.5);
		abt_scrollwin.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
		ustring s_about;
		s_about += "&#10;";
		s_about += "GTK User Interface for MPD&#10;&#10;";
		s_about += "© 2008 - 2009 Johan Spee&#10;www.coonsden.com&#10;&#10;";
		s_about += "<small><b>License</b>&#10;&#10;";
		s_about += "Guimup is free software. You can redistribute it and [or] modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.&#10;&#10;";
		s_about += "Guimup is distributed in the hope that it will be useful, but without any warrenty; without even the implied warranty of merchantability or fitness for a particular purpose. See the GNU General Public License for details.&#10;&#10;";
		s_about += "You should have received a copy of the GNU General Public License along with Guimup (the file named COPYING).&#10;If not, see http://www.gnu.org/licenses/&#10;";
		s_about += "</small>";
		abt_text.set_size_request(240,-1);
		abt_text.set_line_wrap(true);
		abt_text.set_markup(s_about);
	add(mainVBox);
	// Add the notebook & buttons to the main VBox
	mainVBox.pack_start(theNotebook, true, true, 0);
	mainVBox.pack_start(theButtonBox, false, false, 0);
 	theButtonBox.pack_start(but_Cancel, false, false, 0);
	theButtonBox.pack_start(but_Save, false, false, 0);
  	// Add the tabs to the Notebook
  	theNotebook.append_page(vbx_connect, " Connect ");
  	theNotebook.append_page(vbx_server, " Server ");
  	theNotebook.append_page(vbx_client, " Client ");
  	theNotebook.append_page(vbx_about, " About ");
	// Fill the tab pages
	vbx_connect.pack_start(cnt_cb_autocon,false, false, 0);
	vbx_connect.pack_start(cnt_cb_override,false, false, 0);
	vbx_connect.pack_start(cnt_fr_settings,false, false, 0);
		cnt_fr_settings.add(cnt_framevbox);
			cnt_framevbox.pack_start(cnt_hb_host,false, false, 0);
				cnt_hb_host.pack_start(cnt_et_host,false, false, 0);
				cnt_hb_host.pack_end(cnt_lb_host,true, true, 0);
			cnt_framevbox.pack_start(cnt_hb_port,false, false, 0);
				cnt_hb_port.pack_start(cnt_et_port,false, false, 0);
				cnt_hb_port.pack_end(cnt_lb_port,true, true, 0);
			cnt_framevbox.pack_start(cnt_hb_pswd,false, false, 0);
				cnt_hb_pswd.pack_start(cnt_et_pswd,false, false, 0);
				cnt_hb_pswd.pack_end(cnt_lb_pswd,true, true, 0);
	vbx_connect.pack_start(cnt_btcbox, true, false, 0);
		cnt_btcbox.pack_start(cnt_btConnect, false, false, 6);
		cnt_btcbox.pack_start(cnt_spacer, true, true, 0);
	vbx_connect.pack_end(cnt_bottomhbox, false, false, 0);
		cnt_bottomhbox.pack_start(cnt_image, false, false, 8);
		cnt_bottomhbox.pack_end(cnt_label, true, true, 0);
	
	vbx_server.pack_start(srv_fr_output, false, false, 2);
		srv_fr_output.add(srv_frout_vbox);
			srv_frout_vbox.pack_start(srv_cb_out1,false, false, 0);
			srv_frout_vbox.pack_start(srv_cb_out2,false, false, 0);
			srv_frout_vbox.pack_start(srv_cb_out3,false, false, 0);
			srv_frout_vbox.pack_start(srv_cb_out4,false, false, 0);
			srv_frout_vbox.pack_end(srv_cb_out5,false, false, 0);
	
	vbx_server.pack_start(srv_hb_mpath,false, false, 0);
		srv_hb_mpath.pack_start(srv_et_mpath,false, false, 0);
		srv_hb_mpath.pack_end(srv_lb_mpath,true, true, 0);
	vbx_server.pack_start(srv_hb_ppath,false, false, 0);
		srv_hb_ppath.pack_start(srv_et_ppath,false, false, 0);
		srv_hb_ppath.pack_end(srv_lb_ppath,true, true, 0);

	vbx_server.pack_end(srv_fr_manage, false, false, 2);
		srv_fr_manage.add(srv_frmng_vbox);
			srv_frmng_vbox.pack_start(srv_cb_launch,false, false, 0);
			srv_frmng_vbox.pack_start(srv_cb_kill,false, false, 0);
	
	vbx_client.pack_start(cli_fr_fonts, false, false, 0);
		cli_fr_fonts.add(cli_framevbox);
			cli_framevbox.pack_start(cli_hbx_title, false, false, 0);
				cli_hbx_title.pack_start(cli_spb_title, false, false, 0);
				cli_hbx_title.pack_start(cli_lb_title, true, true, 8);
			cli_framevbox.pack_start(cli_hbx_trackinfo, false, false, 0);
				cli_hbx_trackinfo.pack_start(cli_spb_trackinfo, false, false, 0);
				cli_hbx_trackinfo.pack_start(cli_lb_trackinfo, true, true, 8);
			cli_framevbox.pack_start(cli_hbx_time, false, false, 0);
				cli_hbx_time.pack_start(cli_spb_time, false, false, 0);
				cli_hbx_time.pack_start(cli_lb_time, true, true, 8);	
			cli_framevbox.pack_start(cli_hbx_album, false, false, 0);
				cli_hbx_album.pack_start(cli_spb_album, false, false, 0);
				cli_hbx_album.pack_start(cli_lb_album, true, true, 8);
			cli_framevbox.pack_start(cli_hbx_tracks, false, false, 0);
				cli_hbx_tracks.pack_start(cli_spb_tracks, false, false, 0);
				cli_hbx_tracks.pack_start(cli_lb_tracks, false, false, 8);
			cli_framevbox.pack_start(cli_hbx_fontbtns, false, false, 0);
				cli_hbx_fontbtns.pack_start(cli_bt_reset, false, false, 8);
				cli_hbx_fontbtns.pack_end(cli_bt_apply, false, false, 8);
	vbx_client.pack_start(cli_fr_restart, true, false, 0);
		cli_fr_restart.add(cli_vb_restart);
			cli_vb_restart.pack_start(cli_cb_systray, false, false, 0);
			cli_vb_restart.pack_start(cli_cb_ttips, true, false, 0);
	vbx_client.pack_end(cli_hb_art, false, false, 0);
		cli_hb_art.pack_start(cli_et_art, false, false, 0);
		cli_hb_art.pack_start(cli_lb_art, false, false, 6);
		
	vbx_about.pack_start(abt_hbox, false, false, 0);
		abt_hbox.pack_start(abt_image, false, false, 8);
		abt_hbox.pack_end(abt_label, true, true, 8);
	vbx_about.pack_start(abt_scrollwin, true, true, 0);
		abt_scrollwin.add(abt_text);
  	show_all_children();
}


void gm_settings::on_button_cancel()
{
	this->hide();
}

void gm_settings::on_fonts_apply()
{
	st_config->set_int("Scroller_Fontsize", cli_spb_title.get_value());
	st_config->set_int("TrackInfo_Fontsize", cli_spb_trackinfo.get_value());
	st_config->set_int("Time_Fontsize", cli_spb_time.get_value());
	st_config->set_int("Album_Fontsize", cli_spb_album.get_value());
	st_config->set_int("Tracks_Fontsize", cli_spb_tracks.get_value());
	signal_applyfonts.emit();
}

void gm_settings::on_button_save()
{
	if (st_config == NULL)
	{
		cout << "Settings: Can't set config file" << endl;
		return;
	}
	// connect
	st_config->set_bool("AutoConnect", cnt_cb_autocon.get_active());
	st_config->set_bool("OverrideMPDconf", cnt_cb_override.get_active());
	st_config->set_string("MPD_Host", cnt_et_host.get_text());
	st_config->set_string("MPD_Port", cnt_et_port.get_text());
	st_config->set_string("MPD_Password", cnt_et_pswd.get_text());
	
	// server
	outdev_list oDevices;
    mpd_OutputEntity addDev;
    	// dev1
    if (srv_cb_out1.is_sensitive())
    {
    	addDev.id = 0;
    	addDev.enabled = srv_cb_out1.get_active();
    	oDevices.push_back(addDev);
    }
    	// dev2
    if (srv_cb_out2.is_sensitive())
    {
    	addDev.id = 1;
    	addDev.enabled = srv_cb_out2.get_active();
    	oDevices.push_back(addDev);
    }
    	// dev3
    if (srv_cb_out3.is_sensitive())
    {
    	addDev.id = 2;
    	addDev.enabled = srv_cb_out3.get_active();
    	oDevices.push_back(addDev);
    }
    	// dev4
    if (srv_cb_out4.is_sensitive())
    {
    	addDev.id = 3;
    	addDev.enabled = srv_cb_out4.get_active();
    	oDevices.push_back(addDev);
    }
	    // dev5
    if (srv_cb_out5.is_sensitive())
    {
    	addDev.id = 4;
    	addDev.enabled = srv_cb_out5.get_active();
    	oDevices.push_back(addDev);
    }
	if (st_mpdCom != NULL)
		st_mpdCom->setOutputs(oDevices);
	
	st_config->set_string("MPD_MusicPath", srv_et_mpath.get_text());
	st_config->set_string("MPD_PlaylistPath", srv_et_ppath.get_text());

	st_config->set_bool("QuitMPD_onQuit", srv_cb_kill.get_active());
	st_config->set_bool("StartMPD_onStart", srv_cb_launch.get_active());
	// client
	st_config->set_int("Scroller_Fontsize", cli_spb_title.get_value());
	st_config->set_int("TrackInfo_Fontsize", cli_spb_trackinfo.get_value());
	st_config->set_int("Time_Fontsize", cli_spb_time.get_value());
	st_config->set_int("Album_Fontsize", cli_spb_album.get_value());
	st_config->set_int("Tracks_Fontsize", cli_spb_tracks.get_value());
	st_config->set_bool("show_ToolTips", cli_cb_ttips.get_active());
	st_config->set_bool("use_TrayIcon", cli_cb_systray.get_active());
	st_config->set_string("AlbumArt_File", cli_et_art.get_text());
	// hide the window
	signal_settingssaved.emit();
	this->hide();
}


void gm_settings::on_button_connect()
{
	int portnr;
	std::string str_int = (cnt_et_port.get_text()).data();
	std::istringstream input(str_int);
	input >> portnr;
	ustring host = cnt_et_host.get_text();
	ustring pswd = cnt_et_pswd.get_text();
	bool usem = cnt_cb_override.get_active();
	// signal to the player
	signal_pleaseconnect.emit(host, portnr, pswd, usem);
}


void gm_settings::on_fonts_reset()
{
	cli_spb_title.set_value(12);
	cli_spb_trackinfo.set_value(10);
	cli_spb_time.set_value(12);
	cli_spb_album.set_value(11);
	cli_spb_tracks.set_value(12);
}


void gm_settings::load_and_show()
{
	// get connect settings
	cnt_cb_autocon.set_active(st_config->get_bool("AutoConnect"));
	cnt_cb_override.set_active(st_config->get_bool("OverrideMPDconf"));
	cnt_et_host.set_text(st_config->get_string("MPD_Host"));
	cnt_et_port.set_text(st_config->get_string("MPD_Port"));
	cnt_et_pswd.set_text(st_config->get_string("MPD_Password"));
	// get server settings
	outdev_list oDevices;
	if (st_mpdCom != NULL)
		oDevices = st_mpdCom->getOutputs();
    // Set up the output devices
    std::list<mpd_OutputEntity>::iterator cit;
    mpd_OutputEntity curDev;
    	// dev1
    cit = oDevices.begin();
    if (cit != oDevices.end())
    {
    	srv_cb_out1.set_sensitive(true);
    	curDev = *cit;
    	srv_cb_out1.set_label(curDev.name);
        if (curDev.enabled)
        	srv_cb_out1.set_active(true);
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
    	srv_cb_out2.set_sensitive(true);
    	curDev = *cit;
    	srv_cb_out2.set_label(curDev.name);
        if (curDev.enabled)
        	srv_cb_out2.set_active(true);
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
    	srv_cb_out3.set_sensitive(true);
    	curDev = *cit;
    	srv_cb_out3.set_label(curDev.name);
        if (curDev.enabled)
        	srv_cb_out3.set_active(true);
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
    	srv_cb_out4.set_sensitive(true);
    	curDev = *cit;
    	srv_cb_out4.set_label(curDev.name);
        if (curDev.enabled)
        	srv_cb_out4.set_active(true);
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
		// dev5
    if (cit != oDevices.end())
    {
    	srv_cb_out5.set_sensitive(true);
    	curDev = *cit;
    	srv_cb_out5.set_label(curDev.name);
        if (curDev.enabled)
        	srv_cb_out5.set_active(true);
    	else
        	srv_cb_out5.set_active(false);
    }
    else
    {
    	srv_cb_out5.set_label("*");
    	srv_cb_out5.set_active(false);
    	srv_cb_out5.set_sensitive(false);
    }
	
	srv_et_mpath.set_text(st_config->get_string("MPD_MusicPath"));
	srv_et_ppath.set_text(st_config->get_string("MPD_PlaylistPath"));
	
	srv_cb_kill.set_active(st_config->get_bool("QuitMPD_onQuit"));
	srv_cb_launch.set_active(st_config->get_bool("StartMPD_onStart"));
	// get client settings
	cli_spb_title.set_value(st_config->get_int("Scroller_Fontsize"));
	cli_spb_trackinfo.set_value(st_config->get_int("TrackInfo_Fontsize"));
	cli_spb_time.set_value(st_config->get_int("Time_Fontsize"));
	cli_spb_album.set_value(st_config->get_int("Album_Fontsize"));
	cli_spb_tracks.set_value(st_config->get_int("Tracks_Fontsize"));
	cli_cb_systray.set_active(st_config->get_bool("use_TrayIcon"));
	cli_cb_ttips.set_active(st_config->get_bool("show_ToolTips"));
	cli_et_art.set_text(st_config->get_string("AlbumArt_File"));
	// show the window			 
	this->show();
}


void gm_settings::set_stPos(st_posxy sets)
{
	this->move(sets.x_pos, sets.y_pos);
}

// set a pointer to the parent's config
void gm_settings::set_config(gm_Config *conf)
{
	st_config = conf;
}

// set a pointer to the parent's mpdCom
void gm_settings::set_mpdcom(gm_mpdCom *com)
{
	st_mpdCom = com;
	if (st_mpdCom == NULL)
	{
		cnt_label.set_text("Not Connected");
		cnt_image.set(pxb_con_dis);
	}
	else
	{
		// get version
		ustring version = "<b>Connected</b> to MPD ";
		version += st_mpdCom->get_version();
		cnt_label.set_markup(version);
		cnt_image.set(pxb_con_ok);
	}
}


st_posxy gm_settings::get_stPos()
{
	st_posxy sets;
	int x, y;
    get_position(x, y);
	sets.x_pos = x;
	sets.y_pos = y;
	return sets;
}


std::string gm_settings::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
}


gm_settings::~gm_settings()
{
}
