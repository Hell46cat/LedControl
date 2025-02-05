#include <FastLED.h>
#include <IRLremote.h>

#define NUM_LEDS 49  // Обновлено для работы с 110 диодами
#define DATA_PIN 12
#define IR_PIN 2

CRGB leds[NUM_LEDS];
CHashIR IRLremote;

// Коды кнопок пульта
#define BUTT_UP 0xF39EEBAD
#define BUTT_DOWN 0xC089F6AD
#define BUTT_LEFT 0xE25410AD
#define BUTT_RIGHT 0x14CE54AD
#define BUTT_OK 0x297C76AD
#define BUTT_1 0x4E5BA3AD
#define BUTT_2 0xE51CA6AD
#define BUTT_3 0xE207E1AD
#define BUTT_4 0x517068AD
#define BUTT_5 0x1B92DDAD
#define BUTT_6 0xAC2A56AD
#define BUTT_7 0x5484B6AD
#define BUTT_8 0xD22353AD
#define BUTT_9 0xDF3F4BAD
#define BUTT_0 0xF08A26AD

// Режимы работы
enum Mode {
  MODE_OFF, MODE_MATRIX, MODE_LONG_RAINBOW, MODE_STROBE, MODE_FIRE
};
Mode currentMode = MODE_OFF;
Mode lastActiveMode = MODE_MATRIX;

uint8_t brightness = 200;
uint8_t speed = 50;

unsigned long lastTime = 0;
unsigned long interval = 50; // Интервал в 50 миллисекунд для обновления эффектов

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  Serial.begin(9600);
  IRLremote.begin(IR_PIN);
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime;
    remoteTick();
    if (currentMode != MODE_OFF) {
      switch (currentMode) {
        case MODE_MATRIX: matrixEffect(); break;
        case MODE_LONG_RAINBOW: longRainbowEffect(); break;
        case MODE_STROBE: strobeEffect(); break;
        case MODE_FIRE: fireEffect(); break;
        default: break;
      }
    }
  }
}

void turnOffLeds() {
  FastLED.clear();
  FastLED.show();
}

// Матрица
void matrixEffect() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  leds[random(NUM_LEDS)] = CRGB::Green;
  FastLED.show();
  delay(speed);
}

// Протяжённая радуга
void longRainbowEffect() {
  static uint8_t hue = 0;
  fill_rainbow(leds, NUM_LEDS, hue, 5);
  FastLED.show();
  hue++;
  delay(speed / 2);
}

// Стробоскоп
void strobeEffect() {
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(speed);
  turnOffLeds();
  delay(speed);
}

// Огонь
void fireEffect() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(random(0, 30), 255, random(150, 255));
  }
  FastLED.show();
  delay(speed);
}

// Управление пультом
void remoteTick() {
  if (IRLremote.available()) {
    auto data = IRLremote.read();
    switch (data.command) {
      case BUTT_1: currentMode = MODE_MATRIX; break;
      case BUTT_2: currentMode = MODE_LONG_RAINBOW; break;
      case BUTT_3: currentMode = MODE_STROBE; break;
      case BUTT_4: currentMode = MODE_FIRE; break;
      case BUTT_0:
        if (currentMode == MODE_OFF) {
          currentMode = lastActiveMode;
        } else {
          lastActiveMode = currentMode;
          currentMode = MODE_OFF;
          turnOffLeds();
        }
        break;
      case BUTT_LEFT: if (speed > 10) speed -= 5; break;
      case BUTT_RIGHT: if (speed < 255) speed += 5; break;
      case BUTT_UP: if (brightness < 255) brightness += 25; FastLED.setBrightness(brightness); break;
      case BUTT_DOWN: if (brightness > 25) brightness -= 25; FastLED.setBrightness(brightness); break;
      default: break;
    }
  }
}
