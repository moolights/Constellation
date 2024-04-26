#include <BLEDevice.h> // Gets info about connected devices or ones discovered
#include <BLEServer.h> // Models a server
#include <BLEUtils.h>
#include <BLE2902.h> // (CCCD) "Subscribes" to characteristic notifications i.e. Writing "ON"

#define LED_PIN 2
#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

BLECharacteristic *pCharacteristic;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    BLEDevice::init("Peripheral"); // Create a BLE device (name it how you want)
    BLEServer *pServer = BLEDevice::createServer(); // Establish it as a server
    BLEService *pService = pServer->createService(SERVICE_UUID); // Create a service given a UUID
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE  // Creates a characteristic enabling read and write
    );
    pCharacteristic->addDescriptor(new BLE2902()); // Descriptor adds more details to characteristic and "subscribes" to its notifications
    pService->start(); // Start the server
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); // Tells clients (e.g. iPhone) that the ESP is ready to be found and how it can be found
    pAdvertising->addServiceUUID(SERVICE_UUID); // Lets client devices know about this specific service
    BLEDevice::startAdvertising();
}

// Just a standard on and off LED program
void loop() {
    std::string value = pCharacteristic->getValue();
    if (value == "ON") {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
}
