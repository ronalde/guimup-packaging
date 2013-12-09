/*
*  qm_core.cpp
*  QUIMUP core class
*  © 2008-2013 Johan Spee
*
*  This file is part of Quimup
*
*  QUIMUP is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  QUIMUP is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see http://www.gnu.org/licenses/.
*/


/*
	***************************************************************************
	The only purpose of the 'core' is to provide KDE with a main program window
	(albeit invisible) that can be closed on logout. 
	
	If the player window was the main window it would have to ignore() the 
	close-event to allow close-to-tray behavior.
	This would also ignore the close-event evoked by KDE on shutdown, which
	would abort the logout procedure.
	The 'core' will accept the close-event, and it's child (the player) will be
	closed with it.
	The player can close itself by calling qApp->quit().
	***************************************************************************
*/

#include "qm_core.h"

qm_core::qm_core ()
{
    player = new qm_player();
    player->setAttribute(Qt::WA_QuitOnClose, false);
    player->show();
}

// called from main.cpp
void qm_core::on_message_from_2nd_instance(const QString & msg)
{
    if ( msg.startsWith("file://") || msg.startsWith("-play:file:"))
    {
        player->wakeup_call(true);
        player->browser_window->plist_view->on_open_with_request(msg);
    }
    else
        player->wakeup_call(false);
}

qm_core::~qm_core()
{
	player->on_shudown();
}
