#  Das Produzenten-Verbraucher Problem (*Producer Consumer Pattern*)

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klassen `std::binary_semaphore` und `std::counting_semaphore`

---

### Allgemeines

Unter einer Semaphore versteht man in der Systemprogrammierung eine Datenstruktur,
die aus einem ganzzahligen Wert (Zähler) und zwei atomaren Nutzungsoperationen &bdquo;Reservieren&rdquo; und &bdquo;Freigeben&rdquo; besteht.

Sie eignet sich insbesondere zur Verwaltung eines Code-Abschnitts,
auf den mehrere Prozesse/Threads zugreifen wollen.

Das Konzept der Semaphore als Mechanismus für die Synchronisation wurde von *Edsger W. Dijkstra* erfunden
und 1965 in seinem Artikel *Cooperating Sequential Processes* vorgestellt.

Im Gegensatz zu einem Mutex-Objekt müssen die Threads, die &bdquo;reservieren&rdquo; und &bdquo;freigeben&rdquo; nicht
identisch sein, also sind 
im Gegensatz zu einem `std::mutex`-Objekt ein `std::binary_semaphore`- bzw. ein `std::counting_semaphore`-Objekt nicht an einen Thread gebunden.
Das Anfordern und Freigeben der Semaphore darf in verschiedenen Threads stattfinden.

Intern besitzt die Datenstruktur neben dem Zähler noch eine Warteschlange
für die Aufnahme blockierter Threads.

---

### Wie funktionieren Semaphore?

Semaphore besitzen eine Zählervariable, die man erhöhen oder verringern kann.

Die Methoden `acquire` und `release` der Semaphore-Klasse steuern die Zählervariable.

Die Methode `acquire` fragt den Semaphore nach Zugriff auf die Ressource.
Wenn der Wert > 0 ist, wird der Zugriff gewährt und der Zähler wird um 1 reduziert.

Die `release`-Methode gibt den zuvor gewährten Zugriff frei
und erhöht die Variable des Semaphors wieder um 1.


---

#### Ein erstes Beispiel

Semaphore können in Sender-Empfänger-Abläufen verwendet werden.

Wird zum Beispiel eine Semaphore auf 0 initialisiert, blockiert der Empfänger-Aufruf `acquire`,
bis der Sender `release` ausführt.
Damit wartet der Empfänger auf die Benachrichtigung des Senders:

```cpp
01: class Worker
02: {
03: private:
04:     std::binary_semaphore m_semaphore;
05: 
06: public:
07:     Worker() : m_semaphore{ 0 } {}
08: 
09:     void scheduleJob() {
10: 
11:         std::cout << "scheduleJob: Data preparing ...\n";
12: 
13:         std::this_thread::sleep_for(std::chrono::seconds{ 5 });
14: 
15:         std::cout << "scheduleJob: Data prepared.\n";
16: 
17:         m_semaphore.release();
18:     }
19: 
20:     void executeJob() {
21: 
22:         std::cout << "executeJob:  Waiting for data ...\n";
23: 
24:         m_semaphore.acquire();
25: 
26:         std::cout << "executeJob:  Executing job ...\n";
27: 
28:         std::this_thread::sleep_for(std::chrono::seconds{ 3 });
29: 
30:         std::cout << "executeJob:  Done.\n";
31:     }
32: };
```

*Beispiel*:

```cpp
01: Worker worker{};
02: std::jthread t1{ &Worker::scheduleJob, &worker };
03: std::jthread t2{ &Worker::executeJob, &worker };
```


*Ausgabe*:


```
scheduleJob: Data preparing ...
executeJob:  Waiting for data ...
scheduleJob: Data prepared.
executeJob:  Executing job ...
executeJob:  Done.
```

Die einmalige Synchronisation von Threads lässt sich einfach mit Semaphoren umsetzen:

Das `std::binary_semaphore`-Objekt `m_semaphore` (Zeile 4) kann die Werte 0 oder 1 besitzen.

Im konkreten Anwendungsfall wird es auf `0` (Zeile 7) initialisiert.

Das heißt, dass der Aufruf `release` den Wert auf 1 (Zeile 19) setzt
und den Aufruf `acquire` in Zeile 28 entblockt.

