#include <string.h>
#include <Adafruit_NeoPixel.h>

enum ledMode
{
  STATIC_MODE_E,
  RAINBOW_MODE_E,
  THEATHER_MODE_E,
  THEATHER_RAINBOW_MODE_E,
  COLOE_WIPE_MODE_E
};


/* initialize Led led strip */
void initLed(void);

/* Turn on led strip */
void turnOnLed(void);

/* turn off led strip */
void turnOffLed(void);

/* set static color */
void setStaticColor(int rgb);

/* set brightness */
void setBrightness(int value);

/* different operation modes */
void theaterChaseRainbow(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void rainbowCycle(uint8_t wait);
void rainbow(uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);

