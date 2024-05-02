/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara and MoThunderz
 */
#include <string>
#include "BLEDevice.h"
#include <map>
//#include "BLEScan.h"

// Define UUIDs:
static BLEUUID serviceUUID;
static BLEUUID charUUID_1("beb5483e-36e1-4688-b7f5-ea07361b26a8");
std::map<std::string, BLEUUID> mymap;

// Some variables to keep track on device connected
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

// Define pointer for the BLE connection
static BLEAdvertisedDevice* myDevice;
BLERemoteCharacteristic* pRemoteChar_1;
BLEClient* pClient;

// Callback function for Notify function
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData,
                            size_t length,
                            bool isNotify) {
  if(pBLERemoteCharacteristic->getUUID().toString() == charUUID_1.toString()) {
    // Call back for updates in characteristic
    /*
    // convert received bytes to integer
    uint32_t counter = pData[0];
    for(int i = 1; i<length; i++) {
      counter = counter | (pData[i] << i*8);
    }

    // print to Serial
    Serial.print("Characteristic 1 (Notify) from server: ");
    Serial.println(counter ); 
    */ 
  }
}

// Callback function that is called whenever a client is connected or disconnected
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    // Connection
  }

  void onDisconnect(BLEClient* pclient) {
    // Disconnect
    connected = false;
  }
};

// Function that is run whenever the server is connected
bool connectToServer() {
  //Serial.print("Forming a connection to ");
  //Serial.println(myDevice->getAddress().toString().c_str());

  // Create client
  pClient  = BLEDevice::createClient();

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  // Connected to server

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    // Failed to find our service UUID
    pClient->disconnect();
    return false;
  }
  // Found our service

  connected = true;
  pRemoteChar_1 = pRemoteService->getCharacteristic(charUUID_1);
  if(connectCharacteristic(pRemoteService, pRemoteChar_1) == false)
    connected = false;

  if(connected == false) {
    // At least one characteristic UUID not found
    pClient-> disconnect();
    return false;
  }
  return true;
}

// Function to chech Characteristic
bool connectCharacteristic(BLERemoteService* pRemoteService, BLERemoteCharacteristic* l_BLERemoteChar) {
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  if (l_BLERemoteChar == nullptr) {
    // Failed to get characteristics
    return false;
  }
  
  // Got characteristics
  //Serial.println(" - Found characteristic: " + String(l_BLERemoteChar->getUUID().toString().c_str()));
  if(l_BLERemoteChar->canNotify())
    l_BLERemoteChar->registerForNotify(notifyCallback);
  return true;
}

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  //Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName().size() > 4 && advertisedDevice.getName().substr(0,4) == "Meow") {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      if (mymap.find(advertisedDevice.getName()) == mymap.end()) {
        Serial.println("New Paring!");
        mymap[advertisedDevice.getName()] = advertisedDevice.getServiceUUID(); 
      }
      serviceUUID = advertisedDevice.getServiceUUID();
  
      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
  
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;
  
      } // Found our server
    }
  } // onResult
}; // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  //Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      //Serial.println("We are now connected to the BLE Server.");
    } else {
      //Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    std::string rxValue = pRemoteChar_1->readValue();
    //Serial.print("Characteristic 1 (readValue): ");
    //Serial.println(rxValue.c_str());
    String txValue = "OFF";
    if (random(2)) {
      txValue = "ON";
    }
    // Write to char1
    pRemoteChar_1->writeValue(txValue.c_str(), txValue.length());
    if (txValue == "ON") {
      pClient->disconnect();
    }
    
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  // In this example "delay" is used to delay with one second. This is of course a very basic 
  // implementation to keep things simple. I recommend to use millis() for any production code
  delay(1000);
}