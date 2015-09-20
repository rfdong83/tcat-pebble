/*
 * TCAT app
 * By: Ryan Dong, Michael Huang, and Alan Wu
 */

var NUM_STOPS = 5;
var NUM_ROUTES = 5;
var STOP_URL = 'http://cornelldata.org/api/v0/TCAT-data/stop-locations';
var ROUTE_URL = 'https://cornelldata.org/api/v0/TCAT-data/route-schedules';
var stop_data;
var route_data;

function latLongDist(lat1, long1, lat2, long2){
  return (lat1-lat2)*(lat1-lat2) + (long1-long2)*(long1-long2);
}

function convertTime(s) {
    var colonInd = s.indexOf(":");
    var hours = +s.slice(0,colonInd);
    var minutes = +s.slice(colonInd+1,colonInd+3);
    var PM = 0;
    if (s.slice(colonInd+3) == "PM") {
        PM = 12;
    }
    return (hours+PM)*60 + minutes;
}

function loadStops(){
  var req = new XMLHttpRequest();
  req.open('GET', STOP_URL, true);
  req.onload = function(){
    if(req.readyState === 4){
      stop_data = JSON.parse(req.responseText);
      loadRoutes();
    }
    else{
      console.log('Error');
    }
  };
  req.send(null);
}

function loadRoutes(){
  var req = new XMLHttpRequest();
  req.open('GET',ROUTE_URL, true);
  req.onload = function(){
    if(req.readyState === 4){
      route_data = JSON.parse(req.responseText);
    }
    else{
      console.log('Error');
    }
  };
  req.send(null);
}

function findStops(lat, long){
  var dists = stop_data.map(function(s){
    var dist =  latLongDist(lat, long, s.Latitude, s.Longitude);
    return [s.Name,dist];
  });
  var closest_stops = 
    dists.sort(function(a,b){return a[1] - b[1];})
    .slice(0,Math.min(stop_data.length,NUM_STOPS))
    .map(function(a){return a[0];});
  return closest_stops;
}

function findRoutes(time, day, stop){
  var routes = route_data.filter(function(a){return a.Stop == stop && a.Day == day;})
    .sort(function(a,b){return (convertTime(a.Time) - time) - (convertTime(b.Time) - time);});
  routes = routes.slice(0,Math.min(routes.length, NUM_ROUTES));  
  
  
  var routeString = '';
  for (var i=0; i<routes.length; i++) {
    routeString += routes[i].Route + ',' + routes[i].Time + ',' + (routes[i].Direction == 'Inbound' ? 'I' : 'O') + ',';
  }
  return routeString.substring(0,routeString.length-1);
}


/* ================================================================================
                                  MESSAGING CODE
   ================================================================================*/

// GPS options
var options = {
  enableHighAccuracy: true,
  timeout: 5000,
  maximumAge: 0
};

/* Upon sucessful  */
function success(pos) {
  var crd = pos.coords;
  var dict = {};
  
  var stops = findStops(crd.latitude, crd.longitude);    
  
  dict[0] = stops[0];
  dict[1] = stops[1];
  dict[2] = stops[2];
  dict[3] = stops[3];
  dict[4] = stops[4];
  dict[5] = findRoutes(time, day, stops[0]);
  dict[6] = findRoutes(time, day, stops[1]);
  dict[7] = findRoutes(time, day, stops[2]);
  dict[8] = findRoutes(time, day, stops[3]);
  dict[9] = findRoutes(time, day, stops[4]);
  
  // Send the data 
  Pebble.sendAppMessage(dict,
    function(e) {
      console.log('Send successful.');
    },
    function(e) {
      console.log('Send failed!');
    }
  );
}

// Callback function from miscommunication
function error(err) {
  console.warn('ERROR(' + err.code + '): ' + err.message);
}


// Intial calback function from first communication
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS Ready!');
  
  loadStops();
  navigator.geolocation.getCurrentPosition(success, error, options);
  
});


Pebble.addEventListener('gpsrequest', function(e) {
  console.log('Asking for GPS coords...');
  
  navigator.geolocation.getCurrentPosition(success, error, options);
});

