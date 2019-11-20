#include "WebSock.h"

void setup(void) {
  Serial.begin(115200);
  wsSetup();
}

void loop(void) {
   wsLoop();
}
