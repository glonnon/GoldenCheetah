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

#include "GCMapControl.h"
#include "RideItem.h"
#include "RideFile.h"

using namespace qmapcontrol;

#include <QDebug>

#include <algorithm>

GCMapControl::GCMapControl()
{
	mapControl = new MapControl(QSize(640,480));
	mapAdapter = new OSMMapAdapter();
	mapLayer = new MapLayer("OSM Map", mapAdapter);
	mapControl->addLayer(mapLayer);

	mapControl->showScale(true);

	mapControl->enablePersistentCache(); // TODO: need to get the settings and put it in the correct spot...
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(mapControl);
	layout->addLayout(addZoomButtons());
	setLayout(layout);

}

void GCMapControl::setData(RideItem *_rideItem)
{
	mapLayer->clearGeometries();
	double minLat, minLong, maxLat, maxLong;
	minLat = minLong = 1000;
	maxLat = maxLong = -1000; // larger than 360

	points.clear();

	foreach(RideFilePoint* rfp, _rideItem->ride->dataPoints())
	{
		minLat = std::min(minLat,rfp->latitude);
		maxLat = std::max(maxLat,rfp->latitude);
		
		minLong = std::min(minLong,rfp->longitude);
		maxLong = std::max(maxLong,rfp->longitude);
		
		points.append(new Point(rfp->longitude,rfp->latitude,"what is this"));
	}
	
	QList<QPointF> view;
	view.append(QPointF(minLat,minLong));
	view.append(QPointF(maxLat,maxLong));
	mapControl->setViewAndZoomIn(view);
	mapControl->setView(points.first());
	mapControl->setZoom(13);
	mapControl->setMouseMode(MapControl::Panning);
	
	QPen *linepen = new QPen(QColor(0,0,255,100));
	linepen->setWidth(5);
	LineString *rideString = new LineString(points,"RideString",linepen);
	mapLayer->addGeometry(rideString);
	mapLayer->setVisible(true);
}

QLayout *GCMapControl::addZoomButtons()
{
	// create buttons as controls for zoom
	QPushButton* zoomin = new QPushButton("+");
	QPushButton* zoomout = new QPushButton("-");
	zoomin->setMaximumWidth(50);
	zoomout->setMaximumWidth(50);
    
	connect(zoomin, SIGNAL(clicked(bool)),
			mapControl, SLOT(zoomIn()));
	connect(zoomout, SIGNAL(clicked(bool)),
			mapControl, SLOT(zoomOut()));
        
	//add zoom buttons to the layout of the MapControl
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(zoomin);
	buttonLayout->addWidget(zoomout);

	return buttonLayout;
}

void GCMapControl::resizeEvent(QResizeEvent *ev)
{
	mapControl->resize(ev->size());
	qDebug() << "size changed " << ev->size() << geometry() <<  baseSize() << frameSize() << mapControl->sizeHint();
}


void GCMapControl::mouseMoveEvent(QMouseEvent *event)
{
	qDebug() << "mouse moved";
}
#if false
void GCMapControl::wheelEvent(MSG *message, long *result)
{

}
#endif
