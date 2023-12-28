# Locking Mechanismen (*Locking Mechanisms*)

[Zurück](../../Readme.md)

---

# Verwendete Hilfsmittel:

  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::shared_lock`

---

### Allgemeines

Concurrency (Nebenläufigkeit, Parallelität) und Synchronisation sind entscheidende Aspekte der Multithreading-Programmierung.
In C++ stellt die Standardbibliothek mehrere Synchronisierungsprimitive bereit,
darunter `std::lock_guard`, `std::unique_lock` und `std::shared_lock`,
die dazu beitragen, Thread-Sicherheit zu gewährleisten und *Data Races* zu verhindern,
wenn mehrere Threads auf gemeinsam genutzte Ressourcen zugreifen.


### Beteiligte Klassen und Objekte

Das Verständnis der Unterschiede und Anwendungsfälle dieser Sperrmechanismen ist
für das Schreiben effizienter und korrekter nebenläufiger Programme von entscheidender Bedeutung.
Folgende Klassen werden hierzu von der C++&ndash;Standardbibliothek bereitgestellt:

  * Klasse `std::lock_guard`
    - Leichtgewichtige Realisierung (*lightweight implementation*) eines einfachen Sperrmechanismus.
    - Bietet exklusiven Besitz eines Mutex für einen begrenzten Zeitraum (&bdquo;Scope&rdquo;, Block, Bereich).
    - Die Sperre wird bei der Erstellung des `std::lock_guard`-Objekts erworben und automatisch freigegeben, wenn das Objekt den Gültigkeitsbereich (*Scope*) verlässt.
    - Ein manuelles Entsperren oder erneutes Sperren des Mutex wird nicht unterstützt.
    - Ideal für einfache, kurzzeitige Sperren, die eine automatische Freigabe beim Verlassen des Blocks gewährleisten.

  * Klasse `std::unique_lock`
    - Vielseitiger Sperrmechanismus mit einer umfangreicheren Funktionalität als `std::lock_guard`.
    - Es wird sowohl der exklusive Besitz (*exclusive ownership*) als auch der gemeinsame Besitz (*shared ownership*) eines Mutex-Objekts unterstützt.
    - Ermöglicht manuelles Entsperren als auch erneutes Sperren.
    - Unterstützt Verschiebe-Semantik.
    - Unterstützt zeitgesteuertes Sperren sowie Bedingungsvariablen (`std::condition_variable`).

  * Klasse `std::shared_lock`
    - `std::shared_lock` ist für den gemeinsamen Besitz (*shared ownership*) eines Mutex-Objekts konzipiert und ermöglicht mehrere Leser.
    - Ermöglicht mehreren Threads den gleichzeitigen Erwerb der Sperre für den gemeinsamen lesenden Zugriff.
    - Gewährleistet Thread-Sicherheit, wenn mehrere Threads auf gemeinsam genutzte Ressourcen lesend zugreifen.
    - Ähnlich wie `std::lock_guard` wird kein manuelles Entsperren oder erneutes Sperren unterstützt.

*Zusammenfassung*:

Wenn Sie die Unterschiede zwischen `std::lock_guard`, `std::unique_lock` und `std::shared_lock` verstehen,
können Sie feststellen, welcher Mechanismus Ihren spezifischen Anforderungen entspricht:

Verwenden Sie `std::lock_guard` für einfache exklusive Sperren,
`std::unique_lock` für erweiterte Funktionalität und Flexibilität und `std::shared_lock`
für den gemeinsamen Zugriffs.

Durch den Einsatz des richtigen Sperrmechanismus können Sie Thread-Sicherheit gewährleisten, *Data Races* verhindern
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

[Zurück](../../Readme.md)

---

