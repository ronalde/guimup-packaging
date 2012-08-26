/*
 *  gm_player.cc
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

#include "gm_player.h"
#include "gm_player_pix.h"

gm_Player::gm_Player()
{
	init_widgets();
    init_vars();
	init_signals();
	
	libraryWindow.set_config(&config);
	libraryWindow.set_trSettings();
	
	// right-click popup menu
	Glib::RefPtr<Gdk::Pixbuf> pxbuf;	
	pxbuf = Gdk::Pixbuf::create_from_inline(-1, tr_folder, false);
	img_men_browse.set(pxbuf);
	pxbuf = Gdk::Pixbuf::create_from_inline(-1, tr_update, false);
	img_men_update.set(pxbuf);
	pxbuf = Gdk::Pixbuf::create_from_inline(-1, tag_edit, false);
	img_men_edit.set(pxbuf);
	pxbuf = Gdk::Pixbuf::create_from_inline(-1, art_view, false);
	img_men_art.set(pxbuf);

	{ Gtk::Menu::MenuList& menulist = rClickMenu.items();
	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(" View albumart", img_men_art,
	  	sigc::mem_fun(*this, &gm_Player::on_menu_Art) ) );
	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(" Edit tags", img_men_edit,
	  	sigc::mem_fun(*this, &gm_Player::on_menu_Edit) ) );
	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(" Reload tags", img_men_update,
	  	sigc::mem_fun(*this, &gm_Player::on_menu_Reload) ) );
	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(" Open folder", img_men_browse,
		sigc::mem_fun(*this, &gm_Player::on_menu_Browse) ) );
	} rClickMenu.accelerate(*this);
	// disable until we have a song
	Gtk::Menu::MenuList& menulist = rClickMenu.items();
	for (int i = 0; i < 4; i++)
	    menulist[i].set_sensitive(false);
	// End right-click popup menu
	
	settingsWindow.set_config(&config);
		
	// connect and set status
	set_status(-10, " ");
	if (config.AutoConnect)
	{
		cout << "Autoconnecting, as configured" << endl;
		on_connectrequest(config.MPD_Host, config.MPD_Port, config.MPD_Password, config.OverrideMPDconf);
		// if connection fails MPD is probably not running
		if (!b_connected &&  config.StartMPD_onStart)
		{
			cout << "Starting MPD, as configured" << endl;
			system ( (config.MPD_onStart_command).data() );
			on_connectrequest(config.MPD_Host, config.MPD_Port,
						  config.MPD_Password, config.OverrideMPDconf);
		}
	}
	
	if (!b_connected)
	{
		on_connectsignal(false);
		set_albumart("nopic");
		set_status(-1, "Not connected");
	}
	
	//	if (!config.toggle_Player && b_useTrayIcon): can't hide the player in its own constructor!

	if (config.toggle_Library &&  b_useTrayIcon)
	{
		libraryWindow.move(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
		libraryWindow.present();
		b_libraryWindow_hidden = false;
	}	
}

void gm_Player::on_menu_Art()
{
	if (b_nosong || current_art_path == "nopic" || b_stream)
		return;
	
	// strip off the filename, keep dir only
	ustring path = fullpath.substr(0, fullpath.rfind("/"));
	
	std::vector<std::string> argv;
	argv.push_back(config.Art_Viewer);
	argv.push_back(path);
	Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
}

void gm_Player::on_menu_Edit()
{
	if (b_nosong || b_stream || fullpath.empty())
		return;

	// strip off the filename, keep dir only
	ustring path = fullpath.substr(0, fullpath.rfind("/"));
	
	std::vector<std::string> argv;
	argv.push_back((config.Tag_Editor).c_str());
	argv.push_back(path);
	Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
}

void gm_Player::on_menu_Reload()
{
	if (b_nosong || b_stream || fullpath.empty())
		return;
	
	// strip off the filename, keep dir only
	ustring path = fullpath.substr(0, fullpath.rfind("/"));
	// mpd's music path must also be removed
	ustring to_update = path.substr(mpd_music_path.length(), path.length());
	mpdCom.updatefile(to_update);
}

void gm_Player::on_menu_Browse()
{
	if (b_nosong || b_stream || fullpath.empty())
		return;
	
	// strip off the filename, keep dir only
	ustring path = fullpath.substr(0, fullpath.rfind("/"));
	
	std::vector<std::string> argv;
	argv.push_back("xdg-open");
	argv.push_back(path);
	Glib::spawn_async_with_pipes(std::string("/usr/bin"),argv,Glib::SpawnFlags(0));
}

// from constructor or settingswindow
void gm_Player::on_connectrequest(ustring host, int port, ustring pswd, bool usem)
{
	if ( !mpdCom.mpd_connect(host, port, pswd, usem))
	{
		on_connectsignal(false);
		set_status(-1, "Connection failed");
		set_albumart("nopic");
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
		libraryWindow.connect_server(serverName, serverPort, serverPassword);
		settingsWindow.set_mpdcom( &mpdCom );
	}
	else
	{
		b_connected = false;
		libraryWindow.disconnect_server();
		settingsWindow.set_mpdcom( NULL );
	}
}


void gm_Player::init_vars()
{
	prev_fractint = 0;
	current_status = MPD_STATUS_STATE_UNKNOWN;
	current_volume = 0;
	songTotalTime = 0;
	b_repeat = false;
	b_random = false;
	b_single = false;
	b_consume = false;
	b_connected = false;
	b_mpdSetsVolume = false;
	b_stream = false;
	b_minmax_busy = false;
	b_playerWindow_hidden = false;	
	b_settingsWindow_hidden = false;
	b_useTrayIcon = config.use_TrayIcon;
	on_library_hide();
	b_nosong = false;
    bt_play.grab_focus();
	get_mpd_paths();
	current_art_path = "";
	file_to_update = "/";
	int delay = config.Scroller_Delay;
	if (delay < 20)
		delay = 20;
	titleScroller.set_delay( delay );
	set_fonts();
}

void gm_Player::get_mpd_paths()
{
	mpd_music_path = config.MPD_MusicPath;
	if (mpd_music_path.empty())
		mpd_music_path = mpdCom.get_musicPath();
	
	if (!mpd_music_path.empty() && mpd_music_path.rfind("/") != mpd_music_path.length()-1)
	{
			mpd_music_path += "/";
		config.MPD_MusicPath = mpd_music_path;
	}
	
	mpd_plist_path = config.MPD_PlaylistPath;
	if (mpd_plist_path.empty())
		mpd_plist_path = mpdCom.get_playlistPath();
	if (!mpd_plist_path.empty() && mpd_plist_path.rfind("/") != mpd_plist_path.length()-1)
	{
			mpd_plist_path += "/";
		config.MPD_PlaylistPath = mpd_plist_path;
	}
	
	libraryWindow.set_plistPath(mpd_plist_path);	
}

void gm_Player::set_fonts()
{
	ustring font = config.Font_Family + " bold " + into_string(config.Scroller_Fontsize);
	titleScroller.set_font(font);
	font = config.Font_Family + " " + into_string(config.TrackInfo_Fontsize);
	lb_type.modify_font((Pango::FontDescription)font.data());
	lb_khz.modify_font((Pango::FontDescription)font.data());
	lb_kbps.modify_font((Pango::FontDescription)font.data());
	font = config.Font_Family + " " + into_string(config.Time_Fontsize);
	lb_time.modify_font((Pango::FontDescription)font.data());
	lb_totaltime.modify_font((Pango::FontDescription)font.data());
	font = config.Font_Family + " " + into_string(config.Album_Fontsize);
	lb_album.modify_font((Pango::FontDescription)font.data());
	lb_year.modify_font((Pango::FontDescription)font.data());
	font = config.Font_Family + " " + into_string(config.library_Fontsize);
	libraryWindow.set_listfont(font);
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
			lb_comment.set_text("");
			set_albumart("nopic");
			// titleScroller.pause(true);
			pb_timeprogress.set_fraction(0);

            if (b_useTrayIcon)
            {
            	tIcon_ActionGroup->get_action("Pause")->set_visible(false);
            	tIcon_ActionGroup->get_action("Play")->set_visible(true);
            	trayIcon->set(pxb_st_noconn);
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

			if (b_useTrayIcon)
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
			if (b_useTrayIcon)
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
			if (b_useTrayIcon)
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
			lb_comment.set_text("");
			set_albumart("nopic");
			pb_timeprogress.set_fraction(0);

			if (b_useTrayIcon)
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


void gm_Player::set_albumart(ustring sub_path)
{
	bool b_skip_load = false;
	bool b_file_ok = false;

	if (sub_path.empty() || sub_path == "nopic")
	{
		if (current_art_path == "nopic")
			return;
		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, nopic, false);
		pxb_xfading = pxb_albmart->copy();
		img_aArt.set(pxb_albmart);
		current_art_path = "nopic";
		b_file_ok = true;
		b_skip_load = true;
	}
	
	// stream
	if ((sub_path.substr(0,5)).lowercase() == "http:")
	{
		if (current_art_path == "onair")
			return;
		pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, onair, false);
		pxb_xfading = pxb_albmart->copy();
		img_aArt.set(pxb_albmart);
		current_art_path = "onair";
		b_file_ok = true;
		b_skip_load = true;
	}

	// [1] find a suitable image
	ustring artPath = mpd_music_path + sub_path;
	
	if (!b_skip_load)
	{	
		if (!(config.AlbumArt_File).empty())
			b_file_ok = go_find_art(artPath, (config.AlbumArt_File).data());
		if (!b_file_ok)
			b_file_ok = go_find_art(artPath, "folder");
		if (!b_file_ok)
			b_file_ok = go_find_art(artPath, "album");
		if (!b_file_ok)
			b_file_ok = go_find_art(artPath, "cover");
		if (!b_file_ok)
			b_file_ok = go_find_art(artPath, "front");
		if (!b_file_ok) // anything else?
			b_file_ok = go_find_art(artPath, ".");
		
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
	}

	if (!b_file_ok)
	{
		if (current_art_path == "nopic")
			return; // nothing to do
		else
		{
			current_art_path = "nopic";
			pxb_albmart = Gdk::Pixbuf::create_from_inline(-1, nopic, false);
			pxb_xfading = pxb_albmart->copy();
			b_skip_load = true;
		}
	}

    if (!b_skip_load)
	{
		// [3] add alpha channel (segfault if we don't - even if the image has alpa)
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
		for (int h = 0; h < aArt_H; h++)
		{
			ptr_pixel = ptr_firstpixel + h * aArt_row;
			for (int w = 0; w < aArt_W; w++)
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


	// [5] show pxb_albmart
	if (config.PlayerWindow_Max)
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
	if (config.PlayerWindow_Max)
	{
		windowH = 310;
		btrowPos = 276;
	}
	else
	{
		windowH = 104;
		btrowPos = 70;
	}

	int posx = config.PlayerWindow_Xpos;
	if (posx < 0) posx = 0;
	int posy = config.PlayerWindow_Ypos;
	if (posy < 0) posy = 0;
	move(posx, posy);
	
	// window properties
    set_title("Guimup");
    set_size_request(340, windowH);
    set_resizable(false);
    set_border_width(4);

	Glib::RefPtr<Gdk::Pixbuf> pxb;
	
    std::list< Glib::RefPtr<Gdk::Pixbuf> > window_icons;
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon16, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon24, false);
    window_icons.push_back(pxb);
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_icon32, false);
    window_icons.push_back(pxb);
    set_icon_list(window_icons);

	// widget properties
	eb_background.set_size_request(332,50);  // only used for its background color
	eb_background.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#D3E4F0"));
	vb_lcdisplay.set_size_request(332,50);
	fr_lcdisplay.set_shadow_type(Gtk::SHADOW_IN);
	eb_scroller.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#D3E4F0"));
	eb_scroller.set_events(Gdk::LEAVE_NOTIFY_MASK | Gdk::ENTER_NOTIFY_MASK);
	eb_scroller.set_size_request(328,26);
	titleScroller.set_bg("#D3E4F0");
	titleScroller.set_fg("#1E3C69");
	titleScroller.set_size_request(328,26); // eb_background minus frame-border
	titleScroller.set_double_buffered(true);
	eb_time.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#D3E4F0"));
	eb_time.set_events(Gdk::BUTTON_PRESS_MASK);
    hb_trackdata.set_homogeneous(false);
    lb_type.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
    lb_type.set_alignment(0, 0.5);
    lb_type.set_size_request(40,-1);
	lb_type.set_use_markup(true);
    lb_kbps.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
    lb_kbps.set_alignment(1, 0.5); 
	lb_kbps.set_use_markup(true);
	lb_kbps.set_size_request(64,-1);
    lb_khz.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
    lb_khz.set_alignment(0, 0.5);
	lb_khz.set_use_markup(true);
    lb_time.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
    lb_time.set_alignment(1, 0.5);
    lb_time.set_use_markup(true);
	if (config.show_ToolTips)
		lb_time.set_tooltip_text("Click to toggle +/-");
    lb_totaltime.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
    lb_totaltime.set_alignment(0, 0.5);
    lb_totaltime.set_use_markup(true);	
    pb_timeprogress.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#FFFFFF"));
	pb_timeprogress.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("#1E3C69"));
    pb_timeprogress.set_size_request(332,8);
	pb_timeprogress.set_events(Gdk::BUTTON_PRESS_MASK);
    hb_center.set_size_request(332,200);
    hb_center.set_spacing(12);
	img_aArt.set_tooltip_text("Right-click for menu");
    img_aArt.set_size_request(200,200);
	img_aArt.set_alignment(0.5, 0.5);
	img_aArt.set_double_buffered(true);
    vb_center_right.set_homogeneous(false);
    vb_center_right.set_spacing(4);
    lb_album.set_size_request(120,-1); // makes lines wrap
    lb_album.set_alignment(0, 0);
    lb_album.set_use_markup(true);
    lb_album.set_single_line_mode(false);
    lb_album.set_line_wrap(true);
    lb_year.set_alignment(0, 0);
    lb_year.set_use_markup(true);
	lb_comment.set_use_markup(true);
	lb_comment.set_alignment(0, 0);
	lb_comment.set_size_request(120,-1); // makes lines wrap
	lb_comment.set_line_wrap(true);
    hsc_volume.set_show_fill_level(true);
    hsc_volume.set_draw_value(false);
    hsc_volume.set_increments(5, 10);
    hsc_volume.set_range(0,100); // mpd also reports 0 - 100
	hsc_volume.add_events(Gdk::SCROLL_MASK);
	//hsc_volume.set_size_request(-1,20);
    hb_buttonrow.set_size_request(332,26);
    hb_buttonrow.set_spacing(5);
    hb_buttonrow.set_homogeneous(false);
    img_statusled.set_size_request(45,26);
	/* Dimensions: see footnote [2] */
	
	// inline graphics
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
	if (config.show_ToolTips)
		bt_plst.set_tooltip_text("Library & Playlist");
    	// sizer
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_sizer, false);
    img_mn_sizer.set(pxb);
    bt_sizr.set_image(img_mn_sizer);
	if (config.show_ToolTips)
		bt_sizr.set_tooltip_text("Mini-Maxi mode");
    	// config
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_config, false);
    img_mn_config.set(pxb);
    bt_conf.set_image(img_mn_config);
	if (config.show_ToolTips)
		bt_conf.set_tooltip_text("Settings");
   	// status led
	if (config.show_ToolTips)
		img_statusled.set_tooltip_text("Status led");
    pxb_led_playing = Gdk::Pixbuf::create_from_inline(-1, led_playing, false);
    pxb_led_noconn = Gdk::Pixbuf::create_from_inline(-1, led_noconn, false);
    pxb_led_paused = Gdk::Pixbuf::create_from_inline(-1, led_paused, false);
    pxb_led_stopped = Gdk::Pixbuf::create_from_inline(-1, led_stopped, false);
	
    // put widgets together
	add(eb_all); // main container: 4 rows
	eb_all.add(fx_main);
    fx_main.put(eb_background, 0, 0);
    	// row 1
    eb_background.add(fr_lcdisplay);
        fr_lcdisplay.add(vb_lcdisplay); // 2 rows
	vb_lcdisplay.add(eb_scroller);
				eb_scroller.add(titleScroller);
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
			vb_center_right.pack_start(lb_year, false, false, 0); // pushes the others up
			vb_center_right.pack_start(lb_comment, true, true, 0);
			vb_center_right.pack_end(hsc_volume, false, false, 0);
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
	if (config.PlayerWindow_Max)
    	eb_all.show_all();
	else
	{
		eb_all.show();
		fx_main.show();
		eb_background.show_all();
		pb_timeprogress.show();
		hb_buttonrow.show_all();		
	}

	if (b_useTrayIcon)
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

	hsc_volume.signal_value_changed().connect(sigc::bind<-1, int>(
              sigc::mem_fun(*this, &gm_Player::on_signal), ID_volm));
	
	settingsWindow.signal_pleaseconnect.connect(sigc::mem_fun(*this, &gm_Player::on_connectrequest) );
	settingsWindow.signal_settingssaved.connect(sigc::mem_fun(*this, &gm_Player::on_settingssaved) );
	settingsWindow.signal_applyfonts.connect(sigc::mem_fun(*this, &gm_Player::set_fonts) );
	
	libraryWindow.signal_hide.connect(sigc::mem_fun(*this, &gm_Player::on_library_hide) );

	// return bool
	eb_time.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_timeClicked) );
	eb_all.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_playerRClicked) );
	eb_scroller.signal_enter_notify_event().connect(sigc::mem_fun(*this, &gm_Player::on_enter_scroller) );
	eb_scroller.signal_leave_notify_event().connect(sigc::mem_fun(*this, &gm_Player::on_leave_scroller) );
	pb_timeprogress.signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_progressClicked) );
	hsc_volume.signal_scroll_event().connect(sigc::mem_fun(*this, &gm_Player::on_tray_or_vol_Scrolled) ); 
}


