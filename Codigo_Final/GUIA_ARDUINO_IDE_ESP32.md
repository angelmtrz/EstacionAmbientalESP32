# Guía de puesta en marcha: ESP32 DevKit V1 de 30 pines con Arduino IDE

Esta guía ayuda a instalar el soporte de ESP32, conectar una placa **ESP32 DevKit V1 de 30 pines**, cargar un programa de prueba y resolver problemas comunes. Los nombres exactos de algunos menús pueden variar ligeramente entre Arduino IDE 2.x y 1.8.x, y entre fabricantes de placas compatibles.

## 1. Material necesario

- Placa ESP32 DevKit V1 (30 pines).
- Cable USB que permita **transferencia de datos**; algunos cables solo sirven para cargar.
- Computadora con acceso a Internet para instalar el soporte de placas.
- Arduino IDE, preferiblemente una versión reciente.

## 2. Instalar Arduino IDE y el soporte ESP32

1. Descarga e instala Arduino IDE desde el [sitio oficial de Arduino](https://www.arduino.cc/en/software).
2. Abre Arduino IDE y entra a **Archivo → Preferencias** (en algunos sistemas, **Arduino IDE → Settings/Preferences**).
3. En **URLs adicionales de Gestor de tarjetas**, agrega la URL oficial del índice de placas ESP32:

   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`

   Si ya hay otras direcciones, sepáralas con comas o usa el botón para editar la lista, sin borrar las existentes.
4. Abre **Herramientas → Placa → Gestor de tarjetas** (o el icono **Boards Manager** en Arduino IDE 2.x).
5. Busca **esp32**, selecciona el paquete **esp32 by Espressif Systems** e instálalo. La instalación puede tardar unos minutos.
6. Si Arduino IDE solicita reiniciar, ciérralo y vuelve a abrirlo.

Para más detalles o cambios en el proceso, consulta la [guía oficial de instalación de Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).

## 3. Conectar la placa y seleccionar puerto

1. Conecta el ESP32 directamente a un puerto USB de la computadora, usando un cable de datos.
2. En Arduino IDE, abre **Herramientas → Placa → esp32** y selecciona **ESP32 Dev Module**. En placas de 30 pines de distintos fabricantes, esta opción suele ser una opción inicial adecuada, aunque el nombre impreso en la placa puede variar.
3. Abre **Herramientas → Puerto** y selecciona el puerto que apareció al conectar la placa:
   - Windows: normalmente `COM` seguido de un número.
   - Linux: normalmente `/dev/ttyUSB0` o `/dev/ttyACM0`.
   - macOS: normalmente `/dev/cu.usbserial-*` o `/dev/cu.usbmodem-*`.
4. Si no sabes qué puerto corresponde, desconecta la placa, revisa la lista, vuelve a conectarla y selecciona el puerto nuevo.

## 4. Cargar un programa de prueba

Antes de conectar sensores, verifica que la placa y el entorno funcionen.

1. En Arduino IDE, abre **Archivo → Ejemplos → 01.Basics → Blink**. Si ese ejemplo no aparece para la placa, puedes usar el programa mínimo de abajo.
2. Si utilizas el ejemplo `Blink`, cambia el pin del LED integrado a `2` si tu placa tiene el LED conectado a GPIO2. La ubicación del LED integrado depende del fabricante; si no parpadea, eso por sí solo no confirma que la placa esté averiada.
3. Para una prueba independiente del LED, reemplaza el contenido del sketch por lo siguiente y carga el programa. Abre luego el Monitor Serie a 115200 baudios; debe aparecer un mensaje cada segundo.

```cpp
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 listo");
}

