#include "SPIFFS.h"
String incomingstring;
void setup() {
Serial.begin(115200);

 
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  File file = SPIFFS.open("/test.txt", FILE_WRITE);
 
  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  if (file.print("CAN DATA\r\n")) {
    Serial.println("File was written");
  } else {
    Serial.println("File write failed");
  }
  String Data;
  Data = "Hello This is my Data\r\nIm just testing";
  if (file.print(Data)) {
    Serial.println("File was written\r\n");
  } else {
    Serial.println("File write failed\r\n");
  }
 
  file.close();

  File file2 = SPIFFS.open("/test.txt");
 
    if(!file2){
        Serial.println("Failed to open file for reading");
        return;
    }
 
    Serial.println("File Content:");
 
    while(file2.available()){
 
        Serial.write(file2.read());
    }
 
    file2.close();
}
  


void loop() {
 if(Serial.available()>0){
  incomingstring = Serial.readString();
  Serial.print("\r\nRECEIVED: ");
  Serial.print(incomingstring);
 }

}
