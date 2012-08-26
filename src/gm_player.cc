/*
 *  gm_player.cc
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

#include "gm_player.h"
#include "gm_player_pix.h"

gm_Player::gm_Player()
{
	config.load_config();
	init_widgets();
	init_signals();
    init_vars();
	settingsWindow.set_config(&config);
	// connect and set status
	set_status(-10, " ");
	if (config.get_bool("AutoConnect"))
	{
		cout << "Autoconnecting, as configured" << endl;
		on_connectrequest(config.get_string("MPD_Host"),
						  config.get_int("MPD_Port"),
						  config.get_string("MPD_Password"),
						  config.get_bool("OverrideMPDconf"));
		// if connection fails MPD is probably not running
		if (!b_connected &&  config.get_bool("StartMPD_onStart"))
		{
			cout << "Starting MPD, as configured" << endl;
			system ( (config.get_string("MPD_onStart_command")).data() );
			on_connectrequest(config.get_string("MPD_Host"),
						  config.get_int("MPD_Port"),
						  config.get_string("MPD_Password"),
						  config.get_bool("OverrideMPDconf"));
		}
	}
	
	if (!b_connected)
	{
		on_connectsignal(false);
		set_albumart("", false);
		set_status(-1, "Not connected");
	}
	
	set_childwindows();
}

// from constructor or settingswindow
void gm_Player::on_connectrequest(ustring host, int port, ustring pswd, bool usem)
{
	if ( !mpdCom.mpd_connect(host, port, pswd, usem))
	{
		on_connectsignal(false);
		set_status(-1, "Connection failed");
		set_albumart("", false);
	}
	else
		on_connectsignal(true);
}

// from mpdcom
void gm_Player::on_connectsignal(bool isconnected)
{
	if (isconnected)
	{
		b_connected = true;
		tracksWindow.connect_server(serverName, serverPort, serverPassword);
		settingsWindow.set_mpdcom( &mpdCom );
	}
	else
	{
		b_connected = false;
		tracksWindow.disconnect_server();
		settingsWindow.set_mpdcom( NULL );
	}
}


void gm_Player::init_vars()
{
	prev_fractint = 0;
	current_status = MPD_STATUS_STATE_UNKNOWN;
	current_volume = 0;
	songTotalTime = 0;
	b_connected = false;
	b_mpdSetsVolume = false;
	b_stream = false;
	b_minmax_busy = false;
    bt_play.grab_focus();
	get_mpd_paths();
		current_art_path = "";
	custom_art_file = config.get_string("AlbumArt_File");
	b_timeremain = config.get_bool("use_TimeRemaining");
	int delay = config.get_int("Scroller_Delay");
	if (delay < 20)
		delay = 20;
	titleScroller.set_delay( delay );
	set_fonts();
}

void gm_Player::get_mpd_paths()
{
	mpd_music_path = config.get_string("MPD_MusicPath");
	if (mpd_music_path.empty())
	{
		mpd_music_path = mpdCom.get_musicPath();
		if (!mpd_music_path.empty() && mpd_music_path.rfind("/") != mpd_music_path.length()-1)
			mpd_music_path += "/";
		config.set_string("MPD_MusicPath", mpd_music_path);
	}
	
	mpd_plist_path = config.get_string("MPD_PlaylistPath");
	if (mpd_plist_path.empty())
	{
		mpd_plist_path = mpdCom.get_playlistPath();
		if (!mpd_plist_path.empty() && mpd_plist_path.rfind("/") != mpd_plist_path.length()-1)
			mpd_plist_path += "/";
		config.set_string("MPD_PlaylistPath", mpd_plist_path);
	}
	
	tracksWindow.set_plistPath(mpd_plist_path);	
}

void gm_Player::set_fonts()
{
	ustring font = config.get_string("Font_Family") + " bold " +
						   	config.get_string("Scroller_Fontsize");
	titleScroller.set_font(font);
	font = config.get_string("Font_Family") + " " +
						   	config.get_string("TrackInfo_Fontsize");
	lb_type.modify_font((Pango::FontDescription)font.data());
	lb_khz.modify_font((Pango::FontDescription)font.data());
	lb_kbps.modify_font((Pango::FontDescription)font.data());
	font = config.get_string("Font_Family") + " " +
						   	config.get_string("Time_Fontsize");
	lb_time.modify_font((Pango::FontDescription)font.data());
	lb_totaltime.modify_font((Pango::FontDescription)font.data());
	font = config.get_string("Font_Family") + " " +
						   	config.get_string("Album_Fontsize");
	lb_album.modify_font((Pango::FontDescription)font.data());
	lb_year.modify_font((Pango::FontDescription)font.data());
	font = config.get_string("Font_Family") + " " +
						   	config.get_string("Tracks_Fontsize");
	tracksWindow.set_listfont(font);
}


void gm_Player::set_status(int status, ustring message)
{
	if (!message.empty())
		titleScroller.set_title(message);

    if (status == current_status)
        return;

    switch (status)
    {
        case -1: // not connected
        {
            bt_play.set_image(img_mn_play);
            img_statusled.set(pxb_led_noconn);
			lb_time.set_text("");
			lb_totaltime.set_text("");
			lb_khz.set_text("");
			lb_type.set_text("");
			lb_kbps.set_text("");
			lb_album.set_text("");
			lb_year.set_text("");
			tagInfo.reset();
			set_albumart("",false);
			// titleScroller.pause(true);
			pb_timeprogress.set_fraction(0);

            if (b_trayicon)
            {
            	tIcon_ActionGroup->get_action("Pause")->set_visible(false);
            	tIcon_ActionGroup->get_action("Play")->set_visible(true);
            	trayIcon->set(pxb_st_noconn);
				tIcon_blink();
                trayIcon->set_tooltip(" Not Connected ");
            }
            break;
        }

        case MPD_STATUS_STATE_STOP: // 1
        {
            bt_play.set_image(img_mn_play);
            img_statusled.set(pxb_led_stopped);
			lb_time.set_text("");
			lb_kbps.set_text("");
			lb_khz.set_text("");
			pb_timeprogress.set_fraction(0);

			if (b_trayicon)
			{
				tIcon_ActionGroup->get_action("Pause")->set_visible(false);
            	tIcon_ActionGroup->get_action("Play")->set_visible(true);
            	trayIcon->set(pxb_st_stopped);
			}
            break;
        }

        case MPD_STATUS_STATE_PLAY: // 2
        {
            bt_play.set_image(img_mn_pause);
            img_statusled.set(pxb_led_playing);
			// titleScroller.pause(false);
			if (b_trayicon)
			{
				tIcon_ActionGroup->get_action("Pause")->set_visible(true);
            	tIcon_ActionGroup->get_action("Play")->set_visible(false);
            	trayIcon->set(pxb_st_playing);
			}
            break;
        }

        case MPD_STATUS_STATE_PAUSE: // 3
        {
            bt_play.set_image(img_mn_play);
            img_statusled.set(pxb_led_paused);
			if (b_trayicon)
			{
				tIcon_ActionGroup->get_action("Pause")->set_visible(false);
				tIcon_ActionGroup->get_action("Play")->set_visible(true);
				lb_kbps.set_text("");
				lb_khz.set_text("");				
				trayIcon->set(pxb_st_paused);
			}
            break;
        }

        default: // 0 (MPD_STATUS_STATE_UNKNOWN), -10 etc.
        {
			bt_play.set_image(img_mn_play);
            img_statusled.set(pxb_led_stopped);
			lb_time.set_text("");
			lb_totaltime.set_text("");
			lb_khz.set_text("");
			lb_type.set_text("");
			lb_kbps.set_text("");
			lb_album.set_text("");
			lb_year.set_text("");
			tagInfo.reset();
			set_albumart("",false);
			pb_timeprogress.set_fraction(0);

			if (b_trayicon)
			{
				tIcon_ActionGroup->get_action("Pause")->set_visible(false);
				tIcon_ActionGroup->get_action("Play")->set_visible(true);
				trayIcon->set(pxb_st_stopped);
                trayIcon->set_tooltip(" Idle ");
			}
            break;
        }
    }
    current_status = status;
}


void gm_Player::set_albumart(ustring sub_path, bool stream)
{
	
	if (sub_path.empty())
	{
		if (current_art_path == "nopic")
			return;
		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, nopic, false);
		pxb_xfading = pxb_albmart->copy();
		img_aArt.set(pxb_albmart);
		current_art_path = "nopic";
		return;
	}
	
	if (stream)
	{
		if (current_art_path == "onair")
			return;
		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, onair, false);
		pxb_xfading = pxb_albmart->copy();
		img_aArt.set(pxb_albmart);
		current_art_path = "onair";
		return;
	}
	
	// [1] find a suitable image
	ustring artPath = mpd_music_path + sub_path;
	bool b_file_ok = false;
	if (!custom_art_file.empty())
		b_file_ok = go_find_art(artPath, custom_art_file.data());
	if (!b_file_ok)
		b_file_ok = go_find_art(artPath, "folder");
	if (!b_file_ok)
		b_file_ok = go_find_art(artPath, "album");
	if (!b_file_ok)
		b_file_ok = go_find_art(artPath, "cover");
	if (!b_file_ok)
		b_file_ok = go_find_art(artPath, "front");
	//if (!b_file_ok) // anything else?
		//b_file_ok = go_find_art(artPath, ".");
		
	if (current_art_path == artPath)
		return; // nothing to do
	else
		current_art_path = artPath;

	// [2] put it in a pixbuf	
	if (b_file_ok)
	{
		try
		{
			pxb_albmart = Gdk::Pixbuf::create_from_file(artPath, 200, 200, true);
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

    if (b_file_ok)
	{
		// [3] draw a 1 px border on the image
		if(!pxb_albmart->get_has_alpha())
			pxb_albmart = pxb_albmart->add_alpha(false,0,0,0);
		int imgH = pxb_albmart->get_height();
		int imgW = pxb_albmart->get_width();
		int rows = pxb_albmart->get_rowstride();
		guint8 *ptr_pixel;
		guint8 *ptr_firstpixel = pxb_albmart->get_pixels();

		for (int  h = 0; h < imgH; h++)
		{
			ptr_pixel = ptr_firstpixel + (h * rows);
			for (int  w = 0; w < imgW; w++)
			{
				if (h == 0 || h == imgH-1 || w == 0 || w == imgW-1)
				{
					ptr_pixel[0] = 140; // R
					ptr_pixel[1] = 155; // G
					ptr_pixel[2] = 165; // B
					ptr_pixel[3] = 255; // a
				}
				ptr_pixel += 4;
			}
		}
		pxb_xfading = pxb_albmart->copy();
	}
	else // b_file_ok == false
	{
		if (current_art_path != "nopic")
		{
    		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, nopic, false);
			pxb_xfading = pxb_albmart->copy();
			current_art_path = "nopic";
		}
	}

	// [5] show pxb_albmart
	if (b_winIsMax)
		img_aArt.set(pxb_albmart);
	else
	{
		// set alpha = 0 for pxb_xfading
		ptr_xfad =  pxb_xfading->get_pixels();
        aArt_H = 	pxb_xfading->get_height();
        aArt_W = 	pxb_xfading->get_width();
        aArt_row = 	pxb_xfading->get_rowstride();
		for (int h = 0; h < aArt_H; h++)
		{
			ptr_xfadpix = ptr_xfad + h * aArt_row;
			for (int w = 0; w < aArt_W; w++)
			{
				ptr_xfadpix[3] = 0;
				ptr_xfadpix += 4; // R,G,B,a
			}
		}
		// show pxb_xfading
		img_aArt.set(pxb_xfading);
	}
}


bool gm_Player::go_find_art(ustring & artPath, const char* findthis)
{
    DIR *pdir;
    struct dirent *pent;
    pdir = opendir(artPath.data());
    if (!pdir)
	{
		cout << "Album art: error opening directory" << endl;
		return false;
	}
	else
	{
		errno = 0;
		while ( (pent = readdir(pdir)) )
		{
			std::string str = pent->d_name;
			int len = str.length();
			std::transform(str.begin(), str.end(), str.begin(), tolower);
			if(len > 5)
			{
				if ( str.find(".jpg")  == len - 4 ||
				  	 str.find(".png")  == len - 4 ||
				  	 str.find(".gif")  == len - 4 ||
				  	 str.find(".bmp")  == len - 4 ||
				  	 str.find(".jpeg") == len - 5 )
				{
					if ( str.find(findthis) != Glib::ustring::npos )
					{
						artPath += pent->d_name; // str is lowercase now!
						closedir(pdir);
						return true;
					}
				}
			}
		}
		if (errno)
			cout << "Album art: error reading directory" << endl;
		closedir(pdir);
		return false;
	}
}


void gm_Player::init_widgets()
{
	bool usetips = config.get_bool("show_ToolTips");
	
	b_winIsMax = config.get_bool("PlayerWindow_Max");
	if (b_winIsMax)
	{
		windowH = 310;
		btrowPos = 276;
	}
	else
	{
		windowH = 104;
		btrowPos = 70;
	}

	int posx = config.get_int("PlayerWindow_Xpos");
	if (posx < 0) posx = 0;
	int posy = config.get_int("PlayerWindow_Ypos");
	if (posy < 0) posy = 0;
	move(posx, posy);
	
	// window properties
    set_title("Guimup");
    set_size_request(340, windowH);
    set_resizable(false);
    set_border_width(4);

	Glib::RefPtr<Gdk::Pixbuf> pxb_pointer;
	
    std::list< Glib::RefPtr<Gdk::Pixbuf> > window_icons;
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_icon16, false);
    window_icons.push_back(pxb_pointer);
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_icon24, false);
    window_icons.push_back(pxb_pointer);
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
    window_icons.push_back(pxb_pointer);
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_icon48, false);
    window_icons.push_back(pxb_pointer);
    set_icon_list(window_icons);

	// widget properties
	eb_background.set_size_request(332,50);
    // eb_background is only used for its background color
	eb_background.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#D3E4F0"));
	vb_lcdisplay.set_size_request(332,50);
    fr_lcdisplay.set_shadow_type(Gtk::SHADOW_IN);
	titleScroller.set_bg("#D3E4F0");
	titleScroller.set_fg("#1E3C59");
	titleScroller.set_size_request(328,24); // eb_background minus frame-border
	eb_time.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#D3E4F0"));
	eb_time.set_events(Gdk::BUTTON_PRESS_MASK);
    hb_trackdata.set_homogeneous(false);
    lb_type.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_type.set_alignment(0, 0.5);
    lb_type.set_size_request(40,-1);
	lb_type.set_use_markup(true);
    lb_kbps.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_kbps.set_alignment(1, 0.5); 
	lb_kbps.set_use_markup(true);
	lb_kbps.set_size_request(64,-1);
    lb_khz.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_khz.set_alignment(0, 0.5);
	lb_khz.set_use_markup(true);
    lb_time.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_time.set_alignment(1, 0.5);
    lb_time.set_use_markup(true);
	if (usetips)
		lb_time.set_tooltip_text("Click to toggle +/-");
    lb_totaltime.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_totaltime.set_alignment(0, 0.5);
    lb_totaltime.set_use_markup(true);	
    pb_timeprogress.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#FFFFFF"));
	pb_timeprogress.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    pb_timeprogress.set_size_request(332,8);
	pb_timeprogress.set_events(Gdk::BUTTON_PRESS_MASK);
    hb_center.set_size_request(332,200);
    hb_center.set_spacing(12);
    img_aArt.set_size_request(200,200);
	img_aArt.set_alignment(0.5, 0.5);
    vb_center_right.set_homogeneous(false);
    vb_center_right.set_spacing(4);
    lb_album.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_album.set_size_request(120,-1); // makes lines wrap
    lb_album.set_alignment(0, 0);
    lb_album.set_use_markup(true);
    lb_album.set_single_line_mode(false);
    lb_album.set_line_wrap(true);
    lb_year.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C59"));
    lb_year.set_alignment(0, 0);
    lb_year.set_use_markup(true);
	hb_tbuttons.set_homogeneous(false);
	hb_tbuttons.set_spacing(5);
	hb_tbuttons.set_size_request(-1, 22);
    hsc_volume.set_show_fill_level(true);
    hsc_volume.set_draw_value(false);
    hsc_volume.set_increments(5, 10);
    hsc_volume.set_range(0,100);
	hsc_volume.set_size_request(-1,20);
    hb_buttonrow.set_size_request(332,26);
    hb_buttonrow.set_spacing(5);
    hb_buttonrow.set_homogeneous(false);
    img_statusled.set_size_request(45,26);
	
	/* Dimensions: see footnote [2] */
	
	// inline graphics
    	// prev
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_prev, false);
    img_mn_prev.set(pxb_pointer);
    bt_prev.set_image(img_mn_prev);
    	// stop
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_stop, false);
    img_mn_stop.set(pxb_pointer);
    bt_stop.set_image(img_mn_stop);
    	// play
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_play, false);
    img_mn_play.set(pxb_pointer);
    bt_play.set_image(img_mn_play);
    	// pause
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_pause, false);
    img_mn_pause.set(pxb_pointer);
    	// next
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_next, false);
    img_mn_next.set(pxb_pointer);
    bt_next.set_image(img_mn_next);
    	// playlist
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_playlist, false);
    img_mn_playlist.set(pxb_pointer);
    bt_plst.set_image(img_mn_playlist);
	if (usetips)
		bt_plst.set_tooltip_text("Library & Playlist");
    	// sizer
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_sizer, false);
    img_mn_sizer.set(pxb_pointer);
    bt_sizr.set_image(img_mn_sizer);
	if (usetips)
		bt_sizr.set_tooltip_text("Mini-Maxi mode");
    	// config
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_config, false);
    img_mn_config.set(pxb_pointer);
    bt_conf.set_image(img_mn_config);
	if (usetips)
		bt_conf.set_tooltip_text("Settings");
    	// info (i) icon
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_info_a, false);
	img_mn_info_a.set(pxb_pointer);
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_info_b, false);
	img_mn_info_b.set(pxb_pointer);
	bt_info.set_image(img_mn_info_a);
	if (usetips)
		bt_info.set_tooltip_text("Song information");
    	// random
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, tb_random, false);
    img_tb_random.set(pxb_pointer);
    tbt_random.set_image(img_tb_random);
	if (usetips)
		tbt_random.set_tooltip_text("Random");
    	// repeat
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, tb_repeat, false);
    img_tb_repeat.set(pxb_pointer);
    tbt_repeat.set_image(img_tb_repeat);
	if (usetips)
		tbt_repeat.set_tooltip_text("Repeat");
    	// status led
	if (usetips)
		img_statusled.set_tooltip_text("Status led");
    pxb_led_playing = Gdk::Pixbuf::create_from_inline(-1, led_playing, false);
    pxb_led_noconn = Gdk::Pixbuf::create_from_inline(-1, led_noconn, false);
    pxb_led_paused = Gdk::Pixbuf::create_from_inline(-1, led_paused, false);
    pxb_led_stopped = Gdk::Pixbuf::create_from_inline(-1, led_stopped, false);
	
    // put widgets together
	add(fx_main); // main container: 4 rows
    fx_main.put(eb_background, 0, 0);
    	// row 1
    eb_background.add(fr_lcdisplay);
        fr_lcdisplay.add(vb_lcdisplay); // 2 rows
            vb_lcdisplay.add(titleScroller);
            vb_lcdisplay.add(hb_trackdata); // 5 items
                hb_trackdata.pack_start(lb_type, false, false, 6);
                hb_trackdata.pack_start(lb_kbps, false, false, 0);
                hb_trackdata.pack_start(lb_khz, true, true, 0); // pushes the others left-right
				hb_trackdata.pack_start(eb_time, false, false, 0);
						eb_time.add(lb_time);
				hb_trackdata.pack_end(lb_totaltime, false, false, 6);
		// row 2
	fx_main.put(pb_timeprogress, 0, 56);
		// row 3
	fx_main.put(hb_center, 0, 70); // 2 columns
		hb_center.add(img_aArt);
		hb_center.add(vb_center_right); // 4 items
			vb_center_right.pack_start(lb_album, false, false, 0);
			vb_center_right.pack_start(lb_year, true, true, 0); // pushes the others up
			vb_center_right.pack_start(hb_tbuttons, false, false, 0);
				hb_tbuttons.pack_start(bt_info, true, true, 0);
				hb_tbuttons.pack_start(tbt_repeat, true, true, 0);
				hb_tbuttons.pack_end(tbt_random, true, true, 0);
			vb_center_right.pack_end(hsc_volume, false, false, 2);
		// row 4: 8 items
	fx_main.put(hb_buttonrow, 0, btrowPos);
		hb_buttonrow.pack_start(bt_prev, true, true, 0);
		hb_buttonrow.pack_start(bt_stop, true, true, 0);
		hb_buttonrow.pack_start(bt_play, true, true, 0);
		hb_buttonrow.pack_start(bt_next, true, true, 0);
		hb_buttonrow.pack_start(bt_plst, true, true, 0);
		hb_buttonrow.pack_start(img_statusled, false, false, 0); // fixed
		hb_buttonrow.pack_start(bt_sizr, true, true, 0);
		hb_buttonrow.pack_end(bt_conf, true, true, 0);

    // show the main box and it's child widgets
	if (b_winIsMax)
    	fx_main.show_all();
	else
	{
		fx_main.show();
		eb_background.show_all();
		pb_timeprogress.show();
		hb_buttonrow.show_all();		
	}

	b_trayicon = config.get_bool("use_TrayIcon");
	if (b_trayicon)
    	tIcon_create();
}

