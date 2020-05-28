#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <FS.h>
#include <PubSubClient.h>

#define DHT11_PIN 14
#define DHTTYPE DHT11


static const int RXPin = 4, TXPin = 5;          // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600;           // If Baud rate 9600 didn't work in your case then use 4800
char coord[50];
char ssid[] = "Zephes";                         // Name of your network (HotSpot or Router name)
char pass[] = "Afrika123";                      // Corresponding Password
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;


TinyGPSPlus gps; // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);                // The serial connection to the GPS device
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT11_PIN, DHTTYPE);



void setup()
{
  pinMode(14, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println();
  ss.begin(GPSBaud);
  smartDelay(1000);

  String temp;
  String gps;
  int numberofNetworks = WiFi.scanNetworks();

  WiFi.begin(ssid, pass);
  SPIFFS.begin();  
  
  if (!SPIFFS.exists("/test.txt"))
  {    
    File test = SPIFFS.open("/test.txt", "w");
    test.close();    
    ESP.deepSleep(20000);
  }
  File test1 = SPIFFS.open("/test.txt", "w");
  test1.print ('g');
  test1.println(displayInfo());
  test1.close();

  File test2 = SPIFFS.open("/test.txt", "w");
  test2.print ('t');
  test2.println(displayInfo());
  test2.close();

   for (int i =0; i < numberofNetworks; i++)
  {
    if (WiFi.SSID(i) == ssid)
    Serial.println ("Network Detected");
    //At home, upload data
  }
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client")) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
   
  File test3 = SPIFFS.open("/test.txt", "r");

  test3.readStringUntil('t');
  temp = test3.readStringUntil('\n');
  test3.readStringUntil('l');
  gps = test3.readStringUntil('\n');
  Serial.println(temp);
  Serial.println(gps);
  Serial.println(gps.c_str());
  Serial.println(temp.c_str());

  char testx [7] = "hello";
  

  
  client.publish("gps", temp.c_str());
  client.publish("gps", gps.c_str());
  client.publish("gps", testx);
  client.publish("gps", "hello");
  client.subscribe("gps");
    

  }

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
}


void loop()
{
  client.loop();  
}



static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } 
  while (millis() - start < ms);
      gps.encode(ss.read());    
}



const char* displayInfo()
{ 
  //if (gps.location.isValid() ) 
  {    
    float latitude = (gps.location.lat());                    //Storing the Lat. and Lon. 
    float longitude = (gps.location.lng()); 
        
    sprintf(coord, "%f, %f", latitude, longitude);
    delay (50);
    Serial.print("Temperature = ");
    Serial.println(dht.readTemperature());
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);                              // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);

    return coord;
  } 
}
