#include "esp_camera.h"
#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPS++.h>

// --- PIN DEFINITIONS ---
#define I2C_SDA 14
#define I2C_SCL 15
#define GPS_RX  13
#define GPS_TX  12 

// --- CAMERA PIN MAP (AI-THINKER) ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// --- OBJECTS ---
MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial GPSSerial(2); // Use UART2 for GPS

void setup() {
  // 1. Start Serial for PC (Match Python Baud Rate)
  Serial.begin(115200);

  // 2. Initialize I2C for MPU6500
  Wire.begin(I2C_SDA, I2C_SCL);
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6500 connection failed");
  }

  // 3. Initialize GPS Serial
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  // 4. Camera Configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size and quality settings
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA; // Higher res if PSRAM available
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  Serial.println("RMMS System Online. Ready for detection...");
}

void loop() {
  // Feed GPS data
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }

  // Read MPU6500
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float gZ = (float)az / 16384.0;
  
  // Calculate Tilt (TiltX and TiltY)
  float tiltX = atan2(ax, sqrt(pow(ay, 2) + pow(az, 2))) * 180 / PI;
  float tiltY = atan2(ay, sqrt(pow(ax, 2) + pow(az, 2))) * 180 / PI;

  // Trigger Logic (Signal-Trigger-Response)
  if (abs(gZ) > 1.7) { 
    camera_fb_t * fb = esp_camera_fb_get();
    if (fb) {
      // Send DATA header for Python: DATA,Lat,Lng,AccelZ,TiltX,TiltY,ImgSize
      Serial.print("DATA,");
      Serial.print(gps.location.lat(), 6); Serial.print(",");
      Serial.print(gps.location.lng(), 6); Serial.print(",");
      Serial.print(gZ); Serial.print(",");
      Serial.print(tiltX); Serial.print(",");
      Serial.print(tiltY); Serial.print(",");
      Serial.println(fb->len); 

      // Send the raw image bytes immediately after the text
      Serial.write(fb->buf, fb->len);
      
      esp_camera_fb_return(fb);
      
      // 1 second delay as requested to prevent double triggers
      delay(1000); 
    }
  }
}