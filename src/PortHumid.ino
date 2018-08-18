



// Include the libraries we need
#include <Sensirion.h>
#include "SSD1306.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const uint8_t dataPin  =  13;
const uint8_t clockPin =  15;

float temperature;
float humidity;
float dewpoint;

const char* ssid = "EE-sthpqd";
const char* password = "aide-bat-sixty";
const char* mqtt_server = "192.168.1.133";

Sensirion tempSensor = Sensirion(dataPin, clockPin);

SSD1306  display(0x3c, 5, 4);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void)
{
  // start serial port
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
 
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{ 
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  tempSensor.measure(&temperature, &humidity, &dewpoint);

  
  display.clear();
  display.drawString(0,0, "R: "+String(humidity)+" %");
  display.drawString(0,32, "C: "+String(temperature)+" Deg.");
  display.drawString(0,48, "D: "+String(dewpoint)+" Deg.");
  display.display();

  
  dtostrf(humidity,7,2,msg);
  client.publish("PortHum",msg );

  dtostrf(temperature,7,2,msg);
  client.publish("PortHumTemp",msg );

  dtostrf(dewpoint,7,2,msg);
  client.publish("PortHumDP",msg );

  delay(5000);  
}
