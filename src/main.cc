/*
 *  main.cc
 *  GUIMUP main source file
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

#include <gtkmm/main.h>
#include <glibmm/stringutils.h>
#include <gm_app.h>
#include <iostream>
    using std::cout;
    using std::endl;
#include <glibmm/ustring.h>
    using Glib::ustring;


int main(int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	bool b_allow_instance = false;

	if (argc > 1) // when no args are given argc == 1!
	{
		ustring msg = "";
		if (argv[1] != NULL)
			msg = argv[1];
		
		if (msg == "-h" || msg == "-help")
		{
			cout << "Guimup version 0.3.3" << endl;
			cout << "© 2012 Johan Spee <guimup@coonsden.com>" << endl; 
			cout << "This program is licensed by the GPL and distributed in the hope that it will be useful, but without any warranty." << endl;
			cout << endl << "Command line parameters:" << endl;
			cout << " -h(elp)           show this help information - and exit" << endl;
			cout << " -i(nstance)       run in multiple instance (non-unique) mode" << endl;
			cout << " -p(lay) %U (%F)   play files in list of uri (or paths) in new playlist" << endl;
			cout << " -a(ppend) %U (%F) append files in list of uri (or paths) to playlist" << endl;
			cout << "  %U (%F)          default: play files in new playlist" << endl << endl;
			
			return 0;
		}
	

		for (int a = 1; a < argc; ++a)
		{
			if (argv[a] != NULL)
				msg = argv[a];
			
			if ( msg == "-i" || msg == "-instance")
			{
				b_allow_instance = true;
				break;
			}
		}
	}
	
	Glib::RefPtr<gm_application> app;
	app =  Glib::RefPtr<gm_application>(new gm_application());	

 
/* 		APPLICATION_NON_UNIQUE
	   	* cmd args will not be passed on to first instance.
		* app->is_remote() will return false.                  */
	if (b_allow_instance)
	{
		app->set_flags(Gio::APPLICATION_NON_UNIQUE | Gio::APPLICATION_HANDLES_COMMAND_LINE);
		cout << "Guimup: starting up in multiple instance (non-unique) mode" << endl;
	}
	else
		cout << "Guimup: starting up in single instance (unique) mode" << endl;
	
	if (app->register_application())
	{
		// if this is the 1st instance create player window:
		if (b_allow_instance || !app->is_remote())
			app->create_window();
		else
		{
			// current instance will exit since no window is created.
			cout << "Guimup is already running: closing this instance" << endl;
			// present 1st instance:
			app->activate();
		}
	}
	else
	{
		cout << "Error: Application registration failed" << endl;
		return 0;
	}

	const int status = app->run(argc, argv);
	return status;
}
