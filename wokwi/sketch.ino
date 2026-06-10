#include <Arduino.h>
#include <Wire.h>

#include <TinyGPS++.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_BMP280.h>

#include <U8g2lib.h>

// =====================================================
// PINES I2C
// =====================================================

#define SDA_PIN 21
#define SCL_PIN 22

// =====================================================
// GPS NEO-6M
// =====================================================

#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD 9600

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// =====================================================
// MPU6050
// =====================================================

Adafruit_MPU6050 mpu;
bool mpuOK = false;

// =====================================================
// BMP280
// =====================================================

Adafruit_BMP280 bmp;
bool bmpOK = false;

#define SEA_LEVEL_HPA 1013.25

// Valor conservado del código original
#define OFFSET_CALIBRACION 4.0

// =====================================================
// OLED SSD1306
// =====================================================

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,
  U8X8_PIN_NONE
);

// =====================================================
// TIEMPOS
// =====================================================

unsigned long sensorMillis = 0;
const unsigned long sensorInterval = 500;

unsigned long oledMillis = 0;
const unsigned long oledInterval = 4000;

int pantallaActual = 0;

// =====================================================
// VARIABLES
// =====================================================

float ax = 0;
float ay = 0;
float az = 0;

float gx = 0;
float gy = 0;
float gz = 0;

float temperatura = 0;
float temperaturaCalibrada = 0;
float presion = 0;
float altitud = 0;

// =====================================================
// MOSTRAR MENSAJE EN OLED
// =====================================================

void mostrarMensaje(const char *mensaje) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 20, mensaje);
  u8g2.sendBuffer();
}

// =====================================================
// CONFIGURACIÓN
// =====================================================

void setup() {
  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==============================");
  Serial.println("INICIANDO SISTEMA");
  Serial.println("==============================");

  // ===================================================
  // I2C
  // ===================================================

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  // ===================================================
  // PANTALLA OLED
  // ===================================================

  // U8g2 utiliza la dirección desplazada un bit
  u8g2.setI2CAddress(0x3C << 1);
  u8g2.begin();

  mostrarMensaje("Iniciando...");

  Serial.println("OLED SSD1306 iniciada");

  // ===================================================
  // GPS
  // ===================================================

  gpsSerial.begin(
    GPS_BAUD,
    SERIAL_8N1,
    GPS_RX_PIN,
    GPS_TX_PIN
  );

  Serial.println("GPS iniciado en UART2");
  Serial.println("GPS TX -> GPIO 16");
  Serial.println("GPS RX -> GPIO 17");

  // ===================================================
  // MPU6050
  // ===================================================

  if (mpu.begin(0x68, &Wire)) {
    mpuOK = true;

    mpu.setAccelerometerRange(
      MPU6050_RANGE_8_G
    );

    mpu.setGyroRange(
      MPU6050_RANGE_500_DEG
    );

    mpu.setFilterBandwidth(
      MPU6050_BAND_21_HZ
    );

    Serial.println(
      "MPU6050 encontrado en 0x68"
    );
  } else {
    Serial.println(
      "ERROR: MPU6050 no encontrado"
    );
  }

  // ===================================================
  // BMP280
  // ===================================================

  if (bmp.begin(0x76)) {
    bmpOK = true;

    Serial.println(
      "BMP280 encontrado en 0x76"
    );
  } else if (bmp.begin(0x77)) {
    bmpOK = true;

    Serial.println(
      "BMP280 encontrado en 0x77"
    );
  } else {
    Serial.println(
      "ERROR: BMP280 no encontrado"
    );
  }

  Serial.println("==============================");

  if (mpuOK && bmpOK) {
    mostrarMensaje("Sistema listo");
  } else {
    mostrarMensaje("Revisar sensores");
  }

  delay(1500);
}

// =====================================================
// LEER MPU6050
// =====================================================

void leerMPU6050() {
  if (!mpuOK) {
    return;
  }

  sensors_event_t a;
  sensors_event_t g;
  sensors_event_t t;

  mpu.getEvent(&a, &g, &t);

  // Orientación conservada del código original

  ax = a.acceleration.x;
  ay = a.acceleration.y;
  az = -a.acceleration.z;

  gx = g.gyro.z * 180.0 / PI;
  gy = g.gyro.y * 180.0 / PI;
  gz = g.gyro.x * 180.0 / PI;
}

// =====================================================
// LEER BMP280
// =====================================================

void leerBMP280() {
  if (!bmpOK) {
    return;
  }

  temperatura = bmp.readTemperature();

  temperaturaCalibrada =
    temperatura - OFFSET_CALIBRACION;

  presion =
    bmp.readPressure() / 100.0;

  altitud =
    bmp.readAltitude(SEA_LEVEL_HPA);
}

// =====================================================
// MOSTRAR DATOS EN MONITOR SERIAL
// =====================================================