// Connect signals
void gm_Player::init_signals()
{
	// the mpdCom
	mpdCom.signal_connected.connect(sigc::mem_fun(*this, &gm_Player::on_connectsignal));
	mpdCom.signal_status.connect(sigc::mem_fun(*this, &gm_Player::set_status));
	mpdCom.signal_songInfo.connect(sigc::mem_fun(*this, &gm_Player::on_newSong));
	mpdCom.signal_statusInfo.connect(sigc::mem_fun(*this, &gm_Player::on_newStatus));
	mpdCom.signal_host_port_pwd.connect(sigc::mem_fun(*this, &gm_Player::on_signal_host_port_pwd));

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
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_plst));

    bt_sizr.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_mmax));

    bt_conf.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_sets));

    bt_info.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_info));

    tbt_repeat.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_rept));

    tbt_random.signal_clicked().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_rand));

	hsc_volume.signal_value_changed().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_volm));
	
	settingsWindow.signal_pleaseconnect.connect(sigc::mem_fun(*this, &gm_Player::on_connectrequest) );
	settingsWindow.signal_settingssaved.connect(sigc::mem_fun(*this, &gm_Player::on_settingssaved) );
	settingsWindow.signal_applyfonts.connect(sigc::mem_fun(*this, &gm_Player::set_fonts) );

	// return bool
	eb_time.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_timeClicked) );

	pb_timeprogress.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_progressClicked) );

}


