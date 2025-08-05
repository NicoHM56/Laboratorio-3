#include <Arduino.h>
#include <stdint.h>


#define LED1 32
#define LED2 33
#define LED3 25
#define LED4 14

// LEDs individuales para el timer
#define LED5 12
#define LED6 13
#define LED7 21
#define LED8 22

// LED de coincidencia
#define LED_ALARMA 4

// Botones
#define BTN1 26
#define BTN2 27
#define BTN_RESET 15

// Contadores
volatile int contadorManual = 0;
volatile int contadorTimer = 0;

// Anti-rebote para botones
unsigned long tiempoAnteriorBTN1 = 0;
unsigned long tiempoAnteriorBTN2 = 0;
unsigned long ultimoReset = 0;
const long tiempoEspera = 200; // ms

// Timer
volatile bool banderaTimer = false;
hw_timer_t *Timer0_Cfg = NULL;

// Prototipos
void Mostrar_Binario_LEDS(int valor, bool esManual);
void IRAM_ATTR TMR0_ISR();
void IRAM_ATTR handleBTN1();
void IRAM_ATTR handleBTN2();
void initTMR0();

void setup() {
  // LEDs contador manual
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  // LEDs contador timer
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);

  // LED de alarma
  pinMode(LED_ALARMA, OUTPUT);
  digitalWrite(LED_ALARMA, LOW);

  // Botones
  pinMode(BTN1, INPUT_PULLDOWN);
  pinMode(BTN2, INPUT_PULLDOWN);
  pinMode(BTN_RESET, INPUT);

  attachInterrupt(digitalPinToInterrupt(BTN1), handleBTN1, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN2), handleBTN2, RISING);

  // Timer
  initTMR0();
}

void loop() {
  // Mostrar valores binarios en LEDs
  Mostrar_Binario_LEDS(contadorManual, true);   // LEDs 1–4
  Mostrar_Binario_LEDS(contadorTimer, false);   // LEDs 5–8

  // Reinicio del contador del timer
  if (digitalRead(BTN_RESET) == HIGH && millis() - ultimoReset > tiempoEspera) {
    contadorTimer = 0;
    digitalWrite(LED_ALARMA, LOW);
    ultimoReset = millis();
  }

  // Si el timer lanzó interrupción
  if (banderaTimer) {
    banderaTimer = false;

    contadorTimer++;
    if (contadorTimer > 15) contadorTimer = 0;
  }

  // Activar alarma si ambos contadores son 4
  if (contadorManual == 4 && contadorTimer == 4) {
    digitalWrite(LED_ALARMA, HIGH);
  } else {
    digitalWrite(LED_ALARMA, LOW);
  }
}

// ISR para botón de incremento
void IRAM_ATTR handleBTN1() {
  if (millis() - tiempoAnteriorBTN1 > tiempoEspera) {
    contadorManual = (contadorManual + 1) & 0x0F;
    tiempoAnteriorBTN1 = millis();
  }
}

// ISR para botón de decremento
void IRAM_ATTR handleBTN2() {
  if (millis() - tiempoAnteriorBTN2 > tiempoEspera) {
    contadorManual = (contadorManual - 1) & 0x0F;
    if (contadorManual < 0) contadorManual = 15;
    tiempoAnteriorBTN2 = millis();
  }
}

// Mostrar en LEDs
void Mostrar_Binario_LEDS(int valor, bool esManual) {
  if (esManual) {
    digitalWrite(LED1, valor & 0x01);
    digitalWrite(LED2, (valor >> 1) & 0x01);
    digitalWrite(LED3, (valor >> 2) & 0x01);
    digitalWrite(LED4, (valor >> 3) & 0x01);
  } else {
    digitalWrite(LED5, valor & 0x01);
    digitalWrite(LED6, (valor >> 1) & 0x01);
    digitalWrite(LED7, (valor >> 2) & 0x01);
    digitalWrite(LED8, (valor >> 3) & 0x01);
  }
}

// ISR Timer
void IRAM_ATTR TMR0_ISR() {
  banderaTimer = true;
}

// Inicializar Timer
void initTMR0() {
  Timer0_Cfg = timerBegin(0, 80, true);           // 1 µs por tick
  timerAttachInterrupt(Timer0_Cfg, &TMR0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 250000, true);      // cada 250 ms
  timerAlarmEnable(Timer0_Cfg);
}