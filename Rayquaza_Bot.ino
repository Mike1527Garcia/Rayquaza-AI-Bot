#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include "config.h"

#define SERIAL_BAUD 115200
#define CONFIG_FILE "/rayquaza_config.json"

enum ScreenType {
  SCREEN_HOME = 0,
  SCREEN_VOICE = 1,
  SCREEN_SETTINGS = 2,
};

struct DeviceConfig {
  char wifiSSID[32] = "YOUR_WIFI_SSID";
  char wifiPass[64] = "YOUR_WIFI_PASSWORD";
  char apiKey[128] = "YOUR_API_KEY";
  int volume = 70;
  bool connected = false;
  char ipAddress[16] = "0.0.0.0";
};

DeviceConfig deviceConfig;
ScreenType currentScreen = SCREEN_HOME;
bool wifiConnected = false;
String lastAiResponse = "Esperando entrada...";

void loadConfig();
void saveConfig();
void drawHomeScreen();
void drawVoiceScreen();
void drawSettingsScreen();
void renderScreen();
void updateClock();
void updateBattery();
void connectToWiFi();
String callAi(String prompt);
void processSerialCommand(String cmd);

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(AMP_ENABLE, OUTPUT);
  analogWrite(AMP_ENABLE, deviceConfig.volume);

  if (!SPIFFS.begin(true)) {
    Serial.println("Error: SPIFFS no se pudo montar");
  }

  loadConfig();

  // Inicializar pantalla TFT
  // Asegúrate de configurar TFT_eSPI correctamente en User_Setup.h
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  connectToWiFi();
  renderScreen();
}

void loop() {
  updateClock();
  updateBattery();

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processSerialCommand(input);
  }

  if (wifiConnected) {
    digitalWrite(LED_WIFI, HIGH);
  } else {
    digitalWrite(LED_WIFI, LOW);
  }

  delay(1000);
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(deviceConfig.wifiSSID, deviceConfig.wifiPass);

  Serial.println("Conectando a WiFi...");
  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    deviceConfig.connected = true;
    snprintf(deviceConfig.ipAddress, sizeof(deviceConfig.ipAddress), "%s", WiFi.localIP().toString().c_str());
    Serial.println("\nWiFi conectado");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_WIFI, HIGH);
  } else {
    wifiConnected = false;
    deviceConfig.connected = false;
    Serial.println("\nNo se pudo conectar al WiFi");
    digitalWrite(LED_WIFI, LOW);
  }
}

bool loadConfig() {
  if (!SPIFFS.exists(CONFIG_FILE)) {
    Serial.println("No existe archivo de configuración. Se usan valores por defecto.");
    return false;
  }

  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("No se pudo abrir archivo de configuración");
    return false;
  }

  String json = file.readString();
  file.close();

  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.println("JSON inválido");
    return false;
  }

  if (doc["wifiSSID"]) strlcpy(deviceConfig.wifiSSID, doc["wifiSSID"], sizeof(deviceConfig.wifiSSID));
  if (doc["wifiPass"]) strlcpy(deviceConfig.wifiPass, doc["wifiPass"], sizeof(deviceConfig.wifiPass));
  if (doc["apiKey"]) strlcpy(deviceConfig.apiKey, doc["apiKey"], sizeof(deviceConfig.apiKey));
  if (doc["volume"]) deviceConfig.volume = doc["volume"].as<int>();

  return true;
}

void saveConfig() {
  DynamicJsonDocument doc(1024);
  doc["wifiSSID"] = deviceConfig.wifiSSID;
  doc["wifiPass"] = deviceConfig.wifiPass;
  doc["apiKey"] = deviceConfig.apiKey;
  doc["volume"] = deviceConfig.volume;

  File file = SPIFFS.open(CONFIG_FILE, "w");
  if (!file) {
    Serial.println("No se pudo escribir configuración");
    return;
  }

  serializeJsonPretty(doc, file);
  file.close();
  Serial.println("Configuración guardada");
}

