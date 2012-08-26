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
#include <unique/unique.h>
#include "gm_player.h"
#include <iostream>
    using std::cout;
    using std::endl;

gm_Player *the_player;


/*  
	Here we process the URIs passed to a next instance.

	commands are used as follows:
 	UNIQUE_ACTIVATE > only present player
  	UNIQUE_NEW      > play URIs in new list (and present player)
  	UNIQUE_OPEN     > append URis to list (and present player)
*/
static UniqueResponse on_message_received(UniqueApp* app, UniqueCommand cmd, UniqueMessageData* message)
{
	UniqueResponse resp = UNIQUE_RESPONSE_OK;
	if (cmd == UNIQUE_NEW || cmd == UNIQUE_OPEN)
	{	
		std::vector<ustring> urilist;
		gchar **msg_uris = unique_message_data_get_uris(message);
		gchar *uri = NULL;
		int i = 0;
		if (cmd != UNIQUE_ACTIVATE && msg_uris != NULL)
		{
			uri = msg_uris[i];
			while (uri != NULL)
			{
				if (uri && Glib::str_has_prefix(uri,"file:"))
					urilist.push_back(uri);
				i++;
				uri = msg_uris[i];
			}

			bool b_newlist = false;
			if(cmd == UNIQUE_NEW)
				b_newlist = true;
			
			the_player->on_open_with_request(urilist, b_newlist);
			
			g_strfreev(msg_uris);
		}
		else
			resp = UNIQUE_RESPONSE_FAIL;
	}

	the_player->present();
	return resp;
}


int main(int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	UniqueCommand command = UNIQUE_ACTIVATE;
	bool b_allow_multiple = false;
	int uricount = 0;
	gchar **uris;
	
	for (int a = 1; a < argc; ++a)
	{
		ustring msg = argv[a];
		
		if ( msg == "-p" || msg == "-play")
		{
			command = UNIQUE_NEW;
		}
		if ( msg == "-a" || msg == "-append")
		{
			command = UNIQUE_OPEN;
		}		
		else		
		if ( msg == "-i" || msg == "-instance")
		{
		    b_allow_multiple = true;
		}
		else
		if (msg == "-h" || msg == "-help")
		{
			cout << "Guimup version 0.3.0" << endl;
			cout << "© 2012 Johan Spee <guimup@coonsden.com>" << endl; 
			cout << "This program is licensed by the GPL and distributed in the hope that it will be useful, but without any warranty." << endl;
		    cout << endl << "Command line parameters:" << endl;
		    cout << " -h(elp)           show this information and exit" << endl;
		    cout << " -i(nstance)       force a new instance" << endl;
			cout << " -p(lay) %U        play files in list of uri" << endl;
			cout << " -a(ppend) %U      append files in list of uri" << endl;
			cout << "  %U               default: append files in list of uri" << endl << endl;;

			return 0;
		}
		else
		// URL
		if (str_has_prefix(msg,"file:"))
			uricount++;
	}
	
	if (uricount > 0 && command == UNIQUE_ACTIVATE) // only uri, no -a or -p
		command = UNIQUE_OPEN;
	
	if (uricount > 0)
		uris = argv;

	cout << "Guimup: starting up" << endl;
	UniqueApp *app;
  	app = unique_app_new  ("coonsden.guimup", NULL);
	if (unique_app_is_running (app))
	{
		cout << "Previous instance detected" << endl;
		if (!b_allow_multiple)
		{
      		UniqueResponse response = UNIQUE_RESPONSE_FAIL; 
      		
      		if (command == UNIQUE_ACTIVATE)
			{
        		response = unique_app_send_message (app, command, NULL);
			}
      		else
        	{
				if (uricount > 0)
				{
		      		UniqueMessageData *message = unique_message_data_new ();
					unique_message_data_set_uris(message, argv);
					response = unique_app_send_message (app, command, message);
		      		unique_message_data_free (message);
				}
				else
					response = unique_app_send_message (app, command, NULL);
			}

			g_object_unref (app);

			if (response != UNIQUE_RESPONSE_OK)
				cout << "Previous instance rejected parameter(s)." << endl;

			cout << "Use '-i' parameter to allow a new instance" << endl;
			cout << "Goodbye!" << endl;
			return 0;
			
		}
		else
		    cout << "Allowing new instance." << endl;	
	}

	the_player =  new gm_Player();	
	g_signal_connect (app, "message-received", G_CALLBACK (on_message_received), NULL);


/*  
	Here we process the URIs passed to this instance.

	commands are used as follows:
  	UNIQUE_NEW      > play in new list,
  	UNIQUE_OPEN     > append to list
*/
	if (uricount > 0 && uris != NULL)
	{
		std::vector<ustring> urilist;
		gchar *uri = NULL;

		int i = 0;
		uri = uris[i];
		while (i < argc)
		{
			if (uri && Glib::str_has_prefix(uri,"file:"))
				urilist.push_back(uri);
			i++;
			uri = uris[i];
		}
		
		bool b_newlist = false;
		if(command == UNIQUE_NEW)
		b_newlist = true;
		
		the_player->on_open_with_request(urilist, b_newlist);
	}
	
	Gtk::Main::run(*the_player);
	g_object_unref (app);
	return 0;
}
