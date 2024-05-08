#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

#define LED_PIN 2
#define FEATHER_PIN 18
#define BUZZER_PIN 4

#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define LED_CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"
#define MOTOR_CHARACTERISTIC_UUID "43214321-1234-4321-1234-432112345678"
#define BUZZER_CHARACTERISTIC_UUID "21098765-8765-4321-4321-876543211098"

#define REST      0

BLECharacteristic *pLedCharacteristic;
BLECharacteristic *pMotorCharacteristic;
BLECharacteristic *pBuzzerCharacteristic;

BLEAdvertising *pAdvertising; // Global variable for advertising

bool ledState = false;
bool motorState = false;
bool buzzerState = false;

struct ServoMotorController {
    Servo servo;
    int pin;
    int angle;

    ServoMotorController(int servoPin) : pin(servoPin), angle(90) {  // Start at a neutral angle
        servo.attach(pin, 500, 2500);
    }

    void attach() {
        ESP32PWM::allocateTimer(0);  // Allocate a timer, each servo must have a different timer if used simultaneously
        servo.setPeriodHertz(50);    // Standard 50hz servo
        servo.attach(pin, 500, 2500);
    }
};

ServoMotorController featherServo(FEATHER_PIN);

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
            int frequency = notes[i];
            int duration = durations[i]; // duration in milliseconds
            if (frequency == 0) {  // Rest note
                delay(duration);
            } else {  // Play a tone
                playFrequency(frequency, duration);
            }
        }
    }

    void playFrequency(int frequency, int duration) {
        long delayValue = (long)(1000000 / frequency / 2); // Calculate the delay value between transitions
        long numCycles = frequency * duration / 1000; // Number of cycles of wave
        
        for (long i = 0; i < numCycles; i++) {
            digitalWrite(BUZZER_PIN, HIGH); // Write the buzzer pin high to push the diaphragm
            delayMicroseconds(delayValue); // Wait for the specified delayValue
            digitalWrite(BUZZER_PIN, LOW); // Write the buzzer pin low to pull the diaphragm
            delayMicroseconds(delayValue); // Wait for the specified delayValue
        }
    }

    void stopTone() {
        digitalWrite(buzzerPin, LOW); // Ensure no tone is playing when stopped
    }
};

// Notes to be played
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_D5 587

// Notes and durations
int melody[] = {
    NOTE_D5, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, 0,
    NOTE_D5, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, 0
};

int durations[] = {
    200, 200, 400,  // Decrease note duration from 250 to 200, pause from 500 to 400
    200, 200, 400,
    200, 200, 400,
    200, 200, 400
};

int totalNotes = sizeof(melody) / sizeof(int);  // Number of notes

float songSpeed = .7;

MusicPlayer myMusicPlayer(BUZZER_PIN, melody, durations, totalNotes, songSpeed);

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    featherServo.attach();

    BLEDevice::init("MeowMeow");
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

unsigned long lastMotorMoveTime = 0;
const long motorMoveInterval = 2000;

void loop() {
    static unsigned long lastAdvertiseTime = millis();
    unsigned long advertiseInterval = 30000;
    static unsigned long lastToggleTime = 0;
    const long toggleInterval = 500;
    unsigned long currentMillis = millis();
    
    static std::string previousBuzzerValue = "";
    std::string ledValue = pLedCharacteristic->getValue();
    std::string motorValue = pMotorCharacteristic->getValue();
    std::string buzzerValue = pBuzzerCharacteristic->getValue();

    Serial.print("Motor Value: ");
    Serial.println(motorValue.c_str());

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
        if (currentMillis - lastToggleTime > toggleInterval) {
            if (featherServo.angle == 0) {
                featherServo.servo.write(180);
                featherServo.angle = 180;
            } else {
                featherServo.servo.write(0);
                featherServo.angle = 0;
            }
            lastToggleTime = currentMillis;
        }
    } else {
        featherServo.servo.write(90);  // Neutral position when "ON" is not active
        featherServo.angle = 90;
    }

    if (buzzerValue == "PLAY") {
        myMusicPlayer.playTones();  // Start playing the melody
    } else if (buzzerValue == "STOP") {
        myMusicPlayer.stopTone();  // Stop the melody immediately
    }

    // Restart advertising if needed
    if (millis() - lastAdvertiseTime > advertiseInterval) {
        pAdvertising->stop();
        pAdvertising->start();
        lastAdvertiseTime = millis();
    }
}

