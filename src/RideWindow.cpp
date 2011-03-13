#include "RideWindow.h"

#include "RealtimeData.h"


class RideData : QObject
{
private:
    long time;
    double hr;
    double watts;
    double speed;
    double cadence;
    double load;
    double wheelRpm;
    double distance;

public:
    RideData() {}

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

    static RideData * Create(RealtimeData &data)
    {
        RideData *rd = new RideData();
        rd->setTime(data.getTime());
        rd->setHr(data.getHr());
        rd->setWatts(data.getWatts());
        rd->setSpeed(data.getSpeed());
        rd->setCadence(data.getCadence());
        rd->setLoad(data.getLoad());
        return rd;
    }
};

class RideMetrics
{
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

}