void gm_Player::on_library_hide()
{
	if (!config.toggle_Player)
		b_libraryWindow_hidden = true;
}

bool gm_Player::on_enter_scroller(GdkEventCrossing *e)
{
	titleScroller.on_mouse_enter();
	return false;
}
bool gm_Player::on_leave_scroller(GdkEventCrossing *e)
{
	titleScroller.on_mouse_leave();
	return false;
}


void gm_Player::on_signal_host_port_pwd(ustring host, int port, ustring pwd)
{
	serverName = host;
	serverPort = port;
	serverPassword = pwd;
}


bool gm_Player::on_playerRClicked(GdkEventButton* event)
{
	if( (event->button == 3) )
	{
		rClickMenu.popup(event->button, event->time);
		return true;  // buck stops here
	}
	else
		return false;
}

bool gm_Player::on_timeClicked(GdkEventButton* event)
{
	if( event->button != 1)
		return false;
	
	if (current_status > 0) // STOP, PLAY or PAUSE
		config.use_TimeRemaining = !config.use_TimeRemaining;
	return true;
}


bool gm_Player::on_progressClicked(GdkEventButton* event)
{
	if( event->button != 1)
		return false;
	
	if (current_status != MPD_STATUS_STATE_PLAY && current_status != MPD_STATUS_STATE_PAUSE)
		return false;

	double fraction = ((*event).x)/(double)330;
	pb_timeprogress.set_fraction(fraction);

	int percent = (int)(fraction * songTotalTime);
	mpdCom.set_seek(percent);
	return true;
}

