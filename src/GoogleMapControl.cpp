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
#include "GoogleMapControl.h"
#include "RideItem.h"
#include "RideFile.h"

#include <QDebug>

#include <algorithm>

GoogleMapControl::GoogleMapControl()
{
	view = new QWebView();
	view->load(QUrl("http://www.trolltech.com"));
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(view);
	setLayout(layout);
}

void GoogleMapControl::setData(RideItem *_rideItem)
{
	
	double minLat, minLong, maxLat, maxLong;
	minLat = minLong = 1000;
	maxLat = maxLong = -1000; // larger than 360

	

	foreach(RideFilePoint* rfp, _rideItem->ride->dataPoints())
	{
		minLat = std::min(minLat,rfp->latitude);
		maxLat = std::max(maxLat,rfp->latitude);
		
		minLong = std::min(minLong,rfp->longitude);
		maxLong = std::max(maxLong,rfp->longitude);
		
		//points.append(new Point(rfp->longitude,rfp->latitude,"what is this"));
	}
}

