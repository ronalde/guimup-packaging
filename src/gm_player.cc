/*
 *  gm_player.cc
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

#include "gm_player.h"
#include "gm_player_pix.h"

gm_Player::gm_Player()
{
	// 1
	config = new gm_Config();
	mpdCom = new gm_mpdCom();
	current_sInfo = new gm_songInfo;
	// 2
	mpdCom->set_sInfo(current_sInfo);
	mpdCom->set_config(config); // 1st
    mpdCom->configure();
	// 3
	browserWindow = new gm_browser;
	browserWindow->set_property("destroy_with_parent", true);
	browserWindow->set_config(config); // 1st
	browserWindow->set_mpdCom(mpdCom);
	browserWindow->set_songInfo(current_sInfo);
	browserWindow->set_connected(false);
	
	settingsWindow = new gm_settings;
	settingsWindow->set_property("destroy_with_parent", true);
	settingsWindow->set_config(config); // 1st
	settingsWindow->set_mpdCom(mpdCom);
	settingsWindow->set_connected(false);
	// 4
	set_fonts();
	init_vars();
	// 5
	init_widgets();	
	init_signals();
	set_colors();
    // 6 connect and set status
	set_albumart(TP_NOSONG);
	set_status(-10, " ");
    if (config->AutoConnect)
    {
        cout << "Autoconnecting (as configured)" << endl;
        if (!mpdCom->mpd_connect())
			cout << "Please check the relevant options in the 'settings' window" << endl;
    }
    else
    {
        cout <<  "NOT Connecting (as configured)" << endl;
        on_connected(false);
    }
}


void gm_Player::on_menu_Art()
{
	if (!config->mpd_local_features || current_sInfo->type != TP_SONG && current_sInfo->type != TP_SONGX)
		return;
	
	std::vector<std::string> argv;
	argv.push_back(config->Image_Viewer);
	argv.push_back(current_sInfo->file_dir);
	Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
}


void gm_Player::on_menu_Edit()
{
	if (!config->mpd_local_features || current_sInfo->type != TP_SONG && current_sInfo->type != TP_SONGX)
		return;

	std::vector<std::string> argv;
	argv.push_back((config->Tag_Editor).c_str());
	argv.push_back(current_sInfo->file_dir);
	Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
}


void gm_Player::on_menu_Reload()
{
	// album art
    if (config->mpd_local_features && !config->disable_Albumart)
        set_albumart(current_sInfo->type);

	// tags
	if (!b_connected  || (!config->mpd_rescan_allowed && !config->mpd_update_allowed))
		return;
	if (current_sInfo->type == TP_SONG)
    	mpdCom->rescan_folder(current_sInfo->file_mpd);
}


void gm_Player::on_menu_Browse()
{
	if (!config->mpd_local_features || current_sInfo->type != TP_SONG && current_sInfo->type != TP_SONGX)
		return;
	
	std::vector<std::string> argv;
	argv.push_back(config->File_Manager);
	argv.push_back(current_sInfo->file_dir);
	Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
}

// signal from mpdcom
void gm_Player::on_connected(bool isconnected)
{
    if (isconnected)
    {
        b_connected = true;
		imi_reload->set_sensitive(true);
        set_status(0, ".");
		if (config->mpd_local_features)
		{
			if (!(config->Image_Viewer).empty())
				imi_viewer->set_sensitive(true);
			else
				imi_viewer->set_sensitive(false);
			
			if (!(config->File_Manager).empty())
				imi_fileman->set_sensitive(true);
			else
				imi_fileman->set_sensitive(false);
			
			if (!(config->Tag_Editor).empty())
				imi_tagedit->set_sensitive(true);
			else
				imi_tagedit->set_sensitive(false);
		}
		else
		{
			imi_viewer->set_sensitive(false);
			imi_fileman->set_sensitive(false);
			imi_tagedit->set_sensitive(false);
		}
    }
    else
    {
		imi_viewer->set_sensitive(false);
		imi_fileman->set_sensitive(false);
		imi_tagedit->set_sensitive(false);
		imi_reload->set_sensitive(false);
        b_connected = false;
        set_status(-1, "Not connected");
		// song has already been reset by mpdCom:
		on_newSong(); 
    }
}


void gm_Player::init_vars()
{
	current_status = -10;
	current_volume = 0;
    current_playtime = 0;
	b_connected = false;
	b_stream = false;
	b_minmax_busy = false;
	b_playerWindow_hidden = false;
	b_show_settingsWindow = false;
	b_show_browserWindow = false;
	kbps_count_down = 0;
	b_skip_khz = false;
	b_nosong = false;
	b_no_art_dir = false;
	b_use_trayicon = config->use_TrayIcon;
	
    windowH_min = 100;	// always check at min and max at "case ID_mmax:"
    windowH_max = 306; 	  
	
    btrowPos_min = 65;  // relative to the border!
	btrowPos_max = 271; 
}


void gm_Player::set_fonts()
{
	Pango::FontDescription pFont( Gtk::Settings::get_default()->property_gtk_font_name().get_value() );
	
	pFont.set_weight(Pango::WEIGHT_BOLD);
	pFont.set_size(config->Scroller_Fontsize * PANGO_SCALE);
	titleScroller.set_font(pFont);
	
	pFont.set_weight(Pango::WEIGHT_NORMAL);
	pFont.set_size(config->TrackInfo_Fontsize * PANGO_SCALE);
	lb_type.override_font(pFont);
	lb_khz.override_font(pFont);
	lb_kbps.override_font(pFont);
	pFont.set_size(config->Time_Fontsize * PANGO_SCALE);
	lb_time.override_font(pFont);

	pFont.set_weight(Pango::WEIGHT_NORMAL);
	pFont.set_size(config->Album_Fontsize * PANGO_SCALE);
	fxl_albuminfo.override_font(pFont);
	fxl_albuminfo.reload();
	
	pFont.set_size(config->browser_Fontsize * PANGO_SCALE);	
	browserWindow->set_listfont(pFont);
}


void gm_Player::set_status(int status, ustring message)
{
    if (status == current_status)
        return;
        
    // in 'idle' mode only respond to 'play' and 'not connected'
    if ( current_status == MPD_STATE_UNKNOWN && !(status == MPD_STATE_PLAY || status == -1) )
        return;
        
    if (!message.empty())
        titleScroller.set_title(message);
	
    switch (status)
    {
        case -1: // not connected
        {
            bt_play.set_image(img_mn_play);
			lb_time.set_text("");
			lb_khz.set_text("");
			lb_type.set_text("");
			lb_kbps.set_text("");
			fxl_albuminfo.clear();
			current_sInfo->reset();
			set_albumart(TP_NOSONG);
			pb_timeprogress.set_fraction(0);

            if (b_use_trayicon)
            {
            	mnItem_pause->hide();
            	mnItem_play->show();
            	trayIcon->set(pxb_st_noconn);
                trayIcon->set_tooltip_text(" Not Connected ");
            }
            break;
        }

        case MPD_STATE_STOP: // 1
        {
            bt_play.set_image(img_mn_play);
			lb_time.set_text("");
			lb_kbps.set_text("");
			lb_khz.set_text("");
			pb_timeprogress.set_fraction(0);
			titleScroller.set_pause(true);
			if (b_use_trayicon)
			{
				mnItem_pause->hide();
            	mnItem_play->show();
            	trayIcon->set(pxb_st_stopped);
			}
            break;
        }

        case MPD_STATE_PLAY: // 2
        {
            bt_play.set_image(img_mn_pause);
			titleScroller.set_pause(false);
			if (b_use_trayicon)
			{
				mnItem_pause->show();
            	mnItem_play->hide();
            	trayIcon->set(pxb_st_playing);
			}
            break;
        }

        case MPD_STATE_PAUSE: // 3
        {
            bt_play.set_image(img_mn_play);
			titleScroller.set_pause(true);
			if (b_use_trayicon)
			{
				mnItem_pause->hide();
            	mnItem_play->show();	
				trayIcon->set(pxb_st_paused);
			}
            break;
        }

        default: // 0 (MPD_STATE_UNKNOWN), -10 etc.
        {
			status = MPD_STATE_UNKNOWN;
			titleScroller.set_title(" ");
			bt_play.set_image(img_mn_play);
			lb_time.set_text("");
			lb_khz.set_text("");
			lb_type.set_text("");
			lb_kbps.set_text("");
			fxl_albuminfo.clear();
			pb_timeprogress.set_fraction(0);
			current_sInfo->reset();
			if (b_use_trayicon)
			{
				mnItem_pause->hide();
            	mnItem_play->show();
				trayIcon->set(pxb_st_idle);
                trayIcon->set_tooltip_text(" Idle ");
			}
            break;
        }
    }
    current_status = status;
}


void gm_Player::set_albumart(int type)
{
	bool b_skip_load = false;
	bool b_file_ok = false;
	ustring art_path = current_sInfo->file_dir;

	if (type == TP_NOSONG)
	{
		if (current_art_type == TP_NOSONG)
			return;
		//else
		current_art_type = TP_NOSONG;
		
		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, nopic, false);
		pxb_xfading = pxb_albmart->copy();
		img_aArt.set(pxb_albmart);
		b_file_ok = true;
		b_skip_load = true;
	}
	else
	if (type == TP_STREAM)
	{
		if (current_art_type == TP_STREAM)
			return;
		//else
		current_art_type = TP_STREAM;
		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, onair, false);
		pxb_xfading = pxb_albmart->copy();
		img_aArt.set(pxb_albmart);
		b_file_ok = true;
		b_skip_load = true;
	}
	else
	{	
		current_art_type = current_sInfo->type;
		b_no_art_dir = false;
			b_file_ok = go_find_art(art_path, "album");
		if (!b_file_ok && !b_no_art_dir)
			b_file_ok = go_find_art(art_path, "folder");
		if (!b_file_ok && !b_no_art_dir)
			b_file_ok = go_find_art(art_path, "cover");
		if (!b_file_ok && !b_no_art_dir)
			b_file_ok = go_find_art(art_path, "front");
		if (!b_file_ok && !b_no_art_dir)
			b_file_ok = go_find_art(art_path, current_sInfo->album);
		if (!b_file_ok && !b_no_art_dir) // anything else?
			b_file_ok = go_find_art(art_path, ".");
		
		// [2] put it in a pixbuf	
		if (b_file_ok)
		{

			try
			{
				pxb_albmart = Gdk::Pixbuf::create_from_file(art_path, 200, 200, true);
			}
			catch(Glib::FileError)
			{
				b_file_ok = false;
				cout << "Album art: error reading file" << endl;
			}
			catch(Gdk::PixbufError)
			{
				b_file_ok = false;
				cout << "Album art: error creating pixbuf" << endl;
			}
		}
	}

	if (!b_file_ok)
	{
		if (current_art_type == TP_NOSONG)
			return; // nothing to do
		else
		{
			current_art_type == TP_NOSONG;
			pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, nopic, false);
			pxb_xfading = pxb_albmart->copy();
			b_skip_load = true;
		}
	}

    if (!b_skip_load)
	{
		// [3] add alpha channel (segfault if we don't)
		pxb_albmart = pxb_albmart->add_alpha(false,0,0,0);
		
		int imgH = pxb_albmart->get_height();
		int imgW = pxb_albmart->get_width();
		int row = pxb_albmart->get_rowstride();

		// [4] draw a 1 px border on the image
		guint8 *ptr_pixel;
		guint8 *ptr_firstpixel =  pxb_albmart->get_pixels();
        aArt_H = 	pxb_albmart->get_height();
        aArt_W = 	pxb_albmart->get_width();
        aArt_row = 	pxb_albmart->get_rowstride();
		
		char R = albuminfo_bg_color.get_red();
		char G = albuminfo_bg_color.get_green();
		char B = albuminfo_bg_color.get_blue();
		
		for (int h = 0; h < aArt_H; h++)
		{
			ptr_pixel = ptr_firstpixel + h * aArt_row;
			for (int w = 0; w < aArt_W; w++)
			{
				if (h == 0 || h == imgH-1 || w == 0 || w == imgW-1)
				{
					ptr_pixel[0] = R; 
					ptr_pixel[1] = G; 
					ptr_pixel[2] = B; 
					ptr_pixel[3] = 255; // alpha
				}
				ptr_pixel += 4;
			}
		}

		pxb_xfading = pxb_albmart->copy();
	}

	// [5] show pxb_albmart
	if (config->playerWindow_Max)
		img_aArt.set(pxb_albmart);
	else
	{
		// set alpha = 0 for pxb_xfading
		guint8 *ptr_pixel;
		guint8 *ptr_firstpixel = pxb_xfading->get_pixels();
        aArt_H = 	pxb_xfading->get_height();
        aArt_W = 	pxb_xfading->get_width();
        aArt_row = 	pxb_xfading->get_rowstride();
		for (int h = 0; h < aArt_H; h++)
		{
			ptr_pixel = ptr_firstpixel + h * aArt_row;
			for (int w = 0; w < aArt_W; w++)
			{
				ptr_pixel[3] = 0;
				ptr_pixel += 4; // R,G,B,a
			}
		}
		// show pxb_xfading
		img_aArt.set(pxb_xfading);
	}
}


bool gm_Player::go_find_art(ustring & artPath, ustring findthis)
{
    DIR *pdir;
    struct dirent *pent;
    pdir = opendir(artPath.data());
    if (!pdir)
	{
		cout << "Album art: error opening directory: " << artPath << endl;
		b_no_art_dir = true;
		return false;
	}
	else
	{
		errno = 0;
		while ( (pent = readdir(pdir)) )
		{
			ustring filename = pent->d_name;
			if ( str_has_suffix(filename.lowercase(),".jpg") ||
			  	 str_has_suffix(filename.lowercase(),".png") ||
			  	 str_has_suffix(filename.lowercase(),".gif") ||
			  	 str_has_suffix(filename.lowercase(),".bmp") ||
			  	 str_has_suffix(filename.lowercase(),".jpeg"))
			{
				if ( (filename.lowercase()).find(findthis) != Glib::ustring::npos ) // npos: -1
				{
					artPath += filename;
					closedir(pdir);
					return true;
				}
			}
		}
		if (errno)
		{
			b_no_art_dir = true;
			cout << "Album art: error reading directory" << endl;
		}
		closedir(pdir);
		return false;
	}
}

void gm_Player::set_colors()
{
	// HSV
	double h = config->color_hue;
	double s = (double)config->color_saturation/100;
	double value = (double)(50 - config->color_value)/200; // -0.25 .. +0.25
	
	double v;
	v = (0.5 + value) * 0.92;
	titleinfo_bg_color.set_hsv(h, s, v),
	v = (value * 0.08) + 0.96;
	titleinfo_fg_color.set_hsv(h, s/6, v); 
	v = 0.6 + (0.5 - value) * 0.32;
	albuminfo_bg_color.set_hsv(h, s/4, v); 
	v = (value * 0.80) + 0.20;
	albuminfo_fg_color.set_hsv(h, s/4, v);
	
	eb_center.override_background_color(albuminfo_bg_color, Gtk::STATE_FLAG_NORMAL);// eb_
	fxl_albuminfo.set_fg(albuminfo_fg_color);
	fxl_albuminfo.set_bg(albuminfo_bg_color);
	fxl_albuminfo.reload();
		
	eb_lcd_bg.override_background_color(titleinfo_bg_color, Gtk::STATE_FLAG_NORMAL);
	eb_scroller.override_background_color(titleinfo_bg_color, Gtk::STATE_FLAG_NORMAL);
	eb_time.override_background_color(titleinfo_bg_color, Gtk::STATE_FLAG_NORMAL);

	titleScroller.set_colors(titleinfo_bg_color,titleinfo_fg_color);
	pb_timeprogress.set_colors(titleinfo_bg_color, titleinfo_fg_color); // set_size_request() first!
	
	// for the 'data' labels we blend fg and bg
	titleinfo_fg_color.set_alpha_u(45874.5); // 70% opaque

	lb_type.override_color(titleinfo_fg_color, Gtk::STATE_FLAG_NORMAL);
	lb_kbps.override_color(titleinfo_fg_color, Gtk::STATE_FLAG_NORMAL);
	lb_khz.override_color(titleinfo_fg_color, Gtk::STATE_FLAG_NORMAL);
	lb_time.override_color(titleinfo_fg_color, Gtk::STATE_FLAG_NORMAL);
}


void gm_Player::init_widgets()
{
	if (config->playerWindow_Max)
	{
		btrowPos = btrowPos_max;
		fxl_albuminfo.set_minimode(false);
	}
	else
	{
		btrowPos = btrowPos_min;
		fxl_albuminfo.set_minimode(true);
	}

	int posx = config->playerWindow_Xpos;
	if (posx < 0) posx = 0;
	int posy = config->playerWindow_Ypos;
	if (posy < 0) posy = 0;
	move(posx, posy);
	
	// window properties
    set_title("Guimup");
    set_size_request(340, -1);
    set_resizable(false);
	set_border_width(4);

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

	create_rClickmenu();

	if (config->use_TrayIcon)
    	create_trayIcon();
	
	// widget properties
	fx_main.set_size_request(332,-1);
	eb_lcd_bg.set_size_request(332,54);
	fx_lcdisplay.set_size_request(332,54);
	eb_scroller.set_events(Gdk::LEAVE_NOTIFY_MASK | Gdk::ENTER_NOTIFY_MASK);
	eb_scroller.set_size_request(332,26);
	eb_scroller.set_valign(Gtk::ALIGN_CENTER);
	titleScroller.set_size_request(332,26);
	titleScroller.set_double_buffered(true);
	int delay = config->Scroller_Delay;
	if (config->Scroller_Delay == 0)
		config->Scroller_Delay = 60;
	if (config->Scroller_Delay < 20)
		config->Scroller_Delay = 20;
	titleScroller.set_delay(config->Scroller_Delay );
	eb_time.set_events(Gdk::BUTTON_PRESS_MASK);
    lb_type.set_alignment(0, 1);
    lb_type.set_size_request(70,24);
	lb_type.set_margin_left(6);
	lb_kbps.set_size_request(70,24);
    lb_kbps.set_alignment(1, 1); 
	lb_khz.set_size_request(60,24);
    lb_khz.set_alignment(0, 1);
    lb_time.set_alignment(1, 1);
	lb_time.set_size_request(120,24);
	lb_time.set_tooltip_text("Click to toggle +/-");
	lb_time.set_margin_right(6);
    pb_timeprogress.set_size_request(332,5);
	pb_timeprogress.set_fraction(0);
	eb_center.set_size_request(328,200); // 328 + margin + margin = 332
	gr_center.set_size_request(328,200); // 328 + margin + margin = 332
    gr_center.set_column_spacing(4);
	gr_center.set_margin_left(2);
	gr_center.set_margin_right(2);
	gr_center.set_margin_bottom(2);
	gr_center.set_margin_top(2);
	img_aArt.set_tooltip_text("Right-click for menu");
    img_aArt.set_size_request(200,200);
	img_aArt.set_alignment(0.5, 0.5);
	img_aArt.set_double_buffered(true);
	fxl_albuminfo.set_size_request(124,200);
	gr_volslider.set_valign(Gtk::ALIGN_CENTER);
	gsl_volume.add_events(Gdk::SCROLL_MASK);
	gsl_volume.set_size_request(38,5);
	gsl_volume.set_colors("#344c5c","#ffffff"); // set_size_request() first!
	gsl_volume.set_value(50);
    gr_buttonrow.set_size_request(332,-1);
    gr_buttonrow.set_column_spacing(6);
	bt_prev.set_size_request(36,-1);
	bt_stop.set_size_request(36,-1);
	bt_play.set_size_request(36,-1);
	bt_next.set_size_request(36,-1);
	bt_plst.set_size_request(36,-1);
	bt_sizr.set_size_request(36,-1);
	bt_conf.set_size_request(36,-1);

    	// prev
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_prev, false);
    img_mn_prev.set(pxb);
    bt_prev.set_image(img_mn_prev);
    	// stop
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_stop, false);
    img_mn_stop.set(pxb);
    bt_stop.set_image(img_mn_stop);
    	// play
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_play, false);
    img_mn_play.set(pxb);
    bt_play.set_image(img_mn_play);
    	// pause
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_pause, false);
    img_mn_pause.set(pxb);
    	// next
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_next, false);
    img_mn_next.set(pxb);
    bt_next.set_image(img_mn_next);
    	// playlist
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_playlist, false);
    img_mn_playlist.set(pxb);
    bt_plst.set_image(img_mn_playlist);
	bt_plst.set_tooltip_text("Library & Playlist");
    	// sizer
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_sizer, false);
    img_mn_sizer.set(pxb);
    bt_sizr.set_image(img_mn_sizer);
	bt_sizr.set_tooltip_text("Mini-Maxi mode");
    	// config
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_config, false);
    img_mn_config.set(pxb);
    bt_conf.set_image(img_mn_config);
	bt_conf.set_tooltip_text("Settings");
	
    // put widgets together
	add(fx_main); // 4 rows
    fx_main.put(eb_lcd_bg, 0, 0);
    	// row 1
    eb_lcd_bg.add(fx_lcdisplay);
	fx_lcdisplay.put(eb_scroller, 0, 4);
		eb_scroller.add(titleScroller);
    fx_lcdisplay.put(lb_type, 0, 26);
	fx_lcdisplay.put(lb_kbps, 70, 26);
	fx_lcdisplay.put(lb_khz, 142, 26);
	fx_lcdisplay.put(eb_time, 206, 26);
		eb_time.add(lb_time);
		// row 2
	fx_main.put(pb_timeprogress, 0, 56);
		// row 3
	fx_main.put(eb_center, 0, 63);
		eb_center.add(gr_center); 
		gr_center.add(img_aArt);
		gr_center.add(fxl_albuminfo);
		// row 4
	fx_main.put(gr_buttonrow, 0, btrowPos);
		gr_buttonrow.add(bt_prev);
		gr_buttonrow.add(bt_stop);
		gr_buttonrow.add(bt_play);
		gr_buttonrow.add(bt_next);
		gr_buttonrow.add(bt_plst);
	    gr_buttonrow.add(gr_volslider);
			gr_volslider.add(gsl_volume);
		gr_buttonrow.add(bt_sizr);
		gr_buttonrow.add(bt_conf);
	
	// set tooltips after set_tooltip_text
	set_tooltips();
	
	if (config->playerWindow_Max)
    	fx_main.show_all();
	else
	{
		fx_main.show();
		eb_lcd_bg.show_all();
		pb_timeprogress.show();
		gr_buttonrow.show_all();		
	}
}

// on signal from settingswindow
void gm_Player::set_tooltips()
{
	lb_time.set_has_tooltip(config->show_ToolTips);
	bt_plst.set_has_tooltip(config->show_ToolTips);
	bt_sizr.set_has_tooltip(config->show_ToolTips);
	bt_conf.set_has_tooltip(config->show_ToolTips);
}


void gm_Player::init_signals()
{
	// the mpdCom
	mpdCom->sgnl_connected.connect(sigc::mem_fun(*this, &gm_Player::on_connected));
	mpdCom->sgnl_newSong.connect(sigc::mem_fun(*this, &gm_Player::on_newSong));
	mpdCom->sgnl_statusInfo.connect(sigc::mem_fun(*this, &gm_Player::on_newStatus));

	// return void
    bt_prev.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_prev));

    bt_stop.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_stop));

    bt_play.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_play));

    bt_next.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_next));

    bt_plst.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_libr));

    bt_sizr.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_mmax));

    bt_conf.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_sets));

	settingsWindow->signal_savesettings.connect(sigc::mem_fun(*this, &gm_Player::save_settings) );
	settingsWindow->signal_applyfonts.connect(sigc::mem_fun(*this, &gm_Player::set_fonts) );
	settingsWindow->signal_applycolors.connect(sigc::mem_fun(*this, &gm_Player::set_colors) );
	settingsWindow->signal_settooltips.connect(sigc::mem_fun(*this, &gm_Player::set_tooltips) );
	settingsWindow->signal_settooltips.connect(sigc::mem_fun(*browserWindow,&gm_browser::set_tooltips) );	
	settingsWindow->signal_lib_setfixed.connect(sigc::mem_fun(*browserWindow,&gm_browser::set_lib_fixed) );	
	settingsWindow->signal_plist_setfixed.connect(sigc::mem_fun(*browserWindow,&gm_browser::set_plist_fixed) );
	settingsWindow->signal_setmarkplayed.connect(sigc::mem_fun(*browserWindow,&gm_browser::set_mark_played) );
	// return bool
	eb_time.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_timeClicked) );
	signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_playerRClicked) );
	eb_scroller.signal_enter_notify_event().connect(sigc::mem_fun(*this, &gm_Player::on_enter_scroller) );
	eb_scroller.signal_leave_notify_event().connect(sigc::mem_fun(*this, &gm_Player::on_leave_scroller) );
	pb_timeprogress.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_progressClicked) );
	gsl_volume.signal_scroll_event().connect(sigc::mem_fun(*this, &gm_Player::on_tray_or_vol_Scrolled) );
	gsl_volume.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_volumeClicked) );
}


bool gm_Player::on_volumeClicked(GdkEventButton* event)
{
	if( event->button != 1)
		return false; // pass on

	if (!b_connected)
		return true;
	
	double fraction = ((*event).x)/(double)gsl_volume.get_width();
	int new_volume = (int)(fraction*100.0);
	
	mpdCom->set_volume(new_volume);
	return true;
}


bool gm_Player::on_enter_scroller(GdkEventCrossing *e)
{
	titleScroller.on_mouse_enter();
	return false; // pass on
}


bool gm_Player::on_leave_scroller(GdkEventCrossing *e)
{
	titleScroller.on_mouse_leave();
	return false; // pass on
}


bool gm_Player::on_playerRClicked(GdkEventButton* event)
{
	if( (event->button == 3) && (current_sInfo->type == TP_SONG || current_sInfo->type == TP_SONGX)
	     && current_status != MPD_STATE_UNKNOWN )
	{
		rClickMenu->popup(event->button, event->time);
		return true;
	}
	else
		return false; // pass on
}


bool gm_Player::on_timeClicked(GdkEventButton* event)
{
	if( event->button != 1)
		return false; // pass on
	
	if (current_status > 0) // STOP, PLAY or PAUSE
		config->use_TimeRemaining = !config->use_TimeRemaining;
	return true;
}


bool gm_Player::on_progressClicked(GdkEventButton* event)
{
	if (!b_connected || event->button != 1 || (current_status != MPD_STATE_PLAY && current_status != MPD_STATE_PAUSE))
		return true; // stop here

	double fraction = ((*event).x)/(double)pb_timeprogress.get_width();
	pb_timeprogress.set_fraction(fraction);

	int percent = (int)(fraction * current_sInfo->time);

	mpdCom->set_seek(percent);
	return true;
}


void gm_Player::save_settings()
{
	if (get_visible())
		get_position(config->playerWindow_Xpos, config->playerWindow_Ypos);
	browserWindow->get_configs();
	settingsWindow->get_configs();
	config->save_config();
}


void gm_Player::on_newStatus( mpd_status * sInfo )
{
    // if status has changed...
    if (b_nosong && current_status != MPD_STATE_UNKNOWN)
        set_status(MPD_STATE_UNKNOWN);
    else
        if (mpd_status_get_state(sInfo) != current_status)
            set_status(mpd_status_get_state(sInfo));
     	
	// set the other status info
    if (mpd_status_get_state(sInfo) != MPD_STATE_UNKNOWN && !b_nosong)
    {
        int new_playtime = mpd_status_get_elapsed_time(sInfo);
        
        if (!b_stream && current_sInfo->time > 0)
        {
            if (new_playtime != current_playtime)
            {
                current_playtime = new_playtime;
                // some tracks play a bit too long (honestly)
                if (new_playtime > current_sInfo->time)
                    new_playtime = current_sInfo->time;
				
				double fraction = (double)(new_playtime/current_sInfo->time);
                pb_timeprogress.set_fraction(fraction);
            }
        }
        
        if (b_stream || current_sInfo->time == 0)
        {
            lb_time.set_markup("&#8734;");
        }
        else
        {
            if (config->use_TimeRemaining)
                new_playtime = current_sInfo->time - new_playtime;
                
            ustring time_string;
            if (config->use_TimeRemaining)
                time_string = "-";
			else
				time_string = "";
            time_string += into_time((int)new_playtime);
            lb_time.set_markup("<b>" + time_string + "</b> / " + into_time((int)current_sInfo->time));
        }

		if (kbps_count_down == 0)
		{
		    if (mpd_status_get_kbit_rate(sInfo) > 0)
		        lb_kbps.set_text(into_string(mpd_status_get_kbit_rate(sInfo)) + " kb/s ");
		    else
		        lb_kbps.set_text("");

			if (!b_skip_khz)
			{
				if (mpd_status_get_audio_format(sInfo) != NULL) // NULL possible with streams
				{
				    double dbl_khz = (double)mpd_status_get_audio_format(sInfo)->sample_rate / 1000;
				    lb_khz.set_text(" " + into_string(dbl_khz) + " kHz");
				}
				else
				    lb_khz.set_text("");
				b_skip_khz = true;
			}
			else
				b_skip_khz = false;
			
			kbps_count_down = 4;
		}
		else
			kbps_count_down--;
    }
    
    if (current_volume != mpd_status_get_volume(sInfo))
    {
        /* MPD reports 0 to 100 (%)*/
        current_volume = mpd_status_get_volume(sInfo);
        gsl_volume.set_value(current_volume);
        gsl_volume.set_tooltip_text(into_string(current_volume) + "%");
    }
}