// apply new albumart, fonts etc.
void gm_Player::on_settingssaved()
{
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
			// some library play a bit too long (honestly)
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
			if (config.use_TimeRemaining)
				timenow = songTotalTime - timenow;
			
			ustring timestring = "";
			if (config.use_TimeRemaining)
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
		b_random = sInfo.mpd_random;
		libraryWindow.set_random(b_random);
	}
	
	if (sInfo.mpd_repeat != b_repeat)
	{
		b_repeat = sInfo.mpd_repeat;
		libraryWindow.set_repeat(b_repeat);
	}
	
	if (sInfo.mpd_single != b_single)
	{
		b_single = sInfo.mpd_single;
		libraryWindow.set_single(b_single);
	}
	
	if (sInfo.mpd_consume != b_consume)
	{
		b_consume = sInfo.mpd_consume;
		libraryWindow.set_consume(b_consume);
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
			titleScroller.set_pause(true);
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
					titleScroller.set_pause(true);
					mpdCom.pause();
					break;
            	}
 
				case MPD_STATUS_STATE_PAUSE:
            	{
					set_status(MPD_STATUS_STATE_PLAY);
					titleScroller.set_pause(false);
					mpdCom.resume();
					break;
            	}

				case MPD_STATUS_STATE_STOP:
            	{
                	set_status(MPD_STATUS_STATE_PLAY);
					titleScroller.set_pause(false);
					mpdCom.play();
					break;
            	}

				case MPD_STATUS_STATE_UNKNOWN:
            	{
                	set_status(MPD_STATUS_STATE_PLAY);
					titleScroller.set_pause(false);
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
				
		case ID_sets:
		{
			if (settingsWindow.is_visible())
			{
				settingsWindow.get_position(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);
				settingsWindow.hide();
			}
			else
			{
				settingsWindow.move(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);
				settingsWindow.load_and_show();
				settingsWindow.present();
			}
			break;
		}

		case ID_plst:

		{   
			if (libraryWindow.is_visible())
			{
				libraryWindow.get_position(config.libraryWindow_Xpos, config.libraryWindow_Ypos);
				libraryWindow.hide();
				on_library_hide();
			}
			else
			{
				libraryWindow.move(config.libraryWindow_Xpos, config.libraryWindow_Ypos);
				libraryWindow.show();
				libraryWindow.present();
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

            if (config.PlayerWindow_Max)
            {
                b_minmax_xfade = true;
				alphacycler = 255;
                minmaxtimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Player::miniMice), 12);
                config.PlayerWindow_Max = false;
            }
            else
            {
                b_minmax_xfade = false;
				alphacycler = 0;
                minmaxtimer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &gm_Player::maxiMice), 12);
                config.PlayerWindow_Max = true;
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

