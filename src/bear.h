#pragma once
#include <TFT_eSPI.h>

// 7 emocji — indeks odpowiada wartości V3
// Każda twarz: 15 linii × maks. 40 znaków
// Font GLCD (size=1): 6×8px → całość mieści się w 240×120px (portrait)
// Struktura: oczy i usta wymienione, reszta identyczna z oryginałem

// Bazowy kształt (linie 0-4 i 11-14 niezmienne dla każdej emocji)
// Linie 5-10 zawierają oczy i usta — zmieniane per emocja

static const char* BEAR_NEUTRAL[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        _         _        \\",
    "   |         0}       {0         |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |      .-.      |     /",
    "     `.   | . . /   \\ . . |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char* BEAR_HAPPY[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        _         _        \\",
    "   |         ^}       {^         |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |     \\___/     |     /",
    "     `.   | . .         . . |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char* BEAR_SAD[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        _         _        \\",
    "   |         T}       {T         |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |      ___      |     /",
    "     `.   | . . /   \\ . . |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char* BEAR_ANGRY[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        \\         /        \\",
    "   |         >}       {<         |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |      ___      |     /",
    "     `.   | >  .     .  < |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char* BEAR_SLEEPY[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        _         _        \\",
    "   |        --}       {--        |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |      ~-~      |     /",
    "     `.   | z . /   \\ . z |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char* BEAR_EXCITED[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        _         _        \\",
    "   |         O}       {O         |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |     \\___/     |     /",
    "     `.   | ! .         . ! |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char* BEAR_LOVE[] = {
    " .'\"'.        ___,,,___        .'``.",
    ": (\\  `.\"'\"```         ```\"'\"-'  /) ;",
    " :  \\                         `./  .'",
    "  `.                            :.'",
    "    /        _         _        \\  <3",
    "   |         *}       {*         |",
    "   |         /         \\         |",
    "   |        /           \\        |",
    "   |       /             \\       |",
    "    \\     |      ~*~      |     /",
    "     `.   | <3 /   \\ 3> |   .'",
    "       `-._\\.'.(     ).'./_.-'",
    "           `\\'  `._.'  '/'",
    "             `. --'-- .'",
    "               `-...-'"
};

static const char** BEAR_FACES[] = {
    BEAR_NEUTRAL,   // 0
    BEAR_HAPPY,     // 1
    BEAR_SAD,       // 2
    BEAR_ANGRY,     // 3
    BEAR_SLEEPY,    // 4
    BEAR_EXCITED,   // 5
    BEAR_LOVE       // 6
};

static const uint16_t BEAR_COLORS[] = {
    TFT_WHITE,    // 0 Neutral
    TFT_YELLOW,   // 1 Happy
    0x5D9B,       // 2 Sad — niebieskawa biel
    TFT_RED,      // 3 Angry
    0xC618,       // 4 Sleepy — szary
    TFT_GREEN,    // 5 Excited
    TFT_PINK      // 6 Love
};

#define BEAR_FACE_LINES  15
#define BEAR_EMOTION_MAX  6

inline void drawBearFace(TFT_eSPI& tft, uint8_t emotion) {
    if (emotion > BEAR_EMOTION_MAX) emotion = 0;
    const char** face = BEAR_FACES[emotion];
    uint16_t color = BEAR_COLORS[emotion];

    tft.fillRect(0, 0, 240, 130, TFT_BLACK);
    tft.setTextFont(1);       // GLCD 6×8
    tft.setTextSize(1);
    tft.setTextColor(color, TFT_BLACK);

    for (int i = 0; i < BEAR_FACE_LINES; i++) {
        tft.setCursor(0, 5 + i * 8);
        tft.print(face[i]);
    }
}
