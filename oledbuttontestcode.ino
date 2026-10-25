
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C

#define OLED_SDA 40
#define OLED_SCL 41

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// Button GPIOs
#define BTN_FL 43
#define BTN_FR 44
#define BTN_RL 47
#define BTN_RR 48

const int buttonPins[] = {
  BTN_FL, BTN_FR, BTN_RL, BTN_RR
};

const char* buttonNames[] = {
  "Front Left",
  "Front Right",
  "Rear Left",
  "Rear Right"
};

const char* shortNames[] = {
  "FL", "FR", "RL", "RR"
};

bool lastReading[4] = {HIGH, HIGH, HIGH, HIGH};
bool stableState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastChangeTime[4] = {0, 0, 0, 0};

const unsigned long debounceMs = 35;

int selectedCorner = -1;

void drawScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MAZE SOLVER");
  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  display.setCursor(0, 17);
  display.println("OLED + BUTTON TEST");

  display.setCursor(0, 30);
  display.print("Selected: ");

  if (selectedCorner == -1) {
    display.println("NONE");
  } else {
    display.println(shortNames[selectedCorner]);
  }

  display.setCursor(0, 43);
  display.println("Press a corner button");

  display.setCursor(0, 54);
  display.println("FL   FR   RL   RR");

  display.display();
}

void showPressedButton(int index) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("BUTTON DETECTED");
  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 22);
  display.println(shortNames[index]);

  display.setTextSize(1);
  display.setCursor(0, 45);
  display.println(buttonNames[index]);

  display.setCursor(0, 56);
  display.println("Corner selected!");

  display.display();
}

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    // OLED initialization failed; stop here.
    while (true) {
      delay(100);
    }
  }

  drawScreen();
}

void loop() {
  unsigned long now = millis();

  for (int i = 0; i < 4; i++) {
    bool reading = digitalRead(buttonPins[i]);

    if (reading != lastReading[i]) {
      lastChangeTime[i] = now;
      lastReading[i] = reading;
    }

    if ((now - lastChangeTime[i] >= debounceMs) &&
        (reading != stableState[i])) {

      stableState[i] = reading;

      // Detect button press, not release.
      if (stableState[i] == LOW) {
        selectedCorner = i;
        showPressedButton(i);
      } else {
        drawScreen();
      }
    }
  }
}
