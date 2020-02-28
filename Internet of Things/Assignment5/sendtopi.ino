/* 
send light sensor values to pi through mqtt
receive messages from pi to either turn on or turn off LED 


*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi/MQTT parameters
#define WLAN_SSID "182RSu3"
#define WLAN_PASS "Redrum182u3"
#define BROKER_IP "10.0.0.179"
#define LED 5

// wifi client and mqqt client
WiFiClient client;
PubSubClient mqttclient(client);
int lightstate;
char str[5];

void callback(char *topic, byte *payload, unsigned int length)
{
    payload[length] = '\0'; // add null terminator to byte payload so we can treat it as a string

    if (strcmp((char *)payload, "on") == 0)
    {
        digitalWrite(LED, HIGH);  // turn on LED if message says "on"
    }
    else if (strcmp((char *)payload, "off") == 0)
    {
        digitalWrite(LED, LOW);  // turn off LED if message says "off"
    }
}
void setup()
{
    Serial.begin(115200);

    // connect to wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }

    // print wifi info
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());

    // connect to mqtt server
    mqttclient.setServer(BROKER_IP, 1883);
    connect();
}

void loop()
{
    if (!mqttclient.connected())
    { //make sure mqqt is connected
        connect();
    }

    lightstate = analogRead(A0);            // read the light state captured by the sensor
    sprintf(str, "%d", lightstate);         // convert light sensor value to string, so it can be sent through mqtt
    mqttclient.publish("/led", str, false); // send light state to pi through mqtt
    delay(250);
    mqttclient.loop(); // run client
}

// connect to mqtt
void connect()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Wifi issue"));
        delay(3000);
    }
    Serial.print(F("Connecting to MQTT server... "));
    while (!mqttclient.connected())
    {
        if (mqttclient.connect(WiFi.macAddress().c_str()))
        {
            Serial.println(F("MQTT server Connected!"));
        }
        else
        {
            Serial.print(F("MQTT server connection failed! rc="));
            Serial.print(mqttclient.state());
            Serial.println("try again in 10 seconds");
            // Wait 5 seconds before retrying
            delay(20000);
        }
    }
}