void gm_Player::on_signal_host_port_pwd(ustring host, int port, ustring pwd)
{
	serverName = host;
	serverPort = port;
	serverPassword = pwd;
}


bool gm_Player::on_timeClicked(GdkEventButton*)
{
	if (current_status > 0) // STOP, PLAY or PAUSE
		b_timeremain = !b_timeremain;
	return false;
}


bool gm_Player::on_progressClicked(GdkEventButton* eb)
{
	if (current_status != MPD_STATUS_STATE_PLAY && current_status != MPD_STATUS_STATE_PAUSE)
		return false;

	double fraction = ((*eb).x)/(double)330;
	pb_timeprogress.set_fraction(fraction);

	int percent = (int)(fraction * songTotalTime);
	mpdCom.set_seek(percent);
	return false;
}

// apply new albumart, fonts etc.
void gm_Player::on_settingssaved()
{
	custom_art_file = config.get_string("AlbumArt_File");
	set_fonts();
	get_mpd_paths();
}

void gm_Player::on_newStatus( statInfo sInfo )
{
	// first set the playback status
	if (current_status == MPD_STATUS_STATE_UNKNOWN)
	{
		// only respond to PLAY
		if (sInfo.mpd_status == MPD_STATUS_STATE_PLAY)
			set_status(sInfo.mpd_status);
	}
	else
	{
		if (sInfo.mpd_status != current_status)
		{	
			if (sInfo.mpd_status == MPD_STATUS_STATE_UNKNOWN)
				set_status(sInfo.mpd_status, " ");
			else
				set_status(sInfo.mpd_status);
		}
	}

	// next set the other status info
	if (sInfo.mpd_status == MPD_STATUS_STATE_PLAY)
	{
		double timenow = sInfo.mpd_time;
		songTotalTime = sInfo.mpd_totalTime;
		
		if (!b_stream && songTotalTime > 0)
		{
			double fraction = timenow/songTotalTime;
			// some tracks play a bit too long (honestly)
			if (fraction > 1.0)
				fraction = 1.0;
			// progressbar wants 2.00 decimals
			int fractint = (int)(fraction * 100);
			fraction = (double)fractint / 100.0;
			if (fractint != prev_fractint) /* (check the int: always true for double) */
			{
				pb_timeprogress.set_fraction(fraction);
				prev_fractint = fractint;
			}
		}
		
		if (b_stream || sInfo.mpd_time == MPD_SONG_NO_TIME)
		{
			lb_time.set_text("");
			lb_totaltime.set_text("[»]");
		}
		else
		{
			if (b_timeremain)
				timenow = songTotalTime - timenow;
			
			ustring timestring = "";
			if (b_timeremain)
				timestring = timestring + "- ";
			timestring = timestring + into_time((int)timenow);
			lb_time.set_markup("<b>" + timestring + "</b>");
			
			timestring = " [" + into_time((int)songTotalTime) + "]";
			lb_totaltime.set_text(timestring);
		}

		lb_kbps.set_markup(into_string(sInfo.mpd_kbps) + " kb/s ");

		double dbl_khz = (double)sInfo.mpd_kHz/1000;
		std::string str_dbl;
		std::ostringstream out;
		out << dbl_khz;
		str_dbl = out.str();
    	lb_khz.set_markup(" " + str_dbl + " kHz");
	}
	
	if (current_volume != sInfo.mpd_volume)
	{
		b_mpdSetsVolume = true;
		/* slider Scale is set 0 to 100
		   mpd also reports 0 to 100 */
		current_volume = sInfo.mpd_volume;
		hsc_volume.set_value(sInfo.mpd_volume);
		b_mpdSetsVolume = false;
	}
	
	if (sInfo.mpd_random != b_random)
	{
		//  1st set bool, next set button
		b_random = sInfo.mpd_random;
		tbt_random.set_active(b_random);
	}
	
	if (sInfo.mpd_repeat != b_repeat)
	{
		//  1st set bool, next set button
		b_repeat = sInfo.mpd_repeat;
		tbt_repeat.set_active(b_repeat);
	}
}

