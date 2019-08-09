#include <WiFiClient.h>
#include "ESP32_FTPClient.h"

ESP32_FTPClient::ESP32_FTPClient(char* _serverAdress, char* _userName, char* _passWord){
  userName = _userName;
  passWord = _passWord;
  serverAdress = _serverAdress;
}

WiFiClient* ESP32_FTPClient::GetDataClient() {
  return &dclient;
}

void ESP32_FTPClient::GetLastModifiedTime(const char  * fileName, char* result) {
  Serial.println("Send MDTM");
  client.print(F("MDTM "));
  client.println(F(fileName));
  GetFTPAnswer (result, 4);
}

void ESP32_FTPClient::WriteClientBuffered(WiFiClient* cli, unsigned char * data, int dataLength) {
  
  size_t clientCount = 0;
  for(int i = 0; i < dataLength;i++){
    clientBuf[clientCount] = data[i];
    //client.write(data[i])
    clientCount++;
    if (clientCount > bufferSize-1) {
        cli->write(clientBuf, bufferSize); 
      clientCount = 0;
    }
  }
  if (clientCount > 0){
      cli->write(clientBuf, clientCount); 
  }
}

void ESP32_FTPClient::GetFTPAnswer (char* result, int offsetStart) {
  char thisByte;
  outCount = 0;

  while (!client.available()) delay(1);

  while (client.available()) {
    thisByte = client.read();
    if (outCount < sizeof(outBuf)) {
      outBuf[outCount] = thisByte;
      outCount++;
      outBuf[outCount] = 0;
    }
  }
  if(result != NULL){
    Serial.println("Result start");
    for(int i = offsetStart; i<sizeof(outBuf); i++){
      result[i] = outBuf[i - offsetStart];
    }
    Serial.print("Result: ");
    Serial.write(result);
    Serial.println("Result end");
  }
}

void ESP32_FTPClient::WriteData (unsigned char * data, int dataLength) {
  Serial.println(F("Writing"));
  WriteClientBuffered(&dclient, &data[0], dataLength);
}

void ESP32_FTPClient::CloseFile () {
  Serial.println(F("Close File"));
  dclient.stop();
  GetFTPAnswer();
}

void ESP32_FTPClient::Write(const char * str) {
  GetDataClient()->print(str);
}

void ESP32_FTPClient::CloseConnection() {
  client.println(F("QUIT"));
  client.stop();
  Serial.println(F("Connection closed"));
}

void ESP32_FTPClient::OpenConnection() {
  Serial.print(F("Connecting to: "));
  Serial.println(serverAdress);
  if (client.connect(serverAdress, 21)) {  // 21 = FTP server
    Serial.println(F("Command connected"));
  } 
  GetFTPAnswer();
  

  Serial.println("Send USER");
  client.print(F("USER "));
  client.println(F(userName));
  GetFTPAnswer();

  Serial.println("Send PASSWORD");
  client.print(F("PASS "));
  client.println(F(passWord));
  GetFTPAnswer();
  
  Serial.println("Send SYST");
  client.println(F("SYST"));
  GetFTPAnswer();
}

void ESP32_FTPClient::RenameFile(char* from, char* to) {
  Serial.println("Send RNFR");
  client.print(F("RNFR "));
  client.println(F(from));
  GetFTPAnswer();

  Serial.println("Send RNTO");
  client.print(F("RNTO "));
  client.println(F(to));
  GetFTPAnswer();
}

void ESP32_FTPClient::NewFile (const char* fileName) {
  Serial.println("Send STOR");
  client.print(F("STOR "));
  client.println(F(fileName));
  GetFTPAnswer();
}

void ESP32_FTPClient::InitFile(const char* type){
  Serial.print("Send ");
  Serial.println(type);
  client.println(F(type));
  GetFTPAnswer();

  Serial.println("Send PASV");
  client.println(F("PASV"));
  GetFTPAnswer();

  char *tStr = strtok(outBuf, "(,");
  int array_pasv[6];
  for ( int i = 0; i < 6; i++) {
    tStr = strtok(NULL, "(,");
    if (tStr == NULL) {
      Serial.println(F("Bad PASV Answer"));
      break;
    }
    array_pasv[i] = atoi(tStr);
  }
  unsigned int hiPort, loPort;
  hiPort = array_pasv[4] << 8;
  loPort = array_pasv[5] & 255;

  Serial.print(F("Data port: "));
  hiPort = hiPort | loPort;
  Serial.println(hiPort);
  if (dclient.connect(serverAdress, hiPort)) {
    Serial.println(F("Data connection etablished"));
  }
}

void ESP32_FTPClient::AppendFile (char* fileName) {
  Serial.println("Send APPE");
  client.print(F("APPE "));
  client.println(F(fileName));
  GetFTPAnswer();
}

void ESP32_FTPClient::ChangeWorkDir(const char * dir) {
  Serial.println("Send CWD");
  client.print(F("CWD "));
  client.println(F(dir));
  GetFTPAnswer();
}

void ESP32_FTPClient::DeleteFile(const char * file) {
  Serial.println("Send DELE");
  client.print(F("DELE "));
  client.println(F(file));
  GetFTPAnswer();
}

void ESP32_FTPClient::MakeDir(const char * dir) {
  Serial.println("Send MKD");
  client.print(F("MKD "));
  client.println(F(dir));
  GetFTPAnswer();
}

void ESP32_FTPClient::ContentList(const char * dir, String * list) {
  char _resp[ sizeof(outBuf) ];
  uint16_t _b = 0;
  
  Serial.println("Send MLSD");
  client.print(F("MLSD"));
  client.println(F(dir));
  GetFTPAnswer(_resp);

  // Convert char array to string to manipulate and find response size
  // each server reports it differently, TODO = FEAT
  //String resp_string = _resp;
  //resp_string.substring(resp_string.lastIndexOf('matches')-9);
  //Serial.println(resp_string);

  while( !dclient.available() ) delay(1);

  while(dclient.available()) 
  {
    if( _b < 128 )
    {
      list[_b] = dclient.readStringUntil('\n');
      //Serial.println(String(_b) + ":" + list[_b]);
      _b++;
    }
  }

}

void ESP32_FTPClient::DownloadString(const char * filename, String &str) {
  Serial.println("Send RETR");
  client.print(F("RETR "));
  client.println(F(filename));

  char _resp[ sizeof(outBuf) ];
  GetFTPAnswer(_resp);

  while( !GetDataClient()->available() ) delay(1);
  
  while( GetDataClient()->available() )
  {
    str += GetDataClient()->readString();
  }

}

void ESP32_FTPClient::DownloadFile(const char * filename, unsigned char * buf, size_t length, bool printUART ) {
  Serial.println("Send RETR");
  client.print(F("RETR "));
  client.println(F(filename));
  
  char _resp[ sizeof(outBuf) ];    
  GetFTPAnswer(_resp);

  char _buf[2];

  while( !dclient.available() ) delay(1);

  while(dclient.available()) 
  {
    if( !printUART ) 
      dclient.readBytes(buf, length);

    else
    {
      for(size_t _b = 0; _b < length; _b++ )
      {
        dclient.readBytes(_buf, 1),
        Serial.print(_buf[0], HEX);
      }
    }
  }
}
