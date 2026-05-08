#pragma once
#include <TFT_eSPI.h>
#include "config.h"

// ─── Klawiatura QWERTY na TFT (240×320 portrait) ─────────────────────────────
// Układ:
//   y=0..29   pole hasła (nagłówek + tekst)
//   y=30..49  nazwa sieci
//   y=55..299 klawiatura (4 rzędy klawiszy)
//   y=300..319 przycisk Połącz

#define KB_Y_START    55
#define KB_KEY_H      40     // wysokość klawisza
#define KB_KEY_W_10   24     // szerokość przy 10 klawiszach
#define KB_KEY_W_9    26     // szerokość przy 9 klawiszach
#define KB_KEY_W_7    28     // szerokość przy 7 klawiszach
#define KB_PASS_MAX   63

// Kody specjalne zwracane przez processTouch()
#define KB_NONE      0
#define KB_BACKSPACE 8
#define KB_SHIFT     1
#define KB_CONNECT   2
#define KB_SPACE     ' '

static const char KB_ROW0[] = "QWERTYUIOP";   // 10 klawiszy
static const char KB_ROW1[] = "ASDFGHJKL";    // 9 klawiszy
static const char KB_ROW2[] = "ZXCVBNM";      // 7 klawiszy
// rząd 3: [Shift][  Space  ][⌫][OK]

struct Keyboard {
    TFT_eSPI& tft;
    char password[KB_PASS_MAX + 1];
    uint8_t passLen;
    bool shiftOn;
    char ssid[33];

    Keyboard(TFT_eSPI& display) : tft(display), passLen(0), shiftOn(false) {
        password[0] = '\0';
        ssid[0] = '\0';
    }

    void setSsid(const char* name) {
        strncpy(ssid, name, 32);
        ssid[32] = '\0';
    }

    void reset() {
        passLen = 0;
        password[0] = '\0';
        shiftOn = false;
    }

    void draw() {
        tft.fillScreen(COLOR_BG);

        // Nagłówek sieci
        tft.setTextFont(2);
        tft.setTextColor(TFT_CYAN, COLOR_BG);
        tft.setCursor(4, 4);
        tft.print("Siec: ");
        tft.setTextColor(TFT_WHITE, COLOR_BG);
        tft.print(ssid);

        // Pole hasła
        tft.drawRect(0, 26, 240, 26, TFT_DARKGREY);
        tft.fillRect(1, 27, 238, 24, 0x1082);
        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, 0x1082);
        tft.setCursor(4, 30);
        // maskuj hasło gwiazdkami
        for (uint8_t i = 0; i < passLen; i++) tft.print('*');
        if (passLen == 0) {
            tft.setTextColor(TFT_DARKGREY, 0x1082);
            tft.print("haslo...");
        }

