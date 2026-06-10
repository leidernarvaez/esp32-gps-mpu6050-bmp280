# ESP32 GPS MPU6050 BMP280

Proyecto desarrollado con una ESP32 para la adquisición y visualización de datos mediante:

Módulo GPS NEO-6M
Acelerómetro y giroscopio MPU6050
Sensor BMP280 de temperatura, presión y altitud
Pantalla OLED I2C
Comunicación serial UART
Comunicación I2C

## Proyecto PlatformIO

El código fuente original del proyecto desarrollado en Visual Studio Code con PlatformIO se encuentra en:

platformio.ini
src/main.cpp

No se incluyen archivos compilados ni la carpeta .pio.

## Simulación en Wokwi

Los archivos necesarios para la simulación se encuentran dentro de la carpeta wokwi.

La simulación puede abrirse y ejecutarse desde el siguiente enlace:

https://wokwi.com/projects/466307977978319873

## Conexiones principales

### GPS NEO-6M

TX del GPS → GPIO 16 de la ESP32
RX del GPS → GPIO 17 de la ESP32
VCC → 3.3 V
GND → GND

### Bus I2C

SDA → GPIO 21
SCL → GPIO 22

El MPU6050, el BMP280 y la pantalla OLED comparten el bus I2C.
