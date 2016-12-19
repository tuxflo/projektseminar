# Projektseminar Parallele Programmierung

## Projektplanung

1. Vorführung und Präsentation des Projekts --> 26.01.
2. Abgabe der Projektdokumentation (10 - 30 Seiten) --> spätestens bis 28.02.

## Aufgabenstellungen

[Allgmeine Beschreibung](http://www.htw-dresden.de/htt.//www.htw-dresden.de/~sobe/Projsem_2016/PS_ParallelProg_16_17.pdf)

[Einführung und Aufgaben 2016](http://www.htw-dresden.de/htt.//www.htw-dresden.de/~sobe/Projsem_2016/PS_Intro.pdf)

#### Offizielle Aufgabenstellung
[Parallele / Verteilte Hashmap](https://www.evernote.com/l/AG0OX7QTgktMtbziiHEM7LjVVJ5EtIL6j58)

#### Ausgearbeitete Aufgabenstellung

Zur Implementierung einer verteilten Hashmap soll das Message Passing Interface in der Implementierung [MPICH](www.mpich.org) verwendet werden. Besonders interessant ist hierbei das Feature der _**einseitigen Kommunikation**_. Dieses ermöglicht den direkten Zugriff auf Speicherbereiche anderer Prozesse und reduziert somit den Protokoll-Overhead zweiseitiger Kommunikation. Bei entsprechenden Netzwerk-Rahmenbedinungen ist weiterhin mit einem großen Geschwidigkeitsvorteil gegenüber klassischer (zweiseitiger) _**Send-Receive-Comunication**_ zu rechnen.

Nach intensiver Recherche zeigte sich jedoch, dass _einseitige Kommunikation_ Nachteile bei der
Arbeit mit dynamischen Datanstrukturen hat. Zur Realisierung einer Hashmap ist dies jedoch mindestens bei der Behandlung von Überlauflisten notwendig.

In Ansprache mit Prof. Sobe wurde sich an dieser Stelle auf folgende Herangehensweise geeinigt:

1. Um die Eigenschaften _**einseitiger Kommunikation**_ zu erforschen, wird eine Hashmap zunächst ohne Kollisionsbehandlung durch einseitige Methoden wie _MPI-Put_ und _MPI-Get_ implementiert.

2. Dabei sollen Fehlermeldungen Hash-Kollisionen anzeigen

3. Anschließend wird die Behandlung von Kollisionen durch eine weitere Prozess-Gruppe realisiert, welche mit schreibenden- und lesenden-Prozessen im _**zweiseitigen Modus**_ kommuniziert

So können sowohl die beiden Kommunikationsprinzipien von MPI erforscht, als auch alle wichtigen Eigenschaften verteilter Hashmaps implementiert werden.