// Signal handler.
void gm_Player::on_signal(int sigID)
{
    switch (sigID)
    {
    	case ID_prev:
        {
            if (current_status != MPD_STATE_PLAY || !b_connected)
                break;
            mpdCom->prev();
            break;
        }
        
    	case ID_stop:
        {
            if (current_status == MPD_STATE_UNKNOWN ||
                    current_status == MPD_STATE_STOP || !b_connected)
                break;
                
            set_status(MPD_STATE_STOP, "");
            mpdCom->stop();
            break;
        }
        
    	case ID_play:
        {
            if (!b_connected)
                break;
                
            switch (current_status)
            {
            case MPD_STATE_PLAY:
                {
                    set_status(MPD_STATE_PAUSE, "");
                    mpdCom->pause();
                    break;
                }
                
            case MPD_STATE_PAUSE:
                {
                    set_status(MPD_STATE_PLAY, "");
                    mpdCom->resume();
                    break;
                }
                
            case MPD_STATE_STOP:
                {
                    set_status(MPD_STATE_PLAY, "");
                    mpdCom->play();
                    break;
                }
                
            case MPD_STATE_UNKNOWN:
                {
                    set_status(MPD_STATE_PLAY, "");
                    mpdCom->play();
                    break;
                }
                
            default:
                {
                    break;
                }
            }
            break;
        }
        
    	case ID_next:
        {
            if (current_status != MPD_STATE_PLAY || !b_connected)
                break;
            mpdCom->next();
            break;
        }
				
		case ID_sets:
		{
			if (settingsWindow->get_visible())
			{
				settingsWindow->get_position(config->settingsWindow_Xpos, config->settingsWindow_Ypos);
				settingsWindow->hide();
			}
			else
			{
				settingsWindow->load_config();
				settingsWindow->move(config->settingsWindow_Xpos, config->settingsWindow_Ypos);
				settingsWindow->present();
			}
			break;
		}

		case ID_libr:

		{   
			if (browserWindow->get_visible())
			{
				browserWindow->get_position(config->browserWindow_Xpos,config->browserWindow_Ypos);
				browserWindow->hide();
			}
			else
			{
				browserWindow->move(config->browserWindow_Xpos, config->browserWindow_Ypos);
				browserWindow->present();
			}
			break;
		}

        case ID_mmax:
        {
            if (b_minmax_busy)
                break;
			
            if (config->playerWindow_Max)
            {
				// get actual windowH_max & windowH_min
				windowH_max = windowH = get_allocated_height();
				windowH_min = windowH_max - 206;

                b_minmax_xfade = true;
				alphacycler = 255;
                minmaxtimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Player::miniMice), 12);
                config->playerWindow_Max = false;
            }
            else
            {
				// get actual windowH_max & windowH_min
				windowH_min = windowH = get_allocated_height();
				windowH_max = windowH_min + 206;
					
                b_minmax_xfade = false;
				alphacycler = 0;
                minmaxtimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Player::maxiMice), 12);
                config->playerWindow_Max = true;
            }
            break;
        }
				
        default:
			break;
    }
}


