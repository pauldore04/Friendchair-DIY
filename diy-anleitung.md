**Do-It-Yourself Anleitung für den Friendchair**

***Benötigte  Teile :***   

Esp32, Button v1.2, Grove_LED, Weight Sensor, 3x Kabel,
Micro-USB-Kabel

*Und Natürlich:* Einen Stuhl den du Transformieren willst

***Esp32-Setup:***

1. Starte indem du den Button v1.2 mit einem Kabel verbindest. Dieses Kabel
verbindest du dann mit dem IO14 Eingang deines Esp's.

2. Danach verbindest du deine Grove LED mit einem Kabel, welches du in den
Eingang daneben, dem IO26 Eingang steckst

3. Dein letztes Kabel verbindest du mit dem Weight Sensor, diese Kabel wird
in den IO33 Eingang, links von dem LED Eingang, gesteckt.

4. Deinen Esp32 ist jetzt fertig, er braucht nur noch den Code und dann läuft er schon!

***Esp32-Code:***

Nun muss der ganze Code, welcher in diesem Repository unter friendchair liegt, auf den Esp32 drauf.
Verbinde dafür dein Micro-USB Kabel mit dem Esp32 und die andere Seite mit deinem Computer.

Im Terminal musst du nun in den Friendchair ordner Navigieren, dafür gibst du folgendes ein:
```
cd friendchair
```
Das Terminal sollte dann ungefähr so aussehen :
```
.../iot-2025-08/friendchair (main) $
```
Um nun per platformio den Code auf den Esp zu pushen musst du folgenden Befehl eingeben:
```
pio run --target upload
```
Wenn das geklappt hat, sollte es ungefähr so aussehen:
```
...
Writing at 0x00095660... (61 %)
Writing at 0x0009a929... (64 %)
Writing at 0x0009fd70... (67 %)
Writing at 0x000a527d... (70 %)
Writing at 0x000aaa17... (73 %)
Writing at 0x000b018a... (76 %)
Writing at 0x000b5fa9... (79 %)
Writing at 0x000bba78... (82 %)
Writing at 0x000c1565... (85 %)
Writing at 0x000c9a6f... (88 %)
Writing at 0x000d200a... (91 %)
Writing at 0x000d7173... (94 %)
Writing at 0x000dcc93... (97 %)
Writing at 0x000e2307... (100 %)
Wrote 875840 bytes (551012 compressed) at 0x00010000 in 14.3 seconds (effective 491.5 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
================== [SUCCESS] Took 32.14 seconds ================== 
```
Super, jetzt bist du schon fast fertig!

Um überprüfen zu können ob der Code funktioniert, gib folgendes im Terminal ein:
```
pio device monitor
```
Dies öffnet den Serial Monitor und du solltest nun Output sehen, den der Esp32 sendet.
Da der Esp32 über WLAN kommuniziert müssen wir das noch einrichten. Öffne auf deinem Handy deine WLAN Einstellungen. Suche nach einem
Gerät, welches ESP32_01 oder 02 heisst. Klicke auf dieses drauf und es sollte sich automatisch eine Seite auf deinem Handy öffnen.
Hier musst du nun dein lokales WLAN auswählen und das Passwort eingeben. Wenn du das erfolgreich gemacht hast, schließt sich dieses Fenster automatisch.

**Super! Dein ESP32 ist nun lauffähig!**

Als letzten Schritt muss du nur noch dein Esp32 an deinem gewünschten Stuhl anbringen. Wie du das machst ist
dir selbst überlassen, aber wir haben eine Empfehlung wie wir es machen würden. Wähle eine deiner Armlehnen deines Stuhls aus. Befestige den Esp32 daneben, also in die Richtung vom Stuhl weg.
Den Button und die LED kannste du nun oberhalt deiner Armlehne befestigen, sodass sie dort schnell zu erreichen sind. Das Kabel mit dem
Gewichtssensor führst du die Armlehne entlang nach unten und legst den Sensor auf deinen Stuhl drauf. 

***Exzellent!***

Du hast dir soeben erfolgreich einen Friendchair installiert.

