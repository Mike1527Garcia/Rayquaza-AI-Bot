#ifndef CONFIG_H
#define CONFIG_H

// ==========================
// Configuración básica del hardware
// ==========================

// Pantalla TFT SPI
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_BL 38

// WiFi status LED
#define LED_WIFI 13
#define LED_STATUS 14

// Audio / speaker
#define AMP_ENABLE 5
#define I2S_BCLK 42
#define I2S_LRC 1
#define I2S_DIN 41

// Botones / navegación
#define BTN_BACK 0
#define BTN_MENU 2

// Batería
#define BATTERY_ADC_PIN 4

// ==========================
// Rayquaza Bot defaults
// ==========================
#define DEFAULT_WIFI_SSID "YOUR_WIFI_SSID"
#define DEFAULT_WIFI_PASS "YOUR_WIFI_PASSWORD"
#define DEFAULT_API_KEY "YOUR_API_KEY"

#endif
