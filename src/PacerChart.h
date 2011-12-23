/*
 * Copyright (c) 2009 Steve Gribble (gribble [at] cs.washington.edu) and
 *                    Mark Liversedge (liversedge@gmail.com)
 #               2011 Greg Lonnon (greg.lonnon@gmail.com)
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


#ifndef _GC_PACER_CHART_h
#define _GC_PACER_CHART_h 1
#include "GoldenCheetah.h"

#include <QString>
#include <QDebug>

#include "RealtimeController.h"
#include "RealtimeData.h"
#include "DeviceTypes.h"
#include "DeviceConfiguration.h"

class PacerChart : public RealtimeController, GCWindow
{
private:
    TrainTool *parent;
    bool push, pull, load;
public:

 PacerChart(TrainTool *parent,  DeviceConfiguration *dc) : RealtimeController(parent,dc)
 {


 }

 ~TestController() { }



 int start();
 int stop();
 int pause();
 int restart();
 bool discover(char *) {  return true;  }
 bool doesPush() {  return false; }
 bool doesPull() {  return true; }
 bool doesLoad() {  return false; }
 void setWatts(double watts) { curWatts = watts; }
 void getRealtimeData(RealtimeData &rtData);
 void pushRealtimeData(RealtimeData &rtData);

private:
   double curWatts;

};


#endif // _GC_NullController_h
