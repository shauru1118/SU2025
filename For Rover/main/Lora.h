#include <LoRa.h>

int LORA_Start() {
  return LoRa.begin(433E6);
}

void LORA_Resceive(char* endData) {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int i = 0;
    while (LoRa.available()) {
      endData[i] = (char)LoRa.read();
      i++;
    }
  }
}

void LORA_Send(char data[]) {
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
}