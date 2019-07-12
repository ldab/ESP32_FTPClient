/******************************************************************************

ESP32-CAM remote image access via FTP. Take pictures with ESP32 and upload it via FTP making it accessible for the outisde network. 
Leonardo Bispo
July - 2019
https://github.com/ldab/ESP32_FTPClient

Distributed as-is; no warranty is given.

******************************************************************************/

#include <WiFi.h>
#include <WiFiClient.h> 
#include <ESP32_FTPClient.h>
#include "octocat.h"

#define WIFI_SSID ""
#define WIFI_PASS ""

char ftp_server[] = "ftp.drivehq.com";
char ftp_user[]   = "";
char ftp_pass[]   = "";

ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass);

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

  ftp.OpenConnection();

  // Get directory contentx
  ftp.InitFile("Type A");
  char list[256];
  ftp.ContentList("/My Pictures/", list);
  Serial.print("\nDirectory info: ");
  Serial.println(list);
  ftp.CloseFile();

  // Make a new directory
  ftp.InitFile("Type A");
  ftp.MakeDir("/jagua");  
  ftp.CloseFile();

  // Create the new file and send the image
  ftp.InitFile("Type A");
  ftp.ChangeWorkDir("/wwwhome/images");
  ftp.NewFile("octocat.png");
  ftp.WriteData( octocat_pic, sizeof(octocat_pic) );
  ftp.CloseFile();
  
  // Create the file new and write a string into it
  ftp.InitFile("Type A");
  ftp.NewFile("hello_world.txt");
  ftp.Write("Hello World");
  ftp.CloseFile();

  ftp.CloseConnection();
}

void loop()
{

}