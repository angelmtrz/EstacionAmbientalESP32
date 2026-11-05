#include <MQUnifiedsensor.h>

#define Placa             "ESP32"
#define Voltaje_Resolucion 3.3      // El ADC del ESP32 mide hasta 3.3V
#define Pin_Analogico     34       // El pin 'S' en tu placa de expansión
#define Tipo_Sensor       "MQ-135"
#define Resolucion_ADC    12       // El ESP32 tiene un ADC de 12 bits (0-4095)
#define Ratio_Aire_Limpio 3.6      // Valor estándar para el MQ135 en aire libre

MQUnifiedsensor MQ135(Placa, Voltaje_Resolucion, Resolucion_ADC, Pin_Analogico, Tipo_Sensor);

void setup() {
  Serial.begin(115200);
  
  // Configuración matemática de la librería
  MQ135.setRegressionMethod(1); 
  MQ135.init();

  Serial.println("Calibrando el MQ135 en aire limpio... Espera un momento.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i++) {
    MQ135.update(); 
    calcR0 += MQ135.calibrate(Ratio_Aire_Limpio);
  }
  MQ135.setR0(calcR0/10);

  Serial.print("¡Calibración completada! Tu valor de R0 es: ");
  Serial.println(calcR0/10);
  Serial.println("Copia este número para usarlo en el código final.");

}

void loop() {
  // put your main code here, to run repeatedly:

}