---

#### Ein zweites Beispiel

Um die Verwendung von binären Semaphoren zu veranschaulichen, implementieren wir eine Druckerwarteschlange,
die zum gleichzeitigen Einstellen von Druckaufträgen verwendet werden kann.

Die Druckerwarteschlange wird durch ein binäres Semaphor geschützt, sodass jeweils nur Druckauftrag
(ein Thread) drucken kann.

Studieren Sie den Quellcode und die Ausgaben des Programms:

```
Thread 4ADC: PrintingJob:  Going to enqueue a document
Thread 05B0: PrintingJob:  Going to enqueue a document
Thread 517C: PrintingJob:  Going to enqueue a document
Thread 5010: PrintingJob:  Going to enqueue a document
Thread 1650: PrintingJob:  Going to enqueue a document
Thread 2864: PrintingJob:  Going to enqueue a document
Thread 45C8: PrintingJob:  Going to enqueue a document
Thread 538C: PrintingJob:  Going to enqueue a document
Thread 454C: PrintingJob:  Going to enqueue a document
Thread 5068: PrintingJob:  Going to enqueue a document
Thread 4ADC: PrinterQueue: Printing a Job during 507 millseconds.
Thread 4ADC: PrinterQueue: The document has been printed
Thread 05B0: PrinterQueue: Printing a Job during 506 millseconds.
Thread 05B0: PrinterQueue: The document has been printed
Thread 517C: PrinterQueue: Printing a Job during 1112 millseconds.
Thread 517C: PrinterQueue: The document has been printed
Thread 5010: PrinterQueue: Printing a Job during 1921 millseconds.
Thread 5010: PrinterQueue: The document has been printed
Thread 1650: PrinterQueue: Printing a Job during 1807 millseconds.
Thread 1650: PrinterQueue: The document has been printed
Thread 2864: PrinterQueue: Printing a Job during 662 millseconds.
Thread 2864: PrinterQueue: The document has been printed
Thread 45C8: PrinterQueue: Printing a Job during 763 millseconds.
Thread 45C8: PrinterQueue: The document has been printed
Thread 538C: PrinterQueue: Printing a Job during 1737 millseconds.
Thread 538C: PrinterQueue: The document has been printed
Thread 454C: PrinterQueue: Printing a Job during 1006 millseconds.
Thread 454C: PrinterQueue: The document has been printed
Thread 5068: PrinterQueue: Printing a Job during 644 millseconds.
Thread 5068: PrinterQueue: The document has been printed
```

---

#### Ein drittes Beispiel

*Ausgabe*:

```
>  wait 3 seconds (no thread enabled)
>  enable 3 parallel threads
CABACABACAABCAACABACDDBCDDCDBDCD
>  enable 4 more parallel threads
EFGHDEBCDFGEDHEIJFGEBJEIJFGHEJEBIJFGEJHEIJ
>  stop processing
>  wait for end of threads
FGKBJKIJHFGKJKLIMFGKMHKIMFGKLMKIMHFGKMKLIMMHIMLMHLHLLLLL
>  Done.
```

---


#### Quellcode:

[Teil 1: Einfaches Beispiel](Semaphore_01.cpp).<br />
[Teil 2: Drucker mit Warteschlange](Semaphore_02.cpp).<br />
[Teil 3: Ein Beispiel zur Klasse `std::counting_semaphore`](Semaphore_03.cpp).<br />

---

## Literaturhinweise

Die Idee zu dem Beispiel mit der Druckerwarteschlange stammt aus dem Artikel

[Binary Semaphore Tutorial and Example](https://howtodoinjava.com/java/multi-threading/binary-semaphore-tutorial-and-example/).

Der Quellcode dieses Artikels ist zwar in Java geschrieben, lässt sich aber sehr
einfach nach C++ portieren.

Die Anregungen zum dritten Beispiel stammen im Wesentlichen aus dem Buch

[C++ &ndash; The Complete Guide](https://www.josuttis.com/cppstd20/index.html) von Nicolai M. Josuttis.


---

[Zurück](../../Readme.md)

---