// Signal handler.
void gm_Player::on_signal(int sigID)
{
    switch (sigID)
    {
        case ID_prev:
        {
            if (current_status != MPD_STATUS_STATE_PLAY)
                break;
			mpdCom.prev();
            break;
        }

        case ID_stop:
        {
            if (current_status == MPD_STATUS_STATE_UNKNOWN ||
				current_status == MPD_STATUS_STATE_STOP)
				break;
			
            set_status(MPD_STATUS_STATE_STOP);
			mpdCom.stop();
            break;
        }

        case ID_play:
        {
			switch (current_status)
			{
				case MPD_STATUS_STATE_PLAY:
            	{
                	set_status(MPD_STATUS_STATE_PAUSE);
					mpdCom.pause();
					break;
            	}
 
				case MPD_STATUS_STATE_PAUSE:
            	{
					set_status(MPD_STATUS_STATE_PLAY);
					mpdCom.resume();
					break;
            	}

				case MPD_STATUS_STATE_STOP:
            	{
                	set_status(MPD_STATUS_STATE_PLAY);
					mpdCom.play();
					break;
            	}

				case MPD_STATUS_STATE_UNKNOWN:
            	{
                	set_status(MPD_STATUS_STATE_PLAY);
					mpdCom.play(true);
					break;
            	}
						
				default:
				{
					cout << "'Play' pressed, but status is "<< current_status << endl;
					break;
				}
			}
			break; // case ID_play
        }

        case ID_next:
        {
            if (current_status != MPD_STATUS_STATE_PLAY)
                break;
			mpdCom.next();
            break;
        }
				
		case ID_info:
		{
			if (tagInfo.is_visible())
				tagInfo.hide();
			else
			{
				int xpos, ypos;
				get_position(xpos, ypos);
				if (xpos + 360 + 250 < Gdk::screen_width())
					tagInfo.show_at(xpos + 360, ypos);
				else
					tagInfo.show_at(xpos - 270, ypos);
			}
			break;
		}
				
		case ID_sets:
		{
			if (settingsWindow.is_visible())
				settingsWindow.hide();
			else
				settingsWindow.load_and_show();
			break;
		}

		case ID_plst:
		{
			if (tracksWindow.is_visible())
				tracksWindow.hide();
			else
				tracksWindow.show();
			break;
		}
				
        case ID_rand:
        {
            bool status = tbt_random.get_active();
            if (b_random != status) // avoid loop!
            {
				b_random = status;
				mpdCom.set_random(status);
            }
            break;
        }

        case ID_rept:
        {
			bool status = tbt_repeat.get_active();
            if (b_repeat != status) // avoid loop!
            {
				b_repeat = status;
				mpdCom.set_repeat(status);;
            }
            break;
        }

        case ID_volm: // see footnote [1]
        {
			if ( b_mpdSetsVolume)
                break;

			int vol = (int)hsc_volume.get_value();
			if (vol != current_volume)
			{
				current_volume = vol;
				mpdCom.set_volume(vol);
			}
            break;
        }

        case ID_mmax:
        {
            if (b_minmax_busy)
                break;

            if (b_winIsMax)
            {
                b_minmax_xfade = true;
				alphacycler = 255;
                minmaxtimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Player::miniMice), 12);
                b_winIsMax = false;
            }
            else
            {
                b_minmax_xfade = false;
				alphacycler = 0;
                minmaxtimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Player::maxiMice), 12);
                b_winIsMax = true;
            }
            break;
        }
				
        default:
		{			
        	cout << "Button has no function yet" << endl;
			break;
		}
    }
}


