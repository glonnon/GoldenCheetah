/*
 * Copyright (c) 2009 Steve Gribble (gribble [at] cs.washington.edu) and
 *                    Mark Liversedge (liversedge@gmail.com)
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


#ifndef _GC_NullController_h
#define _GC_NullController_h 1
#include "GoldenCheetah.h"

#include <QString>
#include <QDebug>

#include "RealtimeController.h"
#include "RealtimeData.h"
#include "DeviceTypes.h"
#include "DeviceConfiguration.h"

class NullController : public RealtimeController
{
 private:
    TrainTool *parent;

 public:

  // hostname and port are the hostname/port of the server to which
  // this NullControlller should connect.
  NullController(TrainTool *parent,
                          DeviceConfiguration *dc);
  ~NullController() { }

  int start();
  int stop();
  int pause();
  int restart();
  bool discover(char *) {  return true;  }
  bool doesPush() {  return false; }
  bool doesPull() {  return true; }
  bool doesLoad() {  return false; }
  void setLoad(double watts) { load = watts; }
  void getRealtimeData(RealtimeData &rtData);
  void pushRealtimeData(RealtimeData &rtData);

 private:
    double load;
};

class TestController : public RealtimeController, QDialog
{
private:
    TrainTool *parent;
    bool push, pull, load;
public:

 // hostname and port are the hostname/port of the server to which
 // this NullControlller should connect.
 TestController(TrainTool *parent,  DeviceConfiguration *dc) : RealtimeController(parent,dc)
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
