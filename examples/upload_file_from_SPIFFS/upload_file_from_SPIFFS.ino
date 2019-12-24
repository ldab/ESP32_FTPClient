/******************************************************************************

ESP32-CAM remote image access via FTP. Take pictures with ESP32 and upload it via FTP making it accessible for the outisde network.
Leonardo Bispo
July - 2019
https://github.com/ldab/ESP32_FTPClient

//1 - Install SPIFF Library and choose 2MB minimum for the file example in the data directory
https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/

Documentation about mandatories libraries
https://github.com/espressif/arduino-esp32/blob/master/libraries/FS/src/FS.cpp
https://github.com/espressif/arduino-esp32/blob/master/libraries/FS/src/FS.h
https://github.com/ldab/ESP32_FTPClient/blob/master/src/ESP32_FTPClient.h

//2 - Save the *.ino example

//3 - Configure WIFI and Server FTP & Run it

Distributed as-is; no warranty is given.

******************************************************************************/
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>
#include "octocat.h"
//Adding SPIFFS library
#include "SPIFFS.h"

//Update wifi parameters and ftp server parameters
#define WIFI_SSID ""
#define WIFI_PASS ""

char ftp_server[] = "";
char ftp_user[]   = "";
char ftp_pass[]   = "";

// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 2);

void setup()
{
  Serial.begin( 115200 );

  WiFi.begin( WIFI_SSID, WIFI_PASS );

  Serial.println("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Open Fle from SPIFFS and get stream
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/ESP32-WROOM-32_BANNER_1_0.jpg");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  ftp.OpenConnection();

  // Get directory content
  ftp.InitFile("Type A");
  String list[128];
  ftp.ContentList("", list);
  Serial.println("\nDirectory info: ");
  for(int i = 0; i < sizeof(list); i++)
  {
    if(list[i].length() > 0)
      Serial.println(list[i]);
    else
      break;
  }


  // Create the new file and send the image
  ftp.InitFile("Type I");
  ftp.NewFile("ESP32-WROOM.jpg");
  //Becarefull file reference
  ftp.WriteData(&file);
  ftp.CloseFile();

  //Close Stream from SPIFFS
  file.close();

  ftp.CloseConnection();
}

void loop()
{

}