void gm_Player::tIcon_create()
{
    // systray icons
    pxb_st_playing = Gdk::Pixbuf::create_from_inline(-1, st_playing, false);
    pxb_st_noconn = Gdk::Pixbuf::create_from_inline(-1, st_noconn, false);
    pxb_st_paused = Gdk::Pixbuf::create_from_inline(-1, st_paused, false);
    pxb_st_stopped = Gdk::Pixbuf::create_from_inline(-1, st_stopped, false);
    // menu icons (custom stock items)
    Glib::RefPtr<Gtk::IconFactory> gm_Ifact = Gtk::IconFactory::create();
    Gtk::IconSource gm_Isrc;
	
	Glib::RefPtr<Gdk::Pixbuf> pxb_pointer;
	
    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, mn_playlist, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset1;
    gm_Iset1.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::INDEX, gm_Iset1);

    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, st_exit, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset2;
    gm_Iset2.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::QUIT, gm_Iset2);

    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, st_prev, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset3;
    gm_Iset3.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_PREVIOUS, gm_Iset3);

    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, st_stop, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset4;
    gm_Iset4.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_STOP, gm_Iset4);

    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, st_play, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset5;
    gm_Iset5.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_PLAY, gm_Iset5);

    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, st_pause, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset6;
    gm_Iset6.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_PAUSE, gm_Iset6);

    pxb_pointer = Gdk::Pixbuf::create_from_inline(-1, st_next, false);
    gm_Isrc.set_pixbuf(pxb_pointer);
    Gtk::IconSet gm_Iset7;
    gm_Iset7.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_NEXT, gm_Iset7);

    gm_Ifact->add_default();

    // Setting up the UIManager:
    tIcon_ActionGroup = Gtk::ActionGroup::create();
	
    tIcon_ActionGroup->add(
    Gtk::Action::create("Prev", Gtk::Stock::MEDIA_PREVIOUS),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_prev) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Stop",Gtk::Stock::MEDIA_STOP),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_stop) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Pause",Gtk::Stock::MEDIA_PAUSE),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_play) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Play",Gtk::Stock::MEDIA_PLAY),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_play) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Next",Gtk::Stock::MEDIA_NEXT),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_next) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Quit", Gtk::Stock::QUIT),
    sigc::mem_fun(*this, &gm_Player::hide) );

    tIcon_UIManager = Gtk::UIManager::create();
    tIcon_UIManager->insert_action_group(tIcon_ActionGroup);

    Glib::ustring ui_info =
    "<ui>"
    "  <popup name='Popup'>"
    "    <menuitem action='Prev' />"
    "    <menuitem action='Stop' />"
    "    <menuitem action='Pause'/>"
    "    <menuitem action='Play' />"
    "    <menuitem action='Next' />"
    "    <separator/>"
    "    <menuitem action='Quit' />"
    "  </popup>"
    "</ui>";

    tIcon_UIManager->add_ui_from_string(ui_info);

    tIcon_ActionGroup->get_action("Pause")->set_visible(false);

    // Setting up the StatusIcon:
    trayIcon = Gtk::StatusIcon::create(pxb_st_noconn);
    trayIcon->set_tooltip("Guimup");

    // StatusIcon signals
    trayIcon->signal_activate().connect(
    sigc::mem_fun(*this, &gm_Player::tIcon_toggle_hide) );

    trayIcon->signal_popup_menu().connect(
    sigc::mem_fun(*this, &gm_Player::tIcon_on_popup) );
}


