# Locking Mechanismen (*Locking Mechanisms*)

[Zur�ck](../../Readme.md)

---

# Verwendete Hilfsmittel:

  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::shared_lock`

---

### Allgemeines

Concurrency (Nebenl�ufigkeit, Parallelit�t) und Synchronisation sind entscheidende Aspekte der Multithreading-Programmierung.
In C++ stellt die Standardbibliothek mehrere Synchronisierungsprimitive bereit,
darunter `std::lock_guard`, `std::unique_lock` und `std::shared_lock`,
die dazu beitragen, Thread-Sicherheit zu gew�hrleisten und *Data Races* zu verhindern,
wenn mehrere Threads auf gemeinsam genutzte Ressourcen zugreifen.


### Beteiligte Klassen und Objekte

Das Verst�ndnis der Unterschiede und Anwendungsf�lle dieser Sperrmechanismen ist
f�r das Schreiben effizienter und korrekter nebenl�ufiger Programme von entscheidender Bedeutung.
Folgende Klassen werden hierzu von der C++&ndash;Standardbibliothek bereitgestellt:

  * Klasse `std::lock_guard`
    - Leichtgewichtige Realisierung (*lightweight implementation*) eines einfachen Sperrmechanismus.
    - Bietet exklusiven Besitz eines Mutex f�r einen begrenzten Zeitraum (&bdquo;Scope&rdquo;, Block, Bereich).
    - Die Sperre wird bei der Erstellung des `std::lock_guard`-Objekts erworben und automatisch freigegeben, wenn das Objekt den G�ltigkeitsbereich (*Scope*) verl�sst.
    - Ein manuelles Entsperren oder erneutes Sperren des Mutex wird nicht unterst�tzt.
    - Ideal f�r einfache, kurzzeitige Sperren, die eine automatische Freigabe beim Verlassen des Blocks gew�hrleisten.

  * Klasse `std::unique_lock`
    - Vielseitiger Sperrmechanismus mit einer umfangreicheren Funktionalit�t als `std::lock_guard`.
    - Es wird sowohl der exklusive Besitz (*exclusive ownership*) als auch der gemeinsame Besitz (*shared ownership*) eines Mutex-Objekts unterst�tzt.
    - Erm�glicht manuelles Entsperren als auch erneutes Sperren.
    - Unterst�tzt Verschiebe-Semantik.
    - Unterst�tzt zeitgesteuertes Sperren sowie Bedingungsvariablen (`std::condition_variable`).

  * Klasse `std::shared_lock`
    - `std::shared_lock` ist f�r den gemeinsamen Besitz (*shared ownership*) eines Mutex-Objekts konzipiert und erm�glicht mehrere Leser.
    - Erm�glicht mehreren Threads den gleichzeitigen Erwerb der Sperre f�r den gemeinsamen lesenden Zugriff.
    - Gew�hrleistet Thread-Sicherheit, wenn mehrere Threads auf gemeinsam genutzte Ressourcen lesend zugreifen.
    - �hnlich wie `std::lock_guard` wird kein manuelles Entsperren oder erneutes Sperren unterst�tzt.

*Zusammenfassung*:

Wenn Sie die Unterschiede zwischen `std::lock_guard`, `std::unique_lock` und `std::shared_lock` verstehen,
k�nnen Sie feststellen, welcher Mechanismus Ihren spezifischen Anforderungen entspricht:

Verwenden Sie `std::lock_guard` f�r einfache exklusive Sperren,
`std::unique_lock` f�r erweiterte Funktionalit�t und Flexibilit�t und `std::shared_lock`
f�r den gemeinsamen Zugriffs.

Durch den Einsatz des richtigen Sperrmechanismus k�nnen Sie Thread-Sicherheit gew�hrleisten, *Data Races* verhindern
und effiziente Multithreaded-Programme erstellen.


### Beispiel


```cpp
```


```cpp
```


*Ausgabe*:


```
```

---

#### Quellcode:

[XXX.cpp](XXX.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Aufsatz

[Understanding Locking Mechanisms in C++](https://medium.com/@elysiumceleste/understanding-locking-mechanisms-in-c-std-lock-guard-std-unique-lock-and-std-shared-lock-a8aac4d575ce)

von Elysium Celeste.

---

[Zur�ck](../../Readme.md)

---

