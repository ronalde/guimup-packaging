/*
*  qm_streamloader.h
*  QUIMUP class to download streams
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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QTextStream>
#include <QTime>
#include <QUrl>


class qm_stream_loader: public QObject
{
	Q_OBJECT

public:
	qm_stream_loader();
	virtual ~qm_stream_loader();
	
public slots:
	void download_this(QString input);
	
private slots:
	void on_download_finished(QNetworkReply *reply);
	void on_download_progress(qint64,qint64);
	
private:

	QNetworkAccessManager manager;
	QNetworkReply *thedownload;
	QTime download_time;
	
signals:
	void streamdownload_done(QStringList);
	
	
};


