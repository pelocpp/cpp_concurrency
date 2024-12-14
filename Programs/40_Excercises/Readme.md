# Aufgaben

[Zurück](../../Readme.md)

---

## Inhalt

  * [Aufgabe 1: Zeitvergleich `std::thread` und `std::async`](#link1)
  * [Aufgabe 2: Mehrere Autos und ein Parkplatz](#link2)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::thread`
  * Klasse `std::jthread`
  * Klasse `std::counting_semaphore`
  * Klasse `std::stop_token` und Methode `request_stop`

---

#### Aufgabe 1: Zeitvergleich `std::thread` und `std::async` <a name="link1"></a>


Erzeugen Sie 100.000 Threads mit `std::thread` und `std::async`.
In beiden Fällen sollen die Threads einen Wert berechnen und auf eine Variable
in der Umgebung (*Scope*) zugreifen.

Auf diese Weise soll vermieden werden, dass der *Optimizer* bei der Code-Generierung
zu sehr Einfluss nehmen kann.

Letzten Endes soll gemessen werden, wie lange es dauert, mit `std::thread` oder `std::async`
mehrere Threads zu starten und ein berechnetes Ergebnis abzuholen.

Betrachten Sie im Task Manager den jeweiligen Verbrauch an Betriebssystemresourcen.

---

#### Aufgabe 2: Mehrere Autos und ein Parkplatz <a name="link2"></a>

Wir betrachten eine Simulation von fahrenden Autos und einem Parkplatz.
Folgende Randbedingungen sind gegeben:

  * Es fahren 6 Autos &ndash; jedes Auto wird durch ein eigenes Thread-Objekt kontrolliert bzw. verwaltet.
  * Nach ca. 5 Sekunden Fahrzeit möchten die Autos auf einem Parkplatz parken.
  * Auf dem Parkplatz können maximal nur 3 Autos parken.
  * Ein Auto parkt 3 Sekunden, danach verlässt es den Parkplatz und fährt wiederum für ca. 5 weitere Sekunden.
  * Nach 20 Sekunden soll die Simulation kontrolliert beendet werden.

Erstellen Sie eine Simulation, die durch entsprechende Ausgaben in der Konsole
fahrende Autos und die Belegung des Parkplatzes erkennen lässt.

Verwenden Sie ein `std::counting_semaphore`-Objekt, um damit die Zufahrt zum
Parkplatz zu kontrollieren.

*Zusatzaufgabe*:<br/>
Realisieren Sie zum Vergleich eine Parkplatz-Klasse, die die Klasse `std::condition_variable` verwendet. 

###### Ausgabe 

Eine mögliche Ausgabe des Programms könnte so aussehen:

```
[1]:    FirstParkingArea has 3 empty lots.
[1]:    Starting Simulation:
[2]:    Car 1 is driving
[3]:    Car 2 is driving
[4]:    Car 5 is driving
[5]:    Car 6 is driving
[6]:    Car 3 is driving
[7]:    Car 4 is driving
[3]:    Want to park car 2 now
[3]:    Car 2 is parking now!
[7]:    Want to park car 4 now
[7]:    Car 4 is parking now!
[3]:    Want to leave parking area with car 2
[3]:    Car2 has left parking area
[3]:    Car 2 is driving
[6]:    Want to park car 3 now
[6]:    Car 3 is parking now!
[5]:    Want to park car 6 now
[5]:    Car 6 is parking now!
[2]:    Want to park car 1 now
[4]:    Want to park car 5 now
[7]:    Want to leave parking area with car 4
[7]:    Car4 has left parking area
[2]:    Car 1 is parking now!
[7]:    Car 4 is driving
...
...
...
[1]:    Issuing Stop Requests ...
[5]:    Want to leave parking area with car 6
[5]:    Car6 has left parking area
[2]:    Car 1 is parking now!
[5]:    Car 6 finished driving!
[6]:    Want to leave parking area with car 3
[6]:    Car3 has left parking area
[6]:    Car 3 finished driving!
[4]:    Want to leave parking area with car 5
[4]:    Car5 has left parking area
[4]:    Car 5 finished driving!
[2]:    Want to leave parking area with car 1
[2]:    Car1 has left parking area
[2]:    Car 1 finished driving!
[7]:    Want to park car 4 now
[7]:    Car 4 is parking now!
[3]:    Want to park car 2 now
[3]:    Car 2 is parking now!
[7]:    Want to leave parking area with car 4
[7]:    Car4 has left parking area
[7]:    Car 4 finished driving!
[3]:    Want to leave parking area with car 2
[3]:    Car2 has left parking area
[3]:    Car 2 finished driving!
[1]:    Stopped Simulation.
```

---

#### Lösungen

[*Exercise_01_Thread_Comparison.cpp*](Exercise_01_Thread_Comparison.cpp)<br />
[*Exercise_02_CarParking.cpp*](Exercise_02_CarParking.cpp)

---


[Zurück](../../Readme.md)

---






