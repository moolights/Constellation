#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
int LED = 4;

void setup() {
  Serial.begin(19200);
  SerialBT.begin("ESP32");
  pinMode(LED, OUTPUT);
}

void loop() {
  
  if (SerialBT.available()) {
    char incoming = (char)SerialBT.read();
    Serial.println(incoming);

    switch (incoming) {
      case '1':
        digitalWrite(LED, HIGH);
        Serial.println("LED is On.");
        break;
       case '0':
        digitalWrite(LED, LOW);
        Serial.println("LED is Off");
        break;
    }
  }
  delay(20);
}
