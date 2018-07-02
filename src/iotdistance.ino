
#include <ESP8266WiFi.h>

#define SWVER "1.0b"


#define PIN_LED  14
#define PIN_ECHO 13
#define PIN_TRIG 12

const char* ssid = "" ;
const char* passwd = "" ;

WiFiServer server(80) ;

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

  if ( duration == 0 )
    return 0 ;
  else
    return duration * 0.034 / 2 ; // cm/second
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
    delay(1000) ;
    Serial.print(".") ;
  }

  Serial.print("wifi connected to ") ;
  Serial.println (ssid);
  // led pin init
  pinMode(PIN_LED, OUTPUT) ;

  for (int i = 0 ; i < 4 ; i++) {
    delay(1000);
    digitalWrite(PIN_LED, HIGH) ;
    delay(1000);
    digitalWrite(PIN_LED, LOW) ;
  }

  server.begin() ;
}


String sheader  = "HTTP/1.1 200 OK\r\nContent-Type:application/json;\r\n\r\n" ;

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

  } else {
    Serial.println("invalid request") ;
    response += "{ \"invalid\": 1" ;
  }

  byte mac[6] ;
  WiFi.macAddress(mac) ;
  response += ", \"mac\": \""  ;
  response += String(mac[5], HEX) ;
  response += ":"  ;
  response += String(mac[4], HEX) ;
  response += ":"  ;
  response += String(mac[3], HEX) ;
  response += ":"  ;
  response += String(mac[2], HEX) ;
  response += ":"  ;
  response += String(mac[1], HEX) ;
  response += ":"  ;
  response += String(mac[0], HEX) ;
  response += "\", \"version\": \"" ;
  response += String(SWVER)  ;
  response += "\", \"sensor\": \"distance\"}\n" ;
  client.print(response) ;
  digitalWrite(PIN_LED, HIGH) ;
  delay(1200);
  digitalWrite(PIN_LED, LOW) ;
}
