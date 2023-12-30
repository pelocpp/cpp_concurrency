# Locking Mechanismen (*Locking Mechanisms*)

[Zur�ck](../../Readme.md)

---

## Verwendete Hilfsmittel:

  * Klasse `std::mutex`
  * Klasse `std::recursive_mutex`

  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::scoped_lock`
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


### Mutex-Klassen


#### Klasse `std::mutex`

Sperrbares Objekt (Methoden `lock` und `unlock`), das verhindert, das kritische Codeabschnitte von anderen Threads
zum selben Zeitpunkt ausgef�hrt werden k�nnen und auf dieselben Speicherorte zugreifen.

  * Mit einem `std::mutex`-Objekt kann man *Race Conditions* zwischen mehreren Threads verhindern, indem man den Zugriff auf eine gemeinsam genutzte Ressource explizit sperren (`lock`) und entsperren (`unlock`) kann.
  * Nachteil: Kommt es &ndash; aus welchen Gr�nden auch immer &ndash; nicht zum Aufruf von `unlock` nach einem `lock`-Aufruf,
    ger�t die Programmausf�hrung in den UB-Zustand (*undefined behaviour*).


#### Klasse `std::recursive_mutex`

Die `std::recursive_mutex`-Klasse ist eine Variante der `std::mutex`-Klasse,
die es einem Thread erm�glicht, mehrere Sperren zu halten.

Erst wenn alle Sperren aufgehoben sind, k�nnen andere Threads dieses Mutexobjekt erwerben.

Ein Anwendungsfall f�r die `std::recursive_mutex`-Klasse wird in dem Entwurfsmuster
[Strategisches Locking (Strategized Locking)](../../Programs/22_StrategizedLocking/Readme.md)
aufgezeigt.

Die Klasse `std::recursive_mutex` ist dann erforderlich, wenn Sie zum einen
threadsichere Datenstrukturen entwerfen und zum anderen die Methoden der
�ffentlichen Schnittstelle von unterschiedlichen Ebenen aus
auf die kritischen Abschnitte bzw. Daten zugreifen.

### H�llen-Klassen f�r Mutexobjekte

#### Klasse `std::lock_guard`

Leichtgewichtige Realisierung (*lightweight implementation*) eines einfachen Sperrmechanismus.
Bietet exklusiven Besitz eines `std::mutex`-Objekts f�r einen begrenzten Zeitraum (&bdquo;Scope&rdquo;, Block, Bereich).

  * Ein `std::lock_guard`-Objekt umschlie�t ein `std::mutex`-Objekt.
  * Bei der Erstellung eines `std::lock_guard`-Objekts wird der Mutex automatisch gesperrt.
  * Bei der Zerst�rung beim Verlassen des G�ltigkeitsbereichs (*Scope*) wird der Mutex automatisch entsperrt.
  * Die Sperre wird bei der Erstellung des `std::lock_guard`-Objekts erworben und automatisch freigegeben, wenn das Objekt den G�ltigkeitsbereich (*Scope*) verl�sst.
  * Ein manuelles Entsperren oder erneutes Sperren des Mutex wird nicht unterst�tzt.
  * Ein `std::lock_guard`-Objekt kann ein bereits gesperrtes Mutexobjekt &bdquo;�bernehmen&rdquo; (siehe Parameter `std::adopt_lock`).
  * Ideal f�r einfache, kurzzeitige Sperren, die eine automatische Freigabe beim Verlassen des Blocks gew�hrleisten.


#### Klasse `std::unique_lock`

Die Klasse `std::unique_lock` bietet einen vielseitigen Sperrmechanismus
mit einer umfangreicheren Funktionalit�t als `std::lock_guard`.

  * Ein `std::unique_lock`-Objekt umschlie�t ein `std::mutex`-Objekt.
  * Im Gegensatz zu einem `std::lock_guard`-Objekt kann ein `std::unique_lock`-Objekt nach der Erstellung explizit gesperrt und entsperrt werden, es erm�glicht also sowohl manuelles Entsperren als auch erneutes Sperren.
  * Sie k�nnen immer einen `std::unique_lock`-Objekt anstelle eines `std::lock_guard`-Objekts verwenden, aber nicht umgekehrt.
  * Ein `std::unique_lock`-Objekt kann erzeugt werden, ohne dass zum Erzeugungszeitpunkt das Mutex-Objekt gesperrt ist (siehe Gebrauch von `std::defer_lock`).
  * Es wird sowohl der exklusive Besitz (*exclusive ownership*) als auch der gemeinsame Besitz (*shared ownership*) eines Mutex-Objekts unterst�tzt.
  * Unterst�tzt Verschiebe-Semantik.
  * Unterst�tzt zeitgesteuertes Sperren sowie Bedingungsvariablen (`std::condition_variable`).


Wir gehen noch auf die Klasse `std::condition_variable` ein:

Ein `std::unique_lock`-Objekt muss von der &bdquo;empfangenden&rdquo; Seite
(der Seite, die benachrichtigt wird) verwendet werden, um bei Gebrauch eines `std::condition_variable`-Objekts
entsprechende Benachrichtigungen zu empfangen.

Der Grund, warum ein `std::unique_lock` f�r eine `std::condition_variable` erforderlich ist, besteht darin,
dass er das zugrunde liegende `std::mutex`-Objekt jedes Mal sperren kann,
wenn die Bedingungsvariable (`std::condition_variable`) nach einer g�ltigen Benachrichtigung
aus einer Wartezeit aufwacht und einen kritischen Codeabschnitt ausf�hrt.

Das `std::unique_lock`-Objekt entsperrt das zugrunde liegende Mutex-Objekt jedes Mal, wenn

  * der Aufruf der `wait`-Methode an der Bedingungsvariablen f�lschlicherweise aktiviert wird und erneut gewartet werden muss, oder
  * bei automatischer Zerst�rung des `std::unique_lock`-Objekts, wenn der kritische Abschnitt ausgef�hrt und abgelaufen ist und der G�ltigkeitsbereich des `std::unique_lock`-Objekts verlassen wird.



#### Klasse `std::scoped_lock`

Die Klasse `std::scoped_lock` ist ein sehr einfacher Mechanismus f�r sperrbare Objekte,
�hnlich zur Klasse `std::lock_guard`, mit der Ausnahme, dass `std::scoped_lock`
f�r mehrere Mutex-Objekte gleichzeitig verwendet werden kann!

  * Ein `std::scoped_lock`-Objekt umschlie�t ein oder mehrere Mutex-Objekte, genauso wie ein `std::lock_guard`-Objekt,
    mit der Ausnahme, dass ein `std::lock_guard`-Objekt jeweils nur **ein** Mutexobjekt umschlie�en kann!
  * Bei der Erstellung sperrt ein `std::scoped_lock`-Objekt automatisch ein oder mehrere Mutex-Objekte.
  * Bei der Zerst�rung (Verlassen des G�ltigkeitsbereichs) entsperrt ein `std::scoped_lock`-Objekt automatisch alle Mutexobjekte.

  

#### Klasse `std::shared_lock`

  * `std::shared_lock` ist f�r den gemeinsamen Besitz (*shared ownership*) eines Mutex-Objekts konzipiert und erm�glicht mehrere Leser.
  * Erm�glicht mehreren Threads den gleichzeitigen Erwerb der Sperre f�r den gemeinsamen lesenden Zugriff.
  * Gew�hrleistet Thread-Sicherheit, wenn mehrere Threads auf gemeinsam genutzte Ressourcen lesend zugreifen.
  * �hnlich wie `std::lock_guard` wird kein manuelles Entsperren oder erneutes Sperren unterst�tzt.

*Zusammenfassung*:

Wenn Sie die Unterschiede zwischen `std::lock_guard`, `std::unique_lock` und `std::shared_lock` verstehen,
k�nnen Sie feststellen, welcher Mechanismus Ihren spezifischen Anforderungen entspricht:

Verwenden Sie `std::lock_guard` f�r einfache exklusive Sperren,
`std::unique_lock` f�r erweiterte Funktionalit�t und Flexibilit�t und `std::shared_lock`
f�r den gemeinsamen Zugriffs.

Durch den Einsatz des richtigen Sperrmechanismus k�nnen Sie Thread-Sicherheit gew�hrleisten, *Data Races* verhindern
und effiziente Multithreaded-Programme erstellen.

---

#### Quellcode:

[Examples.cpp](Examples.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Aufsatz

[Understanding Locking Mechanisms in C++](https://medium.com/@elysiumceleste/understanding-locking-mechanisms-in-c-std-lock-guard-std-unique-lock-and-std-shared-lock-a8aac4d575ce)

von Elysium Celeste.

Zum Zweiten sind auch viele Informationen aus dem Artikel

https://gabrielstaples.com/cpp-mutexes-and-locks

[C++ Mutexes, Concurrency, and Locks](https://gabrielstaples.com/cpp-mutexes-and-locks/)

�bernommen worden.

---

[Zur�ck](../../Readme.md)

---

