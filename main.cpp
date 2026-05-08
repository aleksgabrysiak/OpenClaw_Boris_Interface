#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <BlynkSimpleEsp32.h>

#include "config.h"
#include "bear.h"
#include "wifi_setup.h"

// ─── Hardware ─────────────────────────────────────────────────────────────────
TFT_eSPI tft;
SPIClass touchSPI(VSPI);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

WifiSetup wifiSetup(tft);

// ─── Stan aplikacji ───────────────────────────────────────────────────────────
enum AppState {
    STATE_WIFI_CONNECTING,
    STATE_WIFI_ERROR,
    STATE_WIFI_SCAN,
    STATE_WIFI_KEYBOARD,
    STATE_WIFI_CONNECTING_MANUAL,
    STATE_BLYNK_CONNECTING,
    STATE_MAIN
};

AppState currentState = STATE_WIFI_CONNECTING;

// ─── Dane z Blynk ─────────────────────────────────────────────────────────────
char    blynkMessage[256]  = "Czekam na wiadomosc...";
uint8_t bearEmotion        = 0;
bool    needRedrawFace     = false;
bool    needRedrawMessage  = false;

// ─── Timery ───────────────────────────────────────────────────────────────────
uint32_t connectStartMs = 0;
uint32_t lastTouchMs    = 0;
#define  TOUCH_DEBOUNCE_MS 300

// ─── Mapa touch → piksele ─────────────────────────────────────────────────────
uint16_t mapTouch(uint16_t raw, uint16_t rMin, uint16_t rMax, uint16_t pMax) {
    raw = constrain(raw, rMin, rMax);
    return map(raw, rMin, rMax, 0, pMax);
}

bool getTouchPx(uint16_t& px, uint16_t& py) {
    if (!ts.touched()) return false;
    TS_Point p = ts.getPoint();
    px = mapTouch(p.x, TOUCH_X_MIN, TOUCH_X_MAX, 239);
    py = mapTouch(p.y, TOUCH_Y_MIN, TOUCH_Y_MAX, 319);
    return true;
}

// ─── Blynk V0 — wiadomość od OpenClaw ─────────────────────────────────────────
BLYNK_WRITE(V0) {
    strncpy(blynkMessage, param.asStr(), 255);
    blynkMessage[255] = '\0';
    needRedrawMessage = true;
}

// ─── Blynk V3 — emocja twarzy ─────────────────────────────────────────────────
BLYNK_WRITE(V3) {
    uint8_t val = (uint8_t)constrain(param.asInt(), 0, BEAR_EMOTION_MAX);
    if (val != bearEmotion) {
        bearEmotion = val;
        needRedrawFace = true;
    }
}

BLYNK_CONNECTED() {
    Blynk.syncVirtual(V0);
    Blynk.syncVirtual(V3);
}

// ─── Rysowanie ekranu głównego ────────────────────────────────────────────────
void drawMessage() {
    tft.fillRect(0, 130, 240, 90, COLOR_BG);
    tft.setTextFont(2);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextWrap(true);
    tft.setCursor(4, 134);
    // Ogranicz do ~5 linii po ~28 znaków
    char buf[141];
    strncpy(buf, blynkMessage, 140);
    buf[140] = '\0';
    tft.print(buf);
    tft.setTextWrap(false);
}

void drawButtons() {
    // Zaczep
    tft.fillRoundRect(4, 224, 112, 88, 10, COLOR_BTN_POKE);
    tft.setTextFont(4);
    tft.setTextColor(COLOR_BTN_TXT, COLOR_BTN_POKE);
    tft.setCursor(22, 254);
    tft.print("Zaczep");

    // Przytul
    tft.fillRoundRect(124, 224, 112, 88, 10, COLOR_BTN_HUG);
    tft.setTextFont(4);
    tft.setTextColor(COLOR_BTN_TXT, COLOR_BTN_HUG);
    tft.setCursor(138, 254);
    tft.print("Przytul");
}

void drawMainScreen() {
    tft.fillScreen(COLOR_BG);
    drawBearFace(tft, bearEmotion);
    drawMessage();
    drawButtons();
}

// ─── Łączenie z Blynk ─────────────────────────────────────────────────────────
void showBlynkConnecting() {
    tft.fillScreen(COLOR_BG);
    tft.setTextFont(4);
    tft.setTextColor(TFT_CYAN, COLOR_BG);
    tft.setCursor(10, 80);
    tft.print("Blynk...");
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE, COLOR_BG);
    tft.setCursor(10, 130);
    tft.print("Laczenie z chmura");
}

void setState(AppState s) {
    currentState = s;
    connectStartMs = millis();
}

// ─── setup() ──────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);

    // Wyświetlacz
    tft.init();
    tft.setRotation(0);      // portrait
    tft.fillScreen(TFT_BLACK);
    tft.setTextWrap(false);

    // Touch (VSPI, osobna magistrala)
    touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
    ts.begin(touchSPI);
    ts.setRotation(0);

    // Splash
    tft.setTextFont(4);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(40, 100);
    tft.print("Boris CYD");
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(40, 140);
    tft.print("OpenClaw Interface");
    delay(1000);

    // Pierwsza próba połączenia z hardcoded WiFi
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(4, 4);
    tft.print("WiFi: ");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print(WIFI_SSID);
    tft.setCursor(4, 24);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("Laczenie...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    setState(STATE_WIFI_CONNECTING);
}

