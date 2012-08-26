/*
 *  gm_app.h
 *  GUIMUP application
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

#ifndef GM_APPLICATION_H
#define GM_APPLICATION_H

#include <gtkmm/application.h>
#include <gm_player.h>
#include <iostream>
    using std::cout;
    using std::endl;
#include <glibmm/ustring.h>
    using Glib::ustring;


class gm_application: public Gtk::Application
{
	
	public:
		gm_application();
		void create_window();
	
	protected:
	  //Overrides of default signal handlers:
	  virtual void on_activate();
	  virtual int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line);

	private:
		gm_Player *the_player;
		void on_window_hide(gm_Player*);
};

#endif // GM_APPLICATION_H
