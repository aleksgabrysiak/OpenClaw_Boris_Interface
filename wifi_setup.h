#pragma once
#include <TFT_eSPI.h>
#include <WiFi.h>
#include "config.h"
#include "keyboard.h"

// ─── Ekran listy sieci WiFi i wpisywania hasła ────────────────────────────────

#define NET_LIST_MAX   8
#define NET_ITEM_H    35
#define NET_ITEM_Y0   50    // y początku listy (po nagłówku)

struct WifiSetup {
    TFT_eSPI& tft;
    Keyboard  kb;

    int      netCount;
    char     netSSID[NET_LIST_MAX][33];
    int32_t  netRSSI[NET_LIST_MAX];
    uint8_t  selectedIdx;

    WifiSetup(TFT_eSPI& display) : tft(display), kb(display), netCount(0), selectedIdx(0) {}

    // ── Ekran ostrzeżenia o braku WiFi ────────────────────────────────────────
    void drawErrorScreen() {
        tft.fillScreen(COLOR_BG);
        tft.setTextFont(4);
        tft.setTextColor(COLOR_WARN, COLOR_BG);
        tft.setCursor(10, 30);
        tft.print("Brak WiFi!");

        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, COLOR_BG);
        tft.setCursor(10, 80);
        tft.print("Nie mozna polaczyc z:");
        tft.setCursor(10, 100);
        tft.setTextColor(TFT_CYAN, COLOR_BG);
        tft.print(WIFI_SSID);

        // Przycisk "Wybierz siec"
        tft.fillRoundRect(20, 200, 200, 50, 8, TFT_BLUE);
        tft.setTextFont(4);
        tft.setTextColor(TFT_WHITE, TFT_BLUE);
        tft.setCursor(28, 213);
        tft.print("Wybierz siec");
    }

    // Zwraca true gdy użytkownik dotknął przycisku "Wybierz sieć"
    bool errorScreenTapped(uint16_t tx, uint16_t ty) {
        return (tx >= 20 && tx <= 220 && ty >= 200 && ty <= 250);
    }

    // ── Skanowanie i wyświetlenie listy sieci ─────────────────────────────────
    void scanAndDraw() {
        tft.fillScreen(COLOR_BG);
        tft.setTextFont(2);
        tft.setTextColor(TFT_CYAN, COLOR_BG);
        tft.setCursor(4, 4);
        tft.print("Skanowanie sieci...");

        int found = WiFi.scanNetworks();
        netCount = min(found, NET_LIST_MAX);

        // Sortuj po RSSI (prosty bubble sort)
        for (int i = 0; i < netCount - 1; i++) {
            for (int j = i + 1; j < netCount; j++) {
                if (WiFi.RSSI(j) > WiFi.RSSI(i)) {
                    // swap w WiFi scan nie jest możliwy, przechowaj lokalnie
                }
            }
        }

        // Zachowaj lokalnie (WiFi.scanNetworks wyniki są ulotne)
        for (int i = 0; i < netCount; i++) {
            strncpy(netSSID[i], WiFi.SSID(i).c_str(), 32);
            netSSID[i][32] = '\0';
            netRSSI[i] = WiFi.RSSI(i);
        }
        WiFi.scanDelete();

        drawNetList();
    }

    void drawNetList() {
        tft.fillScreen(COLOR_BG);
        tft.setTextFont(2);
        tft.setTextColor(TFT_CYAN, COLOR_BG);
        tft.setCursor(4, 4);
        tft.print("Dostepne sieci:");

        // Przycisk odświeżania
        tft.fillRoundRect(170, 2, 68, 22, 4, 0x2945);
        tft.setTextFont(1);
        tft.setTextColor(TFT_WHITE, 0x2945);
        tft.setCursor(178, 8);
        tft.print("Odswiez");

        for (int i = 0; i < netCount; i++) {
            drawNetItem(i);
        }

        if (netCount == 0) {
            tft.setTextFont(2);
            tft.setTextColor(TFT_ORANGE, COLOR_BG);
            tft.setCursor(20, 100);
            tft.print("Nie znaleziono sieci");
        }
    }

    void drawNetItem(int idx) {
        uint16_t y = NET_ITEM_Y0 + idx * NET_ITEM_H;
        uint16_t bg = (idx % 2 == 0) ? COLOR_NET_BG : 0x0841;

        tft.fillRect(0, y, 240, NET_ITEM_H - 1, bg);

        // SSID
        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, bg);
        tft.setCursor(4, y + 4);
        // Ucinaj długie nazwy
        char truncated[20];
        strncpy(truncated, netSSID[idx], 19);
        truncated[19] = '\0';
        tft.print(truncated);

        // Siłka sygnału (paski)
        drawSignalBars(210, y + 6, netRSSI[idx]);
    }

    void drawSignalBars(uint16_t x, uint16_t y, int32_t rssi) {
        // 4 paski: każdy 5×(h) px z odstępem 2px
        uint8_t bars = 0;
        if (rssi > -50) bars = 4;
        else if (rssi > -65) bars = 3;
        else if (rssi > -75) bars = 2;
        else if (rssi > -85) bars = 1;

        for (uint8_t i = 0; i < 4; i++) {
            uint8_t h = 4 + i * 4;
            uint16_t bx = x + i * 7;
            uint16_t by = y + 16 - h;
            uint16_t color = (i < bars) ? TFT_GREEN : TFT_DARKGREY;
            tft.fillRect(bx, by, 5, h, color);
        }
    }

    // Zwraca indeks sieci jeśli kliknięto element listy, -1 jeśli odśwież, -2 brak
    int netListTapped(uint16_t tx, uint16_t ty) {
        if (ty < 2 && ty < 24 && tx >= 170) return -1; // Odśwież
        if (ty >= NET_ITEM_Y0) {
            int idx = (ty - NET_ITEM_Y0) / NET_ITEM_H;
            if (idx >= 0 && idx < netCount) return idx;
        }
        return -2;
    }

    // ── Ekran klawiatury dla wybranej sieci ───────────────────────────────────
    void drawKeyboard(int netIdx) {
        selectedIdx = netIdx;
        kb.reset();
        kb.setSsid(netSSID[netIdx]);
        kb.draw();
    }

    // Zwraca true gdy hasło potwierdzone — w kb.getPassword() jest hasło
    bool keyboardTouched(uint16_t tx, uint16_t ty) {
        char key = kb.processTouch(tx, ty);
        return kb.handleKey(key);
    }

    // ── Ekran łączenia ─────────────────────────────────────────────────────────
    void drawConnecting(const char* ssid) {
        tft.fillScreen(COLOR_BG);
        tft.setTextFont(4);
        tft.setTextColor(TFT_CYAN, COLOR_BG);
        tft.setCursor(10, 80);
        tft.print("Laczenie...");
        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, COLOR_BG);
        tft.setCursor(10, 130);
        tft.print(ssid);
    }

    // ── Ekran błędu połączenia ─────────────────────────────────────────────────
    void drawConnectFailed() {
        tft.fillScreen(COLOR_BG);
        tft.setTextFont(4);
        tft.setTextColor(TFT_RED, COLOR_BG);
        tft.setCursor(10, 80);
        tft.print("Blad polaczenia");
        tft.setTextFont(2);
        tft.setTextColor(TFT_WHITE, COLOR_BG);
        tft.setCursor(10, 140);
        tft.print("Sprawdz haslo i sprobuj");
        tft.setCursor(10, 160);
        tft.print("ponownie.");
        delay(2500);
    }

    const char* getSelectedSSID() const { return netSSID[selectedIdx]; }
    const char* getPassword() const     { return kb.getPassword(); }
};
