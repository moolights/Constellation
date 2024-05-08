#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <map>

#define DEVICE_INFO_SERVICE_UUID        "0000180A-0000-1000-8000-00805F9B34FB"
#define DEVICE_INFO_CHARACTERISTIC_UUID "00002A50-0000-1000-8000-00805F9B34FB"

#define SCAN_TIME 10 // Scan for 10 seconds

std::map<std::string, std::vector<std::pair<std::string, std::string>>> deviceServices; // Maps device address to a list of services and their characteristics

bool connectToServer(BLEAdvertisedDevice device);

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        Serial.println("Device found: " + String(advertisedDevice.toString().c_str()));
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(DEVICE_INFO_SERVICE_UUID))) {
            Serial.println("Found Our Service on " + String(advertisedDevice.getAddress().toString().c_str()));
            advertisedDevice.getScan()->stop();
            connectToServer(advertisedDevice);
        }
    }
};

BLECharacteristic *pDeviceInfoCharacteristic = nullptr; // Declare globally, remove any duplicate declarations

bool connectToServer(BLEAdvertisedDevice device) {
    BLEClient* pClient = BLEDevice::createClient();
    Serial.println("Connecting to " + String(device.getAddress().toString().c_str()));

    // Connect to the BLE Server.
    if (!pClient->connect(&device)) {
        Serial.println("Failed to connect to server.");
        return false;
    }
    Serial.println("Connected to the server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(BLEUUID(DEVICE_INFO_SERVICE_UUID));
    if (pRemoteService == nullptr) {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(DEVICE_INFO_SERVICE_UUID);
        return false;
    }
    Serial.println("Found our service");

    discoverCharacteristics(pRemoteService, device.getAddress().toString(), pDeviceInfoCharacteristic);

    pClient->disconnect();
    return true;
}

void discoverCharacteristics(BLERemoteService* pRemoteService, std::string address, BLECharacteristic* pDeviceInfoCharacteristic) {
    std::vector<std::pair<std::string, std::string>> charUuids;
    std::string deviceInfo = "Device: " + address + "\n";

    auto characteristics = pRemoteService->getCharacteristics();
    for (auto& characteristic : *characteristics) {
        BLERemoteCharacteristic* pRemoteCharacteristic = characteristic.second;
        std::string characteristicValue = pRemoteCharacteristic->readValue();
        charUuids.push_back({pRemoteCharacteristic->getUUID().toString(), characteristicValue});
        deviceInfo += "Characteristic: " + pRemoteCharacteristic->getUUID().toString() + "\n";
    }

    deviceServices[address] = charUuids;

    pDeviceInfoCharacteristic->setValue(deviceInfo);
    pDeviceInfoCharacteristic->notify();
}


BLEServer *pServer = nullptr;

void setup() {
    Serial.begin(115200);
    BLEDevice::init("Bridge");

    // Create BLE Server
    pServer = BLEDevice::createServer();

    // Create Device Info Service
    BLEService *pDeviceInfoService = pServer->createService(BLEUUID(DEVICE_INFO_SERVICE_UUID));

    // Create a characteristic for device info
    pDeviceInfoCharacteristic = pDeviceInfoService->createCharacteristic(
        BLEUUID(DEVICE_INFO_CHARACTERISTIC_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );

    // Start the service
    pDeviceInfoService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLEUUID(DEVICE_INFO_SERVICE_UUID));
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    // Initialize BLE Scan
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void loop() {
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults foundDevices = pBLEScan->start(SCAN_TIME, false);
    Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    delay(2000);
}

