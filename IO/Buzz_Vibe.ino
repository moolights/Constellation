// Define pin 4 for buzzer and vibrating motor
const int buzzer = 4;

const float songSpeed = 0.5;

// Defining frequency
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_D5 587

// Music notes, 0 is pulse
int notes[] = {
    NOTE_D5, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, 0,
    NOTE_D5, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, 0};
// Durations in ms
int durations[] = {
    125, 125, 375,
    125, 125, 375,
    125, 125, 375,
    125, 125, 375};

void setup()
{
  const int totalNotes = sizeof(notes) / sizeof(int);
  // Loop through each note
  for (int i = 0; i < totalNotes; i++)
  {
    const int currentNote = notes[i];
    float wait = durations[i] / songSpeed;
    // Play tone if currentNote is not 0 frequency, otherwise pause (noTone)
    if (currentNote != 0)
    {
      tone(buzzer, notes[i], wait); // tone(pin, frequency, duration)
    }
    else
    {
      noTone(buzzer);
    }
    // delay is used to wait for tone to finish playing before moving to next loop
    delay(wait);
  }
}

void loop()
{
}
