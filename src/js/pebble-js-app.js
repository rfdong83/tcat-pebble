/*
 * pebble-js-app.js
 * Sends the sample message once it is initialized.
 */

var options = {
  enableHighAccuracy: true,
  timeout: 5000,
  maximumAge: 0
};

function success(pos) {
  var crd = pos.coords;

  console.log('Your current position is:');
  console.log('Latitude : ' + crd.latitude);
  console.log('Longitude: ' + crd.longitude);
  console.log('More or less ' + crd.accuracy + ' meters.');    
  
  // Generate a dictionary of stop data
  var dict = {
  };
  
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

function error(err) {
  console.warn('ERROR(' + err.code + '): ' + err.message);
}

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS Ready!');
  
});


Pebble.addEventListener('gpsrequest', function(e) {
  console.log('Asking for GPS coords...');
  
  navigator.geolocation.getCurrentPosition(success, error, options);
});
