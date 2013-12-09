/*
*  qm_colorpicker.cpp
*  QUIMUP window to select custom colors
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


#include "qm_colorpicker.h"

qm_colorpicker::qm_colorpicker(QWidget *parent)
{
    setParent(parent);
    
    QPixmap qpx;
    qpx = QPixmap(":/mn_icon.png");
    setWindowIcon(qpx);
    setWindowTitle(tr("Custom colors") );
    
    sl_H_front = new QSlider(Qt::Horizontal, this);
    sl_H_front->setRange(0, 255);
    sl_H_front->setSingleStep(1);
    sl_H_front->setPageStep(10);
    
    sl_S_front = new QSlider(Qt::Horizontal, this);
    sl_S_front->setRange(0, 255);
    sl_S_front->setSingleStep(1);
    sl_S_front->setPageStep(10);
    
    sl_V_front = new QSlider(Qt::Horizontal, this);
    sl_V_front->setRange(0, 255);
    sl_V_front->setSingleStep(1);
    sl_V_front->setPageStep(10);
    
    sl_H_back = new QSlider(Qt::Horizontal, this);
    sl_H_back->setRange(0, 255);
    sl_H_back->setSingleStep(1);
    sl_H_back->setPageStep(10);
    
    sl_S_back = new QSlider(Qt::Horizontal, this);
    sl_S_back->setRange(0, 255);
    sl_S_back->setSingleStep(1);
    sl_S_back->setPageStep(10);
    
    sl_V_back = new QSlider(Qt::Horizontal, this);
    sl_V_back->setRange(0, 255);
    sl_V_back->setSingleStep(1);
    sl_V_back->setPageStep(10);
    
    lb_cust_color = new QLabel;
    lb_cust_color->setFrameShape(QFrame::StyledPanel);
    lb_cust_color->setFrameShadow(QFrame::Sunken);
    lb_cust_color->setAlignment(Qt::AlignCenter);
    lb_cust_color->setTextFormat(Qt::RichText);
    lb_cust_color->setMinimumSize(QSize(240, 36));
    lb_cust_color->setAutoFillBackground(true);
    lb_cust_color->setText("<b>" + tr("Custom Colors") + "</b>");
    
    bt_ok = new QPushButton(tr("OK"), this);
    bt_cancel = new QPushButton(tr("Cancel"), this);
    
    
    QWidget *main_widget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(main_widget);
    setCentralWidget(main_widget);
    
    QGridLayout *HSV_front_GridBox = new QGridLayout();
    HSV_front_GridBox->addWidget(new QLabel(tr("Text"), this), 0, 1);
    HSV_front_GridBox->addWidget(new QLabel("H", this), 1, 0);
    HSV_front_GridBox->addWidget(new QLabel("S", this), 2, 0);
    HSV_front_GridBox->addWidget(new QLabel("V", this), 3, 0);
    
    HSV_front_GridBox->addWidget(sl_H_front, 1, 1);
    HSV_front_GridBox->addWidget(sl_S_front, 2, 1);
    HSV_front_GridBox->addWidget(sl_V_front, 3 ,1);
    
    mainLayout->addLayout(HSV_front_GridBox);
    
    QGridLayout *HSV_back_GridBox = new QGridLayout();
    HSV_back_GridBox->addWidget(new QLabel(tr("Backround"), this), 0, 1);
    HSV_back_GridBox->addWidget(new QLabel("H", this), 1, 0);
    HSV_back_GridBox->addWidget(new QLabel("S", this), 2, 0);
    HSV_back_GridBox->addWidget(new QLabel("V", this), 3, 0);
    
    HSV_back_GridBox->addWidget(sl_H_back, 1, 1);
    HSV_back_GridBox->addWidget(sl_S_back, 2, 1);
    HSV_back_GridBox->addWidget(sl_V_back, 3 ,1);
    
    mainLayout->addLayout(HSV_back_GridBox);
    
    mainLayout->addWidget(lb_cust_color);
    
    QHBoxLayout *ok_cancel_Layout = new QHBoxLayout();
    ok_cancel_Layout->setAlignment(Qt::AlignRight);
    ok_cancel_Layout->setMargin(10);
    ok_cancel_Layout->setSpacing(10);
    ok_cancel_Layout->addWidget(bt_ok);
    ok_cancel_Layout->addWidget(bt_cancel);
    
    mainLayout->addLayout(ok_cancel_Layout);
    
    connect( sl_H_front, SIGNAL(valueChanged(int)), SLOT(set_custom_fg()) );
    connect( sl_S_front, SIGNAL(valueChanged(int)), SLOT(set_custom_fg()) );
    connect( sl_V_front, SIGNAL(valueChanged(int)), SLOT(set_custom_fg()) );
    
    connect( sl_H_back, SIGNAL(valueChanged(int)), SLOT(set_custom_bg()) );
    connect( sl_S_back, SIGNAL(valueChanged(int)), SLOT(set_custom_bg()) );
    connect( sl_V_back, SIGNAL(valueChanged(int)), SLOT(set_custom_bg()) );
    
    connect( bt_cancel, SIGNAL(clicked()), this, SLOT(on_cancel()) );
    connect( bt_ok, SIGNAL(clicked()), this, SLOT(on_ok()) );
}


void qm_colorpicker::on_cancel()
{
    this->hide();
}


void qm_colorpicker::on_ok()
{
    emit sgnl_newcolors(fgcolor, bgcolor);
    this->hide();
}


void qm_colorpicker::set_colors(QColor fg, QColor bg)
{
    sl_H_front->setValue(fg.hue());
    sl_S_front->setValue(fg.saturation());
    sl_V_front->setValue(fg.value());
    sl_H_back->setValue(bg.hue());
    sl_S_back->setValue(bg.saturation());
    sl_V_back->setValue(bg.value());
}


void qm_colorpicker::set_custom_fg()
{
    // h,s,v > 0 to work around a setHsv bug
    QPalette pal (lb_cust_color->palette());
    int h = sl_H_front->value();
    if (h == 0) h = 1;
    int s = sl_S_front->value();
    if (s == 0) s = 1;
    int v = sl_V_front->value();
    if (v == 0) v = 1;
    fgcolor.setHsv(h, s, v);
    pal.setColor(QPalette::Foreground, fgcolor);
    lb_cust_color->setPalette(pal);
}


void qm_colorpicker::set_custom_bg()
{
    // h,s,v > 0 to work around a setHsv bug
    QPalette pal (lb_cust_color->palette());
    int h = sl_H_back->value();
    if (h == 0) h = 1;
    int s = sl_S_back->value();
    if (s == 0) s = 1;
    int v = sl_V_back->value();
    if (v == 0) v = 1;
    bgcolor.setHsv(h, s, v);
    pal.setColor(QPalette::Window, bgcolor);
    lb_cust_color->setPalette(pal);
}


qm_colorpicker::~qm_colorpicker()
{}
