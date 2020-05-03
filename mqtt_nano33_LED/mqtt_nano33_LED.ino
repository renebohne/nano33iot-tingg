#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include "credentials.h"

#define LED_PIN   LED_BUILTIN

const char* ssid = networkSSID;
const char* password = networkPASSWORD;
const char* mqttServer = mqttSERVER;
const char* mqttUsername = mqttUSERNAME;
const char* mqttPassword = thingKEY;
const char* mqttDeviceId = thingID;

char subTopic[] = "ledControl";     //payload[0] will control/set LED
char pubTopic[] = "ledState";       //payload[0] will have ledState value

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledState = 0;

void setup_wifi() 
{
  delay(10);
  
  Serial.println();
  Serial.print("Connecting to wifi ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("topic: ");
  Serial.print(topic);
  Serial.print(" message: ");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') 
  {
    digitalWrite(LED_PIN, HIGH);   
    ledState = 1;
    char payLoad[1];
    itoa(ledState, payLoad, 10);
    client.publish(pubTopic, payLoad);
  } 
  else 
  {
    digitalWrite(LED_PIN, LOW); 
    ledState = 0;
    char payLoad[1];
    itoa(ledState, payLoad, 10);
    client.publish(pubTopic, payLoad);
  }

}

void reconnect() 
{
  while (!client.connected()) 
  {
    Serial.print("connecting to mqtt.tingg.io...");
 
    if (client.connect(mqttDeviceId, mqttUsername, mqttPassword)) 
    {
      Serial.println("connected.");
      client.subscribe(subTopic);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(LED_PIN, OUTPUT);     
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) 
  {
    lastMsg = now;
    char payLoad[1];
    itoa(ledState, payLoad, 10);
    client.publish(pubTopic, payLoad);
  }
}
