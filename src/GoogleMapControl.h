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

#ifndef _GC_GoogleMapControl_h
#define _GC_GoogleMapControl_h

#include <QWidget>
#include <QtWebKit>
#include <string>

class QMouseEvent;
class RideItem;

class GoogleMapControl : public QWidget
{

 private:
	QWebView *view;
	RideItem *ride;
	std::string CreatePolyLine(RideItem *);
	std::string CreateIntervalMarkers(RideItem *);

 protected:
	void resizeEvent(QResizeEvent *ev);
	void createHtml();

 public:
	GoogleMapControl();
	virtual ~GoogleMapControl() { }
	void setData(RideItem *file);
};

#endif
