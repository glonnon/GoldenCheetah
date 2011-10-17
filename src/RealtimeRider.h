#ifndef REALTIMERIDER_H
#define REALTIMERIDER_H

#include <QWidget>
#include <QDebug>
#include "RideItem.h"
#include "RealtimeData.h"

/// The rider class holds all the information needed to display the rider
/// on the web page
class RiderBridge : public QObject
{
    Q_OBJECT

protected:
    QString name;

    QString markerColor;

    // current ride item
    RideItem *ride;
    // current data
    RideFilePoint rfp;

    // currnet position in the ride
    int curPosition;
    double curTime; // in seconds

    int load;  // load generator (computrainer only)

public:
    RiderBridge() {}
    void setRide(RideItem *ri) { ride = ri; }

    // list of all properties that are visible
    // in js

    Q_PROPERTY(QString name READ getName WRITE setName);
    QString getName() { return name; }
    void setName(QString s) { name = s; }

    Q_PROPERTY(int time READ getSecs WRITE setSecs);
    double getSecs() { return rfp.secs; }
    void setSecs(double s) { rfp.secs = s; }

    Q_PROPERTY(double hr READ getHr WRITE setHr);
    double getHr() { return rfp.hr; }
    void setHr(double h) { rfp.hr= h; }

    Q_PROPERTY(double Watts READ getWatts WRITE setWatts);
    double getWatts() { return rfp.watts; }
    void setWatts(double w) { rfp.watts = w; }

    Q_PROPERTY(double speed READ getSpeed WRITE setSpeed);
    double getSpeed() { return rfp.kph; }
    void setSpeed(double s) { rfp.kph = s; }

   // Q_PROPERTY(double wheelRpm READ getWheelRpm WRITE setWheelRpm);
   // double getWheelRpm() { return rfp.cad; }
   // void setWheelRpm(double w) { rfp.cad = w; }

    Q_PROPERTY(double load READ getLoad WRITE setLoad);
    double getLoad() { return load; }
    void setLoad(double l) { load = l; }

    Q_PROPERTY(double cadence READ getCadence WRITE setCadence);
    double getCadence() { return rfp.cad; }
    void setCadence(double c) { rfp.cad = c; }

    Q_PROPERTY(double distance READ getDistance WRITE setDistance);
    double getDistance() { return rfp.km; }
    void setDistance(double d) { rfp.km = d;}

    Q_PROPERTY(double distance READ getDistance WRITE setDistance);
    double getLon() { return rfp.lon; }
    void setLon(double l) { rfp.lon = l;}

    Q_PROPERTY(double lat READ getLat WRITE setLat);
    double getLat() { return rfp.lat; }
    void setLat(double l) { rfp.lat = l;}

    Q_PROPERTY(QVariantList routeLon READ getRouteLon);
    QVariantList getRouteLon() {
        QVariantList route;
        foreach(RideFilePoint *rfp, ride->ride()->dataPoints())
        {
            route.push_back(QVariant(rfp->lon));
        }
        return route;
    }
    Q_PROPERTY(QVariantList routeLat READ getRouteLat);
    QVariantList getRouteLat() {
        QVariantList route;
        foreach(RideFilePoint *rfp, ride->ride()->dataPoints())
        {
            route.push_back(QVariant(rfp->lat));
        }
        return route;
    }

    Q_PROPERTY(QVariantList profile READ getProfile);
    QVariantList getProfile() {
        QVariantList route;
        foreach(RideFilePoint *rfp, ride->ride()->dataPoints())
        {
            route.push_back(QVariant(rfp->alt));
        }
        return route;
    }

    Q_PROPERTY(int position READ getPosition);
    int getPosition() {
        return curPosition;
    }


public slots:
    // set the curTime and find the position in the array closest to the time
    Q_INVOKABLE void SetTime(double secs)
    {
        // find the curPosition
        QVector<RideFilePoint*> dataPoints =  ride->ride()->dataPoints();
        curTime = secs;

        if(dataPoints.count() < curPosition)
        {
            curPosition = 0;
            return;
        }
        // make sure the current position is less than the new time
        if (dataPoints[curPosition]->secs < secs)
        {
            for( ; curPosition < dataPoints.count(); curPosition++)
            {
                if(dataPoints[curPosition]->secs < secs)
                {
                    return;
                }
            }
        }
        else
        {
            for( ; curPosition > 0; curPosition--)
            {
                if(dataPoints[curPosition]->secs > secs)
                {
                    return;
                }
            }
        }
        // update the rfp
        rfp = *dataPoints[curPosition];
    }
};

class BikePhysicsEngine
{
    // check out http://www.flacyclist.com/content/perf/science.html for the
    // math behind this...
    // another good link is: http://en.wikipedia.org/wiki/Bicycle_performance

    double ccr;  // roll resistance 0.001 track, 0.002 concrete, 0.004 asphalt, 0.008 rough road
    double weight; // kg rider + bike
    double wind; // head wind only
    double cda; // areo
    double alt;
    double temp; // C
    double air; // air pressure
    double speed;
    double grade;

    // calcuated values
    double f_air;
    double f_roll;
protected:

    double calcFAir(double speed)
    {
        return 0.5 * cda * (air/((temp + 273.15) * 287.058) * pow((speed + wind), 2));
    }

    double calcFGrade()
    {
        return 9.8 * weight * grade;
    }

    double calcFRoll()
    {
        return 9.8 * weight * ccr;
    }

public:
    void setCcr(double c) { ccr = c; }
    void setWeight(double w) { weight = w; }
    void setWind(double w) { wind = w; }
    void setGrade(double g) { grade = g; }
    void setSpeed(double s) { speed = s; }

    double SpeedGivenPower(double watts)
    {
        return watts/20;
    }
    double PowerGivenSpeed(double speed)
    {
        return calcFAir(speed) + calcFGrade() + calcFRoll();
    }
};


class RealtimeRider: public RiderBridge
{
    Q_OBJECT;
    //Route route;
    BikePhysicsEngine engine;
    double prevMSecs;

protected:
    virtual int findCurrentPosition()
    {
        QVector<RideFilePoint*> route =  ride->ride()->dataPoints();
        // this is by distance
        for(int i = 0; i < route.count(); i++)
        {
            if(rfp.km < route[i]->km)
                return i;
        }
        return 1;
    }
    int findCurrentPositionByPower()
    {
        double speed = engine.SpeedGivenPower(rfp.watts);
        return (int) speed;

    }
    int findCurrentPositionByTime()
    {

        return 0;
    }

public slots:
    virtual void telemetryUpdate(const RealtimeData &data)
    {
        rfp.watts = data.getWatts();
        rfp.kph = data.getSpeed();
        rfp.cad = data.getCadence();
        rfp.km = data.getDistance();
        rfp.secs = data.getMsecs() / 1000;
        load = data.getLoad();
        curPosition = findCurrentPosition();
    }
public:
    RealtimeRider(MainWindow *)
    {
        curPosition = 1;
       // connect(main,SIGNAL(telemetryUpdate(const RealtimeData &)), this,SLOT(telemetryUpdate(const RealtimeData &)));
    }
};

class DistanceRider : public RealtimeRider
{

};


class TimeRider : public RealtimeRider
{


};

class PowerRider : public RealtimeRider
{


};


#endif // REALTIMERIDER_H
