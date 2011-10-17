#include "RealtimeRider.h"


class Route
{
    QVector<RideFilePoint> route;
public:
    Route(const QVector<RideFilePoint*> &r)
    {
        foreach(RideFilePoint *rfp, r)
        {
            route.push_back(*rfp);
        }
    }
    double calcGrade(int position);
    int getPositionPointByTime(double time);
    int getPositionByDistance(double km);
    RideFilePoint getPoint(int position);
};


class Bike
{
    double ccr;  // road resistance
    double speed; // speed
public:
    double getCcr() { return ccr;}
    void setCcr(double c) { ccr = c; }
    double getSpeed() { return speed; }
    void setSpeed(double s) { speed = s; }
};

class Rider
{
private:
    Bike bike;
    Route route;

};


// look 50 meters down and find the local max
double Route::calcGrade(int position)
{
    int i = position;
    double distance = 0.05;  // 50 meters
    double endDistance = route[position].km + distance;
    while(route[i].km < endDistance)
    {
        i++;
    }
    double grade = (route[i].alt - route[position].alt) / (route[i].km - route[position].km) * 100;
    return grade;
}


