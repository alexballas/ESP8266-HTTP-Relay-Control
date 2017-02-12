#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
const char* ssid = "my_wifi_ssid";
const char* password = "my_wifi_password";

ESP8266WebServer server ( 80 );

const int relay = 13;

void handleRoot() {
	String temp = "<html>\
  <head>\
	<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />\
  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>\
  <script>\
  $(document).ready(function(){\
    $.get('status', function(data, status){\
          if (parseInt(data) == 0) {\
              $('#mainButton').html('OPEN');\
          } else {\
              $('#mainButton').html('CLOSE');\
          };\
    });\
		var clicked = false;\
    $('button').click(function(){\
			if (!clicked){\
				clicked = true;\
				setTimeout(function(){ clicked = false; }, 2000);\
			}else{\
				return false;\
			}\
	    $.get('status', function(data, status){\
	          if (parseInt(data) == 1) {\
	              $('#mainButton').html('OPEN');\
	              $.get('off', function(data1, status1){});\
	          } else {\
	              $('#mainButton').html('CLOSE');\
	              $.get('on', function(data2, status2){});\
	          };\
	    });\
    });\
  });\
  </script>\
   <title>NodeMCU HTTP relay</title>\
    <style>\
      body { background-size: cover; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; }\
      .button {\
    background-color: #4CAF50;\
    border: none;\
    color: white;\
    padding: 15px 32px;\
    text-align: center;\
    text-decoration: none;\
    display: inline-block;\
    font-size: 60px;\
    margin: 4px 2px;\
    cursor: pointer;\
  }\
    </style>\
  </head>\
  <body>\
    <button id='mainButton' class='button'></button>\
  </body>\
</html>";

	server.send ( 200, "text/html", temp );
}

void handleNotFound() {
	digitalWrite ( relay, 1 );
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
	digitalWrite ( relay, 0 );
}

void setup ( void ) {
	pinMode ( relay, OUTPUT );
	digitalWrite ( relay, 1 );
	Serial.begin ( 115200 );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( MDNS.begin ( "esp8266" ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on ( "/", handleRoot );
	server.on ( "/status", []() {
    String text;
    text += digitalRead ( relay );
		server.send ( 200, "text/plain", text );
	} );
  server.on ( "/on", []() {
    digitalWrite ( relay, 1 );
    server.send ( 200, "text/plain", "OK" );
  } );
  server.on ( "/off", []() {
    digitalWrite ( relay, 0 );
    server.send ( 200, "text/plain", "OK" );
  } );
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );
}

void loop ( void ) {
	server.handleClient();
}
