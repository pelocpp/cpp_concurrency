# Semaphore

[Zurück](../../Readme.md)

---

### Allgemeines

Unter einer Semaphore versteht man in der Systemprogrammierung eine Datenstruktur,
die aus einem ganzzahligen Wert (Zähler) und zwei atomaren Nutzungsoperationen &ldquo;Reservieren&rdquo; und &ldquo;Freigeben&rdquo; besteht.

Sie eignet sich insbesondere zur Verwaltung eines Code-Abschnitts,
auf den mehrere Prozesse/Threads zugreifen wollen.

Das Konzept der Semaphore als Mechanismus für die Synchronisation wurden von Edsger W. Dijkstra erfunden
und 1965 in seinem Artikel *Cooperating Sequential Processes* vorgestellt.

Im Gegensatz zu einem Mutex-Objekt müssen die Threads, die &ldquo;reservieren&rdquo; und &ldquo;freigeben&rdquo;, nicht identisch sein.

Im Gegensatz zu einem `std::mutex` ist eine `std::counting_semaphore` nicht an einen Thread gebunden.
Das heißt, dass das Anfordern und Freigeben der Semaphore in verschiedenen Threads stattfinden kann.

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
11:         using namespace std::chrono_literals;
12: 
13:         std::cout << "scheduleJob: Data preparing ...\n";
14: 
15:         std::this_thread::sleep_for(5000ms);
16: 
17:         std::cout << "scheduleJob: Data prepared.\n";
18: 
19:         m_semaphore.release();
20:     }
21: 
22:     void executeJob() {
23: 
24:         using namespace std::chrono_literals;
25: 
26:         std::cout << "executeJob:  Waiting for data ...\n";
27: 
28:         m_semaphore.acquire();
29: 
30:         std::cout << "executeJob:  Executing job ...\n";
31: 
32:         std::this_thread::sleep_for(3000ms);
33: 
34:         std::cout << "executeJob:  Done.\n";
35:     }
36: };
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
die zum gleichzeitigen Einstellen von Druckenaufträgen verwendet werden kann.

Die Druckerwarteschlange wird durch ein binäres Semaphor geschützt, sodass jeweils nur Druckauftrag
(ein Thread) drucken kann.

Studieren Sie den Quellcode und die Ausgabe des Programms:

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


#### Quellcode:

[Teil 1: Einfaches Beispiel](Semaphore_01.cpp).

[Teil 2: Drucker mit Warteschlange](Semaphore_02.cpp).

---

[Zurück](../../Readme.md)

---
