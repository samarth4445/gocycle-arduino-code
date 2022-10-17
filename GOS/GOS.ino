#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>


TinyGPSPlus gps;
SoftwareSerial SerialGPS(19, 21); 

const char* ssid = "NIMBARGI";
const char* password = "samarth@2506";

const char* serverName = "https://gocycle-server.onrender.com/location";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;


WiFiServer server(80);
void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

void loop()
{
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
        int cycleid = 101;

        String httpRequestData = "{\"cycleid\": 101, ";
        httpRequestData += "\"longitude\": ";
        httpRequestData += LongitudeString;
        httpRequestData += ", \"latitude\": ";
        httpRequestData += LatitudeString;
        httpRequestData += "}";

        Serial.println(httpPOSTRequest(serverName, httpRequestData));

      }

    }
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  delay(100);

}

String httpGETRequest(const char* serverName) {  // ye code chapa hai
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();
    
  http.begin(client, serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}

int httpPOSTRequest(const char* serverName, String requestData){
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(requestData);

  return httpResponseCode;
}