void gm_Player::create_trayIcon()
{
    // status icons
    pxb_st_playing = Gdk::Pixbuf::create_from_inline(-1, st_playing, false);
    pxb_st_noconn = Gdk::Pixbuf::create_from_inline(-1, st_noconn, false);
    pxb_st_paused = Gdk::Pixbuf::create_from_inline(-1, st_paused, false);
    pxb_st_stopped = Gdk::Pixbuf::create_from_inline(-1, st_stopped, false);
	pxb_st_idle = Gdk::Pixbuf::create_from_inline(-1, st_idle, false);
	
    // menu
	Glib::RefPtr<Gdk::Pixbuf> pxb;

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_exit, false);
	img_tr_quit.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, st_prev, false);
	img_tr_prev.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, st_stop, false);
	img_tr_stop.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, st_play, false);
	img_tr_play.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, st_pause, false);
	img_tr_pause.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, st_next, false);
	img_tr_next.set(pxb);

    tIconMenu = new Gtk::Menu();
	
	Gtk::ImageMenuItem *Item1 = new Gtk::ImageMenuItem(img_tr_prev,"Previous");
	tIconMenu->add(*Item1);
	Item1->signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_Player::on_signal), ID_prev));
	
	Gtk::ImageMenuItem *Item2 = new Gtk::ImageMenuItem(img_tr_stop,"Stop");
	tIconMenu->add(*Item2);
	Item2->signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_Player::on_signal), ID_stop));

	mnItem_play = new Gtk::ImageMenuItem(img_tr_play,"Play");
	tIconMenu->add(*mnItem_play);
	mnItem_play->signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_Player::on_signal), ID_play));

	mnItem_pause = new Gtk::ImageMenuItem(img_tr_pause,"Pause");
	tIconMenu->add(*mnItem_pause);
	mnItem_pause->signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_Player::on_signal), ID_play));

	Gtk::ImageMenuItem *Item5 = new Gtk::ImageMenuItem(img_tr_next,"Next");
	tIconMenu->add(*Item5);
	Item5->signal_activate().connect(sigc::bind<-1, int>(sigc::mem_fun(*this, &gm_Player::on_signal), ID_next));

	Gtk::ImageMenuItem *Item6 = new Gtk::ImageMenuItem(img_tr_quit,"Quit Guimup");
	tIconMenu->add(*Item6);
	Item6->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::hide) );	

	tIconMenu->show_all();
	mnItem_pause->hide();
	
    // Setting up the StatusIcon:
	trayIcon = gm_trayIcon::create();
	trayIcon->set_tooltip_text("Guimup");
    trayIcon->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::toggle_hide_show) );
    trayIcon->signal_popup_menu().connect(sigc::mem_fun(*this, &gm_Player::on_trayIcon_popup) );
	trayIcon->signal_scroll_event().connect(sigc::mem_fun(*this, &gm_Player::on_tray_or_vol_Scrolled) ); 		// gtkmm 2.16	!
	trayIcon->signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_trayIcon_clicked) ); 			// gtkmm 2.16   !	
}


