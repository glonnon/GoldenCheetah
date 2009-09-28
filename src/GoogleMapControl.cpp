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
#include "GoogleMapControl.h"
#include "RideItem.h"
#include "RideFile.h"
#include "Settings.h"

#include <QDebug>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#define GOOGLE_KEY "ABQIAAAAS9Z2oFR8vUfLGYSzz40VwRQ69UCJw2HkJgivzGoninIyL8-QPBTtnR-6pM84ljHLEk3PDql0e2nJmg"

GoogleMapControl::GoogleMapControl()
{
	view = new QWebView();
	//	view->load(QUrl("http://www.trolltech.com"));
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(view);
	setLayout(layout);
}

void GoogleMapControl::resizeEvent(QResizeEvent *ev)
{
	createHtml();
	QWidget::resizeEvent(ev);
}

void GoogleMapControl::setData(RideItem *_rideItem)
{
	ride = _rideItem;
	createHtml();
}

void GoogleMapControl::createHtml()
{
	if(ride == NULL) return;

	double minLat, minLong, maxLat, maxLong;
	minLat = minLong = 1000;
	maxLat = maxLong = -1000; // larger than 360

	foreach(RideFilePoint* rfp, ride->ride->dataPoints())
	{
		minLat = std::min(minLat,rfp->latitude);
		maxLat = std::max(maxLat,rfp->latitude);
		
		minLong = std::min(minLong,rfp->longitude);
		maxLong = std::max(maxLong,rfp->longitude);
		
	}

	int width = this->width();
	int height = this->height();

	double startLat = ride->ride->dataPoints().first()->latitude;
	double startLong = ride->ride->dataPoints().first()->longitude;

	std::ostringstream oss;
	
	oss.precision(6);
	oss.setf(ios::fixed,ios::floatfield);

	oss << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"" << endl
		<< "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << endl
		<< "<html xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:v=\"urn:schemas-microsoft-com:vml\">" << endl
		<< "<head>" << endl
 		<< "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>" << endl
		<< "<title>GoldenCheetah</title>" << endl
		<< "<script src=\"http://maps.google.com/maps?file=api&amp;v=2&amp;key=" << GOOGLE_KEY <<"\"" << endl
		<< "type=\"text/javascript\"></script>" << endl
		<< "<script type=\"text/javascript\">"<< endl
		<< "var map;" << endl
		<< "function initialize() {" << endl
		<< "if (GBrowserIsCompatible()) {" << endl
		<< "map = new GMap2(document.getElementById(\"map_canvas\"));" << endl
		<< "map.setCenter(new GLatLng(" << startLat <<"," << startLong << "), 13);" << endl
		<< "map.addControl(new GLargeMapControl3D());" << endl
		<< "map.addControl(new GMapTypeControl());" << endl
		<< "map.addControl(new GScaleControl());" << endl
		<< CreatePolyLine(ride) << endl
		<< CreateIntervalMarkers(ride) << endl
		<< "map.addOverlay(polyline);"<< endl 
		<< "map.enableScrollWheelZoom();" << endl
		<< "}" << endl
		<< "}" << endl
		<< "function animate() {"  << endl  
		<< "map.panTo(new GLatLng(" << maxLat << "," << minLong<< "));" << endl
		<< "}" << endl
		<< "</script>" << endl
		<< "</head>" << endl
		<< "<body onload=\"initialize()\" onunload=\"GUnload()\">" << endl
		<< "<div id=\"map_canvas\" style=\"width: " << width <<"px; height: "<< height <<"px\"></div>" << endl
		<< "<form action=\"#\" onsubmit=\"animate(); return false\">" << endl
		<< "</form>" << endl
		<< "</body>" << endl
		<< "</html>" << endl; 
	
	
	QString htmlFile(QDir::tempPath());
	htmlFile.append("/maps.html");
	QFile file(htmlFile);
	file.remove();
	file.open(QIODevice::ReadWrite);
	file.write(oss.str().c_str(),oss.str().length());
	file.flush();
	file.close();		

	QString filename("file:///");
	filename.append(htmlFile);
	
	QUrl url(filename);
	// need to do it this way, for google masp terms
	view->load(url); 
}

string GoogleMapControl::CreatePolyLine(RideItem *ride)
{
	ostringstream oss;
	oss << "var polyline = new GPolyline([";

	oss.precision(6);
	oss.setf(ios::fixed,ios::floatfield);
	
	foreach(RideFilePoint* rfp, ride->ride->dataPoints())
	{
		oss << "new GLatLng(" << rfp->latitude << "," << rfp->longitude << ")," << endl;
	}	oss << "],\"ff0000\",5);";
		
	return oss.str();

}

