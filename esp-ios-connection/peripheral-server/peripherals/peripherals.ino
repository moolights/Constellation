#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_PIN 2
#define MOTOR_PIN 5
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
        int wholenote = (60000 * 4) / (160 * speed);  // Calculate the whole note duration
        for (int i = 0; i < totalNotes; i++) {
            int noteDuration = durations[i];
            bool dotted = noteDuration < 0;
            if (dotted) {
                noteDuration = -noteDuration;
            }
            int noteLength = wholenote / noteDuration;
            if (dotted) {
                noteLength = noteLength * 3 / 2;  // Adjust for dotted notes
            }
            int frequency = notes[i * 2];  // Note frequencies are every other entry
            if (frequency == REST) {
                noTone(buzzerPin);
            } else {
                tone(buzzerPin, frequency, noteLength);
            }
            delay(noteLength);  // Wait for the note to finish before moving to the next
        }
    }

    void stopTone() {}
};

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

int melody[] = {

    REST,1,
    REST,1,
    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,4, NOTE_G3,8, NOTE_G3,-4, NOTE_G3,8, NOTE_G3,4, 
    NOTE_G3,4, NOTE_G3,4, NOTE_G3,8, NOTE_G3,4,
    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,-1, 
};

const int durations[] = {
    1, 4, 4, 4, 4, 4, 8, -4, 4,
    4, 4, 4, 4, 4, 8, -4, 4,
    4, 4, 4, 4, 4, 8, -4, 4,
    4, 8, -4, 8, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 8, -4, 4,
    4, 4, 4, 4, 4, 8, -4, 4,
    4, 4, 4, 4, 4, 8, -4, 4, -1
};

int totalNotes = sizeof(melody) / (2 * sizeof(int));  // Number of note-duration pairs

MusicPlayer myMusicPlayer(BUZZER_PIN, melody, durations, totalNotes, 1.0);

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);

    BLEDevice::init("Meow");
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

    static std::string previousBuzzerValue = "";
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
    } else if (buzzerValue == "STOP") {
        Serial.print("IN THE STOP WRITE");
        if (buzzerState) {
            myMusicPlayer.stopTone();
        }
    }

    // Restart advertising if needed
    if (millis() - lastAdvertiseTime > advertiseInterval) {
        pAdvertising->stop();
        pAdvertising->start();
        lastAdvertiseTime = millis();
    }
}

