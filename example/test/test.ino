#include "LAN8720_ETHERNET.h"

byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x00, 0x01};
IPAddress myaddress(2, 2, 6, 10);
IPAddress dns(2, 0, 0, 2);
IPAddress gateway(2, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);



void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, myaddress, dns, gateway, subnet);
}

void loop() {

}
