#include "WebSock.h"

void setup() {
  Serial.begin(115200);
  wsSetup();
}

void loop(void){
  wsLoop();
}
