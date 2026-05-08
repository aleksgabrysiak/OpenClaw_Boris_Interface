#pragma once

// ─── WiFi (domyślna sieć hardcoded) ───────────────────────────────────────────
#define WIFI_SSID     "SSID"
#define WIFI_PASS     "Password"

// ─── Blynk IoT 2.0 ────────────────────────────────────────────────────────────
// Uzupełnij po stworzeniu szablonu na blynk.cloud
#define BLYNK_TEMPLATE_ID   "TMPL4hxDCnrEK"
#define BLYNK_TEMPLATE_NAME "Boris_Interface"
#define BLYNK_AUTH_TOKEN    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

// ─── Timeouty (ms) ────────────────────────────────────────────────────────────
#define WIFI_CONNECT_TIMEOUT_MS   15000
#define BLYNK_CONNECT_TIMEOUT_MS  10000

// ─── Touch SPI (magistrala VSPI, osobna od displeja) ──────────────────────────
#define TOUCH_CLK   25
#define TOUCH_MISO  39
#define TOUCH_MOSI  32
#define TOUCH_CS    33
#define TOUCH_IRQ   36

// ─── Kalibracja touch dla CYD 2432S028 ────────────────────────────────────────
// Wartości raw XPT2046 → piksele (portrait 240×320)
// Zmierz dokładne wartości używając przykładu TFT_eSPI Touch_calibrate
#define TOUCH_X_MIN   200
#define TOUCH_X_MAX  3700
#define TOUCH_Y_MIN   240
#define TOUCH_Y_MAX  3800

// ─── Kolory UI ────────────────────────────────────────────────────────────────
#define COLOR_BG        TFT_BLACK
#define COLOR_FACE      TFT_WHITE
#define COLOR_TEXT      TFT_CYAN
#define COLOR_BTN_POKE  0x07FF   // cyan
#define COLOR_BTN_HUG   0xF81F   // magenta
#define COLOR_BTN_TXT   TFT_BLACK
#define COLOR_WARN      TFT_ORANGE
#define COLOR_NET_BG    0x1082   // ciemnoszary
#define COLOR_NET_SEL   0x2945   // zaznaczony element
#define COLOR_KEY_BG    0x4208   // klawiatura tło
#define COLOR_KEY_TXT   TFT_WHITE
#define COLOR_KEY_SPEC  0x8410   // klawisze specjalne