void gm_Player::on_set_libtoggle()
{
	config.toggle_Library = !config.toggle_Library;
	
	if (!config.toggle_Library)
	{
		if (!config.toggle_Player)
		{
			TogglePlayer->set_active(true);
			config.toggle_Player = true;
		}
		
		if (!get_window()->is_visible())
			b_playerWindow_hidden = true;
	}

}

void gm_Player::on_set_playertoggle()
{
	config.toggle_Player = !config.toggle_Player;
	
	if (!config.toggle_Player)
	{
		if (!config.toggle_Library)
		{
			ToggleLibrary->set_active(true);
			config.toggle_Library = true;
		}
		
		if (!libraryWindow.is_visible())
			b_libraryWindow_hidden = true;
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
	Glib::RefPtr<Gdk::Pixbuf> pxb;
	
    pxb = Gdk::Pixbuf::create_from_inline(-1, mn_playlist, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset1;
    gm_Iset1.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::INDEX, gm_Iset1);

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_exit, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset2;
    gm_Iset2.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::QUIT, gm_Iset2);

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_prev, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset3;
    gm_Iset3.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_PREVIOUS, gm_Iset3);

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_stop, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset4;
    gm_Iset4.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_STOP, gm_Iset4);

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_play, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset5;
    gm_Iset5.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_PLAY, gm_Iset5);

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_pause, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset6;
    gm_Iset6.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_PAUSE, gm_Iset6);

    pxb = Gdk::Pixbuf::create_from_inline(-1, st_next, false);
    gm_Isrc.set_pixbuf(pxb);
    Gtk::IconSet gm_Iset7;
    gm_Iset7.add_source (gm_Isrc);
    gm_Ifact->add(Gtk::Stock::MEDIA_NEXT, gm_Iset7);

    gm_Ifact->add_default();

    // Setting up the UIManager:
    tIcon_ActionGroup = Gtk::ActionGroup::create();

	if (config.toggle_Player)
		TogglePlayer = Gtk::ToggleAction::create("TogglePlayer", "Restore Player", "", true);
	else
		TogglePlayer = Gtk::ToggleAction::create("TogglePlayer", "Restore Player", "", false);		
	TogglePlayer->signal_toggled().connect(sigc::mem_fun(*this, &gm_Player::on_set_playertoggle) );
	tIcon_ActionGroup->add(TogglePlayer);

	if (config.toggle_Library)
		ToggleLibrary = Gtk::ToggleAction::create("ToggleLibrary", "Restore Library", "", true);
	else
		ToggleLibrary = Gtk::ToggleAction::create("ToggleLibrary", "Restore Library", "", false);		
	ToggleLibrary->signal_toggled().connect(sigc::mem_fun(*this, &gm_Player::on_set_libtoggle) );
	tIcon_ActionGroup->add(ToggleLibrary);
	
    tIcon_ActionGroup->add(Gtk::Action::create("Prev", Gtk::Stock::MEDIA_PREVIOUS, "Previous"),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_prev) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Stop",Gtk::Stock::MEDIA_STOP, "Stop"),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_stop) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Pause",Gtk::Stock::MEDIA_PAUSE, "Pause"),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_play) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Play",Gtk::Stock::MEDIA_PLAY, "Play"),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_play) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Next",Gtk::Stock::MEDIA_NEXT, "Next"),
    sigc::bind(sigc::mem_fun(*this, &gm_Player::on_signal), (int)ID_next) );

    tIcon_ActionGroup->add(
    Gtk::Action::create("Quit", Gtk::Stock::QUIT, "Quit Guimup"),
    sigc::mem_fun(*this, &gm_Player::hide) );

    tIcon_UIManager = Gtk::UIManager::create();
    tIcon_UIManager->insert_action_group(tIcon_ActionGroup);

    Glib::ustring ui_info =
    "<ui>"
    "  <popup name='Popup'>"
  "    <menuitem action='TogglePlayer' />"	
  "    <menuitem action='ToggleLibrary' />"			
	"	<separator/>"
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
	trayIcon = gm_trayIcon::create();
	// trayIcon = Glib::RefPtr<gm_trayicon>(new gm_trayicon());
	trayIcon->set_tooltip("Guimup");
	
	// StatusIcon signals
    //void
    trayIcon->signal_activate().connect(sigc::mem_fun(*this, &gm_Player::tIcon_toggle_hide) );
    trayIcon->signal_popup_menu().connect(sigc::mem_fun(*this, &gm_Player::tIcon_on_popup) );
    //bool
	trayIcon->signal_scroll_event().connect(sigc::mem_fun(*this, &gm_Player::on_tray_or_vol_Scrolled) ); 		// gtkmm 2.16	!
	trayIcon->signal_button_press_event().connect(sigc::mem_fun(*this, &gm_Player::on_trayClicked) ); 			// gtkmm 2.16   !	
}


