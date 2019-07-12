class ESP32_FTPClient
{
  private:
  void WriteClientBuffered(WiFiClient* cli, unsigned char * data, int dataLength);
  char outBuf[256];
  unsigned char outCount;
  WiFiClient client;
  WiFiClient dclient;
  char* userName;
  char* passWord;
  char* serverAdress;
  unsigned char clientBuf[1500];
  size_t bufferSize = 1500;
  WiFiClient* GetDataClient();
  
  public:
  ESP32_FTPClient(char* _serverAdress, char* _userName, char* _passWord);
  void OpenConnection();
  void CloseConnection();
  void NewFile (const char* fileName);
  void AppendFile( char* fileName);
  void WriteData (unsigned char * data, int dataLength);
  void CloseFile ();
  void GetFTPAnswer (char* result = NULL, int offsetStart = 0);
  void GetLastModifiedTime(const char* fileName, char* result);
  void RenameFile(char* from, char* to);
  void Write(const char * str);
  void InitFile(const char* type);
  void ChangeWorkDir(const char * dir);
  void DeleteFile(const char * file);
  void MakeDir(const char * dir);
  void ContentList(const char * dir, char * list);
};