bool gm_Player::on_trayIcon_clicked(GdkEventButton* eb)
{
	if (eb->button == 2)
	{
		on_signal(ID_play);
		return true;
	}
	else
		return false; // pass on
}


bool gm_Player::on_tray_or_vol_Scrolled(GdkEventScroll* es)
{
	if (!b_connected)
		return true;
	
	if (es->direction == GDK_SCROLL_UP)
	{
		mpdCom->volume_up(8);
	}
	else
	{
		mpdCom->volume_down(8);
	}
    return true; // stop here
}


void gm_Player::toggle_hide_show()
{
	if (b_playerWindow_hidden)
	{
		if (b_show_settingsWindow)
		{
			settingsWindow->move(config->settingsWindow_Xpos, config->settingsWindow_Ypos);
			settingsWindow->present();
		}
		
		if (b_show_browserWindow)
		{
			browserWindow->move(config->browserWindow_Xpos, config->browserWindow_Ypos);
			browserWindow->present();
		}

		move(config->playerWindow_Xpos, config->playerWindow_Ypos);
		present();

		b_playerWindow_hidden = false;
	}
	else
	{		
		if (settingsWindow->get_visible())
		{
			b_show_settingsWindow = true;
			settingsWindow->get_position(config->settingsWindow_Xpos, config->settingsWindow_Ypos);
			settingsWindow->hide();
		}
		else
			b_show_settingsWindow = false;
		
		if (browserWindow->get_visible())
		{
			b_show_browserWindow = true;
			browserWindow->get_position(config->browserWindow_Xpos,config->browserWindow_Ypos);
			browserWindow->hide();
		}
		else
			b_show_browserWindow = false;
		

		get_position(config->playerWindow_Xpos, config->playerWindow_Ypos);
		/* 
		get_window() gets the Gdk::Window
		without it the program (i.e. the Gtk::Window) will be terminated
		*/
		get_window()->hide(); 
		b_playerWindow_hidden = true;
	}
}