// quick ideas on a math pipeline, kindof like this...
// but more of a pipeline...
// it makes the math somewhat easier to do and understand...

class RideFilePointAlgorithm
{
protected:
	RideFilePoint prevRfp;
	bool first;
	RideFilePointAlgorithm() { first = false; }
};
	
class AltGained : private RideFilePointAlgorithm
{
protected:
	double peak;
	double valley;
public:
	AltGained() { peak  = -50000; valley = 50000; }
	
	void operator()(RideFilePoint rfp)
	{
		peak = max(peak,rfp.alt);
		valley = min(valley,rfp.alt);
	}
	int TotalGained() { return peak-valley; }
	operator int() { return TotalGained(); }
};

class AvgHR
{
	int samples;
	int totalHR;
public:
	AvgHR() : samples(0),totalHR(0.0) {}
	void operator()(RideFilePoint rfp)
	{
		totalHR += rfp.hr;
		samples++;
	}
	int HR() { return totalHR / samples; }
	operator int() { return HR(); }
};	

class AvgPower
{
	int samples;
	double totalPower;
public:
	AvgPower() : samples(0), totalPower(0.0) { }
	void operator()(RideFilePoint rfp)
	{
		totalPower += rfp.watts;
		samples++;
	}
	int Power() { return (int) (totalPower / samples); }
	operator int() { return Power(); }
};

// TODO: make a generic converting class
#define MILES_PER_KM 0.62137119
#define  KM_PER_MILES (1/MILES_PER_KM)
#define  FEET

void GoogleMapControl::CreateMarker(ostringstream &oss, const RideFilePointVector &interval,
									const RideFilePoint &rfp)
{
	boost::shared_ptr<QSettings> settings;
	bool isMetric = true;
	settings = GetApplicationSettings();
	QVariant unit = settings->value(GC_UNIT);
    if(unit.toString() != "Metric")
        isMetric = false;


	// want to see avg power, avg speed, alt changes, avg hr
	double distance = interval.back().km -
		interval.front().km ;

	if(!isMetric) distance *= MILES_PER_KM;

	int secs = interval.back().secs -
		interval.front().secs;

	double avgSpeed = (distance/((double)secs)) * 3600;
	QTime time;
	time = time.addSecs(secs);

	AvgHR avgHr = for_each(interval.begin(),
						   interval.end(),
						   AvgHR());

	AvgPower avgPower = for_each(interval.begin(),
								 interval.end(),
										 AvgPower());

	AltGained altGained =for_each(interval.begin(),
								  interval.end(),
								  AltGained());

	oss << "marker = new GMarker(new GLatLng( ";
	oss<< rfp.latitude << "," << rfp.longitude << "));" << endl;
	oss << "marker.bindInfoWindowHtml(" <<endl;
	oss << "\"<p><h3>Lap: " << rfp.interval << "</h3></p>" ;
	oss << "<p>Distance: " << distance << "</p>" ;
	oss << "<p>Time: " << time.toString().toStdString() << "</p>";
	oss << "<p>Avg Speed</>: " << avgSpeed << "</p>";
	if(avgHr != 0) {
		oss << "<p>Avg HR: " << avgHr << "</p>";
	}
	if(avgPower != 0)
	{
				oss << "<p>Avg Power: " << avgPower << "</p>";
	}
	oss << "<p>Alt Gained: " << altGained << "</p>";
	oss << "\");" << endl;
	oss << "map.addOverlay(marker);" << endl;
}

string GoogleMapControl::CreateIntervalMarkers(RideItem *ride)
{
	ostringstream oss;
	oss.precision(6);
	oss.setf(ios::fixed,ios::floatfield);

	oss << "var marker;" << endl;
	int currentInterval = 0;
	
	RideFilePointVector intervalPoints;
	RideFilePointVector totalRide;

	foreach(RideFilePoint* rfp, ride->ride->dataPoints())
	{
		totalRide.push_back(*rfp);
		intervalPoints.push_back(*rfp);
		if(currentInterval < rfp->interval)
		{
			CreateMarker(oss,intervalPoints,*rfp);
			currentInterval = rfp->interval;
			intervalPoints.clear();
		}
	}
	// total ride information
	CreateMarker(oss,totalRide,intervalPoints.back());

	return oss.str();
}

		
