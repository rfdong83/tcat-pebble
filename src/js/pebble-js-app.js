/*
 * pebble-js-app.js
 * Sends the sample message once it is initialized.
 */
var NUM_STOPS = 5;
var NUM_ROUTES = 10;
var STOP_URL = 'http://cornelldata.org/api/v0/TCAT-data/stop-locations';
var STOP_SCHED = 'http://cornelldata.org/api/v0/TCAT-data/stop-schedules';

function latLongDist(lat1, long1, lat2, long2){
  return (lat1-lat2)*(lat1-lat2) + (long1-long2)*(long1-long2);
}

function findStops(lat, long){
  var req = new XMLHttpRequest();
  req.open('GET', STOP_URL, true);
  req.onload = function(){    
    if(req.readyState === 4){
      var stops = JSON.parse(req.responseText);
      var dists = stops.map(function(s){
        var dist =  latLongDist(lat, long, s.Latitude, s.Longitude);
        return [s.Name,dist];
      });
      var closest_stops = dists.sort(function(a,b){
        return a[1] - b[1];})
        .slice(0,Math.min(stops.length,NUM_STOPS))
        .map(function(a){return a[0];});
      console.log(closest_stops);
    }    
    else{
      console.log('Error');
    }
  };
  req.send(null);
}

function findRoute(time, stop, iter){
  var req = new XHLHttpRequest();
  req.open('GET',STOP_SCHED+'?Stop='+stop);
  req.onload = function(){
    if
  }
}

function findRoutes(stops){
  var req = new XMLHttpRequest();
  var iter = 0;
  req.open('GET', STOP_SCHED)
}

findStops(42.489067, -76.485206);

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS Ready!');

  // Construct a dictionary
  var dict = {
    'KEY_DATA':'Hello from PebbleKit JS!'
  };

  // Send a string to Pebble
  Pebble.sendAppMessage(dict,
    function(e) {
      console.log('Send successful.');
    },
    function(e) {
      console.log('Send failed!');
    }
  );
});