// window is closed (or hidden)
bool gm_Player::on_delete_event(GdkEventAny* event)
{
	if(b_use_trayicon)
	{
    	toggle_hide_show();
    	return true;
	}
	else
	{
		save_settings();
		return false; // pass on
	}
}

// window calls this on quit
void gm_Player::on_hide()
{
	save_settings();
		
	if (config->QuitMPD_onQuit)
	{
		cout << "executing: " << config->MPD_onQuit_command << endl;
		system ( (config->MPD_onQuit_command).data() );
	}

	if (b_connected)
	{
		cout << "Disconnecting mpd" << endl;
		mpdCom->mpd_disconnect(false);
	}
	cout << "Goodbye!" << endl;
}


void gm_Player::on_trayIcon_popup(guint button, guint32 time)
{    
    trayIcon->popup_menu_at_position(*tIconMenu, button, time);
}


void gm_Player::on_newSong( )
{

	if (current_sInfo == NULL)
		return;

	if(current_sInfo->nosong)
	{
		if  (b_nosong)
			return;
		
		img_aArt.set_tooltip_text("No song");
		pb_timeprogress.set_fraction(0);
        b_nosong = true;
		set_albumart(TP_NOSONG);
        return;
	}
    else
		b_nosong = false;
	
	kbps_count_down = 0;
	b_skip_khz = false;
	
	cout << "New song: " << (current_sInfo->songNr + 1) << endl;
	
	if (current_sInfo->type != TP_SONG)
	{
		imi_reload->hide();
	}
	else
		imi_reload->show();
	
	if (current_sInfo->type == TP_STREAM)
	{
		set_albumart(TP_STREAM);
		lb_type.set_text("URL");
		fxl_albuminfo.set_info("Audio stream","","");
		ustring artist_title = "no info";
		if (!current_sInfo->title.empty())
		{
			if (!current_sInfo->title.empty())
				titleScroller.set_title(current_sInfo->title);
			if (b_use_trayicon)
				trayIcon->set_tooltip_text(current_sInfo->title);
			if (!current_sInfo->name.empty())
				fxl_albuminfo.set_info(current_sInfo->name,"","");
			else
				fxl_albuminfo.set_info("Audio stream","","");
		}
		else
		{
			if (!current_sInfo->name.empty())
			{
				titleScroller.set_title(current_sInfo->name);
				if (b_use_trayicon)
					trayIcon->set_tooltip_text(current_sInfo->name);
				fxl_albuminfo.set_info("Audio stream","","");
			}
			else
			{
				titleScroller.set_title("[no info]");
				if (b_use_trayicon)
					trayIcon->set_tooltip_text("[no info]");
				fxl_albuminfo.set_info("Audio stream","","");                  
			}
		}	
		b_stream = true;
		pb_timeprogress.set_fraction(0);
		img_aArt.set_tooltip_text("Streaming media");
		return;
	}
	else
		b_stream = false;
	
	// album art
    if ( config->mpd_local_features && !config->disable_Albumart)
        set_albumart(current_sInfo->type);
    else
        set_albumart(TP_NOSONG);

	// extension
	if (current_sInfo->file_name.empty())
	{
		lb_type.set_text("?");
	}
	else
	{
		ustring xtension = ((current_sInfo->file_name).substr((current_sInfo->file_name).rfind(".")+1, (current_sInfo->file_name).length()) ).uppercase();
		lb_type.set_text(xtension);
	}

	// artist & title
	if (current_sInfo->artist.empty() && current_sInfo->title.empty())
	{
		if (!(current_sInfo->name).empty())
		{
			titleScroller.set_title(current_sInfo->name);
			if (b_use_trayicon)
                trayIcon->set_tooltip_text(current_sInfo->name);
		}
		else
		if (!(current_sInfo->file_name).empty())
        {
			titleScroller.set_title(current_sInfo->file_name);
			if (b_use_trayicon)
				trayIcon->set_tooltip_text(current_sInfo->file_name);

        }
        else
		{
            titleScroller.set_title("[no info]");
           	if (b_use_trayicon)
                trayIcon->set_tooltip_text( "[no info]");
		}
	}
	else
	{
		titleScroller.set_title(current_sInfo->artist, current_sInfo->title);
		if (b_use_trayicon)
			trayIcon->set_tooltip_markup( " <b>" + escapeString(current_sInfo->artist) + "</b> &#10; <i>" + escapeString(current_sInfo->title) + "</i> "); // escape for markup!!
	}

	fxl_albuminfo.set_info(current_sInfo->album, current_sInfo->year, current_sInfo->comment);

	img_aArt.set_tooltip_text("Right-click for menu");
}


