# Verriegelungen und Barrieren: Klassen `std::latch` und `std::barrier`

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::latch`
  * Klasse `std::barrier`
  * Klasse `std::jthread`
  * Klasse `std::future`

---

## Allgemeines

C++ hat weitere Klassen erhalten, um die Synchronisierung von Threads zu vereinfachen: *Latches* und *Barrieren*.


*Hinweis*:<br />Windows-Systemprogrammierern dürften *Latches* und *Barrieren* in
gewisser Weise bekannt vorkommen: Sie haben viele Gemeinsamkeiten mit den
Win32-Betriebssystemfunktionen `WaitForSingleObject` und `WaitForMultipleObjects`.

---

## Einführung

Ein *Latch* ist eine Art &bdquo;Countdown&rdquo;-Zähler,
der mit einem Startwert initialisiert wird und beim Eintreten bestimmter Ereignisse heruntergezählt wird.
Erreicht der Zähler den Wert 0, meldet er auf eine bestimmte Art &bdquo;fertig&rdquo;.

So kann man beispielsweise 5 Threads beauftragen, etwas zu tun, und den Zähler
mit 5 vorbelegen. Wenn dann jeder Thread sein Ende dem *Latch*
als Ereignis mitteilt, meldet dieser nach 5 fertigen Threads, dass
die gesamte Ausführung abgeschlossen ist.

Beliebig viele Threads können ein *Latch*-Objekt zum Herunterzählen auf 0 verwenden.
Jeder Thread darf beliebig oft herunterzählen.
Steht der Wert des *Latch*-Objekts auf 0, kann es seinen Zustand nicht mehr verändern.
Es eignet sich also nur für einen einmaligen *Countdown*.

---

*Barrieren* sind so organisiert, dass sie mehrfach Ereignisse verschiedener Threads synchronisieren können.
Auch hier handelt es sich um einen Zähler, den jeder Thread aber nur einmal herunterzählen kann. Steht das *Barrieren*-Objekt auf 0,
wird eine vordefinierte Aktion aufgerufen und das *Barrieren*-Objekt lässt sich erneut verwenden.
Das ist hilfreich, wenn das Programm nach einem bestimmten Ereignis immer wieder darauf warten muss,
dass eine bestimmte Anzahl paralleler Reaktionen erfolgt ist.

---

## `std::latch`

Einen groben Überblick auf die wesentlichen Methoden der Klasse `std::latch` gibt *Tabelle* 1:

| Element | Beschreibung |
| :---- | :---- |
| Konstruktor | `explicit latch(std::ptrdiff_t expected);`<br/>Erzeugt ein `std::latch`-Objekt und initialisiert seinen internen Zähler. |
| Methode<br/>`count_down()` | `void count_down(std::ptrdiff_t n = 1);`<br/>Dekrementiert den internen Zähler atomar um den Wert *n*, ohne den Anrufer zu blockieren. |
| Methode<br/>`wait()` | `void wait() const;`<br/>Blockiert den aufrufenden Thread, bis der interne Zähler 0 erreicht. Wenn er bereits Null ist, wird sofort zurückgekehrt. |
| Methode<br/>`arrive_and_wait()` | `void arrive_and_wait(std::ptrdiff_t n = 1);`<br/>Dekrementiert den internen Zähler atomar um *n* und blockiert (falls erforderlich) den aufrufenden Thread, bis der Zähler Null erreicht. |

*Tabelle* 1: Wesentliche Elemente der Klasse `std::latch`.

Knapp formuliert kann man sagen, dass die Klasse `std::latch`
ein Abwärtszähler vom Typ `ptrdiff_t` ist, mit dem Threads synchronisiert werden können.
Der Wert des Zählers wird bei der Erstellung initialisiert.
Threads können an der Verriegelung (engl. &bdquo;*latch*&rdquo;)  blockieren,
bis der Zähler auf Null dekrementiert wird.
Es gibt keine Möglichkeit, den Zähler zwischendurch zu vergrößern oder zurückzusetzen.

---

## `std::barrier`

Einen groben Überblick auf die wesentlichen Methoden der Klasse `std::barrier` gibt *Tabelle* 2:

| Element | Beschreibung |
| :---- | :---- |
| Konstruktor | `explicit barrier(std::ptrdiff_t expected);`<br/>Erzeugt ein `std::barrier`-Objekt und initialisiert seinen internen Zähler. |
| Methode<br/>`wait()` | `void wait() const;`<br/>Blockiert den aufrufenden Thread, bis alle Threads den Synchronisationspunkt erreicht haben. |
| Methode<br/>`arrive_and_wait()` | `void arrive_and_wait();`<br/>Erreicht die Barriere und verringert den Zähler um 1, blockiert dann, bis die aktuelle Phase abgeschlossen ist. |
| Methode<br/>`arrive_and_drop()` | `void arrive_and_drop();`<br/>Dekrementiert sowohl den initial erwarteten Zähler für die nachfolgende Phase als auch die aktuelle Phase um 1. |

*Tabelle* 2: Wesentliche Elemente der Klasse `std::barrier`.

Die Klasse `std::barrier` bietet einen Thread-Koordinierungsmechanismus,
mit dem eine festgelegte Anzahl von Threads blockiert werden kann,
bis diese alle den Synchronisationspunkt (*Barriere*) erreicht haben.
Im Gegensatz zu `std::latch` sind Barrieren wiederverwendbar:
Sobald die ankommenden Threads am Synchronisationspunkt einer Barriere ihre Blockade verlassen,
kann dieselbe Barriere erneut verwendet werden.

---

## Beispiele zu `std::latch`


### Mehrere Tätigkeiten zeitlich aufeinander abstimmen

Das erste Beispiel betrachtet zwei Threads, die unterschiedliche Tätigkeiten durchführen.
Der Hauptthread arbeitet an einem bestimmten Punkt erst dann weiter,
wenn beide Threads ihre Tätigkeiten beendet haben.

### Gemeinsamer Start in der Ausführung verschiedener Tätigkeiten

Ein zweites Beispiel demonstriert einen Einsatz der `arrive_and_wait`-Methode.
Es werden eine Reihe von Threads gestartet, die vorbereitende Tätigkeiten absolvieren.
An einem bestimmten Punkt sind sie mit den Vorbereitungen fertig und rufen die `arrive_and_wait`-Methode auf.
Diese Methode blockiert solange (bei geeigneter Initialisierung eines beteiligten `std::latch`-Objekts),
bis alle Threads ihre Vorbereitungen abgeschlossen haben.
Kehrt `arrive_and_wait` aus der Blockade zurück, können alle Threads &ndash; mehr oder minder &ndash; zeitgleich
mit Folgeaktivitäten starten.

### Summenbildung von disjunkten Zahlenbereichen

Das dritte Beispiel ist ähnlich zum Beispiel aus dem Projekt `std::packaged_task`.
Es wird die Summation einer Reihe natürlicher Zahlen auf mehrere Threads (hier: `std::async`) aufgeteilt.
Der Worker-Thread liefert zu Demonstrationszwecken das Ergebnis nicht über ein `std::future`-Objekt zurück,
sondern legt es an einer bestimmten Position (Parameter `index`) in einem globalen `std::array`-Objekt ab.

Auf diese Weise ist für den Initiator nicht ersichtlich, wann alle Teilergebnisse vorliegen.
Dies erfolgt mit einem `std::latch`-Objekt, das pro Worker-Thread, wenn dieser fertig ist, mittels
der `count_down`-Methode dekrementiert wird.

Zentraler Warteplatz im Initiator ist ein Aufruf der `wait`-Methode am `std::latch`-Objekt.
Kann dieses passiert werden, weiß der Initiator, dass das `std::array`-Objekt mit allen Ergebnissen
vollständig belegt sein muss.

*Ausgabe*:

```cpp
[2]: Calculating from 101 up to 201...
[1]: Calculating from 1 up to 101...
[3]: Calculating from 201 up to 301...
[4]: Calculating from 301 up to 401...
[5]: All calculations done :)
[5]: Partial result: 5050
[5]: Partial result: 15050
[5]: Partial result: 25050
[5]: Partial result: 35050
[5]: Total: 80200
```

### Modelliering von Master- und Slave-Threads

Im Prinzip ist dieses Beispiel ähnlich gelagert wie das Beispiel zuvor.
Es wird eine Synchronisation von einem Master-Thread und mehreren Slave-Threads demonstriert.
Dieses Mal findet die Synchronisation in beide Richtungen statt.

Zunächst startet der Master-Thread seine Slave-Threads und wartet auf deren Beendigung.
Dazu dekrementieren die Slave-Threads ein (erstes) `std::latch`-Objekt,
der Master-Thread *wartet* an diesem `std::latch`-Objekt.

Die Slave-Threads ihrerseits verlassen allerdings nach dem Ende ihrer Tätigkeit nicht die Thread-Prozedur,
sondern blockieren an einem (zweiten) `std::latch`-Objekt.
Wenn das erste `std::latch`-Objekt den Wert 0 hat und der Master-Thread seine Blockade verlässt,
dekrementiert er das zweite `std::latch`-Objekt einmal &ndash; es wurde mit dem Wert 1 vorbelegt.
Nun endet die Blockade aller wartenden Slave-Threads und diese verlassen ihre Thread-Prozedur
zu einem kontrollierten Zeitpunkt.

Das Beispiel demonstriert, wie beide Seiten ihren Ablauf synchronisieren können
und mögliche Aufräumarbeiten (Freigabe von Ressourcen) zum richtigen Zeitpunkt durchführbar sind.

*Ausgabe*:

```cpp
[1]: Working starts:
[2]: Worker (1): Started working.
[3]: Worker (2): Started working.
[4]: Worker (3): Started working.
[5]: Worker (4): Started working.
[6]: Worker (5): Started working.
[3]: Worker (2): Work done!
[6]: Worker (5): Work done!
[2]: Worker (1): Work done!
[5]: Worker (4): Work done!
[4]: Worker (3): Work done!
[1]: Working done.
[4]: Worker (3): Exit.
[5]: Worker (4): Exit.
[3]: Worker (2): Exit.
[2]: Worker (1): Exit.
[6]: Worker (5): Exit.
```

---

## Beispiele zu `std::barrier`


### Potenzen berechnen

In diesem einfachen Beispiel werden Potenten natürlicher Zahlen berechnet und in einem `std::vector`-Objekt abgelegt.
Man beachte im Quellcode: Die Ausgaben auf die Konsole erfolgen direkt mit `std::cout`,
also nicht mit einer thread-sicheren `Logger`-Klasse. Warum geht das in diesem Beispiel?

*Ausgabe*:

```
         13960               1              2              3              4              5              6
          8780               1              4              9             16             25             36
          2368               1             16             81            256            625           1296
          6880               1            256           6561          65536         390625        1679616
          2368               1          65536       43046721     4294967296   152587890625  2821109907456
