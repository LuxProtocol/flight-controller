#pragma once


// --- Buzzer Pins
#define BUZZER 15

// --- Note Frequencies
#define NOTE_Eb5  622
#define NOTE_Bb5  932
#define NOTE_Gb5  740
#define NOTE_Eb6  1245

// ---------------- Buzzer setup ----------------
void setupTune(){
  int startupMelody[] = { NOTE_Eb5, NOTE_Bb5, NOTE_Gb5, NOTE_Eb6 };
  // How long the note actually rings out (in milliseconds)
  int noteDurations[] = { 500, 250, 250, 1000 }; 
  int silenceGaps[]   = { 500,  250, 250,  0 };

  for (int i = 0; i < 4; i++) {
    // 1. Play the note
    tone(BUZZER, startupMelody[i]);
    delay(noteDurations[i]);
    
    // 2. Stop the note explicitly to create silence
    noTone(BUZZER);
    delay(silenceGaps[i]); 
  }
}
// ---------------- Buzzer setup ----------------