ustring gm_Player::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	ustring result = str_int;
	return result;	
}


ustring gm_Player::into_time(int time)
{
    ustring formattedTime = "";
    int mins = (int)(time / 60);
    formattedTime += into_string(mins) + ":";
    int secs = time % 60;
    if(secs < 10)
	formattedTime += "0";
    formattedTime += into_string(secs);
	return formattedTime;
}


bool gm_Player::miniMice()
{
	// first time around get some parameters
	if (!b_minmax_busy)
	{
		b_minmax_busy = true;
		ptr_orig = 	pxb_albmart->get_pixels();
		ptr_xfad =  pxb_xfading->get_pixels();
        aArt_H = 	pxb_xfading->get_height();
        aArt_W = 	pxb_xfading->get_width();
        aArt_row = 	pxb_xfading->get_rowstride();
	}
    // fade the album art first
    if (b_minmax_xfade)
    {
		alphacycler -= 16;
        if (alphacycler < 0)
			alphacycler = 0;
		
		for (int h = 0; h < aArt_H; h++)
		{
			ptr_xfadpix = ptr_xfad + h * aArt_row;
			ptr_origpix = ptr_orig + h * aArt_row;

			for (int w = 0; w < aArt_W; w++)
			{
				if (alphacycler < ptr_origpix[3])
					ptr_xfadpix[3] = alphacycler;
				else
					ptr_xfadpix[3] = ptr_origpix[3];
				
				ptr_xfadpix += 4; // R,G,B,a
				ptr_origpix += 4;
			}
		}
		
		img_aArt.set(pxb_xfading);
		
		fxl_albuminfo.set_alpha(alphacycler);
		
		if (alphacycler == 0)
		{
            b_minmax_xfade = false;
		    //eb_center.hide();
			eb_center.hide();
		}
        return true; // continue
    }

    windowH -= 12;
    if (windowH >= windowH_min)
    {
        btrowPos -= 12;
        set_size_request(340, windowH);
        fx_main.move(gr_buttonrow, 0, btrowPos);
        return true; // continue resizing
    }
    else
    {
        btrowPos = btrowPos_min;
        set_size_request(340, windowH_min);
        fx_main.move(gr_buttonrow, 0, btrowPos);
        b_minmax_busy = false;
		minmaxtimer.disconnect();
		fxl_albuminfo.set_minimode(true);
        return false; // done
    }
}


