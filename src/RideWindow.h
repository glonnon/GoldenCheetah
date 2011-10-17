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

#ifndef RIDEWINDOW_H
#define RIDEWINDOW_H

#include <QWidget>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QWebInspector>
#include <QDebug>
#include <limits>
#include "TrainTool.h"
#include "RealtimeController.h"
#include "RideFile.h"

class RiderBridge;

class RideWindow : public GcWindow
{
    Q_OBJECT

protected:
    RideItem *ride;
    QWebView *view;
    bool rideLoaded;
    MainWindow *main;

    virtual void loadRide();
    RiderBridge *rider;

    void EnableWebInspector(QWebPage *page)
    {
        // put the QWebInspector on the page...
        QWebInspector * qwi = new QWebInspector();
        qwi->setPage(page);
        qwi->setVisible(true);
    }
public:
    explicit RideWindow(MainWindow *);
signals:

public slots:
    void addJSObjects();
    void rideSelected();
};

class MapWindow : public RideWindow
{

protected:
    virtual void loadRide()
    {
        RideWindow::loadRide();
        view->page()->mainFrame()->load(QUrl("qrc:/ride/web/MapWindow.html"));
       //EnableWebInspector(view->page());  // turns on the javascript debugger
    }
public:
    explicit MapWindow(MainWindow *main) : RideWindow(main) {}
};


class StreetViewWindow : public RideWindow
{
protected:
    virtual void loadRide()
    {
        RideWindow::loadRide();
        view->page()->mainFrame()->load(QUrl("qrc:/ride/web/StreetViewWindow.html"));
        // EnableWebInspector(view->page());  // turns on the javascript debugger
    }
public:
     explicit StreetViewWindow(MainWindow *main) : RideWindow(main) {}

};

#endif // RIDEWINDOW_H
