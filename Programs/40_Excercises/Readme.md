# Aufgaben

[Zur�ck](../../Readme.md)

---

## Inhalt

  * [Aufgabe 1](#link1)
  * [Aufgabe 2](#link2)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::thread`
  * Klasse `std::jthread`
  * Klasse `std::counting_semaphore`

---

#### Aufgabe 1: Zeitvergleich `std::thread` und `std::async` <a name="link1"></a>


Erzeugen Sie 1 Million Threads mit `std::thread` und `std::async`.
In beiden F�llen sollen die Threads einen R�ckgabewert haben.

Im Falle von `std::thread` ben�tigt man hier zus�tzlich ein `std::promise`-Objekt.

Es soll letzten Endes gemessen, wie lange es dauert, mit `std::thread` oder `std::async`
eine Million Threads zu starten und ein berechnetes Ergebnis abzuholen.

Betrachten Sie im Task Manager den jeweiligen Verbrauch an Betriebssystemresourcen.

---

#### Aufgabe 2: Mehrere Autos und ein Parkplatz <a name="link2"></a>

Wir betrachten eine Simulation von fahrenden Autos und einem Parkplatz.
Folgende Randbedingungen sind gegeben:

  * Es fahren 10 Autos &ndash; jedes Auto wird durch ein eigenes Thread-Objekt kontrolliert bzw. verwaltet.
  * Nach 5 Sekunden Fahrzeit m�chte ein Auto auf einem Parkplatz parken.
  * Auf dem Parkplatz k�nnen maximal 5 Autos parken.
  * Ein Auto parkt 3 Sekunden, danach verl�sst es den Parkplatz wieder und f�hrt f�r 5 weitere Sekunden.

Erstellen Sie eine Simulation, die durch entsprechende Ausgaben in der Konsole
fahrende Autos und die Belegung des Parkplatzes erkennen l�sst.

Verwenden Sie ein `std::counting_semaphore`-Objekt, um damit die Zufahrt zum
Parkplatz zu kontrollieren.


---

#### L�sungen

[*Exercise_01_Thread_Comparison.cpp*](Exercise_01_Thread_Comparison.cpp)<br />
[*Exercise_02_CarParking.cpp*](Exercise_02_CarParking.cpp)

---


[Zur�ck](../../Readme.md)

---






