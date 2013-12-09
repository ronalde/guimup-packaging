/*
*  qm_Scroller.ccp
*  QUIMUP graphical text scroller
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

#include "qm_scroller.h"

qm_Scroller::qm_Scroller(QWidget *parent)
{
    // defaults etc.
    new_string = "";
    QLabel::setParent(parent);
    steptime = 80;  // msec
    scrollstep = 1;
    scrollpos = -scrollstep;
    b_busy = false;
    b_pause = false;
    b_fast = false;
    b_scrolling = false;
    W_display = 100;
    H_display = 10;
    the_font = font();
    QPalette pal (this->palette()); // just grab any palette
    QColor background = pal.color(QPalette::Window);
    cR = background.red();
    cG = background.green();
    cB = background.blue();
    QLabel::setAutoFillBackground(true);
    QLabel::setAlignment(Qt::AlignCenter|Qt::AlignCenter|Qt::AlignVCenter);
    virtual_label = new QLabel();
    virtual_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    virtual_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    virtual_label->setAutoFillBackground(true);
    virtual_label->setTextFormat(Qt::RichText);
    virtual_label->setWordWrap(false);
    steploop = new QTimer(this);
    QObject::connect(steploop, SIGNAL(timeout()), this, SLOT(scrollerstep()));
}

void qm_Scroller::setFont(const QFont & fnt)
{
    the_font = fnt;
    virtual_label->setFont(the_font);
    // re-render when scrolling
    if (b_scrolling)
    {
        // reset
        steploop->stop();
        scrollpos = -scrollstep;
        b_scrolling = false;
    }
    render();
}

void qm_Scroller::pause(bool ps)
{
    b_pause = ps;
}

void qm_Scroller::setPalette(QPalette pal)
{
    virtual_label->setPalette(pal);
    QLabel::setPalette(pal);
    QColor background = pal.color(QPalette::Window);
    cR = background.red();
    cG = background.green();
    cB = background.blue();
    // re-render when scrolling
    if (b_scrolling)
    {
        // reset
        steploop->stop();
        scrollpos = -scrollstep;
        b_scrolling = false;
    }
    render();
}

void qm_Scroller::setGeometry(QRect qrt)
{
    W_display = qrt.width();
    H_display = qrt.height();
    QImg_display = QImage(W_display, H_display, QImage::Format_RGB32);
    QLabel::setGeometry(QRect(qrt.x(), qrt.y(), W_display, H_display));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void qm_Scroller::set_title(QString artist, QString title)
{
    if (b_scrolling)
    {
        // reset
        steploop->stop();
        scrollpos = -scrollstep;
        b_scrolling = false;
    }
    
    if (title.isEmpty())
    {
        new_string = "&nbsp;&#187;&nbsp;<b>" + artist + "</b>&nbsp;&#171;&nbsp;";
    }
    else
        new_string = "&nbsp;&#187;&nbsp;<b>" + artist + "&nbsp;:&nbsp;" + title + "</b>&nbsp;&#171;&nbsp;";
        
    render();
}

void qm_Scroller::render()
{
    virtual_label->setText(new_string);
    W_text = (virtual_label->minimumSizeHint()).width ();
    virtual_label->setGeometry(QRect(0, 0, W_text, H_display));
    
    QPixmap qpx  = QPixmap::grabWidget(virtual_label);
    QImg_fulltext = qpx.toImage();
    
    if (QImg_fulltext.width() >  W_display)
        start_scroll();
    else
        setPixmap(qpx);
}

void qm_Scroller::start_scroll()
{
    steploop->stop();
    int delay = steptime;
    
    if (b_fast)
        delay = 10;
        
    b_scrolling = true;
    b_pause = false;
    steploop->start(delay);
}


void qm_Scroller::enterEvent ( QEvent * event )
{
    if (!b_scrolling || b_pause)
        return;
    b_fast = true;
    start_scroll();
    event->accept();
}


void qm_Scroller::leaveEvent ( QEvent * event )
{
    if (!b_scrolling || b_pause)
        return;
    b_fast = false;
    start_scroll();
    event->accept();
}


void qm_Scroller::scrollerstep()
{
    if (b_pause || b_busy || !b_scrolling)
        return;
        
    b_busy = true;
    
    scrollpos ++;
    
    if (scrollpos >=  W_text)
        scrollpos -= W_text;
        
    int w_get = 0;
    
    for (int  h = 0; h < H_display; h++)
    {
        for (int  w = 0; w < W_display; w++)
        {
            w_get = scrollpos + w;
            // wrap at the end
            if (w_get >= W_text)
                w_get -= (W_text);
                
            QRgb px =  QImg_fulltext.pixel(w_get, h);
            
            // fade in & out
            if (w < 24)
            {
                uchar r = (w * qRed(px)  +  (24-w)*cR) / 24;
                uchar g = (w * qGreen(px)+  (24-w)*cG) / 24;
                uchar b = (w * qBlue(px) +  (24-w)*cB) / 24;
                px = qRgb( r, g, b);
            }
            if (w > W_display - 24)
            {
                int	x = W_display - w;
                uchar r = (x * qRed(px)  +  (24-x)*cR) / 24;
                uchar g = (x * qGreen(px)+  (24-x)*cG) / 24;
                uchar b = (x * qBlue(px) +  (24-x)*cB) / 24;
                px = qRgb( r, g, b);
            }
            
            QImg_display.setPixel(w ,h ,px);
            
        }
    }
    // display the result
    setPixmap(QPixmap::fromImage(QImg_display));
    b_busy = false;
}


// toggle step between 1 (false) and 4 pixels
void qm_Scroller::set_fast(bool isfast)
{
    b_fast = isfast;
    if (b_scrolling)
    {
        steploop->stop();
        if (b_fast)
        {
            steploop->start(10);
        }
    }
}

// delay time between scroll steps (msec)
void qm_Scroller::set_delay(int delay)
{
    if (delay < 20)
        delay = 20;
    steptime = delay;
}


qm_Scroller::~qm_Scroller()
{}
