#include <WiFi.h>
//#include <PubSubClient.h>//already included in thingesp module.
#include <DHT.h>;
#include <ThingESP.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;



//wifi credintials
const char* ssid = "AndroidAP_275";
const char* password = "password marchipoya";

//thingsboard credintials
#define TOKEN "topsecret123" //Access token of device Display
char ThingsboardHost[] = "demo.thingsboard.io";

//thingesp credintials
ThingESP32 thing("deekshith", "WeatherReport", "123456789");


WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

//DHT22 pins and intisilization
#define DHTPIN 15   
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

int chk;
float hum;  
float temp;
float f;

void setup()
{
  Serial.begin(115200);
  dht.begin(); //starts dht
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("connected to");
  Serial.println(ssid);
  client.setServer( ThingsboardHost, 1883 );
  thing.initDevice();//begin device
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }
}

void loop()
{
  if ( !client.connected() ) 
  {
      reconnect();
  }
  getAndSendTemperatureAndHumidityData();
  thing.Handle();
  delay(2000);
}

void getAndSendTemperatureAndHumidityData()
{
  
  hum = dht.readHumidity();
  temp= dht.readTemperature();
   f = dht.readTemperature(true);
  // Prepare a JSON payload string
  String payload = "{";
 payload += "\"Humidity\":";payload += hum; payload += ",";
 payload += "\"Temperature\":";payload += temp; 
 payload +="\"Pressure\":";payload += bmp.readPressure();
  payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish("v1/devices/me/telemetry", attributes);
  Serial.println( attributes );
   
}

String HandleResponse(String query)
{
  String res = " Humidity: "+(String)hum+"%\nTemperatue: "+(String)temp +"°C, " + (String)f + "°F \nPressure: " + (String)bmp.readPressure()+" pa";
  if (query == "status") {
    return res;
  }
  else if(query =="hi") {
    return "Hello, Welcome to Weather Report ChatBot \n enter \"Status\": to get weather details";
  }
  else return "Possible commands are 'status'";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Esp8266", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 5 seconds]" );
      delay( 5000 );
    }
  }
}