void gm_Player::tIcon_toggle_hide()
{
    if ( get_window()->is_visible() )
    {
         get_position(win_pos.first, win_pos.second);
         get_window()->hide();
    }
    else
    {
        get_window()->show();
        move(win_pos.first, win_pos.second);
    }
}


bool gm_Player::on_delete_event(GdkEventAny* event)
{
	if(b_trayicon)
	{
    	tIcon_toggle_hide();
    	return true;
	}
	else
		return false; // quit
}

// window calls this on quit
void gm_Player::on_hide()
{
	int posx, posy;
	get_position(posx, posy);
	config.set_int("PlayerWindow_Xpos", posx);
	config.set_int("PlayerWindow_Ypos", posy);
	config.set_bool("PlayerWindow_Max", b_winIsMax);
	config.set_bool("use_TimeRemaining", b_timeremain);
	// tracks window
	tr_settings trs = tracksWindow.get_trSettings();
	config.set_int("TracksWindow_panePos", trs.pane_pos);
	config.set_int("TracksWindow_Xpos", trs.x_pos);
    config.set_int("TracksWindow_Ypos", trs.y_pos);
    config.set_int("TracksWindow_W", trs.w_width);
    config.set_int("TracksWindow_H", trs.w_height);
	config.set_int("Tracks_DBmode", trs.db_mode);
	// settings window
	st_posxy sts = settingsWindow.get_stPos();
	config.set_int("SettingsWindow_Xpos", sts.x_pos);
    config.set_int("SettingsWindow_Ypos", sts.y_pos);
	// finally
	config.save_config();
	if (config.get_bool("QuitMPD_onQuit"))
	{
		cout << "executing: " << config.get_string("MPD_onQuit_command") << endl;
		system ( (config.get_string("MPD_onQuit_command")).data() );
	}
	cout << "Goodbye!" << endl;
}