        drawKeys();
        drawConnectBtn();
    }

    void drawKeys() {
        drawRow(0, KB_ROW0, 10, KB_KEY_W_10, 0);
        drawRow(1, KB_ROW1,  9, KB_KEY_W_9,  (240 - 9*KB_KEY_W_9)/2);
        drawRow(2, KB_ROW2,  7, KB_KEY_W_7,  (240 - 7*KB_KEY_W_7 - 2*44)/2 + 44);
        drawSpecialRow();
    }

    void drawRow(uint8_t row, const char* keys, uint8_t count, uint8_t kw, uint8_t xOff) {
        uint16_t y = KB_Y_START + row * KB_KEY_H;
        for (uint8_t i = 0; i < count; i++) {
            uint16_t x = xOff + i * kw;
            drawKey(x, y, kw - 2, KB_KEY_H - 2, shiftOn ? keys[i] : tolower(keys[i]));
        }
    }

    void drawSpecialRow() {
        uint16_t y = KB_Y_START + 3 * KB_KEY_H;
        // Shift (44px)
        tft.fillRoundRect(0, y, 42, KB_KEY_H - 2, 4, shiftOn ? TFT_YELLOW : COLOR_KEY_SPEC);
        tft.setTextFont(1);
        tft.setTextSize(1);
        tft.setTextColor(shiftOn ? TFT_BLACK : COLOR_KEY_TXT, shiftOn ? TFT_YELLOW : COLOR_KEY_SPEC);
        tft.setCursor(8, y + 14);
        tft.print("Shift");

        // Space (110px)
        tft.fillRoundRect(44, y, 108, KB_KEY_H - 2, 4, COLOR_KEY_BG);
        tft.setTextColor(COLOR_KEY_TXT, COLOR_KEY_BG);
        tft.setCursor(82, y + 14);
        tft.print("___");

        // Backspace (44px)
        tft.fillRoundRect(154, y, 42, KB_KEY_H - 2, 4, COLOR_KEY_SPEC);
        tft.setTextColor(COLOR_KEY_TXT, COLOR_KEY_SPEC);
        tft.setCursor(162, y + 14);
        tft.print("<--");

        // OK (44px)
        tft.fillRoundRect(198, y, 42, KB_KEY_H - 2, 4, TFT_GREEN);
        tft.setTextColor(TFT_BLACK, TFT_GREEN);
        tft.setCursor(210, y + 14);
        tft.print("OK");
    }

    void drawKey(uint16_t x, uint16_t y, uint8_t w, uint8_t h, char c) {
        tft.fillRoundRect(x + 1, y + 1, w, h, 4, COLOR_KEY_BG);
        tft.setTextFont(2);
        tft.setTextSize(1);
        tft.setTextColor(COLOR_KEY_TXT, COLOR_KEY_BG);
        tft.setCursor(x + w/2 - 4, y + h/2 - 7);
        tft.print(c);
    }

    void drawConnectBtn() {
        tft.fillRoundRect(20, 300, 200, 18, 6, TFT_BLUE);
        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, TFT_BLUE);
        tft.setCursor(72, 302);
        tft.print("Polacz");
    }

    void redrawPassField() {
        tft.fillRect(1, 27, 238, 24, 0x1082);
        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, 0x1082);
        tft.setCursor(4, 30);
        if (passLen == 0) {
            tft.setTextColor(TFT_DARKGREY, 0x1082);
            tft.print("haslo...");
        } else {
            for (uint8_t i = 0; i < passLen; i++) tft.print('*');
        }
    }

    // Zwraca: KB_NONE, KB_BACKSPACE, KB_SHIFT, KB_CONNECT, ' ', lub znak litery
    char processTouch(uint16_t tx, uint16_t ty) {
        // Przycisk Połącz (dolny obszar)
        if (ty >= 298) return KB_CONNECT;

        if (ty < KB_Y_START || ty >= KB_Y_START + 4 * KB_KEY_H) return KB_NONE;

        uint8_t row = (ty - KB_Y_START) / KB_KEY_H;

        if (row == 3) {
            // Rząd specjalny
            if (tx < 44)  return KB_SHIFT;
            if (tx < 154) return KB_SPACE;
            if (tx < 198) return KB_BACKSPACE;
            return KB_CONNECT;
        }

        if (row == 0) {
            uint8_t idx = tx / KB_KEY_W_10;
            if (idx < 10) return shiftOn ? KB_ROW0[idx] : tolower(KB_ROW0[idx]);
        }
        if (row == 1) {
            int8_t xOff = (240 - 9*KB_KEY_W_9) / 2;
            int16_t local = (int16_t)tx - xOff;
            if (local >= 0) {
                uint8_t idx = local / KB_KEY_W_9;
                if (idx < 9) return shiftOn ? KB_ROW1[idx] : tolower(KB_ROW1[idx]);
            }
        }
        if (row == 2) {
            int8_t xShift = (240 - 7*KB_KEY_W_7 - 2*44) / 2 + 44;
            int16_t local = (int16_t)tx - xShift;
            if (local >= 0) {
                uint8_t idx = local / KB_KEY_W_7;
                if (idx < 7) return shiftOn ? KB_ROW2[idx] : tolower(KB_ROW2[idx]);
            }
        }
        return KB_NONE;
    }

    // Obsłuż wciśnięcie — zwraca true gdy użytkownik potwierdził (Połącz)
    bool handleKey(char key) {
        if (key == KB_NONE) return false;

        if (key == KB_SHIFT) {
            shiftOn = !shiftOn;
            drawSpecialRow();
            return false;
        }
        if (key == KB_BACKSPACE) {
            if (passLen > 0) { passLen--; password[passLen] = '\0'; }
            redrawPassField();
            return false;
        }
        if (key == KB_CONNECT) return true;

        if (passLen < KB_PASS_MAX) {
            password[passLen++] = key;
            password[passLen] = '\0';
        }
        redrawPassField();
        return false;
    }

    const char* getPassword() const { return password; }
};
