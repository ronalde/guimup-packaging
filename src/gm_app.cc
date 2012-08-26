/*
 *  gm_app.cc
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


#include <gm_app.h>


gm_application::gm_application()
: Gtk::Application("org.coonsden.guimup", Gio::APPLICATION_HANDLES_COMMAND_LINE)
{
}


void gm_application::create_window()
{
	the_player =  new gm_Player();
	add_window(*the_player);

	//Delete the window when it is hidden.
	the_player->signal_hide().connect(sigc::bind<gm_Player*>(sigc::mem_fun(*this,&gm_application::on_window_hide), the_player));
	
	the_player->show();
}


int gm_application::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
  /* 
	 	Handle command-line arguments that were passed either to the
	 	main (first) instance or to subsequent instances.
  		Note that this is done in the main (first) instance.
  */
  int argc = 0;
  char** argv =	command_line->get_arguments(argc);

  bool b_newlist = true;
  int uricount = 0;
  std::vector<ustring> urilist;
	
	if(argc > 1)
	{
	  
		for (int a = 1; a < argc; ++a)
		{
			ustring msg = argv[a];

			if ( msg == "-p" || msg == "-play")
			{
				b_newlist = true; // play (default)
			}
			if ( msg == "-a" || msg == "-append")
			{
				b_newlist = false; //append
			}		
			else
			// URL
			if (str_has_prefix(msg,"file:"))
			{
				urilist.push_back(argv[a]);
				uricount++;
			}
			else
			// plain /path/to/file
			if (str_has_prefix(msg,"/"))
			{
				// make it look like a file url
				ustring uri = "file://";
				uri += argv[a];
				urilist.push_back(uri);
				uricount++;
			}	
		}

		if (uricount && the_player)
		{
			cout << "List of URIs received" << endl;
			the_player->on_open_with_request(urilist, b_newlist);
		}
		  
	}

	return EXIT_SUCCESS;
}


void gm_application::on_window_hide(gm_Player* window)
{
    delete window;
}


void gm_application::on_activate()
{
	if (the_player != NULL)
	{
  		the_player->activate_from_remote();
	}
	else
		cout << "Can't show player (Window not created?)" << endl;	
}