bool gm_Player::maxiMice()
{
	// first time around get some parameters
	if (!b_minmax_busy)
	{
		b_minmax_busy = true;
		ptr_orig = 	pxb_albmart->get_pixels();
		ptr_xfad = 	pxb_xfading->get_pixels();
        aArt_H = 	pxb_xfading->get_height();
        aArt_W = 	pxb_xfading->get_width();
        aArt_row = 	pxb_xfading->get_rowstride();
	}
	
    // fade the album art last
    if (b_minmax_xfade)
    {
		alphacycler += 16;
        if (alphacycler > 255)
			alphacycler = 255;
		
		for (int h = 0; h < aArt_H; h++)
		{
			ptr_xfadpix = ptr_xfad + h * aArt_row;
			ptr_origpix = ptr_orig + h * aArt_row;
			for (int w = 0; w < aArt_W; w++)
			{
				if (alphacycler < ptr_origpix[3])
					ptr_xfadpix[3] = alphacycler;
				else
					ptr_xfadpix[3] = ptr_origpix[3];
				
				ptr_xfadpix += 4; // R,G,B,a
				ptr_origpix += 4;
			}
		}
		
		fxl_albuminfo.set_alpha(alphacycler);
			
		if (alphacycler == 255)
		{
            b_minmax_xfade = false;		
        	b_minmax_busy = false;
			img_aArt.set(pxb_albmart);
			fxl_albuminfo.set_minimode(false);
			minmaxtimer.disconnect();
        	return false; // done
		}
		else
			img_aArt.set(pxb_xfading);
		
		return true; // continue xfade
    }

    windowH += 12;
    if (windowH <= windowH_max)
    {
        btrowPos += 12;
        fx_main.move(gr_buttonrow, 0, btrowPos);
        set_size_request(340, windowH);
        return true; // continue resizing
    }
    else
    {
		btrowPos = btrowPos_max;
        fx_main.move(gr_buttonrow, 0, btrowPos);
        set_size_request(340, windowH_max);
        //eb_center.show_all(); // new album art must have alpha = 0
		eb_center.show_all();
        b_minmax_xfade = true;
        return true; // continue with xfade
    }
}

