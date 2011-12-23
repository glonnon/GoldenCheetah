/*
 * Copyright (c) 2011 Greg Lonnon (greg.lonnon@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "MainWindow.h"

void PacerChart(MainWindow *mw)
{
    parent = mw;

    setInstanceName("PacerChart");
    setControls(NULL);
    setContentsMargins(0,0,0,0);
    QHBoxLayout *layout = new QHBoxLayout();
    setLayout(layout);
    QVBoxLayout *buttonLayout = new QVBoxLayout();

    QPushButton *plusButton = new QPushButton();
    QPushButton *minusButton = new QPushButton();

    buttonLayout->addItem(plusButton);
    buttonLayout->addItem(minusButton);

    layout->addLayout(buttonLayout);


    parent = mw;
    view = new QWebView();
    view->setPage(new myWebPage());
    view->setContentsMargins(0,0,0,0);
    view->page()->view()->setContentsMargins(0,0,0,0);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setAcceptDrops(false);
    layout->addWidget(view);

    webBridge = new WebBridge(mw, this);

    connect(this, SIGNAL(rideItemChanged(RideItem*)), this, SLOT(rideSelected()));
    connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(updateFrame()));
    connect(mw, SIGNAL(intervalsChanged()), webBridge, SLOT(intervalsChanged()));
    connect(mw, SIGNAL(intervalSelected()), webBridge, SLOT(intervalsChanged()));
    connect(mw, SIGNAL(intervalZoom(IntervalItem*)), this, SLOT(zoomInterval(IntervalItem*)));

    first = true;

}

