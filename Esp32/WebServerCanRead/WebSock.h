#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "scopeLam.h"
#include "page2.h"

const char* ssid     = "di";
const char* password = "12345678";
WebServer server(80);   //instantiate server at port 80 (http port)

void wsSetup(){
  Serial.print("Setting AP (Access Point)…");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  server.on("/", [](){
    server.send(200, "text/html", homePage);
    Serial.println("Homepage sent");
  });
 
  server.on("/page2", [](){
    server.send(200, "text/html", page2);
    Serial.println("page2 sent");
  });

  server.begin(); 
  Serial.println("Web server started!");
}
 
void wsLoop(){
  server.handleClient();
}
 