void mostrarSerial() {
  Serial.println();
  Serial.println("--------- BMP280 ---------");

  if (bmpOK) {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" C");

    Serial.print("Temperatura calibrada: ");
    Serial.print(temperaturaCalibrada);
    Serial.println(" C");

    Serial.print("Presion: ");
    Serial.print(presion);
    Serial.println(" hPa");

    Serial.print("Altitud: ");
    Serial.print(altitud);
    Serial.println(" m");
  } else {
    Serial.println("BMP280 no disponible");
  }

  Serial.println("--------- MPU6050 --------");

  if (mpuOK) {
    Serial.print("AX: ");
    Serial.print(ax);
    Serial.println(" m/s2");

    Serial.print("AY: ");
    Serial.print(ay);
    Serial.println(" m/s2");

    Serial.print("AZ: ");
    Serial.print(az);
    Serial.println(" m/s2");

    Serial.print("GX: ");
    Serial.print(gx);
    Serial.println(" grados/s");

    Serial.print("GY: ");
    Serial.print(gy);
    Serial.println(" grados/s");

    Serial.print("GZ: ");
    Serial.print(gz);
    Serial.println(" grados/s");
  } else {
    Serial.println("MPU6050 no disponible");
  }

  Serial.println("----------- GPS ----------");

  Serial.print("Caracteres recibidos: ");
  Serial.println(gps.charsProcessed());

  if (gps.location.isValid()) {
    Serial.print("Latitud: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitud: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Esperando coordenadas...");
  }

  if (gps.satellites.isValid()) {
    Serial.print("Satelites: ");
    Serial.println(gps.satellites.value());
  }

  if (gps.altitude.isValid()) {
    Serial.print("Altitud GPS: ");
    Serial.print(gps.altitude.meters());
    Serial.println(" m");
  }

  Serial.println("--------------------------");
}

// =====================================================
// PANTALLA BMP280
// =====================================================

void pantallaBMP280() {
  char linea[32];

  u8g2.drawStr(0, 10, "BMP280");

  if (!bmpOK) {
    u8g2.drawStr(
      0,
      30,
      "Sensor no detectado"
    );

    return;
  }

  snprintf(
    linea,
    sizeof(linea),
    "TEMP: %.2f C",
    temperaturaCalibrada
  );

  u8g2.drawStr(0, 28, linea);

  snprintf(
    linea,
    sizeof(linea),
    "PRES: %.1f hPa",
    presion
  );

  u8g2.drawStr(0, 42, linea);

  snprintf(
    linea,
    sizeof(linea),
    "ALT: %.1f m",
    altitud
  );

  u8g2.drawStr(0, 56, linea);
}

// =====================================================
// PANTALLA MPU6050
// =====================================================

void pantallaMPU6050() {
  char linea[32];

  u8g2.drawStr(0, 10, "MPU6050");

  if (!mpuOK) {
    u8g2.drawStr(
      0,
      30,
      "Sensor no detectado"
    );

    return;
  }

  u8g2.drawStr(
    0,
    20,
    "Aceleracion"
  );

  snprintf(
    linea,
    sizeof(linea),
    "X:%5.1f Y:%5.1f",
    az,
    ay
  );

  u8g2.drawStr(0, 34, linea);

  snprintf(
    linea,
    sizeof(linea),
    "Z:%5.1f",
    ax
  );

  u8g2.drawStr(0, 46, linea);

  snprintf(
    linea,
    sizeof(linea),
    "GX:%4.0f",
    gx
  );

  u8g2.drawStr(0, 60, linea);
}

// =====================================================
// PANTALLA GPS
// =====================================================

void pantallaGPS() {
  char linea[32];

  u8g2.drawStr(0, 10, "GPS NEO-6M");

  snprintf(
    linea,
    sizeof(linea),
    "SAT: %lu",
    gps.satellites.value()
  );

  u8g2.drawStr(0, 24, linea);

  snprintf(
    linea,
    sizeof(linea),
    "CH: %lu",
    gps.charsProcessed()
  );

  u8g2.drawStr(0, 36, linea);

  if (gps.location.isValid()) {
    snprintf(
      linea,
      sizeof(linea),
      "LAT %.4f",
      gps.location.lat()
    );

    u8g2.drawStr(0, 50, linea);

    snprintf(
      linea,
      sizeof(linea),
      "LON %.4f",
      gps.location.lng()
    );

    u8g2.drawStr(0, 62, linea);
  } else {
    u8g2.drawStr(
      0,
      52,
      "Esperando GPS..."
    );
  }
}

// =====================================================
// ACTUALIZAR OLED
// =====================================================

void actualizarOLED() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);

  if (pantallaActual == 0) {
    pantallaBMP280();
  } else if (pantallaActual == 1) {
    pantallaMPU6050();
  } else {
    pantallaGPS();
  }

  u8g2.sendBuffer();
}

// =====================================================
// BUCLE PRINCIPAL
// =====================================================

void loop() {
  // Leer permanentemente los datos UART del GPS

  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Leer sensores cada 500 ms

  if (
    millis() - sensorMillis
    >= sensorInterval
  ) {
    sensorMillis = millis();

    leerMPU6050();
    leerBMP280();
    mostrarSerial();
  }

  // Cambiar la pantalla cada cuatro segundos

  if (
    millis() - oledMillis
    >= oledInterval
  ) {
    oledMillis = millis();

    pantallaActual++;

    if (pantallaActual > 2) {
      pantallaActual = 0;
    }
  }

  actualizarOLED();

  delay(20);
}