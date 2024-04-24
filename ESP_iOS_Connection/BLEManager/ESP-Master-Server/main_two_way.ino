/*
  ESP-NOW Multi Unit Demo
  esp-now-multi.ino
  Broadcasts control messages to all devices in network
  Load script on multiple devices

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

uint8_t stored_maddr[16][6];
uint8_t num_maddr = 0;
uint8_t connect_status = 0;

struct_message *myData;

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
// Formats MAC Address
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

int addrComp(const uint8_t *or_addr, uint8_t len) {
  if (connect_status == 0) {
    return 0;
  }
  for (int i = 0; i < connect_status; i++) {
    for (int j = 0; j < len; j++) {
      if (or_addr[j] != stored_maddr[i][j]) {
        return 0;
      }
    }
  }
  return 1;
}


void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen)
// Called when data is received
{
  // Only allow a maximum of 250 characters in the message + a null terminating byte
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
  strncpy(buffer, (const char *)data, msgLen);

  // Make sure we are null terminated
  buffer[msgLen] = 0;

  // Format the MAC address
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  
  if (!addrComp(macAddr,6) && connect_status < 16){
    Serial.printf("Stored\n");
    memcpy(stored_maddr[connect_status], macAddr, 6);
    connect_status++;
  }
  Serial.printf("Connections: %d\n", connect_status);
  // Send Debug log message to the serial port
  Serial.printf("Received message from: %s - %s\n", macStr, buffer);
  memcpy(myData->a, "Yes", sizeof(byte) * 4);
  main_send((uint8_t *) myData,macAddr);
}


void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
// Called when data is sent
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void main_send(uint8_t *message, const uint8_t *macAddr)
// Emulates a broadcast
{
  // Broadcast a message to every device in range
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, macAddr, 6);
  if (!esp_now_is_peer_exist(macAddr))
  {
    esp_now_add_peer(&peerInfo);
  }
  // Send message
  esp_err_t result = esp_now_send(macAddr, message, sizeof(struct_message));

  // Print results to serial monitor
  if (result == ESP_OK)
  {
    Serial.println("Broadcast message success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESP-NOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else
  {
    Serial.println("Unknown error");
  }
}

void setup()
{

  // Set up Serial Monitor
  Serial.begin(115200);
  delay(1000);

  // Set ESP32 in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Broadcast Demo");

  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Disconnect from WiFi
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESP-NOW Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  }
  else
  {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }
  myData = (struct_message *) malloc(sizeof(struct_message));
}

void loop()
{
  if (connect_status) {
    delay(1000);
    int int_value = random(0,connect_status-1);
    memcpy(myData->a, "Yesnt", 6);
    myData->b = random(0,180);
    main_send((uint8_t *) myData,stored_maddr[int_value]);
  }
}