void drawHomeScreen() {
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(85, 15);
  tft.println("Rayquaza Bot");

  // Indicador WiFi
  if (wifiConnected) {
    tft.fillRoundRect(220, 15, 70, 20, 8, TFT_GREEN);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    tft.setCursor(233, 18);
    tft.print("WiFi");
  } else {
    tft.fillRoundRect(220, 15, 70, 20, 8, TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setCursor(235, 18);
    tft.print("OFF");
  }

  // Fecha y hora
  tft.setTextSize(1);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setCursor(20, 18);
  tft.print("Date: ");
  tft.print("27/09/2026");

  tft.setCursor(20, 32);
  tft.print("Time: ");
  tft.print("12:00");

  // Personaje Rayquaza placeholder (icono simple)
  tft.fillRoundRect(55, 70, 210, 110, 20, TFT_DARKGREY);
  tft.fillCircle(130, 125, 28, TFT_BLUE);
  tft.fillCircle(190, 125, 28, TFT_BLUE);
  tft.fillTriangle(100, 150, 160, 170, 220, 150, TFT_GREEN);
  tft.fillRoundRect(90, 155, 140, 18, 6, TFT_GREEN);

  // Animación base
  tft.drawRoundRect(55, 70, 210, 110, 20, TFT_YELLOW);
  tft.drawRoundRect(60, 75, 200, 100, 18, TFT_DARKGREY);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(75, 195);
  tft.println("Desliza la pantalla");

  // Botón basico para navegación
  tft.fillRoundRect(25, 210, 90, 25, 10, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(45, 216);
  tft.print("HOME");

  tft.fillRoundRect(205, 210, 90, 25, 10, TFT_NAVY);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.setCursor(225, 216);
  tft.print("CHAT");
}

void drawVoiceScreen() {
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(80, 15);
  tft.println("Chat de Voz AI");

  tft.fillRoundRect(50, 60, 220, 90, 18, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextSize(1);
  tft.setCursor(70, 90);
  tft.print("Pregunta AI:");

  tft.fillRoundRect(50, 160, 220, 30, 10, TFT_GREEN);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.setCursor(110, 170);
  tft.print("GRABAR");

  // Respuesta
  tft.setTextSize(1);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setCursor(30, 205);
  tft.print("Respuesta:");
  tft.setCursor(110, 205);
  tft.print(lastAiResponse.substring(0, 18));

  // Botones inferiores
  tft.fillRoundRect(20, 210, 80, 20, 8, TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setCursor(35, 214);
  tft.print("VOLVER");

  tft.fillRoundRect(220, 210, 80, 20, 8, TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setCursor(235, 214);
  tft.print("HOME");
}

void drawSettingsScreen() {
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(90, 15);
  tft.println("Configuracion");

  tft.setTextSize(1);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setCursor(20, 45);
  tft.print("WiFi: ");
  tft.print(deviceConfig.wifiSSID);

  tft.setCursor(20, 65);
  tft.print("Pass: ");
  tft.print("********");

  tft.setCursor(20, 85);
  tft.print("API: ");
  tft.print(deviceConfig.apiKey[0] != '\0' ? "SET" : "NO SET");

  tft.setCursor(20, 105);
  tft.print("Volumen: ");
  tft.print(deviceConfig.volume);

  tft.setCursor(20, 125);
  tft.print("Bateria: ");
  tft.print("87%");

  tft.setCursor(20, 145);
  tft.print("IP: ");
  tft.print(deviceConfig.ipAddress);

  tft.fillRoundRect(20, 200, 80, 20, 8, TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setCursor(30, 204);
  tft.print("BACK");

  tft.fillRoundRect(220, 200, 80, 20, 8, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(235, 204);
  tft.print("SAVE");
}

void renderScreen() {
  switch (currentScreen) {
    case SCREEN_HOME:
      drawHomeScreen();
      break;
    case SCREEN_VOICE:
      drawVoiceScreen();
      break;
    case SCREEN_SETTINGS:
      drawSettingsScreen();
      break;
  }
}

void updateClock() {
  // Una implementación simple, se puede mejorar con RTC más adelante
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  lastUpdate = millis();

  // No se usa RTC, solo muestra un valor estático
  if (currentScreen == SCREEN_HOME) {
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(20, 32);
    tft.print("Time: ");
    tft.print("12:00");
  }
}

void updateBattery() {
  // Simulación de nivel de batería. Se puede leer con ADC real en hardware
  float batteryPercent = 87.0;
  if (currentScreen == SCREEN_SETTINGS) {
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(20, 125);
    tft.print("Bateria: ");
    tft.print(batteryPercent);
    tft.print("%");
  }
}

String callAi(String prompt) {
  if (String(deviceConfig.apiKey) == "" || String(deviceConfig.apiKey) == "YOUR_API_KEY") {
    return "API no configurada. Ajusta la clave en Configuración.";
  }

  if (!wifiConnected) {
    return "Sin conexión WiFi. Conecta antes de usar IA.";
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, "https://api.openai.com/v1/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + String(deviceConfig.apiKey));

  DynamicJsonDocument payload(4096);
  payload["model"] = "gpt-4o-mini";
  JsonArray messages = payload["messages"].to<JsonArray>();
  JsonObject userMsg = messages.createNestedObject();
  userMsg["role"] = "user";
  userMsg["content"] = prompt;

  String requestBody;
  serializeJson(payload, requestBody);

  int httpCode = http.POST(requestBody);
  String response = "";

  if (httpCode > 0) {
    response = http.getString();
  } else {
    response = "Error al consultar IA";
  }

  http.end();

  DynamicJsonDocument reply(4096);
  deserializeJson(reply, response);

  if (reply["choices"][0]["message"]["content"].is<String>()) {
    return reply["choices"][0]["message"]["content"].as<String>();
  }

  return "No se obtuvo respuesta útil del AI.";
}

void processSerialCommand(String cmd) {
  cmd.trim();
  if (cmd == "HOME") {
    currentScreen = SCREEN_HOME;
    renderScreen();
  } else if (cmd == "VOICE") {
    currentScreen = SCREEN_VOICE;
    renderScreen();
  } else if (cmd == "SETTINGS") {
    currentScreen = SCREEN_SETTINGS;
    renderScreen();
  } else if (cmd.startsWith("WIFI ")) {
    String rest = cmd.substring(5);
    int firstSpace = rest.indexOf(' ');
    if (firstSpace > 0) {
      String ssid = rest.substring(0, firstSpace);
      String p = rest.substring(firstSpace + 1);
      ssid.toCharArray(deviceConfig.wifiSSID, sizeof(deviceConfig.wifiSSID));
      p.toCharArray(deviceConfig.wifiPass, sizeof(deviceConfig.wifiPass));
      saveConfig();
      Serial.println("WiFi actualizado");
    }
  } else if (cmd.startsWith("API ")) {
    String key = cmd.substring(4);
    key.trim();
    key.toCharArray(deviceConfig.apiKey, sizeof(deviceConfig.apiKey));
    saveConfig();
    Serial.println("API Key actualizada");
  } else if (cmd.startsWith("VOL ")) {
    int value = cmd.substring(4).toInt();
    if (value >= 0 && value <= 100) {
      deviceConfig.volume = value;
      analogWrite(AMP_ENABLE, deviceConfig.volume);
      saveConfig();
      Serial.println("Volumen actualizado");
    }
  } else if (cmd == "SAVE") {
    saveConfig();
  } else if (cmd == "CONNECT") {
    connectToWiFi();
    renderScreen();
  } else if (cmd.startsWith("ASK ")) {
    String prompt = cmd.substring(4);
    String response = callAi(prompt);
    lastAiResponse = response;
    Serial.println(response);
    currentScreen = SCREEN_VOICE;
    renderScreen();
  }
}
