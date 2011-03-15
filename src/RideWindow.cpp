#include <QWebPage>
#include <QWebFrame>


#include "RideWindow.h"
#include "RideFile.h"
#include "RealtimeData.h"



/// The rider class holds all the information needed to display the rider
/// on the web page
class Rider : public QObject
{
protected:
    QString name;
    long time;
    double hr;
    double watts;
    double speed;
    double cadence;
    double load;
    double wheelRpm;
    double distance;
    double lon;
    double lat;

    // current ride file.
    RideFile *rideFile;

public:
    Rider() {}

    Q_PROPERTY(long Time READ getTime WRITE setTime);
    long getTime() { return time;}
    void setTime(long t) { time = t; }

    Q_PROPERTY(double HR READ GetHr WRITE setHR);
    double getHr() { return hr; }
    void setHr(double h) { hr= h; }

    Q_PROPERTY(double Watts READ getWatts WRITE setWatts);
    double getWatts() ;
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

    virtual void Update(RealtimeData &data)
    {
        time = data.getTime();
        hr = data.getHr();
        watts = data.getWatts();
        speed = data.getSpeed();
        cadence = data.getCadence();
        load = data.getLoad();

        // simple model...  just use speed...
        // a better model is to use a physical model... rolling resistance, frontal area, grade, headwind, temp, elevation, etc...
        //
        // calculate distance
        distance = time * speed / 3600;

        // find the nearest lng/lat
        RideFilePoint *rfp;

        foreach(rfp, rideFile->dataPoints())
        {
            if(rfp->km > distance)
            {
                lon = rfp->lon;
                lat = rfp->lat;
            }

        }
    }

    // signal the webpage to update...
    void UpdateWebPage()
    {}

};

/// rides at a particular wattage
class Pacer : Rider
{
public:
    void setLoad(double l) { load = l;}

    virtual void Update(RealtimeData &data)
    {
        // use the load to figure out where the rider location

    }
};


/// rides the rideFile exactly
class GhostRider : Rider
{
public:
    virtual void Update(RealtimeData &data)
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
    RideMetrics(RealtimeWindow *window) {}

};


RideWindow::RideWindow(MainWindow * parent, TrainTool *_trainTool, const QDir &dir, RealtimeWindow *_realtimeWindow) :
    QWidget(parent)
{
    setInstanceName("Ride Window");
    trainTool = _trainTool;
    realtimeWindow = _realtimeWindow;
    view = new QWebView();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(view);
    setLayout(layout);
    QWebPage *page = view->page();
    QWebFrame *frame = page->mainFrame();

    Rider *rider = new Rider();
    // load the ridewindow page
    frame->load(QUrl(QString("qrc:/ride/RideWindow.html")));

    // put the object into the frame
    frame->addToJavaScriptWindowObject("Rider",rider);





}
