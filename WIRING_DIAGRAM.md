# Rayquaza AI Bot - Wiring Diagram

```
        ESP32-S3-CAM
        ┌──────────────────────┐
        │                      │
        │  TFT Display SPI     │
        │  CS  = GPIO 10       │
        │  DC  = GPIO 9        │
        │  RST = GPIO 8        │
        │  MOSI= GPIO 11       │
        │  SCLK= GPIO 12       │
        │  BL  = GPIO 38       │
        └──────────────────────┘
                |
                |
       +--------+---------+
       |                  |
       |  Micrófono I2S  |
       |  BCLK=GPIO42     |
       |  LRCL=GPIO1      |
       |  DIN =GPIO41     |
       |                  |
       +--------+---------+
                |
                |
        +-------+--------+
        | Amplificador   |
        | PAM8002A       |
        | OUT -> Speaker |
        | GPIO5 enable   |
        +----------------+
```

## Notas importantes

- Revisa la versión real del módulo TFT y sus pines, porque pueden variar entre placas.
- El módulo de cámara puede compartir pines de SPI, por lo que la configuración exacta depende del fabricante.
- Si tu ESP32-S3-CAM usa un pin diferente para la cámara o la pantalla, ajusta `config.h`.

## Cableado sugerido

- TFT CS -> GPIO 10
- TFT DC -> GPIO 9
- TFT RST -> GPIO 8
- TFT MOSI -> GPIO 11
- TFT SCLK -> GPIO 12
- TFT BL -> GPIO 38

- Micrófono BCLK -> GPIO 42
- Micrófono WS/LRCL -> GPIO 1
- Micrófono DIN -> GPIO 41

- Amplificador enable -> GPIO 5
- Speaker -> salida del amplificador

```
         +3.3V
          |
          +---- ESP32-S3-CAM
          |
          +---- Microfono / Display / Amplifier
```
