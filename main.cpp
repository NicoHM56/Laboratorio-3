/*##############################################################################################################
//Datos importantes

Universidad del Valle de Guatemala
BE3029 - Electronica Digital 2
MCU: ESP32 dev kit 1.0

RISING: https://forum.arduino.cc/t/rising-falling-question/70217
##############################################################################################################*/


// Ejercicio A funcional

#include <Arduino.h>
#include <stdint.h>

// Definir pines de LEDs
#define LED1 2
#define LED2 4
#define LED3 15
#define LED4 16

// Botones
#define BTN1 12
#define BTN2 14

// Variables
volatile int contador = 0; // 'volatile' porque se modifica en ISRs, las funciones 'volatile' son aquellas que puedenn cambiar asi que el programa debe estar más al pendiente de ellas

// Anti-rebote para interrupciones
volatile unsigned long tiempoAnteriorBTN1 = 0;
volatile unsigned long tiempoAnteriorBTN2 = 0;
const long tiempoEspera = 200; // Tiempo en ms para el anti-rebote

void Mostrar_Binario_LEDS(void);

// Funciones de ISRs para los botones
void IRAM_ATTR handleBTN1();
void IRAM_ATTR handleBTN2();

// Configuración y ISR del Timer0 
void initTMR0(void);
void IRAM_ATTR TMR0_ISR(void);
hw_timer_t *Timer0_Cfg = NULL;


// ISR para BTN1 (incrementar contador)
void IRAM_ATTR handleBTN1() {
  
  if ((millis() - tiempoAnteriorBTN1) > tiempoEspera) {
    contador++;
    if (contador > 15) {
      contador = 0;
    }
    tiempoAnteriorBTN1 = millis(); 
  }
}

// ISR para BTN2 (decrementar contador)
void IRAM_ATTR handleBTN2() {
  
  if ((millis() - tiempoAnteriorBTN2) > tiempoEspera) {
    contador--;
    if (contador < 0) {
      contador = 15;
    }
    tiempoAnteriorBTN2 = millis(); // Actualizar el tiempo de la última pulsación válida
  }
}

// ISR del Timer0
void IRAM_ATTR TMR0_ISR() {
  //SOLO EXISTE, NO TOCAR O MUERE TODO
}


void setup() {
 
  //Configurar LEDS
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);


  // Configurar botones
  pinMode(BTN1, INPUT_PULLDOWN);
  pinMode(BTN2, INPUT_PULLDOWN);

  // Agrega las funciones de interrupción a los pines de los botones de incremento (+) y decremneto (-)
  attachInterrupt(digitalPinToInterrupt(BTN1), handleBTN1, RISING); // RISING indica que se debe disparar la interrupción cuando la señal pasa de LOW a HIGH.
  attachInterrupt(digitalPinToInterrupt(BTN2), handleBTN2, RISING);

  // Inicializar el Timer0
  initTMR0();
}

void loop() {
  Mostrar_Binario_LEDS();
}

void Mostrar_Binario_LEDS(void) {
  digitalWrite(LED1, (contador >> 0) & 1); 
  digitalWrite(LED2, (contador >> 1) & 1); 
  digitalWrite(LED3, (contador >> 2) & 1); 
  digitalWrite(LED4, (contador >> 3) & 1); 
}

// Inicialización del Timer0
void initTMR0(void) {
  Timer0_Cfg = timerBegin(0, 80, true); 
  timerAttachInterrupt(Timer0_Cfg, &TMR0_ISR, true); 
  timerAlarmWrite(Timer0_Cfg, 500000, true); // Disparar cada  500ms
  timerAlarmEnable(Timer0_Cfg); // Habilitar la alarma del timer
}
  