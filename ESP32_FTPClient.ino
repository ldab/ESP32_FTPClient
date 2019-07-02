#include <WiFi.h>
#include <WiFiClient.h>    
#include "ESP32_FTPClient.h"

#define ftp_server        "..."
#define ftp_username      "..."
#define ftp_password      "..."

#define wifi_ssid         "..."
#define wifi_password     "..."

ESP32_FTPClient ftp (ftp_server,ftp_username,ftp_password);



void setup() {
  Serial.begin(115200);
  WiFi.begin(wifi_ssid, wifi_password);
  
  Serial.println("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
      
  }
  Serial.println("IP address: ");
      
  Serial.println(WiFi.localIP());

  ftp.OpenConnection();
}

void loop() {
  delay(1000);
  
  //Create the file new and write a string into it
  ftp.InitFile("Type A");
  ftp.NewFile("codeaixtreme/hello_world.txt");
  ftp.Write("Hello World");
  ftp.CloseFile();

  //Append an string to a file (it does not need to exist)
  ftp.InitFile("Type A");
  ftp.AppendFile("codeaixtreme/hello_world_append.txt");
  ftp.Write("Hello World");
  ftp.CloseFile();
}
