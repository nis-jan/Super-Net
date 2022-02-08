# SUPER-NET

Ziel des Projekts ist es ein ESP32 mit LoRa Modul(nachfolgend "**Node**") so zu programmieren,
dass dieser sich über eine serielle USB-Schnittstellemit einem Endgerät verbinden lässt
um damit anderen Engeräten über die Funkverbindung via LoRa Nachrichten zukommen zu lassen.

Dabei wird nicht etwa das Things-Network(TTN) oder LoRaWan genutzt, sondern die "Nodes"
funktionieren eigenständig und nutzen lediglich das LoRa modul zum senden und empfangen von Daten.

Damit soll es möglich sein, auch in Gebieten wo kein Empfang ist ähnlich wie mit einem Walkie-Talkie
miteinander zu kommunizieren.
Falls ein "Node" nicht in Reichweite eines anderen ist, fungieren andere, sich in Reichweite
befindliche, "Nodes" als Router und leiten die Nachricht an ihr richtiges Ziel weiter.
Dabei wird auch automatisch die zuverlässligste Route gewählt.
Ein "Node" kann also auch mit Solarpanel oder Akku autark ohne Endgerät, quasi als reiner Router,
verwendet werden. Platziert man ein paar davon in einem Gebiet, verbessert das die Zuverlässigkeit
der Übertragung von Nachrichten.
 

# Repo-Aufbau

Die Dokumentation zu diesem Projekt wurde mit **Doxygen** erstellt und befindet sich in: Documentation/html/

Diese Repo enthält drei Applikationen:

- Die Software in C++ für die "**Nodes**"
- Eine Desktop-Anwendung in Python um dem "Node" befehle zu übermitteln und Nachrichten zu empfangen
- Eine Android-App um das gleiche in schick auf dem Handy machen zu können

Da die "Node" Software, welche in C++ geschrieben ist, das eigentliche Projekt ist und der Rest nur zur
Interaktion damit erstellt wurde (und Zeitdruck :D) beschränkt sich die Doku hauptsächlich
darauf.

## Super-Net Code

Der Code für die Nodes befindet sich im Ordner Software/Super-Net/src und Software/Super-Net/lib
wobei im src-Ordner in der main.cpp nur der eigentliche Einstiegspunkt in die Software ist und
der Hauptteil der Software sich im libordner befindet.
Es handelt sich um ein PlatformIO-Projekt, welches konfiguriert ist für die beiden Gerätearten

- Heltec Wifi LoRa V2
- Heltec Wireless Stick

in der platformio.ini, lassen sich aber weitere Gerätekonfiguration hinzufügen.
Das Projekt lässt sich nur in mit PlatformIO, einer Erweiterung für VSCode erstellen!
