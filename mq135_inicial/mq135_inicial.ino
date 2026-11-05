// Definimos el pin analógico conectado a la 'S' del GPIO 34
const int pinMQ135 = 34; 

void setup() {
  Serial.begin(115200);
  pinMode(pinMQ135, INPUT);
  Serial.println("Calentando el sensor MQ135...");

}

void loop() {
  // Leer el valor analógico (0 a 4095 en el ESP32)
  int lecturaRaw = analogRead(pinMQ135);
  
  // Convertir la lectura a voltaje simulado (Referencia 3.3V del ADC)
  float voltaje = (lecturaRaw / 4095.0) * 3.3; 

  Serial.print("Lectura ADC: ");
  Serial.print(lecturaRaw);
  Serial.print(" | Voltaje aproximado: ");
  Serial.println(voltaje);

  delay(1000); // Muestra lecturas cada segundo

}
