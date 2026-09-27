#ifndef RAYQUAZA_ANIMATION_H
#define RAYQUAZA_ANIMATION_H

#include <TFT_eSPI.h>

extern TFT_eSPI tft;

class RayquazaAnimator {
  private:
    int frameCounter = 0;
    bool isSpeaking = false;
    int mouthOpen = 0;
    
  public:
    RayquazaAnimator() {}
    
    void setSpeaking(bool speaking) {
      isSpeaking = speaking;
      if (!speaking) {
        mouthOpen = 0;
        frameCounter = 0;
      }
    }
    
    void drawRayquaza(int x, int y, bool animated = true) {
      // Cuerpo principal - Verde azulado
      tft.fillRoundRect(x + 20, y + 30, 110, 60, 15, 0x0A8B5C); // Verde oscuro
      
      // Cabeza
      tft.fillCircle(x + 75, y + 20, 20, 0x0A8B5C);
      
      // Ojos
      tft.fillCircle(x + 65, y + 15, 5, TFT_YELLOW);
      tft.fillCircle(x + 85, y + 15, 5, TFT_YELLOW);
      
      // Pupilas
      tft.fillCircle(x + 66, y + 15, 2, TFT_BLACK);
      tft.fillCircle(x + 84, y + 15, 2, TFT_BLACK);
      
      // Boca (abierta/cerrada según estado)
      if (animated && isSpeaking) {
        frameCounter++;
        mouthOpen = (frameCounter / 5) % 2; // Alterna cada 5 frames
      }
      
      if (mouthOpen == 1) {
        // Boca abierta
        tft.fillRoundRect(x + 70, y + 25, 10, 6, 2, TFT_RED);
        tft.drawLine(x + 70, y + 28, x + 80, y + 28, TFT_BLACK);
      } else {
        // Boca cerrada
        tft.drawLine(x + 70, y + 26, x + 80, y + 26, TFT_BLACK);
      }
      
      // Aletas/alas (arriba)
      tft.fillTriangle(x + 30, y + 25, x + 25, y + 5, x + 35, y + 15, 0x049E7A);
      tft.fillTriangle(x + 120, y + 25, x + 125, y + 5, x + 115, y + 15, 0x049E7A);
      
      // Cola
      tft.fillTriangle(x + 130, y + 50, x + 160, y + 45, x + 145, y + 65, 0x049E7A);
      tft.fillTriangle(x + 160, y + 45, x + 175, y + 35, x + 165, y + 55, 0x0A8B5C);
      
      // Gemas en el cuerpo (indicadores de energía)
      tft.fillCircle(x + 50, y + 50, 3, TFT_CYAN);
      tft.fillCircle(x + 75, y + 55, 3, TFT_CYAN);
      tft.fillCircle(x + 100, y + 50, 3, TFT_CYAN);
      
      // Efecto brillo si está activo
      if (isSpeaking) {
        tft.drawRoundRect(x + 20, y + 30, 110, 60, 15, TFT_YELLOW);
      }
    }
    
    void drawRayquazaResting(int x, int y) {
      isSpeaking = false;
      frameCounter = 0;
      drawRayquaza(x, y, false);
    }
    
    void update() {
      if (isSpeaking) {
        frameCounter++;
      }
    }
};

#endif
