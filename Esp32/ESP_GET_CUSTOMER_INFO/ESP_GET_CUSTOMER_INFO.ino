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

void loop() {
  CAN_frame_t rx_frame;
  //receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    if (rx_frame.MsgID == 0x19FF5102) {//If a message from this message id comes over CAN then do something with it if its anything else dont bother
      if (rx_frame.FIR.B.FF == CAN_frame_std) {
        printf("New standard frame\n");
      }
      else {
        printf("New extended frame\n");
      }
      if (rx_frame.FIR.B.RTR == CAN_RTR) {
        printf(" RTR from 0x%08x, DLC %d\r\n", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      }
      else {
        //        for(int i = 0; i < 1; i++){
        printf(" from 0x%08x, DLC %d\n", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for (int i = 0; i <= 8; i++) {
          printf(".Received Data is: %d\n", rx_frame.data.u8[i]);

        }
      }
    }
    else {
      Serial.print("Not What You Want\n");
    }
  }

  else {
    //Send message to ISO CAN terminal
    CAN_frame_t tx_frame;
    tx_frame.FIR.B.FF = CAN_frame_ext;
    tx_frame.MsgID = 0x19FF5003;
    //tx_frame.MsgID = 0x1CEF0102;
    //tx_frame.FIR.B.DLC = 1;
    tx_frame.FIR.B.DLC = 8;
    //tx_frame.data.u8[0] = 0x03; // Get bail weight
    //Serial.print("Sending 0x03");
    tx_frame.data.u8[0] = 0x00;
    tx_frame.data.u8[1] = 0xFF;
    tx_frame.data.u8[2] = 0xFF;
    tx_frame.data.u8[3] = 0xFF;
    tx_frame.data.u8[4] = 0xFF;
    tx_frame.data.u8[5] = 0xFF;
    tx_frame.data.u8[6] = 0xFF;
    tx_frame.data.u8[7] = 0xFF;
    ESP32Can.CANWriteFrame(&tx_frame);
    Serial.println("Sending 0x00 to ECU");
  }
}
