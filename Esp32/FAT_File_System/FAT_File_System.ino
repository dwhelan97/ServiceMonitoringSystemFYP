//Reference:
//https://techtutorialsx.com/2018/08/05/esp32-arduino-spiffs-reading-a-file/
#include "SPIFFS.h"
//String incomingstring;
//PrintToFAT();
//void PrintToFat(incomingstring);
void setup() {
Serial.begin(115200); 
 Serial.print("Enter what youd like to store and press enter: ");
}
void loop() {
 String incomingstring;
 
  if(Serial.available()>0){
  incomingstring = Serial.readString();

   if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  File file = SPIFFS.open("/test.txt", FILE_WRITE);
 
  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  if (file.print(incomingstring)) {
    Serial.println("File was written\r\n");
  } else {
    Serial.println("File write failed\r\n");
  }
    file.close();
  
  Serial.print("Going into READ");
  //void ReadFAT();
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
}
  
  
//void ReadFAT(){
//  File file2 = SPIFFS.open("/test.txt");
// 
//    if(!file2){
//        Serial.println("Failed to open file for reading");
//        return;
//    }
// 
//    Serial.println("File Content:");
// 
//    while(file2.available()){
// 
//        Serial.write(file2.read());
//    }
// 
//    file2.close();
//}
