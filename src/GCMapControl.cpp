/* 
 * Copyright (c) 2006 Greg Lonnon (greg.lonnon@gmail.com)
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

GCMapControl::GCMapControl()  : MapControl(QSize(1024,768))
{
	mapAdapter = new OSMMapAdapter();
	mapLayer = new MapLayer("OSM Map", mapAdapter);
	addLayer(mapLayer);
	showScale(true);
	addZoomButtons();
}

void GCMapControl::setData(RideItem *_rideItem)
{
	qDebug() << "GCMapControl:" << "setData";
	
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
		qDebug() << "max/min: " << minLat << maxLat << minLong << maxLong;
		qDebug() << "current point " << rfp->latitude << rfp->longitude;
	}
	
	QList<QPointF> view;
	view.append(QPointF(minLat,minLong));
	view.append(QPointF(maxLat,maxLong));
	setViewAndZoomIn(view);
	setView(points.first());
	setZoom(13);
	setMouseMode(MapControl::Panning);
	
	QPen *linepen = new QPen(QColor(0,0,255,100));
	linepen->setWidth(5);
	LineString *rideString = new LineString(points,"RideString",linepen);
	mapLayer->addGeometry(rideString);
	
	

}

void GCMapControl::addZoomButtons()
{
	// create buttons as controls for zoom
	QPushButton* zoomin = new QPushButton("+");
	QPushButton* zoomout = new QPushButton("-");
	zoomin->setMaximumWidth(50);
	zoomout->setMaximumWidth(50);
    
	connect(zoomin, SIGNAL(clicked(bool)),
			this, SLOT(zoomIn()));
	connect(zoomout, SIGNAL(clicked(bool)),
			this, SLOT(zoomOut()));
        
	// add zoom buttons to the layout of the MapControl
	QVBoxLayout* innerlayout = new QVBoxLayout;
	innerlayout->addWidget(zoomin);
	innerlayout->addWidget(zoomout);
	setLayout(innerlayout);
	
}

void GCMapControl::resizeEvent(QResizeEvent *ev)
{
	resize(ev->size());
	qDebug() << "size changed " << ev->size();
}
