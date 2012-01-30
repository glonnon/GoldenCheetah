/*
 * Copyright (c) 2012 Mark Liversedge (liversedge@gmail.com)
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

#ifndef _GC_GcCalendar_h
#define _GC_GcCalendar_h 1

#include "GoldenCheetah.h"

#include <QtGui>

#include "MainWindow.h"
#include "GcCalendarModel.h"
#include "RideItem.h"
#include "RideNavigator.h"

// Catch signal, no background and do embossed text
class GcLabel : public QLabel
{
    Q_OBJECT

    int xoff, yoff;
    bool bg, selected; // bg = highlighted, selected = user selected too

public:
    GcLabel(const QString & text, QWidget * parent = 0) : QLabel(text, parent), xoff(0), yoff(0), bg(false), selected(false), bgColor(Qt::lightGray) {}
    ~GcLabel(){}
 
signals:
    void clicked();
 
public slots:
    void setYOff(int x) { yoff = x; }
    void setXOff(int x) { xoff = x; }
    void setBg(bool x) { bg = x; }
    bool getBg() { return bg; }
    void setBgColor(QColor bg) { bgColor = bg; }
    void setSelected(bool x) { selected = x; }
 
protected:
    void mouseReleaseEvent(QMouseEvent *) {
        emit clicked();
    }
    void paintEvent(QPaintEvent *);
    QColor bgColor; 
};

class GcCalendar : public QWidget // not a GcWindow - belongs on sidebar
{
    Q_OBJECT
    G_OBJECT

    public:

        GcCalendar(MainWindow *);

    public slots:

        void setRide(RideItem *ride);
        void configChanged();

        void dayClicked(int num); // for when a day is selected
        void next();
        void previous();


    protected:
        MainWindow *mainWindow;
        int month, year;

        QVBoxLayout *layout;
        QGridLayout *dayLayout; // contains the day names and days

        GcLabel *dayNumber; // the big NUmber at top
        GcLabel *dayName;   // what day of the week
        GcLabel *dayDate;   // Date string

        GcLabel *left, *right; // < ... >
        GcLabel *monthName; // January 2012
        GcLabel *dayNames[7]; // Mon .. Sun

        QList<GcLabel*> dayLabels; // 1 .. 31

        QSignalMapper *signalMapper; // for mapping dayLabels "clicked"

        QPalette black, grey, white;
        QList<FieldDefinition> fieldDefinitions;
        GcCalendarModel *calendarModel;

};
#endif // _GC_GcCalendar_h