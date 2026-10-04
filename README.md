<a name="oben"></a>

<div align="center">

|[:skull:ISSUE](https://github.com/frankyhub/FanControl/issues?q=is%3Aissue)|[:speech_balloon: Forum /Discussion](https://github.com/frankyhub/FanControl/discussions)|[:grey_question:WiKi](https://github.com/frankyhub/FanControl/wiki)||
|--|--|--|--|
| | | | |
|![Static Badge](https://img.shields.io/badge/RepoNr.:-%20124-blue)|<a href="https://github.com/frankyhub/FanControl/issues">![GitHub issues](https://img.shields.io/github/issues/frankyhub/FanControl)![GitHub closed issues](https://img.shields.io/github/issues-closed/frankyhub/FanControl)|<a href="https://github.com/frankyhub/FanControl/discussions">![GitHub Discussions](https://img.shields.io/github/discussions/frankyhub/FanControl)|<a href="https://github.com/frankyhub/FanControl/releases">![GitHub release (with filter)](https://img.shields.io/github/v/release/frankyhub/FanControl)|
|![GitHub Created At](https://img.shields.io/github/created-at/frankyhub/FanControl)| <a href="https://github.com/frankyhub/FanControl/pulse" alt="Activity"><img src="https://img.shields.io/github/commit-activity/m/badges/shields" />| <a href="https://github.com/frankyhub/FanControl/graphs/traffic"><img alt="ViewCount" src="https://views.whatilearened.today/views/github/frankyhub/github-clone-count-badge.svg">  |<a href="https://github.com/frankyhub?tab=stars"> ![GitHub User's stars](https://img.shields.io/github/stars/frankyhub)|

---

## Story
In diesem Projekt wird ein ESP32-Kühlsystem aufgebaut, das Temperatur sowie Luftfeuchtigkeit in Echtzeit überwacht, Lüfter bei Grenzwerten automatisch steuert und eine komfortable Überwachung per Web-Dashboard und OLED-Display ermöglicht. Drei LEDs grün (TEMPERATUR OK), orange (WARNUNG: TEMP HOCH) und rot (KÜHLUNG AKTIV) zeigen den Temperaturstatus an. Liegt die IST-Temperatur über der SOLL-Temperatur leuchtet die rote LED und zwei Relais-Ausgänge schalten die beiden Lüfter ein. Der Temperatur-Grenzwert (ALARM-PARAMETER) kann am WEB-Dashboard mit der Tastatur oder der Maus verändert werden. 

Das Programm FanControR3.ino stellt einen dritten Relais-Ausgang zur Verfügung, der bereits mit der WARNUNG: TEMP HOCH (gelbe LED) einschaltet.

 ## FanControl WEB-Dashboard
 
![Bild](/pic/FanControl.gif)

 ## FanControl WEB-Dashboard Snapshot

![Bild](/pic/FanControl.png)

 ## FanControl Gehäusefront

![Bild](/pic/Front.png)

 ## FanControl Gehäuse Rückseite

![Bild](/pic/Back.png)

 ## FanControl OLED-Display

![Bild](/pic/OLED.png)

 


## Hardware

| Stück | Beschreibung | 
| -------- | -------- | 
|  1 |  ESP32 DevKit V4 | 
|  1 | 0.96" OLED Display |
| 1 |  Aktiver Piezo-Buzzer| 
|  5 |  LED rot |
|  1 |  LED gelb |
|  1 | LED grün| 
|  1 | Step Down Modul 12V->5V| 
|  1 | 12V Netzteil|
|  3 | Buchsen (Lüfter und Netzteil)|
|  3 | Stecker (Lüfter und Netzteil)|
|  1 | Schalter| 
|   5 |Widerstand 100 Ohm LED Frontseite|
|   2 |Widerstand 1200 Ohm LED Rückseite|
|   1 |Widerstand 10k Ohm DHT11/22 |
|  2 | Hot End Lüfter/ PC-Lüfter |
| 2 | Relais 5V |
| 1 |  DHT11 oder DHT22 Sensor |
| 1 | Sperrholzplatte 600x300x3mm  | 
| 1 | Drähte, Verbrauchsmaterial | 
| 1 | 3D Druckteile  | 
| 2| WAGO 5fach Klemmen  | 
| 1 | Holzkleber  | 
| -------- | -------- |  

##  Verdrahtung

| OLED-Display | ESP32 |
| -------- | -------- | 
| VDD | Vin|
|GND| GND|
|SCK| GPIO 22|
|SDA| GPIO 21|
| -------- | -------- | 

### Gegebenenfalls den Sensor außerhalb des Gehäuses an einer geeigneten Messstelle montieren.

| DHT22-Sensor | ESP32 | 
| -------- | -------- | 
|  VCC| Vin|
|Data | GPIO 4|
| GND (|GND|
| -------- | -------- | 


| Piezo-Buzzer | ESP32 | 
| -------- | -------- | 
|Plus | GPIO 18|
|Minus | GND|
| -------- | -------- | 

| LED Ampel | ESP32 | 
| -------- | -------- | 
|Rot |  GPIO 25|
|Gelb | GPIO 26 |
|Grün| GPIO 27|
| GND| GND|
| -------- | -------- | 
| Anzeige| Lüfter| 
|Rot Lüft.1| GPIO 16|
|Rot Lüft.2|  GPIO 17|
| -------- | -------- | 

|  Piezo-Buzzer  | ESP32 |  
| -------- | -------- | 
|  Plus |GPIO 18|
| Minus | GND|
| -------- | -------- | 

|  Relais-Modul  | ESP32 | 
| -------- | -------- | 
| VCC| Vin|
|GND| GND|
|IN1| GPIO 33|
| IN2| GPIO 32|
| IN2| GPIO 14 optional|
| -------- | -------- | 

### ESP32 Dev Kit V4 Boardverwalter: ESP32 Dev Module V3.3.11

![Bild](/pic/ESP32Verdrahtung.png)




### Libraries

</div>

#include <WiFi.h>

#include <WebServer.h>

#include <Wire.h>

#include <Adafruit_GFX.h> //V1.12.6 https://github.com/adafruit/Adafruit-GFX-Library

#include <Adafruit_SSD1306.h> //V2.5.17 https://github.com/adafruit/Adafruit_SSD1306

#include <DHT.h> //V1.4.7 https://github.com/adafruit/DHT-sensor-library

//Adafruit_Sensor V1.1.15 https://github.com/adafruit/Adafruit_Sensor


<div align="center">
 
### ESP32 Dev Kit V4 Shield

![Bild](/pic/Shield.png)

### Das Relaismodul

![Bild](/pic/Relais.png)



Da der ESP32 nicht ausreichend Strom zum schalten der Lüfter liefert, verwenden wir ein Relaismodul.

Die Eingänge haben Optokoppler und bieten eine optische Trennung (Galvanische Isolierung). Sie trennen die 
empfindliche Steuerelektronik des Mikrocontrollers von der Lastseite (die Relaisspulen), damit Spannungsspitzen 
den Controller nicht beschädigen.

Die Spannungsversorgung des Relaisblocks erfolgt mit 5V.

Die externe Spannungsversorgung für die Lüfter wird am Wechslerkontakt der Relais angeschlossen. Je nach Lüftertyp können das auch 12V  (PC-Lüfter) sein.

Die Masse (GND) der externen Quelle und des Microcontrollers müssen verbunden sein (Common Ground). 



![Bild](/pic/Relais2.png)

### Relais Verdrahtung

Der Code, um ein Relais mit dem ESP32 zu steuern, ist genauso einfach wie die Steuerung einer LED oder die Ansteuerung eines anderen Aktors. Der Relaisausgang schaltet mit einem LOW Signal. Bei einem HIGH Signal vom ESP32, ist das Relais in Ruhestellung. 

![Bild](/pic/Verdrahtung.png)


![Bild](/pic/DHT11_Verdrahtung.png)



## 3D Druckteile

![Bild](/pic/3D.png)

## Verdrahtung

![Bild](/pic/Innen.png)


## Gehäuse

### xtool Lasercutter Datei

![Bild](/pic/xtool.png)


![Bild](/pic/FanControl1.png)

![Bild](/pic/FanControl2.png)

![Bild](/pic/FanControl3.png)

![Bild](/pic/FanControl4.png)

![Bild](/pic/FanControl5.png)



---

<div style="position:absolute; left:2cm; ">   
<ol class="breadcrumb" style="border-top: 2px solid black;border-bottom:2px solid black; height: 45px; width: 900px;"> <p align="center"><a href="#oben">nach oben</a></p></ol>
</div>  

---


