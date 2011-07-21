#ifndef RIDEWINDOW_H
#define RIDEWINDOW_H

#include <QWidget>
#include <QWebView>

#include <QDebug>

#include "TrainTool.h"
#include "RealtimeController.h"
#include "RideFile.h"

/// The rider class holds all the information needed to display the rider
/// on the web page
class Rider : public QObject
{
    Q_OBJECT

protected:
    QString name;
    int time;
    double hr;
    double watts;
    double speed;
    double cadence;
    double load;
    double wheelRpm;
    double distance;
    double lon;
    double lat;
    QString markerColor;

    // current ride item
    RideItem *ride;

public:
    Rider() {}

    Q_PROPERTY(int time READ getTime WRITE setTime);
    int getTime() { return time; }
    void setTime(int t) { time = t; }

    Q_PROPERTY(double hr READ getHr WRITE setHr);
    double getHr() { return hr; }
    void setHr(double h) { hr= h; }

    Q_PROPERTY(double Watts READ getWatts WRITE setWatts);
    double getWatts() { return watts; }
    void setWatts(double w) { watts = w; }

    Q_PROPERTY(double speed READ getSpeed WRITE setSpeed);
    double getSpeed() { return speed; }
    void setSpeed(double s) { speed = s; }

    Q_PROPERTY(double wheelRpm READ getWheelRpm WRITE setWheelRpm);
    double getWheelRpm() { return wheelRpm; }
    void setWheelRpm(double w) { wheelRpm = w; }

    Q_PROPERTY(double load READ getLoad WRITE setLoad);
    double getLoad() { return load; }
    void setLoad(double l) { load = l; }

    Q_PROPERTY(double cadence READ getCadence WRITE setCadence);
    double getCadence() { return cadence; }
    void setCadence(double c) { cadence = c; }

    Q_PROPERTY(double distance READ getDistance WRITE setDistance);
    double getDistance() { return distance; }
    void setDistance(double d) { distance = d;}

    Q_PROPERTY(double distance READ getDistance WRITE setDistance);
    double getLon() { return lon; }
    void setLon(double l) { lon = l;}

    Q_PROPERTY(double lat READ getLat WRITE setLat);
    double getLat() { return lat; }
    void setLat(double l) { lat = l;}

    Q_PROPERTY(QString markerColor READ getMarkerColor WRITE setMarkerColor);
    QString getMarkerColor() { return markerColor; }
    void setMarkerColor(QString m) { markerColor = m;}

public slots:
    virtual void Update() = 0;

    // signal the webpage to update...
    void UpdateWebPage()
    {}

};

// The sample class
class Sample : QObject
{


};

class RealtimeRider: public Rider
{
    Q_OBJECT

    TrainTool *window;
    RideItem *ride;
    boost::shared_ptr<QTimer> timer;
    double totDistance;
    double totTime;

public:
    virtual void Update()
    {
        RealtimeData data;
        //window->getRealtimeData(data);
        time = data.getMsecs() / 1000; // we need sec resolution...
        hr = data.getHr();
        watts = data.getWatts();
        speed = data.getSpeed();
        cadence = data.getCadence();
        load = data.getLoad();

        // simple model...  just use speed...
        // a better model is to use a physical model... rolling resistance, frontal area, grade, headwind, temp, elevation, etc...
        //
        // calculate distance
        totDistance = totDistance + (speed * (time - totTime));
        totTime = time;

        // find the nearest lng/lat
        RideFilePoint *rfp;

        foreach(rfp, ride->ride()->dataPoints())
        {
            if(rfp->km > totDistance)
            {
                lon = rfp->lon;
                lat = rfp->lat;
            }

        }
    }
    RealtimeRider(TrainTool *w,RideItem *r)
    {
        totDistance = 0;
        totTime = 0;
        window = w;
        ride = r;
        timer = boost::shared_ptr<QTimer>(new QTimer());
        connect(&*timer,SIGNAL(timeout()),this,SLOT(Update()));
        timer->start(1000);
    }
};

/// rides at a particular wattage
class Pacer : public Rider
{
    Q_OBJECT

public:
    void setLoad(double l) { load = l;}

    virtual void Update()
    {
        // use the load to figure out where the rider location

    }
};


/// rides the rideFile exactly
class GhostRider : public Rider
{
    Q_OBJECT

public:
    virtual void Update()
    {
        // use time to find the rider location

    }
};

// This object stores the calculated rider metrics
// such as xpower, bike score, avg power, kjoules
class RideMetrics
{
protected:

public:
    RideMetrics(TrainTool *) {}

};

// test rider... is just that...  A guy that walks from 300 watts to 400 watts and back,
// so I don't have to pedal while developing...  and he never tires...  TestRider is Pro Tour material...
class TestRider  :  public Rider
{
    Q_OBJECT

private:
    int secs;
    int watts;
    bool goingUp;
    boost::shared_ptr<QTimer> timer;
    RideItem *ride;

     void update_watts()
     {
         watts += (goingUp ? 1 : -1);
         if(watts > 400 ) goingUp = false;
         else if(watts < 300 ) goingUp = true;
         secs++;
         setTime(secs);
         qDebug() << "current secs" << secs;
#if 0
         if(ride->ride()->dataPoints().size() < secs)
         {
             RideFilePoint * rfp = ride->ride()->dataPoints()[secs];
             lat = rfp->lat;
             lon = rfp->lon;
         }
#endif
     }
public:
     TestRider()
     {
        secs = 0;
        watts = 300;
        goingUp = true;
        timer = boost::shared_ptr<QTimer>(new QTimer());
        connect(&*timer,SIGNAL(timeout()),this,SLOT(Update()));
        timer->start(1000);
     }
     void setRide(RideItem *r) { ride = r; }
     virtual void Update()
     {
         update_watts();
     }
};

class RideWindow : public GcWindow
{
    Q_OBJECT

    RideItem *ride;
    QWebView *view;
    bool rideLoaded;
    TrainTool *trainTool;
    RealtimeController *deviceController;

    void loadNoRide();
    void loadRide(RideItem *);
    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);
    Rider *rider;

public:
    explicit RideWindow(MainWindow *);


signals:

public slots:
    void addJSObjects();

};

#endif // RIDEWINDOW_H
