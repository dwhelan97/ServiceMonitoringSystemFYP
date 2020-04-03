//#include <CAN.h>
#include <CAN.h>
#include <CAN_config.h>
#include <can_regdef.h>
#include <ESP32CAN.h>

CAN_device_t CAN_cfg;

void setup() {
    Serial.begin(115200);
    Serial.println("iotsharing.com CAN demo");
    CAN_cfg.speed=CAN_SPEED_250KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //initialize CAN Module
    ESP32Can.CANInit();
}

void loop() {
    CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    /*if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

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
         
        }
      }
    
else{*/
    //Send message to ISO CAN terminal
    CAN_frame_t tx_frame;
    tx_frame.FIR.B.FF = CAN_frame_ext;
    //tx_frame.MsgID = 0x19FF5003;
    tx_frame.MsgID = 0x1CEF0102;
    //tx_frame.FIR.B.DLC = 1;
    tx_frame.FIR.B.DLC = 8;
    //tx_frame.data.u8[0] = 0x03; // Get bail weight
    //Serial.print("Sending 0x03");
    //Mchale Visit 
    tx_frame.data.u8[0] = 0x60;
    tx_frame.data.u8[1] = 0x00;
      tx_frame.data.u8[2] = 0x00;
      tx_frame.data.u8[3] = 0x00;
      tx_frame.data.u8[4] = 0x00;
      tx_frame.data.u8[5] = 0x00;
      tx_frame.data.u8[6] = 0x00;
      tx_frame.data.u8[7] = 0xFF;
    ESP32Can.CANWriteFrame(&tx_frame);
    Serial.println("Sending 0x00 to ECU");
    delay(5000);
    //Send message to ISO CAN terminal
   // CAN_frame_t tx_frame;
    //tx_frame.FIR.B.FF = CAN_frame_ext;
    //tx_frame.MsgID = 0x19FF5003;
    tx_frame.MsgID = 0x1CEF0102;
    //tx_frame.FIR.B.DLC = 1;
    tx_frame.FIR.B.DLC = 8;
    //tx_frame.data.u8[0] = 0x03; // Get bail weight
    //Serial.print("Sending 0x03");
    tx_frame.data.u8[0] = 0x60;
    tx_frame.data.u8[1] = 0x01;
      tx_frame.data.u8[2] = 0x00;
      tx_frame.data.u8[3] = 0x00;
      tx_frame.data.u8[4] = 0x00;
      tx_frame.data.u8[5] = 0x00;
      tx_frame.data.u8[6] = 0x00;
      tx_frame.data.u8[7] = 0xFF;
    ESP32Can.CANWriteFrame(&tx_frame);
    Serial.println("Sending 0x01 to ECU");
    delay(5000);
    //Mchale Visit
//}
}
