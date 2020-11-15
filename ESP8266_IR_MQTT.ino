#include <PubSubClient.h>
#include "heltec.h"
#include "ESP8266WiFi.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "config.h"


#ifndef STASSID
#define STASSID "<SSID>"
#define STAPSK  "<WIFI PSK>"
#endif

#ifndef MQTTHOST
#define MQTTHOST "<MQTT host>"
#define MQTTPORT 1883
#define MQTTUSER "<MQTT user>"
#define MQTTPASS "<MQTT password>"
#define MQTTCLIENT_ID "IR_bridge_1"
#define MQTTTOPIC "IR_bridge/1"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* mqttServer = MQTTHOST;
const uint16_t mqttPort = MQTTPORT;
const char* mqttUser = MQTTUSER;
const char* mqttPassword = MQTTPASS;
const char* mqttClient = MQTTCLIENT_ID;
const char* mqttTopic = MQTTTOPIC;

const int RECV_PIN = 0; //an IR detector connected to D3
const int IR_ADDRESS = 17795; //Matches a Sherwood R-125RDS in the Harmony config


WiFiClient espClient;
PubSubClient client(espClient);

IRrecv irrecv(RECV_PIN);
decode_results results;


void setup() {
  Serial.begin(115200);
  
  irrecv.enableIRIn(); // Start the IR receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(RECV_PIN);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
  WIFISetUp();

}




void loop() {
  if (irrecv.decode(&results)) {

    serialPrintUint64(results.value, HEX);
    Serial.print(":");
    serialPrintUint64(results.address, HEX);
    Serial.print(":");
    serialPrintUint64(results.command, HEX);
    Serial.println("");
    
    char irValue[16];
    itoa(results.value, irValue, 16);

    Heltec.display->clear();
    String dispString = String(irValue) + ":" + String(results.address) + ":" + String(results.command);
    Heltec.display->drawString(0, 0, "Received:");
    Heltec.display->drawString(0, 9, dispString);
    if (results.address == IR_ADDRESS){

      char outputString[16];
      itoa(results.command, outputString, 10);
      if (client.publish(mqttTopic, outputString)) {
        Heltec.display->drawString(0, 18, "Published to " + String(mqttTopic));
      } else {
        Heltec.display->drawString(0, 18, "Publishing failed");
      }    
    }
    Heltec.display->display();
    irrecv.resume(); // Receive the next value
  }
  delay(100);
  client.loop();
}



void WIFISetUp()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  WiFi.setAutoConnect(true);
  WiFi.begin(ssid,password);//fill in "Your WiFi SSID","Your Password"
  delay(100);
  
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Connecting to WiFi");
  Heltec.display->display();
  delay(100);
  
  WiFi.waitForConnectResult();
  Heltec.display->clear();
  if(WiFi.status() == WL_CONNECTED)
  {
    Heltec.display->drawString(0, 0, "Connected, IP address: ");
    IPAddress ip = WiFi.localIP();
    String ipString = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    Heltec.display->drawString(0, 9, ipString);
    Heltec.display->display();
    delay(1000);
    
    Heltec.display->clear();
    client.setServer(mqttServer, mqttPort);
    //client.setCallback(callback);
   
    while (!client.connected()) {
      
      Heltec.display->drawString(0, 0, "Connecting to MQTT...");
   
      if (client.connect(mqttClient, mqttUser, mqttPassword )) {
   
        Heltec.display->drawString(0, 9, "Connected.");  
        Heltec.display->display();
   
      } else {
   
        Heltec.display->drawString(0, 9, "Failed with state ");
        Heltec.display->drawString(0, 9, String(client.state()));
        Heltec.display->display();
        delay(2000);
   
      }
  }
 
  }
  else
  {
    Heltec.display->drawString(0, 0, "Setup failed");
    Heltec.display->drawString(0, 9, "WiFi Status: " + String(WiFi.status()));
    Heltec.display->display();
    delay(1000);
    Heltec.display->clear();
  }
}

