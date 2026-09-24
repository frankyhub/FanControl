/*************************************************************************************************
                                      PROGRAMMINFO
**************************************************************************************************

  Funktion:  Lüftersteuerung mit WEB Server, 0,96" OLED Display, LED Ampel, Relais 1+2 (Staus rot), Relais 3 (Status gelb oder rot)

**************************************************************************************************
  Version: 22.09.2026
**************************************************************************************************
  Board: ESP32 DEV KIT V4 Boardverwalter: ESP32 Dev Module V3.3.11
  
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  http://arduino.esp8266.com/stable/package_esp8266com_index.json
  https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
**************************************************************************************************
  Libraries:
  DHT sensor library (von Adafruit)
  Adafruit SSD1306 (von Adafruit)
**************************************************************************************************
  C++ Arduino IDE V1.8.19

**************************************************************************************************
  Einstellungen:
  https://dl.espressif.com/dl/package_esp32_index.json
  http://dan.drown.org/stm32duino/package_STM32duino_index.json
  http://arduino.esp8266.com/stable/package_esp8266com_index.json

  Benötigte Komponenten:

  DHTESP32 NodeMcu Entwicklungsboard
  2x Breadboard 400
  0.96" OLED Display
  Aktiver Piezo-Buzzer
  LED Ampel Modul
  2x Hot End Lüfter
  2 Kanal Relais 5V
  Jumper Wire Kabel
  Jumper Kabel Buchse-Stecker
  DHT11 oder DHT22 Sensor

  Verdrahtung:
  Komponente  Pin (Modul) Anschluss (ESP32 / System)
  OLED-Display  VDD Vin
              GND GND
              SCK GPIO 22
              SDA GPIO 21
  DHT11-Sensor  VCC (links) Vin
              Data (Mitte-links)  GPIO 4
              GND (rechts)  GND
              Piezo-Buzzer  Plus (+)  GPIO 18
              Minus (-) GND
  LED-Ampel R (Rot)   GPIO 25
          Y (Gelb)  GPIO 26 
          G ( Grün) GPIO 27
          GND GND

  Relais-Modul  VCC Vin
              GND GND
              IN1 GPIO 33
              IN2 GPIO 32
  Relais-Modul (Schraubklemmen)
              COM (Mitte)  
              Vin (ESP32)
  Lüfter 1 & 2  Rotes Kabel (+) NO (links am Relais)
  Schwarzes Kabel (-) GND (ESP32)


  OLED-Display  VDD Vin
              GND GND
              SCK GPIO 22
              SDA GPIO 21

  DHT11-Sensor
            VCC (links) Vin
            Data (Mitte-links) GPIO 4
            GND (rechts) GND
  Piezo-Buzzer
            Plus (+)GPIO 18
            Minus (-) GND

  LED-Ampel
            R (Rot)   GPIO 25
            Y (Gelb)  GPIO 26
            G ( Grün) GPIO 27
            GND GND
  Relais-Modul
            VCC Vin
            GND GND
            IN1 GPIO 33
            IN2 GPIO 32
  Relais-Modul (Schraubklemmen)
            COM (Mitte)  Vin (ESP32)

  Lüfter 1 & 2
            Rotes Kabel (+) NO (links am Relais)
            Schwarzes Kabel (-) GND (ESP32)

**************************************************************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Importiert das externe HTML-Dashboard
#include "web_page.h"

// --- WLAN-KONFIGURATION ---
const char* wifi_ssid = "TP-ESP32";
const char* wifi_pass = "gigaset4035";

// --- PIN-BELEGUNG (HARDWARE) ---
#define DHT_PIN 4          // Daten-Pin DHT11
#define PIEZO_PIN 18       // Buzzer (Pluspol)
#define LED_RED 25         // Status: Kritisch
#define LED_YELLOW 26      // Status: Warnung
#define LED_GREEN 27       // Status: Normal
#define FAN_RELAY_1 33     // Lüfter 1
#define FAN_RELAY_2 32     // Lüfter 2
#define FAN_RELAY_3 14     // Lüfter 3

#define LED_16 16         // Status: Gelb
#define LED_17 17         // Status: Warnung
#define LED_5 5           // Status: Normal

// --- RELAIS-STEUERUNG ---
// Die meisten Relais schalten bei LOW (Active LOW)
#define RELAY_ACTIVE LOW
#define RELAY_INACTIVE HIGH

// --- SENSOR-INIT ---
DHT dhtSensor(DHT_PIN, DHT22);

// --- OLED-INIT ---
#define OLED_W 128
#define OLED_H 64
// Standard I2C-Adresse 0x3C für 128x64 Displays
Adafruit_SSD1306 oledDisplay(OLED_W, OLED_H, &Wire, -1);

WebServer webDashboard(80); // HTTP Web-Server auf Port 80

// --- GLOBALE VARIABLEN ---
float tempC = 0.0;
float humidity = 0.0;
float maxTempLimit = 30.0; // Schwellenwert (über WLAN anpassbar)
float smoothedTemp = 0.0;  // Gefilterter Temperaturwert

int systemStatus = 0; // 0=Grün, 1=Gelb, 2=Rot
bool coolingActive = false; // Status der Lüfter

// --- Filter: Gleitender Durchschnitt zur Rauschunterdrückung ---
const int filterSize = 10;
float tempBuffer[filterSize];
int bufferIndex = 0;
float tempSum = 0;

// --- Timer für Multitasking (ersetzt blockierendes delay) ---
unsigned long timeLastSensor = 0;
unsigned long timeLastDisplay = 0;

// --- Variablen für akustische Signale ---
bool isBuzzerOn = false;
int audioMode = 0;
unsigned long audioTimer = 0;
bool actionTriggered = false; // Verhindert Endlosschleife des Sounds
int melodyStep = 0;           // Zähler für die Melodie-Noten

// --- AUDIO-ROUTINE (NON BLOCKING) ---
void updateAudioAlerts() {
  if (!isBuzzerOn) {
    noTone(PIEZO_PIN);
    return;
  }
  unsigned long currentMillis = millis();

  // Modus 1: Schöner Warn-Alarm (Eleganter Akkord, 4x wiederholt)
  if (audioMode == 1) {
    // 4x A-Moll Arpeggio mit markanten Pausen (0) dazwischen
    int alertNotes[] = {
      880, 1047, 1318, 0, 0, // 1. Mal
      880, 1047, 1318, 0, 0, // 2. Mal
      880, 1047, 1318, 0, 0, // 3. Mal
      880, 1047, 1318        // 4. Mal
    };
    int numNotes = 18;

    // Wechselt die Note alle 110ms
    if (currentMillis - audioTimer > 110) {
      audioTimer = currentMillis;
      if (melodyStep < numNotes) {
        if (alertNotes[melodyStep] == 0) {
          noTone(PIEZO_PIN); // Kurze Pause zwischen den Akkorden
        } else {
          tone(PIEZO_PIN, alertNotes[melodyStep]);
        }
        melodyStep++;
      } else {
        isBuzzerOn = false;
        noTone(PIEZO_PIN);
      }
    }
  }
  // Modus 2: "System Bereit" / Deaktivierung (Die 6-Noten Level-Up Melodie)
  else if (audioMode == 2) {
    // Eine komplexe, aufsteigende A-Dur Melodie
    int successNotes[] = {440, 554, 659, 880, 1108, 1318};
    int numNotes = 6;

    // Wechselt die Note alle 90ms (fließend und schnell)
    if (currentMillis - audioTimer > 90) {
      audioTimer = currentMillis;
      if (melodyStep < numNotes) {
        tone(PIEZO_PIN, successNotes[melodyStep]);
        melodyStep++;
      } else {
        isBuzzerOn = false;
        noTone(PIEZO_PIN);
      }
    }
  }
}

void playActivationChime() {
  if (!actionTriggered) {
    isBuzzerOn = true;
    audioMode = 1;
    melodyStep = 0;
    audioTimer = millis();
    actionTriggered = true; // Sperre: Nur einmal abspielen
  }
}

void playDeactivationChime() {
  isBuzzerOn = true;
  audioMode = 2;
  melodyStep = 0;
  audioTimer = millis();
}

// --- WEB-SERVER ROUTINEN ---

// Route: "/" (Startseite)
void routeHomePage() {
  String html = index_html;
  // Aktuellen Grenzwert in das HTML-Template einfügen
  html.replace("%LIMIT%", String(maxTempLimit, 1));
  webDashboard.send(200, "text/html", html);
}

// Route: "/status" (JSON-Daten für AJAX-Updates)
void routeStatusJson() {
  char jsonBuffer[150];
  snprintf(jsonBuffer, sizeof(jsonBuffer),
           "{\"temp\":\"%.1f\",\"hum\":\"%.0f\",\"avg\":\"%.1f\",\"status\":%d}",
           smoothedTemp, humidity, smoothedTemp, systemStatus
          );
  webDashboard.send(200, "application/json", jsonBuffer);
}

// Route: "/set" (Einstellungen aktualisieren)
void routeSetLimit() {
  if (webDashboard.hasArg("threshold")) {
    maxTempLimit = webDashboard.arg("threshold").toFloat();
    Serial.println("Neues Limit gesetzt: " + String(maxTempLimit));
  }
  webDashboard.send(200, "text/plain", "OK");
}

// --- DISPLAY-UPDATE (GETEILTES LAYOUT MIT IP-FIX) ---
void refreshOLED() {
  oledDisplay.clearDisplay();

  // -- KOPFZEILE: STATUS & GRENZWERT (Oben) --
  if (coolingActive) {
    // Wenn Kühlung aktiv: Invertierter Block
    oledDisplay.fillRect(0, 0, 75, 11, WHITE);
    oledDisplay.setTextColor(BLACK);
    oledDisplay.setCursor(3, 2);
    oledDisplay.setTextSize(1);
    oledDisplay.print("! KUEHLUNG !");
  } else {
    // Normalbetrieb
    oledDisplay.setTextColor(WHITE);
    oledDisplay.setTextSize(1);
    oledDisplay.setCursor(0, 2);
    oledDisplay.print("STATUS: OK");
  }

  // Grenzwert (Limit) oben rechts
  oledDisplay.setTextColor(WHITE);
  oledDisplay.setTextSize(1);
  oledDisplay.setCursor(80, 2);
  oledDisplay.print("Max:");
  oledDisplay.print(maxTempLimit, 0);

  // Horizontale Trennlinie oben
  oledDisplay.drawLine(0, 12, 128, 12, WHITE);

  // -- HAUPTDATEN: GETEILTE ANSICHT (Mitte) --
  // Linke Seite: Temperatur
  oledDisplay.setTextSize(1);
  oledDisplay.setCursor(0, 16);
  oledDisplay.print("Temp(C)");
  oledDisplay.setTextSize(2);
  oledDisplay.setCursor(0, 28);
  oledDisplay.print(smoothedTemp, 1);
  Serial.println ("Temp(C)");
  Serial.println (smoothedTemp);

  // Rechte Seite: Luftfeuchtigkeit
  oledDisplay.setTextSize(1);
  oledDisplay.setCursor(72, 16);
  oledDisplay.print("Rel.F(%)");
  oledDisplay.setTextSize(2);
  oledDisplay.setCursor(72, 28);
  oledDisplay.print(humidity, 0);

  // Vertikale Trennlinie in der Mitte der Daten
  oledDisplay.drawLine(64, 12, 64, 46, WHITE);

  // Horizontale Trennlinie unten
  oledDisplay.drawLine(0, 46, 128, 46, WHITE);

  // -- FUSSZEILE: NETZWERK & IP (Unten) --
  oledDisplay.setTextSize(1);
  oledDisplay.setCursor(0, 52);

  if (WiFi.status() == WL_CONNECTED) {
    oledDisplay.print("IP: ");
    oledDisplay.print(WiFi.localIP());
    Serial.println("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    oledDisplay.print("WLAN GETRENNT");
    Serial.println("WLAN GETRENNT");
  }

  oledDisplay.display(); // Bild auf das OLED übertragen
}

// --- SETUP-ROUTINE ---
void setup() {
  Serial.begin(115200);
  Serial.println(WiFi.localIP());
  // Pin-Modi konfigurieren
  pinMode(PIEZO_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(FAN_RELAY_1, OUTPUT);
  pinMode(FAN_RELAY_2, OUTPUT);
  pinMode(FAN_RELAY_3, OUTPUT);
  pinMode(LED_16, OUTPUT);
  pinMode(LED_17, OUTPUT);
  pinMode(LED_5, OUTPUT);
  // Relais initial ausschalten
  digitalWrite(FAN_RELAY_1, RELAY_INACTIVE);
  digitalWrite(FAN_RELAY_2, RELAY_INACTIVE);
  //digitalWrite(FAN_RELAY_3, RELAY_INACTIVE);

  // Sensor und Puffer initialisieren
  dhtSensor.begin();
  for (int i = 0; i < filterSize; i++) tempBuffer[i] = 0;

  // OLED über I2C starten
  Wire.begin(21, 22);
  if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Initialisierung fehlgeschlagen");
    for (;;); // Stoppt Ausführung bei Display-Fehler
  }

  // Boot-Bildschirm
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(WHITE);
  oledDisplay.setTextSize(1);
  oledDisplay.setCursor(20, 25);
  oledDisplay.println("Verbinde WLAN...");
  Serial.println("Verbinde WLAN...");
  oledDisplay.display();

  // WLAN-Verbindung herstellen
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Web-Server Routinen zuweisen und starten
  webDashboard.on("/", routeHomePage);
  webDashboard.on("/status", routeStatusJson);
  webDashboard.on("/set", routeSetLimit);
  webDashboard.begin();
}

// --- LOGIK-AUSWERTUNG ---
void evaluateConditions() {
  bool isCritical = false;

  // Status 0: Normal (Grün)
  if (smoothedTemp < (maxTempLimit - 2.0)) {
    systemStatus = 0;
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_5, HIGH);
    digitalWrite(LED_16, LOW);
    digitalWrite(LED_17, LOW);           
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(FAN_RELAY_3, HIGH);
    actionTriggered = false; // Reset für den nächsten Aktivierungs-Sound
  }
  // Status 1: Warnung (Gelb)
  else if (smoothedTemp >= (maxTempLimit - 2.0) && smoothedTemp < maxTempLimit) {
    systemStatus = 1;
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_5, LOW); //Anzeige Rückwand
    digitalWrite(LED_16, HIGH);
    digitalWrite(LED_17, LOW); 
    digitalWrite(LED_RED, LOW);
    digitalWrite(FAN_RELAY_3, LOW);
    actionTriggered = false; // Reset
  }
  // Status 2: Kritisch (Rot)
  else {
    systemStatus = 2;
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_5, LOW);
    digitalWrite(LED_16, HIGH);
    digitalWrite(LED_17, HIGH);
    digitalWrite(FAN_RELAY_3, LOW);    
    isCritical = true;
  }

  // Relais & Lüfter schalten
  if (isCritical && !coolingActive) {
    digitalWrite(FAN_RELAY_1, RELAY_ACTIVE);
    digitalWrite(FAN_RELAY_2, RELAY_ACTIVE);
    coolingActive = true;
    playActivationChime(); // Spielt den eleganten Warn-Gong
  }
  else if (!isCritical && coolingActive) {
    digitalWrite(FAN_RELAY_1, RELAY_INACTIVE);
    digitalWrite(FAN_RELAY_2, RELAY_INACTIVE);
    coolingActive = false;
    playDeactivationChime(); // Spielt die Level-Up Melodie


  
 }
}

// --- HAUPTSCHLEIFE ---
void loop() {
  // 1. Web-Anfragen verarbeiten (Muss kontinuierlich laufen)
  webDashboard.handleClient();

  // 2. Audio-Signale im Hintergrund verwalten
  updateAudioAlerts();

  unsigned long currentMillis = millis();

  // 3. Sensor alle 2 Sekunden auslesen
  if (currentMillis - timeLastSensor >= 2000) {
    timeLastSensor = currentMillis;

    float t = dhtSensor.readTemperature();
    float h = dhtSensor.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      tempC = t;
      humidity = h;

      // Gleitenden Durchschnitt berechnen
      tempSum = tempSum - tempBuffer[bufferIndex];
      tempBuffer[bufferIndex] = tempC;
      tempSum = tempSum + tempBuffer[bufferIndex];
      bufferIndex = (bufferIndex + 1) % filterSize;

      smoothedTemp = (tempSum == 0) ? tempC : (tempSum / filterSize);
      if (smoothedTemp < 1) smoothedTemp = tempC; // Verhindert 0-Werte beim Start
    }

    evaluateConditions();
  }

  // 4. OLED-Display jede Sekunde aktualisieren
  if (currentMillis - timeLastDisplay >= 1000) {
    timeLastDisplay = currentMillis;
    refreshOLED();
  }
}
