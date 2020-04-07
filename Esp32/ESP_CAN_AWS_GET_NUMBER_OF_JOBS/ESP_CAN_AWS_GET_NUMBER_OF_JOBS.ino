#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#include <CAN.h>
#include <CAN_config.h>
#include <can_regdef.h>
#include <ESP32CAN.h>


CAN_device_t CAN_cfg;

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "myTopic/1"
#define AWS_IOT_SUBSCRIBE_TOPIC "myTopic/2"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

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
  Serial.println("Sending 0x00 to ECU");
}
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sensor_a0"] = analogRead(0);
  doc["Hall_Sensor"] = hallRead();
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  String mystring(message);
  Serial.println(mystring);
  if (mystring == "AJ1") {
    Serial.println("YES");
//    Serial.println("Sending Reply via MyTopic/1");
//    publishMessageResponse();
    RequestActiveJobs();
    Serial.println("Requesting Active Jobs number");
  } else {
    Serial.println("NO");
  }
}
void publishMessageResponse(int x) {
  StaticJsonDocument<200> doc;
  doc["Active_Jobs"] = x;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup() {
  Serial.begin(115200);
  connectAWS();//Sets up AWS connections and publish / subscribe topics
  
  CAN_cfg.speed = CAN_SPEED_250KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  //initialize CAN Module
  ESP32Can.CANInit();
  
}

void loop() {
  //publishMessage();
  //publishMessageResponse();
  client.loop();
  /////////////////////////////////
    CAN_frame_t rx_frame;
   if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    if (rx_frame.MsgID == 0x19FF5102) {//If a message from this message id comes over CAN then do something with it if its anything else dont bother
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
        if (rx_frame.data.u8[0] == 0 && rx_frame.data.u8[1] == 0) {
          int NumberOfActiveJobs = rx_frame.data.u8[2];
          publishMessageResponse(NumberOfActiveJobs);
          Serial.println("**************************************");
          Serial.println("Number of Active Jobs:" + (String)NumberOfActiveJobs);
          Serial.println("**************************************");
          for (int i = 0; i <= 7; i++) {
            Serial.print(rx_frame.data.u8[i], HEX);
          }
          Serial.print("\n");
        }
        else {
          for (int i = 0; i <= 7; i++) {
            Serial.print(rx_frame.data.u8[i], HEX);
          }
          Serial.print("\n");
        }
      }Serial.println("**************************************");
    }
  }
  /////////////////////////////////
}
