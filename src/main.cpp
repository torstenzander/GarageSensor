#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>

// ======================================================
// Reedkontakt
// ======================================================

constexpr int REED_PIN = 2;

// ======================================================
// ThinkNode M2 / SX1262
// ======================================================

constexpr int LORA_CS = 10;
constexpr int LORA_SCK = 12;
constexpr int LORA_MOSI = 11;
constexpr int LORA_MISO = 13;
constexpr int LORA_RST = 21;
constexpr int LORA_BUSY = 14;
constexpr int LORA_DIO1 = 3;
constexpr int LORA_POWER = 48;

// ======================================================
// Heartbeat
// ======================================================

constexpr unsigned long STATUS_INTERVAL_MS =
    60UL * 1000UL;

// ======================================================
// SX1262
// ======================================================

SX1262 radio = new Module(
    LORA_CS,
    LORA_DIO1,
    LORA_RST,
    LORA_BUSY);

// ======================================================
// Status
// ======================================================

int lastState = -1;
unsigned long lastStatusSend = 0;

// ======================================================
// Zustand senden
// ======================================================

void sendGarageState(int state, bool heartbeat = false)
{
    const char *message =
        (state == LOW)
            ? "GARAGE_CLOSED"
            : "GARAGE_OPEN";

    if (heartbeat)
    {
        Serial.print("Heartbeat -> ");
    }
    else
    {
        Serial.print("Reed -> ");
    }

    Serial.print(
        state == LOW
            ? "GESCHLOSSEN"
            : "OFFEN");

    Serial.print(" -> Sende ");
    Serial.print(message);
    Serial.print(" ... ");

    int result = radio.transmit(message);

    if (result == RADIOLIB_ERR_NONE)
    {
        Serial.println("OK");
    }
    else
    {
        Serial.print("FEHLER ");
        Serial.println(result);
    }

    lastStatusSend = millis();
}

// ======================================================
// Setup
// ======================================================

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println("============================");
    Serial.println(" GARAGE SENSOR");
    Serial.println("============================");

    // Reedkontakt zwischen GPIO2 und GND
    pinMode(REED_PIN, INPUT_PULLUP);

    // --------------------------------------------------
    // SX1262 einschalten
    // --------------------------------------------------

    pinMode(LORA_POWER, OUTPUT);
    digitalWrite(LORA_POWER, HIGH);

    delay(100);

    // --------------------------------------------------
    // SPI
    // --------------------------------------------------

    SPI.begin(
        LORA_SCK,
        LORA_MISO,
        LORA_MOSI,
        LORA_CS);

    // --------------------------------------------------
    // LoRa
    // --------------------------------------------------

    Serial.print("SX1262 init ... ");

    int state = radio.begin(
        868.0,
        125.0,
        9,
        7,
        RADIOLIB_SX126X_SYNC_WORD_PRIVATE,
        14,
        8,
        3.3);

    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("FEHLER: ");
        Serial.println(state);

        while (true)
        {
            delay(1000);
        }
    }

    radio.setDio2AsRfSwitch(true);

    Serial.println("OK");

    // --------------------------------------------------
    // aktuellen Torzustand sofort senden
    // --------------------------------------------------

    lastState = digitalRead(REED_PIN);

    sendGarageState(lastState);
}

// ======================================================
// Loop
// ======================================================

void loop()
{
    int currentState = digitalRead(REED_PIN);

    // --------------------------------------------------
    // Reedkontakt geändert
    // --------------------------------------------------

    if (currentState != lastState)
    {
        // Entprellen
        delay(100);

        currentState = digitalRead(REED_PIN);

        if (currentState != lastState)
        {
            lastState = currentState;

            // sofort senden
            sendGarageState(lastState);
        }
    }

    // --------------------------------------------------
    // Heartbeat alle 60 Sekunden
    // --------------------------------------------------

    if (
        millis() - lastStatusSend >=
        STATUS_INTERVAL_MS)
    {
        sendGarageState(
            lastState,
            true);
    }

    delay(20);
}