# Boris CYD — Interfejs OpenClaw

Firmware dla ESP32-2432S028R ("Cheap Yellow Display") stanowiący fizyczny interfejs dla agenta AI OpenClaw na platformie Blynk IoT 2.0.

## Sprzęt

| Komponent | Opis |
|---|---|
| Płytka | ESP32-2432S028R (Cheap Yellow Display) |
| Wyświetlacz | ILI9341, 2.8", 240×320, SPI |
| Dotyk | XPT2046, rezystancyjny, osobna magistrala SPI |

## Funkcje

- Wyświetla twarz niedźwiadka Borisa w 7 emocjach sterowanych przez OpenClaw
- Pokazuje wiadomości tekstowe od agenta
- Przycisk **Zaczep** — wysyła sygnał `Boris_Poke` do OpenClaw
- Przycisk **Przytul** — wysyła sygnał `Boris_Cuddle` do OpenClaw
- Obsługa WiFi: automatyczne łączenie z zapisaną siecią + UI do wyboru sieci w razie braku połączenia
- Auto-reconnect: po utracie połączenia wraca do interfejsu gdy sieć wróci

## Układ ekranu

```
┌────────────────────────┐
│   twarz niedźwiadka    │  ASCII art, 15 linii
│   (emocja z V3)        │
├────────────────────────┤
│   tekst z V0           │  wiadomość od OpenClaw
│                        │
├────────────────────────┤
│  [ Zaczep ] [ Przytul ]│  przyciski dotykowe
└────────────────────────┘
```

## Emocje twarzy (V3)

| Wartość | Emocja | Kolor |
|---|---|---|
| 0 | Neutral | Biały |
| 1 | Happy | Żółty |
| 2 | Sad | Niebieski |
| 3 | Angry | Czerwony |
| 4 | Sleepy | Szary |
| 5 | Excited | Zielony |
| 6 | Love | Różowy |

## Blynk Virtual Pins

| Pin | Kierunek | Typ | Opis |
|---|---|---|---|
| V0 | OpenClaw → ESP | String | Tekst wiadomości |
| V1 | ESP → OpenClaw | String `"Boris_Poke"` | Przycisk Zaczep |
| V2 | ESP → OpenClaw | Int `1` | Przycisk Przytul |
| V3 | OpenClaw → ESP | Int 0–6 | Emocja twarzy |

## Konfiguracja przed pierwszym uruchomieniem

Edytuj [src/config.h](src/config.h):

```cpp
#define WIFI_SSID     "NazwaSieci"
#define WIFI_PASS     "HasloDoSieci"

#define BLYNK_TEMPLATE_ID   "TMPLxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "Boris_Interface"
#define BLYNK_AUTH_TOKEN    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
```

Dane Blynk znajdziesz na [blynk.cloud](https://blynk.cloud) po stworzeniu szablonu z datastreams V0–V3.

## Struktura projektu

```
esp32_boris/
├── platformio.ini       — konfiguracja PlatformIO
├── include/
│   └── User_Setup.h     — konfiguracja biblioteki TFT_eSPI
└── src/
    ├── config.h         — WiFi, Blynk, pinout, kalibracja dotyku
    ├── bear.h           — ASCII art twarzy (7 emocji) + funkcja rysowania
    ├── keyboard.h       — klawiatura QWERTY na dotyku
    ├── wifi_setup.h     — UI wyboru sieci, skanowanie, ekran hasła
    └── main.cpp         — maszyna stanów, obsługa Blynk, pętla główna
```

## Biblioteki

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) by Bodmer
- [XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen) by Paul Stoffregen
- [Blynk](https://github.com/blynkkk/blynk-library) by Blynk

## Kompilacja i wgrywanie

```bash
pio run -t upload
pio device monitor
```
