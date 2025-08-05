/*##############################################################################################################
//Datos importantes

Universidad del Valle de Guatemala
BE3029 - Electronica Digital 2
MCU: ESP32 dev kit 1.0

RISING: https://forum.arduino.cc/t/rising-falling-question/70217
##############################################################################################################*/

// Ejercicio B
#include <Arduino.h>
#include <stdint.h>


#define LED1 2
#define LED2 4
#define LED3 15
#define LED4 16


volatile bool bandera = false;  // Bandera modificada por el timer
int contador = 0;                 


hw_timer_t *Timer0_Cfg = NULL;


void IRAM_ATTR TMR0_ISR() {
  bandera = true;  // Señal de que han pasado 250ms
}


void Mostrar_Binario_LEDS(uint8_t valor) {
  digitalWrite(LED1, valor & 0x01);
  digitalWrite(LED2, (valor >> 1) & 0x01);
  digitalWrite(LED3, (valor >> 2) & 0x01);
  digitalWrite(LED4, (valor >> 3) & 0x01);
}


void initTMR0() {
  Timer0_Cfg = timerBegin(0, 80, true);                 
  timerAttachInterrupt(Timer0_Cfg, &TMR0_ISR, true);   // Interrupt por flanco
  timerAlarmWrite(Timer0_Cfg, 250000, true);           // 250ms 
  timerAlarmEnable(Timer0_Cfg);                        // Habilitar alarma
}


void setup() {
  // Configurar pines de salida
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  // Inicializar LEDs en 0
  Mostrar_Binario_LEDS(0);

  // Inicializar Timer0
  initTMR0();
}


void loop() {
  if (bandera) {
    bandera = false;
    contador = (contador + 1) & 0x0F;  // Limitar a 4 bits (0–15)
    Mostrar_Binario_LEDS((uint8_t)contador);
  }
}