// Show the Tray-Icon popup menu:
void gm_Player::tIcon_on_popup(guint button, guint32 time)
{
    Gtk::Menu* pMenu = static_cast<Gtk::Menu*>(
    tIcon_UIManager->get_widget("/Popup") );

    trayIcon->popup_menu_at_position(*pMenu, button, time);
}

// Activate blinking on StatusIcon
void gm_Player::tIcon_blink()
{
    if ( trayIcon->get_blinking() )
    return;

    // Stop the blinking mode (after 5 sec):
    struct TimeOut
    {
        static bool stop_blinking(Glib::RefPtr<Gtk::StatusIcon>& refSI)
        {
            refSI->set_blinking(false);
            return false; // terminate timeout
        }
    };

    Glib::signal_timeout().connect(
    sigc::bind(sigc::ptr_fun(&TimeOut::stop_blinking),trayIcon), 5000);
    // start the blinking mode (now)
    trayIcon->set_blinking();
}


void gm_Player::on_newSong( songInfo newSong )
{
	if (newSong.nosong)
	{
		bt_info.set_image(img_mn_info_a);
		cout << "No song" << endl;
		return;
	}

	cout << "New song: " << newSong.songNr + 1 << endl;
	
	ustring fname = "";
	ustring path = "";

		
	if (!newSong.file.empty())
	{
		fname = newSong.file;
		// path, from mpd's music dir, inc the last "/"
		path = fname.substr(0, (fname.rfind("/"))+1);
		// remove path, keep filename only
		fname = fname.substr(fname.rfind("/")+1, fname.length());
	}

	// streams start with "http:"	
	if (((path.substr(0,5)).lowercase()) == "http:")
	{
		set_albumart(path, true);
		lb_type.set_markup("URL");
		lb_album.set_markup("Audio stream");
		lb_year.set_text("");
		// newSong->artist is NULL !
		ustring artist_title = "waiting for info";
		if (!newSong.name.empty())
		{
			artist_title = newSong.name;
		}
		else
		{
			if (!newSong.title.empty())
				artist_title = newSong.title;
		}
		titleScroller.set_title(artist_title);
		trayIcon->set_tooltip(artist_title);
		bt_info.set_image(img_mn_info_a);
		b_stream = true;
		return;
	}
	else
		b_stream = false;
	
	// album art
	set_albumart(path, false);

	// extension
	if (fname.empty())
	{
		fname = "File name?!";
		lb_type.set_text("??");
	}
	else
	{
		ustring xtension = (fname.substr(fname.rfind(".")+1, fname.length()) ).uppercase();
		lb_type.set_markup(xtension);
	}

	// artist & title
	ustring artist = "??";
	if (!newSong.artist.empty())
		artist = newSong.artist;
	
	ustring title = "??";
	if (!newSong.title.empty())	
		title = newSong.title;

	if (artist.empty() && title.empty())
	{
		titleScroller.set_title(fname);
		trayIcon->set_tooltip(fname);
	}
	else
	{
		titleScroller.set_title(artist, title);
		trayIcon->set_tooltip( " " + artist + " : " + title + " ");

	}
	// Now we can (and must) escape for the tagInfo dialog
	artist = escapeString(artist);
	title = escapeString(title);
	
	//album info
	ustring album = "";
	if (!newSong.album.empty())
	{
		album = newSong.album;
		album = escapeString(album);
	}
    lb_album.set_markup("<b>" + album + "</b>");

	ustring track = "";
	if (!newSong.track.empty())
		track = newSong.track;

	ustring disc = "";
	if (!newSong.disc.empty())
		disc = newSong.disc;

	// year
	ustring year = "";
	if (!newSong.date.empty())
	{
		year = newSong.date;
		year = escapeString(year);
	}
	lb_year.set_markup(year);
	
	// genre
	ustring genre = "";
	if (!newSong.genre.empty())
	{
		genre = newSong.genre;
		genre = escapeString(genre);
	}

	bool b_extra_info = false;
	
	// composer
	ustring composer = "";
	if (!newSong.composer.empty())
	{
		composer = newSong.composer;
		composer = escapeString(composer);
		b_extra_info = true;
	}

	// performer
	ustring performer = "";
	if (!newSong.performer.empty())
	{
		performer = newSong.performer;
		performer = escapeString(performer);
		b_extra_info = true;
	}
	
	// comment
	ustring comment = "";
	if (!newSong.comment.empty())
	{
		comment = newSong.comment;
		comment = escapeString(comment);
		b_extra_info = true;
	}
	
	if (b_extra_info)
		bt_info.set_image(img_mn_info_b);
	else
		bt_info.set_image(img_mn_info_a);
	
	tagInfo.set(artist, title, album, track, disc, year,
				genre, composer, performer, comment);
}