Done.
```

Mit dem GCC-Compiler sehen die Thread-Ids etwas anders aus, aber ansonsten läuft das Programm identisch ab:


```
140527120779136               1              2              3              4              5              6
140527078799104               1              4              9             16             25             36
140527120762624               1             16             81            256            625           1296
140527087191808               1            256           6561          65536         390625        1679616
140527103977216               1          65536       43046721     4294967296   152587890625  2821109907456
Done.
```

### Summenbildung von disjunkten Zahlenbereichen

Dieses Beispiel ist sehr ähnlich zum entsprechenden Beispiel mit einem `std::latch`-Objekt.
An Stelle des `std::latch`-Objekts kommt ein `std::barrier`-Objekt zum Einsatz.

### Synchronisation von Arbeitsabläufen

In diesem Beispiel betrachten wir Arbeitsabläufe, die von einem oder mehreren Threads (Prozeduren) abgearbeitet werden.
Im Stile einer *Lastverteilung* (*Load Balancing*) kann ein Arbeitsablauf prinzipiell

  * ganztags,
  * nur vormittags oder
  * nur nachmittags

bearbeitet werden. In jedem Fall wird vor- und nachmittags gearbeitet.
Alle Nachmittagsarbeiten können aber erst dann begonnen werden,
wenn die Vormittagsarbeiten (sowohl Voll- also auch Teilzeitarbeiter) beendet sind.
Wir stellen fest, dass es nach der Vormittagsarbeit einen Synchronisationspunkt gibt:
Die Vollzeitarbeiter und die Vormittags-Teilzeitarbeiter (sofern es welche gibt) melden den Vollzug ihrer Vormittagsarbeit,
erst dann kann am Nachmittag (Vollzeitarbeiter und die Nachmittags-Teilzeitarbeiter &ndash; sofern es welche gibt)
weitergearbeitet werden. Im Ablauf des Programms soll diese Arbeitsweise durch geeignete Ausgaben visualisiert werden:

  * Die Vollzeitarbeiter protokollieren, wann sie vormittags zu arbeiten beginnen, wann sie vormittags fertig sind, wann sie nachmittags zu arbeiten beginnen und wann sie schließlich nachmittags fertig sind.
  * Die Teilzeitarbeiter protokollieren, wann sie entweder vormittags zu arbeiten beginnen und wann sie vormittags fertig sind oder wann sie nachmittags zu arbeiten beginnen und wann sie nachmittags fertig sind.

Man betrachte beim Studieren des Quellcodes den Einsatz der
beiden Methoden `arrive_and_wait` und `arrive_and_drop` am `std::barrier`-Objekt!

1. *Beispiel*: Es arbeiten drei Vollzeit- und drei Vormittags-Teilzeitarbeiter:

*Ausgabe*:

```cpp
[1]: Working starts [PartimeWorker & FulltimeWorker]:
[2]: forenoonWorker (a): Forenoon work starting!
[3]: forenoonWorker (b): Forenoon work starting!
[4]: forenoonWorker (c): Forenoon work starting!
[5]: fulltimeWorker (1): Forenoon work starting!
[4]: forenoonWorker (c): Forenoon work done!
[4]: fulltimeWorker (2): Forenoon work starting!
[6]: fulltimeWorker (3): Forenoon work starting!
[5]: fulltimeWorker (1): Forenoon work done!
[2]: forenoonWorker (a): Forenoon work done!
[6]: fulltimeWorker (3): Forenoon work done!
[4]: fulltimeWorker (2): Forenoon work done!
[3]: forenoonWorker (b): Forenoon work done!
[4]: fulltimeWorker (2): Afternoon work starting!
[5]: fulltimeWorker (1): Afternoon work starting!
[6]: fulltimeWorker (3): Afternoon work starting!
[5]: fulltimeWorker (1): Afternoon work done!
[6]: fulltimeWorker (3): Afternoon work done!
[4]: fulltimeWorker (2): Afternoon work done!
[1]: Working ends starts [PartimeWorker & FulltimeWorker].
```

2. *Beispiel*: Es arbeiten drei Vollzeit- und drei Nachmittags-Teilzeitarbeiter:

*Ausgabe*:

```cpp
[1]: Working starts [PartimeWorker & FulltimeWorker]:
[2]: fulltimeWorker  (1): Forenoon work starting!
[3]: fulltimeWorker  (2): Forenoon work starting!
[4]: fulltimeWorker  (3): Forenoon work starting!
[2]: fulltimeWorker  (1): Forenoon work done!
[3]: fulltimeWorker  (2): Forenoon work done!
[4]: fulltimeWorker  (3): Forenoon work done!
[4]: fulltimeWorker  (3): Afternoon work starting!
[3]: fulltimeWorker  (2): Afternoon work starting!
[2]: fulltimeWorker  (1): Afternoon work starting!
[5]: afternoonWorker (A): Afternoon work starting!
[6]: afternoonWorker (B): Afternoon work starting!
[7]: afternoonWorker (C): Afternoon work starting!
[6]: afternoonWorker (B): Afternoon work done!
[2]: fulltimeWorker  (1): Afternoon work done!
[3]: fulltimeWorker  (2): Afternoon work done!
[7]: afternoonWorker (C): Afternoon work done!
[5]: afternoonWorker (A): Afternoon work done!
[4]: fulltimeWorker  (3): Afternoon work done!
[1]: Working ends starts [PartimeWorker & FulltimeWorker].
```

---

#### Quellcode:

Eine Beschreibung der Beispiele folgt weiter unten:

[*Latches.cpp*](Latches.cpp)<br />
[*Barriers.cpp*](Barriers.cpp)

---

## Literaturhinweise:

Die Anregungen zu den Beispielen aus diesem Abschnitt finden sich teilweise unter

[Latches in C++ 20](https://www.heise.de/developer/artikel/Latches-in-C-20-5033716.html)<br>(abgerufen am 17.02.2021)

und

[Barrieren in C++ 20](https://www.heise.de/developer/artikel/Barrieren-und-atomare-Smart-Pointers-in-C-20-5041229.html)<br>(abgerufen am 17.02.2021)

vor.

Die restlichen Beispiele stammen im Wesentlichen aus dem Buch

[C++ &ndash; The Complete Guide](https://www.josuttis.com/cppstd20/index.html) von Nicolai M. Josuttis.

---

[Zurück](../../Readme.md)

---
