/*
 * Copyright (c) 2010 Greg Lonnon (greg.lonnon@gmail.com)
 * Copyright (c) 2009 Sean C. Rhea (srhea@srhea.net),
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
#include "GcJsonRideFile.h"
#include <algorithm> // for std::sort
#include <QDomDocument>
#include <QVector>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>

#define DATETIME_FORMAT "yyyy/MM/dd hh:mm:ss' UTC'"

static int gcJsonFileReaderRegistered =
    RideFileFactory::instance().registerReader(
        "gcJson", "GoldenCheetah Json Native Format", new GcJsonFileReader());

RideFile *
GcJsonFileReader::openRideFile(QFile &, QStringList &) const
{
    // TBD...
    assert(false);
}

using namespace std;

class RideFilePointJson : public RideFilePoint
{
public:
    RideFilePointJson(RideFilePoint rfp) : RideFilePoint(rfp) {}
    void write(std::ostringstream &oss)
    {
        oss.setf(ios::fixed,ios::floatfield);
        oss.precision(4);
        oss << "  rideFilePoint : {" << endl;
        oss << "secs : " << secs << ",";
        oss << "cad : " << cad << ",";
        oss << "hr : " << hr << ",";
        oss << "km : " << km << ",";
        oss << "kph : " << kph << ",";
        oss << "nm : " << nm << ",";
        oss << "watts : " << watts << ",";
        oss << "alt : " << alt << ",";
        oss.precision(8);
        oss << "lon : " << lon << ",";
        oss << "lat : " << lat << ",";
        oss.precision(4);
        oss << "headwind : " << headwind << ",";
        oss << "interval : " << interval << " }";
    }
};

void GcJsonFileReader::writeRideFile(const RideFile *ride,
                                     std::ostringstream &oss) const
{
    oss << "{  \"ride\": " << endl;
    oss << "    {" << endl;
    oss << "       \"startTime\" : \""
        << ride->startTime().toUTC().toString(DATETIME_FORMAT).toStdString()
        << "\"" << endl;
    oss << "       \"deviceType\" : \"" <<  ride->deviceType().toStdString()
        << "\"" << endl;

    if (!ride->intervals().empty()) {
        oss << "{ \"intervals\" : [" <<  endl;
        foreach(RideFileInterval i, ride->intervals())
        {
            oss << "{";
            oss << "\"name\" : " << i.name.toStdString() << ",";
            oss << "\"start\" : " << i.start << ",";
            oss << "\"stop\" : " << i.stop;
            oss << "}" << endl;
        }
        oss << "]" << endl;
        oss << "}" << endl;
    }

    if (!ride->dataPoints().empty()) {
        oss << "    \"samples\" : [" << endl;
        foreach (const RideFilePoint *point, ride->dataPoints()) {
            RideFilePointJson rfpj(*point);
            oss << "      {" << endl;
            rfpj.write(oss);
            oss << "      }"  << endl;
        }
        oss << "       ]" << endl;
    }
    oss << "    }" << endl;
    oss << "  }" << endl;
}

void
GcJsonFileReader::writeRideFile(const RideFile *ride, QFile &file) const
{
    ostringstream oss;

    writeRideFile(ride,oss);

    if (file.write(oss.str().c_str(),oss.str().length()) != (int)oss.str().length())
        assert(false);
    file.close();
}

