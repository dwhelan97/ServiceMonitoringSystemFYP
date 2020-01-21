/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <CAN.h>
#include <CAN_config.h>
#include <can_regdef.h>
#include <ESP32CAN.h>
#include <WiFi.h>

CAN_device_t CAN_cfg;

// Replace with your network credentials
const char* ssid     = "di";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String BaleState = "off";
String output27State = "off";
int k = 0;
int w = 0;

void wsSetup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  
  // Set outputs to LOW
  //digitalWrite(output26, LOW);
  //digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void wsLoop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              int SendandReceiveBaleInfo();
              k = SendandReceiveBaleInfo();
              //printf("The Bale Weight From Function is: %i",k);
              w = k;
              BaleState = "on";
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              BaleState = "off";
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Service Monitoring System</h1>");
            client.println("<p>BaleWeight  - State " + BaleState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (BaleState=="off") {
              //client.println("<p><a href=\"/26/on\"><button class=\"button\">Click to search last bale weight</button></a></p>");
              client.println("<p><a href=\"/26/on\"><button class=\"button\">Click to send to ECU</button></a></p>");
            } else {
              //client.printf("<p><a href=\"/26/off\"><button class=\"button button2\">%iKg\nClick to close</button></a></p>",w);
              client.printf("<p><a href=\"/26/off\"><button class=\"button button2\">\nClick to close</button></a></p>");
              printf("The Bale Weight is: %i",w);
            } 
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
int SendandReceiveBaleInfo(){
    CAN_cfg.speed=CAN_SPEED_250KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //initialize CAN Module
    ESP32Can.CANInit();
    int val;

    //Send message to ISO CAN terminal
    CAN_frame_t tx_frame;
    tx_frame.FIR.B.FF = CAN_frame_ext;
    //tx_frame.MsgID = 0x19FF5003;
    tx_frame.MsgID = 0x1CEF0102;
    //tx_frame.FIR.B.DLC = 1;
    tx_frame.FIR.B.DLC = 8;
    //tx_frame.data.u8[0] = 0x03; // Get bail weight
    //Serial.print("Sending 0x03");
    tx_frame.data.u8[0] = 60;
    tx_frame.data.u8[1] = 0x00;
      tx_frame.data.u8[2] = 0x00;
      tx_frame.data.u8[3] = 0x00;
      tx_frame.data.u8[4] = 0x00;
      tx_frame.data.u8[5] = 0x00;
      tx_frame.data.u8[6] = 0x00;
      tx_frame.data.u8[7] = 0xFF;
    ESP32Can.CANWriteFrame(&tx_frame);
    Serial.println("Sending 0x00 to ECU");
    delay(10000);
    //Send message to ISO CAN terminal
   // CAN_frame_t tx_frame;
    //tx_frame.FIR.B.FF = CAN_frame_ext;
    //tx_frame.MsgID = 0x19FF5003;
    tx_frame.MsgID = 0x1CEF0102;
    //tx_frame.FIR.B.DLC = 1;
    tx_frame.FIR.B.DLC = 8;
    //tx_frame.data.u8[0] = 0x03; // Get bail weight
    //Serial.print("Sending 0x03");
    tx_frame.data.u8[0] = 60;
    tx_frame.data.u8[1] = 0x01;
      tx_frame.data.u8[2] = 0x00;
      tx_frame.data.u8[3] = 0x00;
      tx_frame.data.u8[4] = 0x00;
      tx_frame.data.u8[5] = 0x00;
      tx_frame.data.u8[6] = 0x00;
      tx_frame.data.u8[7] = 0xFF;
    ESP32Can.CANWriteFrame(&tx_frame);
    Serial.println("Sending 0x01 to ECU");
    delay(10000);

    while(1){
          CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

      //do stuff!
      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("New standard frame");
      else
        printf("New extended frame");

      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
//        for(int i = 0; i < 1; i++){
          printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);        //  for(int i = 0; i < 1; i++){
//          printf("Bale Weight is: %d", rx_frame.data.u8[i]);
          int val =1;
          for(int i = 0; i < 2; i++){
            val = val*rx_frame.data.u8[i];
          }
          printf("Bale Weight Value is : %i",val);
          return val;
        }
        //printf("Bale Weight2 Value is : %i",val);
        
      }
}

}