// escape pango's reserved characters
ustring gm_Player::escapeString(ustring str_in)
{
	ustring str = str_in;
	Glib::ustring::size_type pos;
	// escape &
	for (pos = str.find("&"); pos != Glib::ustring::npos; pos = str.find("&", pos+1))
	{
	    str.replace(pos, 1, "&amp;");
	}
	// escape <
	for (pos = str.find("<"); pos != Glib::ustring::npos; pos = str.find("<", pos+1))
	{
	    str.replace(pos, 1, "&lt;");
	}
	// escape >
	for (pos = str.find(">"); pos != Glib::ustring::npos; pos = str.find(">", pos+1))
	{
	    str.replace(pos, 1, "&gt;");
	}
	// escape '
	for (pos = str.find("'"); pos != Glib::ustring::npos; pos = str.find("'", pos+1))
	{
	    str.replace(pos, 1, "&apos;");
	}
	// escape "
	for (pos = str.find("\""); pos != Glib::ustring::npos; pos = str.find("\"", pos+1))
	{
	    str.replace(pos, 1, "&quot;");
	}

	return str;
}


void gm_Player::create_rClickmenu()
{
	Glib::RefPtr<Gdk::Pixbuf> pxb;

	pxb = Gdk::Pixbuf::create_from_inline(-1, menu_view, false);
	img_men_view.set(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, tr_update, false);
	img_men_reload.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tag_edit, false);
	img_men_edit.set(pxb);
	pxb = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
	img_men_browse.set(pxb);

	rClickMenu = new Gtk::Menu();
	
	imi_viewer = new Gtk::ImageMenuItem(img_men_view,"Image viewer");
	rClickMenu->add(*imi_viewer);
	imi_viewer->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::on_menu_Art));
	
	imi_fileman = new Gtk::ImageMenuItem(img_men_browse,"File manager");
	rClickMenu->add(*imi_fileman);
	imi_fileman->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::on_menu_Browse));

	imi_tagedit = new Gtk::ImageMenuItem(img_men_edit,"Tag editor");
	rClickMenu->add(*imi_tagedit);
	imi_tagedit->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::on_menu_Edit));

	imi_reload = new Gtk::ImageMenuItem(img_men_reload,"Reload info");
	rClickMenu->add(*imi_reload);
	imi_reload->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::on_menu_Reload));

	rClickMenu->show_all();
}


void  gm_Player::on_open_with_request(std::vector<ustring> urilist, bool newlist)
{
	if (urilist.empty() || !b_connected)
		return;

	if (!config->mpd_socket_conn)
	{
		cout << "URIs ignored (no socket connection)" << endl;
		return;
	}
	
	if (newlist)
	{
		cout << "Creating new playlist from URIs" << endl;
		mpdCom->clear_list();
	}
	else
		cout << "Appending URIs to playlist" << endl;

	int i = 0;
	int last = urilist.size();
	std::vector<ustring>::iterator url;
	for(url = urilist.begin(); url != urilist.end(); ++url)
    {
		/* filename_from_uri() replaces '%20' with ' ' 
		   AND removes 'file://' */
		ustring newurl = Glib::filename_from_uri(*url);
		newurl = "file://" + newurl;
	    gm_cmdStruct newCommand;
	    newCommand.cmd = CMD_ADD;
	    newCommand.file = newurl;
		i ++;
		if (i == last) // trigger playlist update
	    	mpdCom->execute_single_command( newCommand, true);
		else
			mpdCom->execute_single_command( newCommand, false);
    }
	
	if (newlist && config->pList_new_startplaying)
		 mpdCom->play(true);
}


gm_Player::~gm_Player()
{

}

/* 
Main window dimensions:
						 4    border
			           332  widgets
			  			 4    border
			  		   --- +
			W:	   		340
  
		 				 4    border
						54	eb_lcd_bg
						 2	  space	
						 5	pb_timeprogress
						 2    space
					   204 	gr_center
					   	 4	  space
						xx	gr_buttonrow (theme sets the button size!)
						 4    border
					   --- +
			H_maxi:	   279+xx	
*/