std::string gm_Player::into_string(int in)
{
	std::string str_int;
	std::ostringstream out;
	out << in;
	str_int = out.str();
	return str_int;	
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
		alphacycler -= 20;
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
		
		if (alphacycler == 0)
		{
            b_minmax_xfade = false;
		    hb_center.hide_all();
		}
        return true; // continue
    }

    windowH -= 13;
    if (windowH >= 104)
    {
        btrowPos -= 13;
        set_size_request(340, windowH);
        fx_main.move(hb_buttonrow, 0, btrowPos);
        return true; // continue resizing
    }
    else
    {
        windowH = 104;
        btrowPos = 70;
        set_size_request(340, windowH);
        fx_main.move(hb_buttonrow, 0, btrowPos);
        b_minmax_busy = false;
		minmaxtimer.disconnect();
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
		alphacycler += 20;
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
		
		
		if (alphacycler == 255)
		{
            b_minmax_xfade = false;		
        	b_minmax_busy = false;
			img_aArt.set(pxb_albmart);
			minmaxtimer.disconnect();
        	return false; // done
		}
		else
			img_aArt.set(pxb_xfading);
		
		return true; // continue xfade
    }

    windowH += 13;
    if (windowH <= 310)
    {
        btrowPos += 13;
        fx_main.move(hb_buttonrow, 0, btrowPos);
        set_size_request(340, windowH);
        return true; // continue resizing
    }
    else
    {
        windowH = 310;
        btrowPos = 276;
        fx_main.move(hb_buttonrow, 0, btrowPos);
        set_size_request(340, windowH);
        hb_center.show_all(); // new album art must have alpha = 0
        b_minmax_xfade = true;
        return true; // continue with xfade
    }
}

// escape pango's reserved characters
ustring gm_Player::escapeString(ustring str)
{
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

void gm_Player::set_childwindows()
{
	tr_settings tsets;
	int pos  = config.get_int("TracksWindow_panePos");
	if (pos < 240)
		pos  = 240;
	tsets.pane_pos = pos;
	int xpos = config.get_int("TracksWindow_Xpos");
	if (xpos < 0)
		xpos = 0;
	tsets.x_pos = xpos;
	int ypos = config.get_int("TracksWindow_Ypos");
	if (ypos < 0)
		ypos = 0;
	tsets.y_pos = ypos;
	int width = config.get_int("TracksWindow_W");
	if (width < 500)
		width = 500;
	tsets.w_width = width;
	int height = config.get_int("TracksWindow_H");
	if (height < 180)
		height = 180;
	tsets.w_height = height;
	int dbm = config.get_int("Tracks_DBmode");
	if (dbm < 0 || dbm > 4)
		dbm = 0;
	tsets.db_mode = dbm;
	
	bool tips = config.get_bool("show_ToolTips");
	tsets.b_ttips = tips;

	tracksWindow.set_trSettings(tsets);
	
	st_posxy ssets;
	
	xpos = config.get_int("SettingsWindow_Xpos");
	if (xpos < 0)
		xpos = 0;
	ssets.x_pos = xpos;
	ypos = config.get_int("SettingsWindow_Ypos");
	if (ypos < 0)
		ypos = 0;
	ssets.y_pos = ypos;	
	
	settingsWindow.set_stPos(ssets);
}

gm_Player::~gm_Player()
{

}

//	Footnotes

/* 
[1]	The Scale widget also sends a value_changed signal
	after widget.set_value(). As a result a feedback
	loop can be started, since mpd will report the change
	back here and trigger a widget.set_value().
	Solution: use boolean b_mpdSetsVolume, so this will
	not act on signals triggered by "on_newStatus"

[2] Main window dimensions:
						4    border
			           332  widgets
			  			 4    border
			  		   --- +
			Width	   340
  
		 				 4    border
						50	eb_background
						 6	  space
						 8	pb_timeprogress
						 6    space
					   200 	hb_center
					   	 6	  space
						26	hb_buttonrow
						 4    border
					   --- +
			H_maxi =   310
			  		   206  hb_center + space
			  		   --- -
			H_mini =   104
*/
