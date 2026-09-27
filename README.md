# 🤖 Rayquaza AI Bot - Arduino IDE Project

Este proyecto es una base para construir un asistente inteligente con ESP32-S3-CAM y pantalla TFT, con un diseño inspirado en la interfaz que describiste:

- Pantalla principal con Rayquaza
- Indicador WiFi en vivo
- Reloj y fecha
- Aviso: "Desliza la pantalla"
- Pantalla de Chat de Voz AI
- Pantalla de Configuración
- Botón de regreso / inicio

## Hardware recomendado

- ESP32-S3-CAM
- Pantalla TFT SPI 320x240
- Micrófono I2S
- Amplificador PAM8002A
- Parlante 8Ω
- Batería LiPo 3.7V

## Requisitos de Arduino IDE

### 1) Instalar soporte para ESP32
1. Abre Arduino IDE.
2. Ve a Archivo > Preferencias.
3. Agrega esta URL:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
4. Ve a Herramientas > Placa > Administrador de tarjetas.
5. Instala `esp32`.

### 2) Instalar librerías
En Herramientas > Administrar librerías instala:
- `TFT_eSPI`
- `ArduinoJson`
- `WiFi`
- `WiFiClientSecure`
- `HTTPClient`

### 3) Configurar la pantalla TFT
La librería `TFT_eSPI` requiere editar el archivo de configuración del usuario para los pines de tu pantalla.

En el proyecto se incluye un ejemplo de configuración de pines en `config.h`, que debes usar junto con la configuración de `TFT_eSPI`.

## Estructura del proyecto

```
Rayquaza-AI-Bot/
├── Rayquaza_Bot.ino
├── config.h
├── secrets.h.example
├── WIRING_DIAGRAM.md
├── README.md
└── .gitignore
```

## Cómo usar

1. Copia `secrets.h.example` a `secrets.h`
2. Cambia la SSID, contraseña y API key
3. Abre `Rayquaza_Bot.ino` en Arduino IDE
4. Selecciona la placa `ESP32-S3 Dev Module`
5. Compila y carga

## Pantallas previstas

### 1. Pantalla principal
- Título: `Rayquaza Bot`
- Indicador WiFi
- Fecha y hora
- Aviso: `Desliza la pantalla`
- Animación de personaje

### 2. Pantalla de voz IA
- Chat de Voz AI
- Preguntas sugeridas
- Grabación de voz
- Respuesta hablada o de texto

### 3. Pantalla de configuración
- WiFi editable
- Contraseña editable
- API key editable
- Volumen
- Batería
- IP

## Comandos seriales útiles

Abre el Monitor Serial en 115200 baudios y usa estos comandos:

- `HOME`
- `VOICE`
- `SETTINGS`
- `WIFI myssid mypassword`
- `API sk-xxxxx`
- `VOL 80`
- `SAVE`
- `CONNECT`

## Nota importante

Este proyecto es una base funcional para prototipo y desarrollo. La parte visual, la animación del personaje y la integración real con IA se pueden ampliar según tu necesidad.

## Licencia

MIT
