//#include <CAN.h>
#include <CAN.h>
#include <CAN_config.h>
#include <can_regdef.h>
#include <ESP32CAN.h>

CAN_device_t CAN_cfg;

void setup() {
  Serial.begin(115200);
  Serial.println("iotsharing.com CAN demo");
  CAN_cfg.speed = CAN_SPEED_250KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  //initialize CAN Module
  ESP32Can.CANInit();
}
String ReturnedJobInfo("");
void loop() {
  CAN_frame_t rx_frame;
  //receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    //    if (rx_frame.MsgID == 0x19FF5102) {//brings back number of active jobs first and then a list containing each JobNumber,CustomerID,JobID
    //    if (rx_frame.MsgID == 0x19FF5302) {
       if (rx_frame.MsgID == 0x19FF5202) {//gives back customer name ending in \0
   // if (rx_frame.MsgID == 0x19FF5302) {
      if (rx_frame.FIR.B.FF == CAN_frame_std) {
        printf("New standard frame\n");
      }
      else {
        printf("New extended frame ");
      }
      if (rx_frame.FIR.B.RTR == CAN_RTR) {
        printf(" RTR from 0x%08x, DLC %d\r\n", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      }
      else {
        printf("from 0x%08x, DLC %d\n", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        if (rx_frame.data.u8[0] == 0 && rx_frame.data.u8[1] == 5) {
            for (int i = 2; i <= 7; i++) {
              if(rx_frame.data.u8[i] != 0){
             //Serial.print((char)rx_frame.data.u8[i]);
              ReturnedJobInfo = ReturnedJobInfo + (char)rx_frame.data.u8[i];
              }else{
                Serial.print(ReturnedJobInfo);
                ReturnedJobInfo = "";
                break ;
              }
            }
            Serial.print("\n");
        }
        if (rx_frame.data.u8[0] == 0 && rx_frame.data.u8[1] == 0) {
          int NumberOfActiveJobs = rx_frame.data.u8[2];
          Serial.println("Number of Active Jobs:" + (String)NumberOfActiveJobs);
          for (int i = 0; i <= 7; i++) {
            Serial.print(rx_frame.data.u8[i], HEX);
          }
          Serial.print("\n");
        }
        else {

          for (int i = 0; i <= 7; i++) {
            Serial.print(rx_frame.data.u8[i],HEX);
          }
          Serial.print("\n");
        }
        
    }
  }
}
else {
  RequestActiveJobs();
 // RequestJobInfo();
   // RequestCustName();
  }
}

void RequestActiveJobs() {
  //Send message to ISO CAN terminal
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_ext;
  tx_frame.MsgID = 0x19FF5003;
  tx_frame.FIR.B.DLC = 8;
  tx_frame.data.u8[0] = 0x00;
  tx_frame.data.u8[1] = 0xFF;
  tx_frame.data.u8[2] = 0xFF;
  tx_frame.data.u8[3] = 0xFF;
  tx_frame.data.u8[4] = 0xFF;
  tx_frame.data.u8[5] = 0xFF;
  tx_frame.data.u8[6] = 0xFF;
  tx_frame.data.u8[7] = 0xFF;
  ESP32Can.CANWriteFrame(&tx_frame);
  Serial.println("Sending RequestActiveJobs [0x00] to ECU");
}
void RequestJobInfo() {
  //Send message to ISO CAN terminal
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_ext;
  tx_frame.MsgID = 0x19FF5003;
  tx_frame.FIR.B.DLC = 8;
  tx_frame.data.u8[0] = 0x02;
  tx_frame.data.u8[1] = 0x00;
  tx_frame.data.u8[2] = 0x05;
  tx_frame.data.u8[3] = 0xFF;
  tx_frame.data.u8[4] = 0x00;
  tx_frame.data.u8[5] = 0xFF;
  tx_frame.data.u8[6] = 0xFF;
  tx_frame.data.u8[7] = 0xFF;
  ESP32Can.CANWriteFrame(&tx_frame);
  Serial.println("Sending RequestJobInfo to ECU");
}
void RequestCustName() {
  //Send message to ISO CAN terminal
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_ext;
  tx_frame.MsgID = 0x19FF5003;
  tx_frame.FIR.B.DLC = 8;
  tx_frame.data.u8[0] = 0x01;
  tx_frame.data.u8[1] = 0x00;
  tx_frame.data.u8[2] = 0x04;
  tx_frame.data.u8[3] = 0xFF;
  tx_frame.data.u8[4] = 0xFF;
  tx_frame.data.u8[5] = 0xFF;
  tx_frame.data.u8[6] = 0xFF;
  tx_frame.data.u8[7] = 0xFF;
  ESP32Can.CANWriteFrame(&tx_frame);
  Serial.println("Sending RequestCustName to ECU");
}