// ─── loop() ───────────────────────────────────────────────────────────────────
void loop() {
    uint32_t now = millis();

    // Blynk run tylko gdy połączony
    if (currentState == STATE_MAIN) {
        Blynk.run();

        // Aktualizacje od Blynk
        if (needRedrawFace) {
            drawBearFace(tft, bearEmotion);
            needRedrawFace = false;
        }
        if (needRedrawMessage) {
            drawMessage();
            needRedrawMessage = false;
        }

        // Wykryj rozłączenie WiFi
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi lost!");
            Blynk.disconnect();
            wifiSetup.drawErrorScreen();
            setState(STATE_WIFI_ERROR);
            return;
        }
    }

    // ── Obsługa dotyku ────────────────────────────────────────────────────────
    uint16_t tx = 0, ty = 0;
    bool touched = false;
    if (now - lastTouchMs > TOUCH_DEBOUNCE_MS) {
        touched = getTouchPx(tx, ty);
        if (touched) lastTouchMs = now;
    }

    // ── Maszyna stanów ────────────────────────────────────────────────────────
    switch (currentState) {

        case STATE_WIFI_CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("WiFi OK: " WIFI_SSID);
                showBlynkConnecting();
                Blynk.config(BLYNK_AUTH_TOKEN);
                Blynk.connect(5000);
                setState(STATE_BLYNK_CONNECTING);
            } else if (now - connectStartMs > WIFI_CONNECT_TIMEOUT_MS) {
                Serial.println("WiFi timeout");
                wifiSetup.drawErrorScreen();
                setState(STATE_WIFI_ERROR);
            }
            break;

        case STATE_WIFI_ERROR:
            if (touched && wifiSetup.errorScreenTapped(tx, ty)) {
                wifiSetup.scanAndDraw();
                setState(STATE_WIFI_SCAN);
            }
            break;

        case STATE_WIFI_SCAN:
            if (touched) {
                int idx = wifiSetup.netListTapped(tx, ty);
                if (idx == -1) {
                    // Odśwież
                    wifiSetup.scanAndDraw();
                } else if (idx >= 0) {
                    wifiSetup.drawKeyboard(idx);
                    setState(STATE_WIFI_KEYBOARD);
                }
            }
            break;

        case STATE_WIFI_KEYBOARD:
            if (touched) {
                bool confirmed = wifiSetup.keyboardTouched(tx, ty);
                if (confirmed) {
                    const char* ssid = wifiSetup.getSelectedSSID();
                    const char* pass = wifiSetup.getPassword();
                    Serial.print("Connecting to: "); Serial.println(ssid);
                    wifiSetup.drawConnecting(ssid);
                    WiFi.disconnect();
                    WiFi.begin(ssid, pass);
                    setState(STATE_WIFI_CONNECTING_MANUAL);
                }
            }
            break;

        case STATE_WIFI_CONNECTING_MANUAL:
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("WiFi manual OK");
                showBlynkConnecting();
                Blynk.config(BLYNK_AUTH_TOKEN);
                Blynk.connect(5000);
                setState(STATE_BLYNK_CONNECTING);
            } else if (now - connectStartMs > WIFI_CONNECT_TIMEOUT_MS) {
                Serial.println("Manual WiFi timeout");
                wifiSetup.drawConnectFailed();
                wifiSetup.drawErrorScreen();
                setState(STATE_WIFI_ERROR);
            }
            break;

        case STATE_BLYNK_CONNECTING:
            if (Blynk.connected()) {
                Serial.println("Blynk connected!");
                drawMainScreen();
                setState(STATE_MAIN);
            } else if (now - connectStartMs > BLYNK_CONNECT_TIMEOUT_MS) {
                Serial.println("Blynk timeout");
                // Spróbuj jeszcze raz lub wróć do WiFi error
                if (WiFi.status() != WL_CONNECTED) {
                    wifiSetup.drawErrorScreen();
                    setState(STATE_WIFI_ERROR);
                } else {
                    // WiFi jest, retry Blynk
                    Blynk.connect(5000);
                    connectStartMs = now;
                }
            }
            break;

        case STATE_MAIN:
            if (touched) {
                // Przycisk Zaczep (lewy)
                if (tx < 120 && ty >= 224) {
                    Serial.println("Sending Boris_Poke");
                    Blynk.virtualWrite(V1, "Boris_Poke");
                    // Feedback wizualny
                    tft.fillRoundRect(4, 224, 112, 88, 10, TFT_WHITE);
                    tft.setTextFont(4);
                    tft.setTextColor(COLOR_BTN_POKE, TFT_WHITE);
                    tft.setCursor(22, 254);
                    tft.print("Zaczep");
                    delay(150);
                    drawButtons();
                }
                // Przycisk Przytul (prawy)
                else if (tx >= 120 && ty >= 224) {
                    Serial.println("Sending Boris_Cuddle");
                    Blynk.virtualWrite(V2, 1);
                    // Feedback wizualny
                    tft.fillRoundRect(124, 224, 112, 88, 10, TFT_WHITE);
                    tft.setTextFont(4);
                    tft.setTextColor(COLOR_BTN_HUG, TFT_WHITE);
                    tft.setCursor(138, 254);
                    tft.print("Przytul");
                    delay(150);
                    drawButtons();
                }
            }
            break;
    }
}
