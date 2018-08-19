#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

#define SWVER "1.1i"

#define PIN_ECHO 13
#define PIN_TRIG 12

const char* ssid = "" ;
const char* passwd = "" ;

WiFiServer server(80) ;

// set Over the Air updates to be on or off

#define OTA  1

int getdistance()
{
  long duration ;
  // clears the trig pin
  digitalWrite(PIN_TRIG, LOW );
  delayMicroseconds(2);
  // sends a 10 us pulse
  digitalWrite(PIN_TRIG, HIGH) ;
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW) ;
  // reads the echo
  duration = pulseIn(PIN_ECHO, HIGH) ;

  int distance = 0 ; 
  if ( duration == 0  || duration > 50000 ) {
    return distance  ;
  } else {
    distance = (int) ( duration * 0.034 / 2)  ; // cm/second
    return distance ; 
  }
}

void setup()
{
  Serial.begin(115200) ;
  delay(10) ;
  Serial.print("ESP Distance v") ;
  Serial.println(SWVER) ;
  // HC-SR04 setup
  pinMode(PIN_TRIG, OUTPUT) ;
  digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO, INPUT) ;
  // wifi setup
  WiFi.mode(WIFI_STA) ;
  WiFi.begin(ssid, passwd) ;

  while ( WiFi.status() != WL_CONNECTED) {
    delay(5000) ;
    Serial.print(".") ;
  }

  Serial.print("wifi connected to ") ;
  Serial.print (ssid);
  Serial.print(" addr:") ; 
  Serial.println(WiFi.localIP()) ; 
  server.begin() ;
}

int doupdate = false ; 

String sheader  = "HTTP/1.1 200 OK\r\nContent-Type:application/json\r\nConnecton: close\r\n\r\n" ;

void loop()
{

  WiFiClient client = server.available() ;

  if ( !client ) return ;

  while ( ! client.available() ) delay(1) ;

  String request = client.readStringUntil('\r');
  client.flush() ;
  String response = sheader ;

  if ( request.indexOf("/distance/get") != -1 ) {
    int distance = getdistance() ;
    // process get request
    response += "{ \"get\": 1"  ;
    response += ", \"distance\": " ;
    response +=  String(distance) ;

  } else if ( request.indexOf("/distance/info") != -1 ) {
    // process info request
    response += "{ \"info\": 1"  ;
    response += ", \"ipaddr\": \""  ;
    response += WiFi.localIP().toString() ;
    response += "\", \"ssid\": \"" ;
    response += String(ssid) ;
    response += "\", \"rssi\": " ;
    response += String(WiFi.RSSI()) ;

  } else if ( request.indexOf("/distance/updatefirmware") != -1 ) {
    response += "{ \"update\": 1"  ;
    response += ", \"ipaddr\": \""  ;
    response += WiFi.localIP().toString() ;
    doupdate = true ; 

  } else {
    Serial.println("invalid request") ;
    response += "{ \"invalid\": 1" ;
    response += ", \"request\": \"" ;
    response += RawJson(request) ; 
    response += "\"" ; 
  }

  byte mac[6] ;
  WiFi.macAddress(mac) ;
  response += ", \"mac\": \""  ;
  response += String(mac[0], HEX) ;
  response += ":"  ;
  response += String(mac[1], HEX) ;
  response += ":"  ;
  response += String(mac[2], HEX) ;
  response += ":"  ;
  response += String(mac[3], HEX) ;
  response += ":"  ;
  response += String(mac[4], HEX) ;
  response += ":"  ;
  response += String(mac[5], HEX) ;
  response += "\", \"version\": \"" ;
  response += String(SWVER)  ;
  response += "\", \"sensor\": \"distance\"}\n" ;
  client.print(response) ;
  Serial.println(response) ;
  delay(1) ; 
  client.stop(); 

  if ( doupdate ) {
      doupdate = false ; 

      String currentver = String("iotdistance-")  + String(SWVER) ; 

      t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.10.9/esp8266/firmware-latest.php",currentver) ;
      switch(ret) {
          case HTTP_UPDATE_FAILED: 
              Serial.println("update failed") ;
              break ; 
          case HTTP_UPDATE_NO_UPDATES: 
              Serial.println("no updates available") ;
              break ; 
          case HTTP_UPDATE_OK: 
              Serial.println("update ok") ;
              break ; 
      }
  }
}
