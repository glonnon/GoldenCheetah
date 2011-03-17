
var curLng = ride.RIDE.SAMPLES[0].LON;
var curLat = ride.RIDE.SAMPLES[0].LAT;

var curRideIndexIndex = 1;
var totRideSamples = ride.RIDE.SAMPLES_length;

var timer_count = 0;
var secs = 0;
var interval = 2;
var timer = 20;

var ridePolyLine = new google.maps.Polyline();
var map;
var positionMarker = new google.maps.Marker();
var mapBounds = new google.maps.LatLngBounds();

function mapUpdate(sampleIndex,sampleInterval)
{
    // add data points to polyline

    var path = ridePolyLine.getPath();
    var index = sampleIndex;
    for(var i = 0; i < sampleInterval; i++)
    {
        var sample = ride.RIDE.SAMPLES[sampleIndex + i];
        lng = sample.LON;
        lat = sample.LAT;
        if(lng == 0) continue;
        if(lat == 0) continue;
        var point = new google.maps.LatLng(lat,lng);
        path.push(point);
        mapBounds.extend(point);
    }
    ridePolyLine.setPath(path);
    ridePolyLine.setMap(map);

    // update positionMarker
    positionMarker.setPosition(new google.maps.LatLng(lat,lng));
    positionMarker.setMap(map);

    // bound the map to the ride
    map.fitBounds(mapBounds);

}

var streetOptions = {
    addressControl : false,
    panControl : false,
    linksControl : false,
    scrollwheel : false,
    zoomControl : false
}

var street = new google.maps.StreetViewPanorama(document.getElementById("streetview"),streetOptions);

function streetUpdate(prevSample,curSample)
{
    prevLatLng = new google.maps.LatLng(prevSample.LAT,prevSample.LON);
    curLatLng = new google.maps.LatLng(curSample.LAT,curSample.LON);
    var h = google.maps.geometry.spherical.computeHeading(prevLatLng,curLatLng);
    street.setPov({ heading: h, pitch: 10, zoom: 1 });
    street.setPosition(curLatLng);
    console.log("heading " + h);
    street.setVisible(true);
}

function metricsUpdate(sampleIndex)
{
    var curSample = ride.RIDE.SAMPLES[sampleIndex];
    var html = "<p>Watts: " + curSample.WATTS
}

// time between updates
var eventCount = 0;
var period = 500;
var sampleInterval = 30;
var streetRate = 2000;
var metricsRate = 1000;
var mapRate = period;
// event loop drives the animation
function animateLoop()
{
    var done = false;
    var sampleIndex = sampleInterval * eventCount;
    if(sampleIndex >= ride.RIDE.SAMPLES.length)
    {
        sampleIndex = ride.RIDE.SAMPLES.length;
        done = true;
    }
    prevSample = curSample;
    curSample = ride.RIDE.SAMPLES[sampleIndex];

    if(eventCount * period % mapRate == 0)
    {
        mapUpdate(sampleIndex,sampleInterval);
    }
    if(eventCount * period % streetRate == 0)
    {
        streetUpdate(prevSample,curSample);
    }
    if(eventCount * period % metricsRate == 0)
    {
        metricsUpdate(sampleIndex);
    }
    eventCount = eventCount +1;

    if(done != true)
    {
        setTimeout("animateLoop()",period);
    }
}
curSample = ride.RIDE.SAMPLES[0];

var eventLoopTimer = setTimeout("animateLoop()",1000);

function initialize() {

    var latlng = new google.maps.LatLng(curLat, curLng);
    var mapOptions = {
        zoom: 14,
        center: latlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map"),mapOptions);
}
