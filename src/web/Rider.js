// This the rider class...
// It can take a JSON file and turn it into a rider.
// or (in the future...) take live input from the RealtimeWindow


function Rider_createRoute()
{
    var path = new Array();
    var bounds = new google.maps.LatLngBounds();
    for(var i = 0; i < this.ride.RIDE.SAMPLES.length; i++)
    {
        var sample = this.ride.RIDE.SAMPLES[i];
        if(sample.LAT == 0) continue;
        if(sample.LON == 0) continue;
        var coord = new google.maps.LatLng(sample.LAT,sample.LON);
        path.push(coord);
        bounds.extend(coord);
    }
    var polyline = new google.maps.Polyline();
    polyline.setPath(path);
    return [polyline,bounds];
}


function Rider_validTime(time)
{
  if((time > 0) && (time <= this.ride.RIDE.SAMPLES.length))
  {
      return time;
  }
  else
  {
      return 0;
  }
}

function Rider_setTime(time)
{
    this.time = this.validTime(time);
    this.curSample = this.ride.RIDE.SAMPLES[this.time];
}

function Rider_isDone(time)
{
    if(this.ride.RIDE.SAMPLES.length < time)
    {
        return true;
    }
    return false;
}

function Rider_computeHeading()
{
    var time1 = this.time;
    var time0 = this.validTime(this.time-1);
    var sample1 = this.getSample(time1);
    var sample0 = this.getSample(time0);
    var coord1 =  new google.maps.LatLng(sample1.LAT,sample1.LON);
    var coord0 =  new google.maps.LatLng(sample0.LAT,sample0.LON);
    return google.maps.geometry.spherical.computeHeading(coord0,coord1);
}

function Rider_getSample(time)
{
    t = this.validTime(time);
    return this.ride.RIDE.SAMPLES[t];
}

function Rider(ridedata)
{
    this.time = 1;  // easier to handle computing heading
    this.ride = ridedata;
    this.curSample = this.ride.RIDE.SAMPLES[this.time];
    this.setTime = Rider_setTime;
    this.createRoute = Rider_createRoute;
    this.route = this.createRoute();
    this.isDone = Rider_isDone;
    this.computeHeading = Rider_computeHeading;
    this.validTime = Rider_validTime;
    this.getSample = Rider_getSample;
}