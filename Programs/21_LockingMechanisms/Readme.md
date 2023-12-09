# Locking Mechanismen (*Locking Mechanisms*)

[Zurück](../../Readme.md)

---


### Allgemeines

Concurrency (Nebenläufigkeit,, Parallelität) und Synchronisation sind entscheidende Aspekte der Multithreading-Programmierung.
In C++ stellt die Standardbibliothek mehrere Synchronisierungsprimitive bereit,
darunter std::lock_guard, std::unique_lock und std::shared_lock,
die dazu beitragen, Thread-Sicherheit zu gewährleisten und *Data Races* zu verhindern,
wenn mehrere Threads auf gemeinsam genutzte Ressourcen zugreifen.


### Beteiligte Klassen und Objekte

Das Verständnis der Unterschiede und Anwendungsfälle dieser Sperrmechanismen ist
für das Schreiben effizienter und korrekter gleichzeitiger Programme von entscheidender Bedeutung.
folgenden Klassen werden hierzu von der C++&ndash;Standardbibliothek bereitgestellt:



  * Klasse `std::lock_guard`
    * Leichtgewichtige Realisierung (*lightweight implementation*) eines einfachen Sperrmechanismus.
    * Bietet den exklusiven Besitz eines Mutex für einen begrenzten Zeitraum (&ldquo;Scope&rdquo;, Block, Bereich).
    * Die Sperre wird bei der Erstellung des `std::lock_guard`-Objekts erworben und automatisch freigegeben,
    * wenn das Objekt den Gültigkeitsbereich (*Scope*) verlässt.
    * Ein manuelles Entsperren oder erneutes Sperren des Mutex wird nicht unterstützt.
    * Ideal für einfache, kurzzeitige Sperren, die eine automatische Freigabe beim Verlassen des Blocks gewährleisten.


  * Schreiber: Instanz von `std::unique_lock`, z.B.
    `std::unique_lock<std::shared_mutex> lock{ m_mutex };`

  * Leser: Instanz von `std::shared_lock`, z.B.
    `std::shared_lock<std::shared_mutex> lock{ m_mutex };`


### Beispiel


Der kritische Bereich wird durch eine Klasse `Snapshots` gekapselt:

```cpp

```

Die beiden Leser erhalten Zugriff auf ein Objekt des Typs `Snapshots`
und lesen gleichzeitig Daten aus dem Feld aus:

```cpp

```

Wir erkennen, dass der schreibende und lesende Zugriff
korrekt ausgeführt wird:


*Ausgabe*:


```
1. reader: 0 snapshots
2. reader: 0 snapshots
1. reader: 0 snapshots
2. reader: 0 snapshots
1. reader: 0 snapshots
2. reader: 0 snapshots
1. reader: 0 snapshots
2. reader: 0 snapshots

2. reader: 2 snapshots
1. reader: 2 snapshots
2. reader: 2 snapshots
1. reader: 2 snapshots
2. reader: 2 snapshots
1. reader: 3 snapshots
2. reader: 3 snapshots

.....

2. reader: 18 snapshots
1. reader: 18 snapshots
2. reader: 18 snapshots
1. reader: 18 snapshots
2. reader: 18 snapshots
1. reader: 19 snapshots
2. reader: 19 snapshots
1. reader: 19 snapshots
2. reader: 19 snapshots
1. reader: 19 snapshots
2. reader: 19 snapshots
```

---

#### Quellcode:

[ReaderWriterLock.cpp](ReaderWriterLock.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Aufsatz

[Daily bit(e) of C++ | std::shared_mutex](https://medium.com/@simontoth/daily-bit-e-of-c-std-shared-mutex-ebe7477a7589) von Šimon Tóth.

---

[Zurück](../../Readme.md)

---

