#include <CAN.h>//CAN Libraries
#include <CAN_config.h>
#include <can_regdef.h>
#include <ESP32CAN.h>
//Com13

CAN_device_t CAN_cfg;
int incomingByte = 0; 
int randNumber;

void setup() {
    Serial.begin(115200);
    CAN_cfg.speed=CAN_SPEED_250KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //start CAN Module
    ESP32Can.CANInit();
    randomSeed(analogRead(0));
    
}

void loop() {
  if (Serial.available() > 0) {
incomingByte = Serial.read(); // read the incoming byte:
Serial.print(" I received:");
Serial.println(incomingByte);
}
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
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
//        for(int i = 0; i < 1; i++){
//          printf("%d", rx_frame.data.u8[i]);
//          //printf("Data %c: \n\r",rx_frame.data.u8[i]);
//        }
if(rx_frame.data.u8[0] = '0x03'){
  printf("Bale Weight Wanted\r\n");
  randNumber = random(40,80);
  rx_frame.data.u8;
  rx_frame.FIR.B.FF = CAN_frame_ext;
  rx_frame.MsgID = 0x19FF5102;
  rx_frame.FIR.B.DLC = 2;
  rx_frame.data.u8[0] = randNumber;
  rx_frame.data.u8[1] = 10;
  // Example
  //data.u8[0] = randNumber & 0xFF;
  //data.u8[1] = (randNumber >> 8) & 0xFF;
  // end
  ESP32Can.CANWriteFrame(&rx_frame);
  int val =1;
  for(int i = 0; i < 2; i++){
    val = val*rx_frame.data.u8[i];
    //printf("%i", rx_frame.data.u8[i]);
    //printf("Data %c: \n\r",rx_frame.data.u8[i]);
  }  
  printf("Multiplied Value is : %i",val);
}else{
  printf("Something Else");
}
      }

    }else
//    {
//      
//      rx_frame.data.u8;
//      rx_frame.FIR.B.FF = CAN_frame_std;
//      rx_frame.MsgID = 1;
//      rx_frame.FIR.B.DLC = 8;
//      rx_frame.data.u8[0] = 'h';
//      rx_frame.data.u8[1] = 'e';
//      rx_frame.data.u8[2] = 'l';
//      rx_frame.data.u8[3] = 'l';
//      rx_frame.data.u8[4] = 'o';
//      rx_frame.data.u8[5] = 'c';
//      rx_frame.data.u8[6] = 'a';
//      rx_frame.data.u8[7] = 'n';
//
//      //rx_frame.data = {'}
//      printf("Sending hellocan \n");
//      ESP32Can.CANWriteFrame(&rx_frame);
//      
//    }
    delay(5000);
}
