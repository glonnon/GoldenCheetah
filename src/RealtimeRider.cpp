


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


class BikePhysicsEngine
{
    // check out http://www.flacyclist.com/content/perf/science.html for the
    // math behind this...

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

