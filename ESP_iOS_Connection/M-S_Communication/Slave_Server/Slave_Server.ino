/*
  Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
  Ported to Arduino ESP32 by Evandro Copercini
  updated by chegewara and MoThunderz
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

#define LED_PIN 2
#define SERVO_PIN 23

// Initialize all pointers
BLEServer* pServer = NULL;                        // Pointer to the server
BLECharacteristic* pCharacteristic_1 = NULL;      // Pointer to Characteristic 1
BLECharacteristic* pCharacteristic_2 = NULL;      // Pointer to Characteristic 2
BLECharacteristic* pCharacteristic_3 = NULL;      // Pointer to Characteristic 3
BLE2902 *pBLE2902_1;                              // Pointer to BLE2902 of Characteristic 1
BLE2902 *pBLE2902_2;                              // Pointer to BLE2902 of Characteristic 2
BLE2902 *pBLE2902_3;                              // Pointer to BLE2902 of Characteristic 3

// Some variables to keep track on device connected
bool deviceConnected = false;
bool oldDeviceConnected = false;

Servo myServo;
int servo_pos = 0;
bool on_servo = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// UUIDs used in this example:
#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_2 "4d3f12b8-78d2-48c2-8997-da30fbd08c26"
#define CHARACTERISTIC_UUID_3 "114c4080-cae6-42c3-bc89-35e465c41219"

// Callback function that is called whenever a client is connected or disconnected
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("connection");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("disconnection");
      deviceConnected = false;
    }
};

void checkToReconnect() //added
{
  // disconnected so advertise
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Disconnected: start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connected so reset boolean control
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    Serial.println("Reconnected");
    oldDeviceConnected = deviceConnected;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // Create the BLE Device
  BLEDevice::init("Meow1");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_1 = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_1,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
                      
  pCharacteristic_2 = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_2,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
                      
  pCharacteristic_3 = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_3,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  // Create a BLE Descriptor
  pBLE2902_1 = new BLE2902();
  pBLE2902_1->setNotifications(true);
  pCharacteristic_1->addDescriptor(pBLE2902_1);

  pBLE2902_2 = new BLE2902();
  pBLE2902_2->setNotifications(true);
  pCharacteristic_2->addDescriptor(pBLE2902_2);

  pBLE2902_3 = new BLE2902();
  pBLE2902_3->setNotifications(true);
  pCharacteristic_3->addDescriptor(pBLE2902_3);

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  myServo.attach(SERVO_PIN);
  myServo.write(0);
}

void loop() {
  checkToReconnect();
  // notify changed value
  if (deviceConnected) {
    std::string value_1 = pCharacteristic_1->getValue();
    std::string value_2 = pCharacteristic_2->getValue();
    std::string value_3 = pCharacteristic_3->getValue();
    if (value_1 == "ON") {
      digitalWrite(LED_PIN, HIGH);
      on_servo = true;
    } else {
      digitalWrite(LED_PIN, LOW);
      //on_servo = false;
    }
    //Serial.print("Value 2: ");
    //Serial.println(value_2.c_str());
    //Serial.print("Value 3: ");
    //Serial.println(value_3.c_str());
    delay(1000);
  }
  Serial.println(on_servo);
  if (on_servo && servo_pos < 180) {
    servo_pos += 45;
    myServo.write(servo_pos);
    delay(1000);
  } else if (on_servo && servo_pos == 180) {
    on_servo = false;
    //delay(1000);
  } else if (!on_servo && servo_pos > 0) {
    servo_pos =0;
    myServo.write(servo_pos);
    Serial.println(servo_pos);
    delay(1000);
  } 
}
