/*
*  qm_streamloader.cpp
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

#include "qm_streamloader.h"

qm_stream_loader::qm_stream_loader()
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(on_download_finished(QNetworkReply*)));
}


void qm_stream_loader::download_this(QString input)
{
/*  convert
    http://radio.com/?u=http://www.party.com/listen.pls&t=.pls
    to
    http://www.party.com/listen.pls  */
    int i = input.lastIndexOf( "http://" );
    input = input.right(input.length() - i);
    i = input.lastIndexOf( '&' );
    input = input.left(i);

    const QUrl the_url = QUrl(input.simplified());

    QNetworkRequest request(the_url);
    download_time.start();
    thedownload = manager.get(request);
    connect(thedownload, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(on_download_progress(qint64,qint64)));
}


void qm_stream_loader::on_download_progress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (download_time.elapsed() > 10000) // msecs
    {
        int percent;
        if (bytesTotal > 0)
            percent = 100 * ((double)bytesReceived/(double)bytesTotal);
        else
            percent = 0;

        if (percent < 50)
        {
            thedownload->abort();
            QStringList result;
            result << "timeout" << QString::number(percent);
            emit streamdownload_done(result);
        }
    }
}


void qm_stream_loader::on_download_finished(QNetworkReply *reply)
{
    QStringList result;

    int counter = 0;
    if (reply->error())
        result << "error" << "Download failed : \n" + reply->errorString();
    else
    {
        if (!reply->isReadable())
            result << "error" << "Download is not readable ";
        else
        {
            QByteArray qba = reply->readAll();
            if ( qba.startsWith("#EXTM3U") )
            {
                result << "ok" << "extended m3u";
                QTextStream in(qba);
                while (!in.atEnd())
                {
                    QString line = in.readLine().simplified();
                    if (line.startsWith("http:"))
                    {
                        result << line;
                        counter++;
                    }
                }
            }
            else
                if ( qba.startsWith("[playlist]"))
                {
                    result << "ok" << "pls";
                    QTextStream in(qba);
                    while (!in.atEnd())
                    {
                        QString line = in.readLine().simplified();
                        int i = line.indexOf("http:");
                        if (line.indexOf("File") != -1 || i != -1 )
                        {
                            result << line.right(line.length() - i);
                            counter++;
                        }
                    }
                }
                else
                    if ( qba.startsWith("<asx "))
                    {
                        result << "ok" << "asx";
                        QTextStream in(qba);
                        while (!in.atEnd())
                        {
                            QString line = in.readLine().simplified();
                            int i = line.indexOf("http:");
                            if (line.indexOf("<ref href") != -1 && i != -1 )
                            {
                                line = line.right(line.length() - i);
                                line.truncate(line.indexOf("\""));
                                result << line;
                                counter++;
                            }
                        }
                    }
                    else
                        if ( qba.startsWith("<?xml") &&  qba.indexOf("xspf.org") != -1 )
                        {
                            result << "ok" << "xspf";
                            QTextStream in(qba);
                            while (!in.atEnd())
                            {
                                QString line = in.readLine().simplified();
                                int i = line.indexOf("http:");
                                if (line.indexOf("<location>") != -1 && i != -1 )
                                {
                                    line = line.right(line.length() - i);
                                    line.truncate(line.indexOf("<"));
                                    result << line;
                                    counter++;
                                }
                            }
                        }
                        else
                            if ( qba.indexOf("#EXTM3U" ) != -1 ) // m3u (with comments)
                            {
                                result << "ok" << "m3u";
                                QTextStream in(qba);
                                while (!in.atEnd())
                                {
                                    QString line = in.readLine().simplified();
                                    if (line.startsWith("http:"))
                                    {
                                        result << line;
                                        counter++;
                                    }
                                }
                            }
                            else
                                if ( qba.indexOf("<html") == -1 && qba.indexOf("http://") != -1)  // flat text list?
                                {
                                    result << "ok" << "flat text m3u";
                                    QTextStream in(qba);
                                    while (!in.atEnd())
                                    {
                                        QString line = in.readLine().simplified();
                                        if (line.startsWith("http:"))
                                        {
                                            result << line;
                                            counter++;
                                        }
                                    }
                                }
        }
    }

    if (counter == 0)
    {
        result.clear();
        result << "nothing" << " No streams found" << (reply->url()).toString();
    }
    emit streamdownload_done(result);
    reply->deleteLater();
}


qm_stream_loader::~qm_stream_loader()
{}
