/*
*  qm_core.h
*  QUIMUP c class
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

///// See notes in qm_core.cpp

#ifndef QM_CORE_H
#define QM_CORE_H

#include <QMainWindow>
#include "qm_player.h"

class qm_core : public QMainWindow
{
	Q_OBJECT
public:
	qm_core();
	virtual ~qm_core();
	
	qm_player *player;	
	
public slots:
	void on_message_from_2nd_instance(const QString&);	
};

#endif // QM_CORE_H