bool gm_Player::on_trayClicked(GdkEventButton* eb)
{
	if (eb->button == 2)
		on_signal(ID_play);
	return false;
}

bool gm_Player::on_tray_or_vol_Scrolled(GdkEventScroll* es)
{
	if (!b_connected)
		return false;
	
	if (es->direction == GDK_SCROLL_UP)
	{
		mpdCom.volume_up(10);
	}
	else
	{
		mpdCom.volume_down(10);
	}
    return true;
}


void gm_Player::tIcon_toggle_hide()
{
	if (config.toggle_Player)   /////// Player Rules ///////////////////////////
	{
		// [1] Player is visible but minimized (or on another desktop): present
		if (get_window() != NULL && get_window()->get_state() == Gdk::WINDOW_STATE_ICONIFIED)
		{
			// first the library
			if (libraryWindow.is_visible() && !config.toggle_Library)
			{
				libraryWindow.get_position(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
				libraryWindow.hide();
				b_libraryWindow_hidden = true;
			}
			 else
			if (libraryWindow.get_window() != NULL && libraryWindow.get_window()->get_state() == Gdk::WINDOW_STATE_ICONIFIED)
			{
				libraryWindow.present();;
			}
			else
			if (b_libraryWindow_hidden && config.toggle_Library)
			{
				libraryWindow.show();
				libraryWindow.move(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
				libraryWindow.present();
				b_libraryWindow_hidden = false;
			}

			// settingswindow: always in sync with the player
			if (settingsWindow.get_window() != NULL && settingsWindow.get_window()->get_state() == Gdk::WINDOW_STATE_ICONIFIED)
			{
				settingsWindow.present();
			}
			else
			if (b_settingsWindow_hidden)
			{
				settingsWindow.show();
				settingsWindow.move(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);			
				settingsWindow.present();
				b_settingsWindow_hidden = false;
			}
			
			// finally: the player on top
			move(config.PlayerWindow_Xpos, config.PlayerWindow_Ypos);
			present();
			
			return;
		}
		
		// [2] Player is visible, not minimized (and not another desktop): hide all
		if ( get_window()->is_visible() )
		{
			get_position(config.PlayerWindow_Xpos, config.PlayerWindow_Ypos);
			get_window()->hide();
			b_playerWindow_hidden = true;
			// settings
			if (settingsWindow.is_visible())
			{
				settingsWindow.get_position(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);
				settingsWindow.hide();
				b_settingsWindow_hidden = true;
			}
			// library
			if (libraryWindow.is_visible())
			{
				libraryWindow.get_position(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
				libraryWindow.hide();
				b_libraryWindow_hidden = true;
			}
			return;
		}
		
		// [3] Player is hidden: show 	
		if (b_playerWindow_hidden)
		{
			// first the library
			if (libraryWindow.is_visible() && !config.toggle_Library)
			{
				libraryWindow.get_position(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
				libraryWindow.hide();
				b_libraryWindow_hidden = true;
			}
			else
			if (libraryWindow.get_window() != NULL && libraryWindow.get_window()->get_state() == Gdk::WINDOW_STATE_ICONIFIED )
			{
					libraryWindow.present();
			}
			else
			if (b_libraryWindow_hidden && config.toggle_Library)
			{
				libraryWindow.show();
				libraryWindow.move(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
				libraryWindow.present();
				b_libraryWindow_hidden = false;
			}
			
			// settingswindow (cannnot be minimized here)
			if (b_settingsWindow_hidden)
			{
				settingsWindow.show();
				settingsWindow.move(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);			
				settingsWindow.present();
				b_settingsWindow_hidden = false;
			}
			// finally: the player on top
			move(config.PlayerWindow_Xpos, config.PlayerWindow_Ypos);
			b_playerWindow_hidden = false;
			present();
			return;
		}		
	}
	else	              /////////////////// Library Rules ////////////////////
	{
		// config.toggle_Player == false: player and settings must be hidden
		if ( get_window()->is_visible() )
		{
			get_position(config.PlayerWindow_Xpos, config.PlayerWindow_Ypos);
			get_window()->hide();
			b_playerWindow_hidden = true;
			// settings
			if (settingsWindow.is_visible())
			{
				settingsWindow.get_position(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);
				settingsWindow.hide();
				b_settingsWindow_hidden = true;
			}
		}
		
		// library (config.toggle_Library == true)
		if (libraryWindow.get_window() != NULL && libraryWindow.get_window()->get_state() == Gdk::WINDOW_STATE_ICONIFIED)
				libraryWindow.present();
		else			
		if ( libraryWindow.is_visible() )
		{
			libraryWindow.get_position(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
			libraryWindow.hide();
			b_libraryWindow_hidden = true;
		}
		else // show, even if !b_libraryWindow_hidden
		{
			libraryWindow.show();
			libraryWindow.move(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
			libraryWindow.present();
			b_libraryWindow_hidden = false;
		}
	}
}


bool gm_Player::on_delete_event(GdkEventAny* event)
{
	if(b_useTrayIcon)
	{
		get_position(config.PlayerWindow_Xpos, config.PlayerWindow_Ypos);
    	tIcon_toggle_hide();
    	return true; // ignore
	}
	else
		return false; // accept // 'get_position' is done in on_hide()
}


// window calls this on quit
void gm_Player::on_hide()
{
	// player window
	if (is_visible())
		get_position(config.PlayerWindow_Xpos, config.PlayerWindow_Ypos);
	// library window
	if (libraryWindow.is_visible())
		libraryWindow.get_position(config.libraryWindow_Xpos,config.libraryWindow_Ypos);
	libraryWindow.get_trSettings();
	// settings window
	if (settingsWindow.is_visible())
		settingsWindow.get_position(config.SettingsWindow_Xpos, config.SettingsWindow_Ypos);
	// finally
	config.save_config();
	if (config.QuitMPD_onQuit)
	{
		cout << "executing: " << config.MPD_onQuit_command << endl;
		system ( (config.MPD_onQuit_command).data() );
	}
	cout << "Disconnecting mpd" << endl;
	mpdCom.mpd_disconnect();
	cout << "Goodbye!" << endl;
}


// Show the Tray-Icon popup menu:
void gm_Player::tIcon_on_popup(guint button, guint32 time)
{
    Gtk::Menu* pMenu = static_cast<Gtk::Menu*>(
    tIcon_UIManager->get_widget("/Popup") );

    trayIcon->popup_menu_at_position(*pMenu, button, time);
}


void gm_Player::on_newSong( songInfo newSong )
{
	fullpath = "";

	if (newSong.nosong)
	{
		if  (b_nosong)
			return;
		
		cout << "No song" << endl;
		img_aArt.set_tooltip_text("No song");
		
		Gtk::Menu::MenuList& menulist = rClickMenu.items();
		for (int i = 0; i < 4; i++)
			menulist[i].set_sensitive(false);
		pb_timeprogress.set_fraction(0.0);
		b_nosong = true;
		return;
	}

	b_nosong = false;
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

	// streams start with "http"
	if (((path.substr(0,5)).lowercase()) == "http:")
	{
		set_albumart(path);
		lb_type.set_markup("URL");
		lb_album.set_markup("Audio stream");
		lb_year.set_text("");
		lb_comment.set_text("");
		// note: newSong->artist is NULL !
		ustring artist_title = "no info";
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
		if (b_useTrayIcon)
			trayIcon->set_tooltip(artist_title);
		
		if (!b_stream)
		{
			Gtk::Menu::MenuList& menulist = rClickMenu.items();
			for (int i = 0; i < 4; i++)
				menulist[i].set_sensitive(false);
		}
		
		b_stream = true;
		pb_timeprogress.set_fraction(0.0);
		img_aArt.set_tooltip_text("Streaming media");
		return;
	}
	else
		b_stream = false;
	
	// album art
	set_albumart(path);

	// extension
	if (fname.empty())
	{
		fname = "File name?";
		lb_type.set_text("?");
	}
	else
	{
		ustring xtension = (fname.substr(fname.rfind(".")+1, fname.length()) ).uppercase();
		lb_type.set_markup(xtension);
	}

	// artist
	ustring artist = "?";
	if (!newSong.artist.empty())
		artist = newSong.artist;
	
	// title
	ustring title = "?";
	if (!newSong.title.empty())	
		title = newSong.title;

	if (artist.empty() && title.empty())
	{
		titleScroller.set_title(fname);
		if (b_useTrayIcon)
			trayIcon->set_tooltip(fname);
	}
	else
	{
		titleScroller.set_title(artist, title);
		if (b_useTrayIcon)
			trayIcon->set_tooltip_markup( " <b>" + escapeString(artist) + "</b> &#10; <i>" + escapeString(title) + "</i> "); // gtkmm 2.16 // escape for markup!!
	}
	
	// Now we can (and must) escape for the tagInfo dialog
	artist = escapeString(artist);
	title = escapeString(title);	
	
	//album
	ustring album = "";
	if (!newSong.album.empty())
	{
		album = newSong.album;
		album = escapeString(album);
	}
    lb_album.set_markup("<b>" + album + "</b>");
	
	//track
	ustring track = "";
	if (!newSong.track.empty())
		track = newSong.track;
	if (track.size() == 1)
		track = "0" + track;

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

	// comment
	ustring comment = "";
	if (!newSong.comment.empty())
	{
		comment = newSong.comment;
		comment = escapeString(comment);
	}

	lb_comment.set_markup("<small>" + comment + "</small>");
	
	fullpath = mpd_music_path + newSong.file;
	img_aArt.set_tooltip_text("Right-click for menu");
	
	Gtk::Menu::MenuList& menulist = rClickMenu.items();
	if (current_art_path == "nopic")
		menulist[0].set_sensitive(false);
	else
		menulist[0].set_sensitive(true);		

	menulist[1].set_sensitive(true);		
	menulist[2].set_sensitive(true);
	menulist[3].set_sensitive(true);
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