void loop() {
  Serial.println("Prueba de comunicacion serie");
  delay(1000);
}
```

4. Pulsa **Verificar** (✓) para compilar y luego **Subir** (→) para cargarlo.
5. Si el IDE muestra `Connecting...` y no empieza la carga, mantén pulsado el botón **BOOT** de la placa. Cuando comience la escritura o aparezca el progreso, suéltalo. En algunas placas no es necesario.
6. Al terminar, abre **Herramientas → Monitor Serie** y elige **115200** baudios. Si no aparece texto, pulsa **EN/RESET** una vez y revisa de nuevo.

## 5. Cargar el código de la estación ambiental

Una vez que la prueba haya funcionado:

1. Conecta los sensores según la tabla de [`README.md`](README.md) de esta carpeta y verifica dos veces alimentación, tierra y señales.
2. Abre el sketch que quieras cargar desde Arduino IDE:
   - Calibración: [`Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino`](Calibracion_Sensores_MQ/Calibracion_Sensores_MQ.ino).
   - Estación y envío a la API: [`Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino`](Estacion_Ambiental_Final/Estacion_Ambiental_Final.ino).
3. Sigue el orden y las instrucciones de calibración descritos en el [`README.md`](README.md) de `Codigo_Final`: primero obtén y actualiza `R0_MQ7` y `R0_MQ135`; después configura Wi-Fi y los datos de acceso a la API y carga el sketch principal.
4. Selecciona de nuevo **ESP32 Dev Module** y el puerto correcto. Verifica el sketch y súbelo.
5. Para revisar los mensajes del programa, configura el Monitor Serie a **115200 baudios**.

Los sketches de esta carpeta usan las bibliotecas `WiFi`, `HTTPClient`, `time` y `math` que se incluyen con el paquete de placas ESP32. Si Arduino IDE solicita una biblioteca, comprueba primero que instalaste **esp32 by Espressif Systems** y que está seleccionada una placa ESP32.

## Solución de problemas

| Síntoma | Posibles causas y pasos para resolverlo |
| --- | --- |
| La placa no aparece en **Puerto** | Prueba otro cable USB de datos y otro puerto USB, evita inicialmente hubs y adaptadores, y vuelve a abrir el menú. Revisa si el sistema detecta un dispositivo USB-serial. Algunos modelos usan un convertidor CP210x y otros CH340/CH341; instala el controlador oficial correspondiente al chip de tu placa si el sistema no lo reconoce. |
| Aparece un puerto, pero no se puede abrir | Cierra otros programas que estén usando el puerto (incluidas otras ventanas del Monitor Serie). En Linux, puede ser necesario tener permisos para acceder al puerto serie; consulta la configuración de tu distribución y vuelve a iniciar sesión después de cambiar permisos o grupos. |
| Error `Failed to connect` o se queda en `Connecting...` | Comprueba que seleccionaste el puerto correcto. Pulsa **Subir** y, cuando aparezca `Connecting...`, mantén presionado **BOOT** hasta que inicie la carga. Si es necesario, pulsa y suelta **EN/RESET** mientras mantienes BOOT. Desconecta temporalmente periféricos de los pines para descartar que interfieran con el arranque. |
| Error `Timed out waiting for packet header` | Repite el procedimiento de BOOT, prueba otro cable/puerto USB y comprueba la alimentación. Asegúrate de que nada fuerce niveles en los pines de arranque durante el reinicio. |
| Error de compilación con `WiFi.h` o `HTTPClient.h` no encontrado | En el Gestor de tarjetas, instala o actualiza **esp32 by Espressif Systems** y selecciona **ESP32 Dev Module** antes de compilar. Estas cabeceras vienen con el core ESP32; no suelen instalarse como bibliotecas separadas. |
| La carga finaliza, pero el Monitor Serie muestra caracteres extraños o nada | Selecciona **115200 baudios**, que es la velocidad usada por estos sketches. Pulsa **EN/RESET**. Verifica que el programa cargado tenga `Serial.begin(115200)` y que el Monitor Serie esté conectado al puerto de la placa. |
| El código de la estación no conecta a Wi-Fi | Comprueba el SSID y la contraseña, la cobertura y que la red sea compatible con el ESP32 (Wi-Fi de 2.4 GHz). El sketch espera conexión Wi-Fi durante el arranque, así que revisa los mensajes por Monitor Serie. |
| La placa se reinicia o no arranca al conectar sensores | Desconecta los sensores y prueba la placa sola. Revisa cortocircuitos, tierra común y alimentación. No apliques 5 V a entradas GPIO/ADC: el ESP32 trabaja con lógica de 3.3 V. Para las señales analógicas, respeta los divisores de tensión indicados para los módulos MQ. |

## Precauciones

- Los GPIO del ESP32 **no son tolerantes a 5 V**. No conectes directamente una señal de 5 V a sus entradas.
- Desconecta la alimentación antes de cambiar el cableado.
- Los pines GPIO34, GPIO35 y GPIO32 usados por la estación son entradas analógicas; no se deben tratar como salidas digitales.
- La placa puede variar entre fabricantes. Comprueba el chip USB-serial, la distribución de pines y la documentación de tu modelo antes de aplicar cambios de cableado o alimentación.
