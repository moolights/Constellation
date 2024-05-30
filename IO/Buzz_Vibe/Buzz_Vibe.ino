#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_PIN 2
#define MOTOR_PIN 5
#define BUZZER_PIN 13

#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define LED_CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"
#define MOTOR_CHARACTERISTIC_UUID "43214321-1234-4321-1234-432112345678"
#define BUZZER_CHARACTERISTIC_UUID "21098765-8765-4321-4321-876543211098"

BLECharacteristic *pLedCharacteristic;
BLECharacteristic *pMotorCharacteristic;
BLECharacteristic *pBuzzerCharacteristic;

BLEAdvertising *pAdvertising; // Global variable for advertising

bool ledState = false;
bool motorState = false;
bool buzzerState = false;

struct MusicPlayer {
    int buzzerPin;
    const int *notes;
    const int *durations;
    int totalNotes;
    float speed;

    MusicPlayer(int pin, const int notesArray[], const int durationsArray[], int numNotes, float songSpeed)
    : buzzerPin(pin), notes(notesArray), durations(durationsArray), totalNotes(numNotes), speed(songSpeed) {
        pinMode(buzzerPin, OUTPUT);
    }

    void playTones() {
        for (int i = 0; i < totalNotes; i++) {
            int currentNote = notes[i];
            float duration = durations[i] / speed;
            if (currentNote != 0) {
                tone(buzzerPin, currentNote, duration);
            } else {
                noTone(buzzerPin);
            }
            delay(duration);
        }
    }
};

#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_D5 587

const int notes[] = {NOTE_D5, NOTE_B4, 0, NOTE_A4, NOTE_G4, 0, NOTE_D5, NOTE_B4, 0, NOTE_A4, NOTE_G4, 0};
const int durations[] = {125, 125, 375, 125, 125, 375, 125, 125, 375, 125, 125, 375};
const int totalNotes = sizeof(notes) / sizeof(int);

MusicPlayer myMusicPlayer(BUZZER_PIN, notes, durations, totalNotes, 0.5);

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);

    BLEDevice::init("MeowESP32");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pLedCharacteristic = pService->createCharacteristic(
        LED_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pMotorCharacteristic = pService->createCharacteristic(
        MOTOR_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pBuzzerCharacteristic = pService->createCharacteristic(
        BUZZER_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );

    pLedCharacteristic->addDescriptor(new BLE2902());
    pMotorCharacteristic->addDescriptor(new BLE2902());
    pBuzzerCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
}

void loop() {
    static unsigned long lastAdvertiseTime = millis();
    unsigned long advertiseInterval = 30000;

    std::string ledValue = pLedCharacteristic->getValue();
    std::string motorValue = pMotorCharacteristic->getValue();
    std::string buzzerValue = pBuzzerCharacteristic->getValue();

    if (ledValue == "ON") {
        if (!ledState) {
            digitalWrite(LED_PIN, HIGH);
            ledState = true;
        }
    } else if (ledValue == "OFF") {
        if (ledState) {
            digitalWrite(LED_PIN, LOW);
            ledState = false;
        }
    }

    if (motorValue == "ON") {
        if (!motorState) {
            digitalWrite(MOTOR_PIN, HIGH);
            motorState = true;
        }
    } else if (motorValue == "OFF") {
        if (motorState) {
            digitalWrite(MOTOR_PIN, LOW);
            motorState = false;
        }
    }

    if (buzzerValue == "PLAY") {
        myMusicPlayer.playTones();
    } 

    // Restart advertising if needed
    if (millis() - lastAdvertiseTime > advertiseInterval) {
        pAdvertising->stop();
        pAdvertising->start();
        lastAdvertiseTime = millis();
    }
}
