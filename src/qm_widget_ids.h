/*
*  qm_widget_ids.h
*  QUIMUP ID's for widgets and tray-menu actions
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

#ifndef QM_WIDGET_ID_H
#define QM_WIDGET_ID_H

enum // ID # for widgets and tray-menu actions
{
	ID_prev, // previous track button
	ID_stop, // stop button
	ID_play, // play/pause button
	ID_next, // next track button
	ID_plst, // playlist button
	ID_mmax, // mini/maxi mode button
	ID_sets, // settings button
	ID_rept, // repeat toggle button
	ID_rand, // random toggle button
	ID_info, // song info button
	ID_volm, // volume slider
	ID_pgrs, // progress bar
	ID_time, // time +- clicklabel
	ID_sled, // statusled clicked
	ID_exit, // 'quit' from systray
	ID_nocn, // 'no connection' for systray
	ID_paus, // 'paused' state for systray
	ID_idle  // 'no song' state for systray
};

#endif // QM_WIDGET_ID_H
