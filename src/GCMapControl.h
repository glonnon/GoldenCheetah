/* 
 * Copyright (c) 2009 Greg Lonnon (greg.lonnon@gmail.com)
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

#ifndef _GC_MapControl_h
#define _GC_MapControl_h

#include "qmapcontrol.h"
#include "QWidget"

class QMouseEvent;
class RideItem;

class GCMapControl : public QWidget
{

 private:
	QList<qmapcontrol::Point*> points;
	qmapcontrol::MapControl *mapControl;
	qmapcontrol::MapAdapter *mapAdapter;
	qmapcontrol::Layer *mapLayer;
	qmapcontrol::Layer *rideLayer;
	QLayout *addZoomButtons();
	
 protected:
	void resizeEvent(QResizeEvent *ev);
	void mouseMoveEvent(QMouseEvent *);
	//	void GCMapControl::wheelEvent(MSG *message, long *result);

 public:
	GCMapControl();
	virtual ~GCMapControl() { }
	void setData(RideItem *file);
	
};

#endif
