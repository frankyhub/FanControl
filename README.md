<a name="oben"></a>

<div align="center">

|[:skull:ISSUE](https://github.com/frankyhub/FanControl/issues?q=is%3Aissue)|[:speech_balloon: Forum /Discussion](https://github.com/frankyhub/FanControl/discussions)|[:grey_question:WiKi](https://github.com/frankyhub/FanControl/wiki)||
|--|--|--|--|
| | | | |
|![Static Badge](https://img.shields.io/badge/RepoNr.:-%20124-blue)|<a href="https://github.com/frankyhub/FanControl/issues">![GitHub issues](https://img.shields.io/github/issues/frankyhub/FanControl)![GitHub closed issues](https://img.shields.io/github/issues-closed/frankyhub/FanControl)|<a href="https://github.com/frankyhub/FanControl/discussions">![GitHub Discussions](https://img.shields.io/github/discussions/frankyhub/FanControl)|<a href="https://github.com/frankyhub/FanControl/releases">![GitHub release (with filter)](https://img.shields.io/github/v/release/frankyhub/FanControl)|
|![GitHub Created At](https://img.shields.io/github/created-at/frankyhub/FanControl)| <a href="https://github.com/frankyhub/FanControl/pulse" alt="Activity"><img src="https://img.shields.io/github/commit-activity/m/badges/shields" />| <a href="https://github.com/frankyhub/FanControl/graphs/traffic"><img alt="ViewCount" src="https://views.whatilearened.today/views/github/frankyhub/github-clone-count-badge.svg">  |<a href="https://github.com/frankyhub?tab=stars"> ![GitHub User's stars](https://img.shields.io/github/stars/frankyhub)|





---


## Story
Diese Anleitung beschreibt den Aufbau einer Lüftersteuerung. Das Gehäuse wird aus 3mm Sperrholz gelasert. In diesem Projekt wird ein ESP32-Kühlsystem aufgebaut, das Temperatur sowie Luftfeuchtigkeit in Echtzeit überwacht, Lüfter bei Grenzwerten automatisch steuert und eine komfortable Überwachung per Web-Dashboard ermöglicht.
 
![Bild](/pic/FanControl.gif)

![Bild](/pic/FanControl.png)

</div>  

## Hardware

| Stück | Beschreibung | 
|  1 |  ESP32 NodeMcu Entwicklungsboard | 
|  1 |0.96" OLED Display |
| 1 |  Aktiver Piezo-Buzzer| 
|  1 |  LED rot |
|  1 |L ED gelb |
|  1 | LED grün| 
|   3 |Widerstand 100 Ohm |
|  2 | Hot End Lüfter |
| 2 |Relais 5V |
| 1 |  DHT11 oder DHT22 Sensor |
| 1 |Sperrholzplatten 600x300x3mm  | 
| 1 | Holzkleber  | 
| -------- | -------- |  

##  Verdrahtung

### OLED-Display 
| OLED-Display | ESP32 | 
| VDD | Vin|
              |GND| GND|
              |SCK| GPIO 22|
              |SDA| GPIO 21|
| -------- | -------- | 
| DHT22-Sensor | ESP32 | 
 | VCC| Vin|
              |Data | GPIO 4|
             | GND (|GND|
| -------- | -------- | 
| Piezo-Buzzer | ESP32 | 
                |Plus (+) | GPIO 18|
              |Minus (-)| GND|
| -------- | -------- | 
|Rot |  GPIO 25|
|Gelb | GPIO 26 |
|Grün| GPIO 27|
|          GND| GND|
| -------- | -------- | 
|  Relais-Modul  | ESP32 | 
 | VCC| Vin|
              |GND| GND|
              |IN1| GPIO 33|
             | IN2| GPIO 32|
| -------- | -------- | 
|  OLED-Display   | ESP32 | 
|  VDD |Vin|
              |GND| GND|
              |SDA| GPIO 22|
              |SCL |GPIO 21|
| -------- | -------- | 
|  DHT11-Sensor   | ESP32 |   
      |      VCC (links) |Vin|
        |    Data (Mitte)-|GPIO 4|
           |   GND (rechts) |GND|
| -------- | -------- | 
|  Piezo-Buzzer  | ESP32 |  
  |          Plus (+)|GPIO 18|
           | Minus (-)| GND|
| -------- | -------- | 
|  LED-Ampel  | ESP32 |  
           |Rot|   GPIO 25|
            |Gelb| GPIO 26|
            | Grün| GPIO 27|
            |GND| GND|
| -------- | -------- |
|  Relais-Modul | ESP32 |   
           |   VCC |  Vin|  
            |  GND|   GND|  
            |  IN1 |  GPIO 33|  
            |  IN2 |  GPIO 32|  
| -------- | -------- |



## Aufbau

### xtool Lasercutter Datei

![Bild](/pic/xtool.png)



---

<div style="position:absolute; left:2cm; ">   
<ol class="breadcrumb" style="border-top: 2px solid black;border-bottom:2px solid black; height: 45px; width: 900px;"> <p align="center"><a href="#oben">nach oben</a></p></ol>
</div>  

---


