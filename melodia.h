#ifndef MELODIA_H
#define MELODIA_H

#include <Arduino.h>

// ==========================================
// CONFIGURACIÓN DEL BUZZER (PKM22EPP-40)
// ==========================================
#define BUZZER 2  // Pin libre del ESP32-C3 Super Mini

// Notas musicales (octava alta para que el PKM22EPP-40 suene con buen volumen)
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760
#define NOTE_B6  1976
#define NOTE_C7  2093

// Melodía de ejemplo (intro clásica tipo Mario Bros)
int melodia[] = { NOTE_E6, NOTE_E6, 0, NOTE_E6, 0, NOTE_C6, NOTE_E6, NOTE_G6 };
int duracionNotas[] = { 120, 120, 120, 120, 120, 120, 200, 300 };
int totalNotas = 8;

bool tocandoMelodia = false;
int notaActual = 0;
unsigned long tiempoNota = 0;

// Configura el pin y da un pitido corto a 4000 Hz (resonancia ideal del PKM22EPP-40)
void iniciarBuzzer() {
  pinMode(BUZZER, OUTPUT);
  tone(BUZZER, 4000, 150);
}

// Inicia la reproducción de la melodía al presionar el botón
void tocarBocina() {
  Serial.println("-> MELODIA INICIADA (H)");
  tocandoMelodia = true;
  notaActual = 0;
  tiempoNota = 0;
}

// Gestiona las notas en segundo plano usando millis() sin frenar el auto ni el Wi-Fi
void actualizarMelodia() {
  if (!tocandoMelodia) return;

  unsigned long ahora = millis();
  if (ahora - tiempoNota >= duracionNotas[notaActual]) {
    if (notaActual < totalNotas) {
      int freq = melodia[notaActual];
      int dur = duracionNotas[notaActual];

      if (freq > 0) {
        tone(BUZZER, freq, dur * 0.9);
      } else {
        noTone(BUZZER);
      }

      tiempoNota = ahora;
      notaActual++;
    } else {
      noTone(BUZZER);
      tocandoMelodia = false;
    }
  }
}

#endif