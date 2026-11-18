/*
  ===========================================================================
  SKETCH DE CALIBRACIÓN EN AIRE LIMPIO PARA SENSORES MQ-7 Y MQ-135
  Tarjeta: ESP32 DevKit V1 (30 pines)
  ---------------------------------------------------------------------------
  IMPORTANTE:
  1. Ejecuta este programa en un ambiente al AIRE LIBRE o AIRE LIMPIO (libre de humo,
     gas de cocina o monóxido de carbono).
  2. Deja calentar los sensores al menos de 3 a 5 minutos antes de tomar los valores
     de R0 finales (los sensores nuevos requieren precalentamiento previo).
  3. Los valores de R0 obtenidos deberán reemplazarse en las constantes de tu código principal.
  ===========================================================================
*/

// Asignación de Pines (Canal ADC1)
const int PIN_MQ7   = 34; // GPIO34 (Con divisor de tensión 10k/10k)
const int PIN_MQ135 = 35; // GPIO35 (Con divisor de tensión 10k/10k)

// Valor de la resistencia de carga (RL) integrada en los módulos MQ comerciales (habitualmente 10 kΩ)
const float RL_MODULE_KOHM = 10.0; 

// Ratio de resistencia en aire limpio (Rs/R0) según datasheets del fabricante:
// - MQ-7 en aire limpio:   Rs / R0 ≈ 27.0
// - MQ-135 en aire limpio: Rs / R0 ≈ 3.6
const float RATIO_CLEAN_AIR_MQ7   = 27.0;
const float RATIO_CLEAN_AIR_MQ135 = 3.6;

// Número de lecturas para promediar y filtrar ruido
const int NUM_MUESTRAS = 100;
const int DELAY_ENTRE_MUESTRAS_MS = 100;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configuración de resolución del ADC del ESP32 a 12 bits (0 - 4095)
  analogReadResolution(12);

  Serial.println("\n========================================================");
  Serial.println("   CALIBRACIÓN DE SENSORES MQ-7 Y MQ-135 (ESP32)");
  Serial.println("========================================================");
  Serial.println("Iniciando fase de estabilización del sensor (10 segundos)...");
  
  for (int i = 10; i > 0; i--) {
    Serial.printf("Esperando... %d s\n", i);
    delay(1000);
  }
}

// Función para obtener el voltaje real del sensor considerando el divisor 10k/10k (x2)
float obtenerVoltajePromedio(int pin) {
  long sumaADC = 0;
  for (int i = 0; i < NUM_MUESTRAS; i++) {
    sumaADC += analogRead(pin);
    delay(DELAY_ENTRE_MUESTRAS_MS);
  }
  float adcPromedio = (float)sumaADC / NUM_MUESTRAS;
  
  // Conversión a voltaje ADC (0 - 3.3V)
  float voltajeADC = (adcPromedio / 4095.0) * 3.3;
  
  // Multiplicación x 2.0 por el divisor de tensión (10k / 10k)
  float voltajeRealSensor = voltajeADC * 2.0;
  
  return voltajeRealSensor;
}

// Función para calcular la resistencia actual del sensor (Rs)
float calcularRs(float voltajeSensor) {
  // Evitar división por cero si el voltaje es extremadamente cercano a 5V o 0V
  if (voltajeSensor >= 4.95) voltajeSensor = 4.95;
  if (voltajeSensor <= 0.05) voltajeSensor = 0.05;
  
  // Fórmula de resistencia del sensor con resistencia de carga RL:
  // Rs = ((Vcc - Vout) / Vout) * RL
  float rs = ((5.0 - voltajeSensor) / voltajeSensor) * RL_MODULE_KOHM;
  return rs;
}

void loop() {
  Serial.println("\n--------------------------------------------------------");
  Serial.println("Tomando lecturas y promediando 100 muestras...");

  // 1. Lectura y cálculo para MQ-7
  float vMQ7  = obtenerVoltajePromedio(PIN_MQ7);
  float rsMQ7 = calcularRs(vMQ7);
  float r0MQ7 = rsMQ7 / RATIO_CLEAN_AIR_MQ7;

  // 2. Lectura y cálculo para MQ-135
  float vMQ135  = obtenerVoltajePromedio(PIN_MQ135);
  float rsMQ135 = calcularRs(vMQ135);
  float r0MQ135 = rsMQ135 / RATIO_CLEAN_AIR_MQ135;

  // 3. Impresión de resultados en Monitor Serie
  Serial.println("\n--- RESULTADOS DE CALIBRACIÓN EN AIRE LIMPIO ---");
  
  Serial.printf("[MQ-7  (CO)]   -> Voltaje: %.3f V | Rs: %.2f kΩ | R0 Calibrado: %.2f kΩ\n", 
                vMQ7, rsMQ7, r0MQ7);
                
  Serial.printf("[MQ-135(CO2)]  -> Voltaje: %.3f V | Rs: %.2f kΩ | R0 Calibrado: %.2f kΩ\n", 
                vMQ135, rsMQ135, r0MQ135);

  Serial.println("\n>>> ANOTA ESTOS VALORES DE R0 <<<");
  Serial.printf("  - R0_MQ7   = %.2f\n", r0MQ7);
  Serial.printf("  - R0_MQ135 = %.2f\n", r0MQ135);
  Serial.println("--------------------------------------------------------");
  
  Serial.println("Repitiendo ciclo de medición en 10 segundos...");
  delay(10000);
}
