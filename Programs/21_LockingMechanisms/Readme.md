# Mutexe und Locking Mechanismen (*Locking Mechanisms*)

[Zurück](../../Readme.md)

---

## Verwendete Hilfsmittel:

  * Klasse `std::mutex`
  * Klasse `std::recursive_mutex`
  * Klasse `std::shared_mutex`
 
  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::scoped_lock`
  * Klasse `std::shared_lock`

---

## Allgemeines

*Concurrency* (Nebenläufigkeit, Parallelität) und *Synchronization* (Synchronisation)
sind entscheidende Aspekte der Multithreading-Programmierung.
In C++ stellt die Standardbibliothek mehrere Synchronisierungsprimitive bereit,
wie etwa `std::mutex`, `std::lock_guard`, `std::unique_lock` usw.,
die dazu beitragen, Thread-Sicherheit zu gewährleisten und *Data Races* zu verhindern,
wenn mehrere Threads auf gemeinsam genutzte Ressourcen gleichzeitig zugreifen.


### Beteiligte Klassen und Objekte

Das Verständnis der Unterschiede und Anwendungsfälle dieser Sperrmechanismen ist
für das Schreiben effizienter und korrekter nebenläufiger Programme von entscheidender Bedeutung.
Folgende Klassen werden hierzu von der C++&ndash;Standardbibliothek bereitgestellt:


#### Mutex-Klassen

##### Klasse `std::mutex`

Die Klasse `std::mutex` steht für sperrbare Objekte. Darunter versteht man Objekte, die zwei
Methoden `lock` und `unlock` besitzen, die verhindern, dass kritische Codeabschnitte von anderen Threads
zum selben Zeitpunkt ausgeführt werden können und auf dieselben Speicherorte wiederum zum selben Zeitpunkt
zugegriffen werden kann.

  * Mit einem `std::mutex`-Objekt kann man *Race Conditions* zwischen mehreren Threads verhindern, indem man den Zugriff auf eine gemeinsam genutzte Ressource explizit sperren (`lock`) und entsperren (`unlock`) kann.
  * Nachteil: Kommt es &ndash; aus welchen Gründen auch immer &ndash; nicht zum Aufruf von `unlock` nach einem `lock`-Aufruf,
    gerät die Programmausführung in den Zustand &bdquo;UB&rdquo; (*Undefined Behaviour*).


##### Klasse `std::recursive_mutex`

Die `std::recursive_mutex`-Klasse ist eine Variante der `std::mutex`-Klasse,
die es einem Thread ermöglicht, mehrere Sperren zu halten.
Erst wenn alle Sperren aufgehoben sind, können andere Threads dieses Mutexobjekt erwerben.

Ein Anwendungsfall für die `std::recursive_mutex`-Klasse wird in dem Entwurfsmuster
[Strategisches Locking (Strategized Locking)](../../Programs/22_StrategizedLocking/Readme.md)
aufgezeigt.

Die Klasse `std::recursive_mutex` ist dann erforderlich, wenn Sie zum einen
threadsichere Datenstrukturen entwerfen und zum anderen die Methoden der
öffentlichen Schnittstelle von unterschiedlichen Ebenen aus
auf die kritischen Abschnitte bzw. Daten der Datenstruktur zugreifen.

#### Hüllen-Klassen für Mutexobjekte

##### Klasse `std::lock_guard`

Leichtgewichtige Realisierung (*lightweight implementation*) eines einfachen Sperrmechanismus.
Bietet exklusiven Besitz eines `std::mutex`-Objekts für einen begrenzten Zeitraum (&bdquo;Scope&rdquo;, Block, Bereich).

  * Ein `std::lock_guard`-Objekt umschließt ein `std::mutex`-Objekt.
  * Bei der Erstellung eines `std::lock_guard`-Objekts wird das Mutexobjekt automatisch gesperrt.
  * Bei der Zerstörung (Verlassen des Gültigkeitsbereichs, *Scope*) wird das Mutexobjekt automatisch entsperrt.
  * Ein manuelles Entsperren oder erneutes Sperren des Mutexobjekts wird nicht unterstützt.
  * Ein `std::lock_guard`-Objekt kann ein bereits gesperrtes Mutexobjekt &bdquo;übernehmen&rdquo; (siehe in den Beispielen Parameter `std::adopt_lock`).
  * Ideal für einfache, kurzzeitige Sperren, die eine automatische Freigabe beim Verlassen des Blocks gewährleisten.


##### Klasse `std::unique_lock`

Die Klasse `std::unique_lock` bietet einen vielseitigen Sperrmechanismus
mit einer umfangreicheren Funktionalität als Klasse `std::lock_guard`.

  * Ein `std::unique_lock`-Objekt umschließt ein `std::mutex`-Objekt.
  * Im Gegensatz zu einem `std::lock_guard`-Objekt kann ein `std::unique_lock`-Objekt nach der Erstellung explizit gesperrt und entsperrt werden, es ermöglicht also sowohl manuelles Entsperren als auch erneutes Sperren.
  * Sie können immer ein `std::unique_lock`-Objekt anstelle eines `std::lock_guard`-Objekts verwenden, aber nicht umgekehrt.
  * Ein `std::unique_lock`-Objekt kann erzeugt werden, ohne dass zum Erzeugungszeitpunkt das Mutexobjekt gesperrt ist (siehe Gebrauch des Parameters `std::defer_lock`).
  * Es wird sowohl der exklusive Besitz (*exclusive ownership*) als auch der gemeinsame Besitz (*shared ownership*) eines Mutexobjekts unterstützt.
  * Unterstützt Verschiebe-Semantik.
  * Unterstützt zeitgesteuertes Sperren sowie Bedingungsvariablen (`std::condition_variable`).


Wir gehen noch auf die Klasse `std::condition_variable` ein:

Ein `std::unique_lock`-Objekt muss von der &bdquo;empfangenden&rdquo; Seite
(der Seite, die benachrichtigt wird) verwendet werden, um bei Gebrauch eines `std::condition_variable`-Objekts
entsprechende Benachrichtigungen empfangen zu können.

Der Grund, warum ein `std::unique_lock` für eine `std::condition_variable` erforderlich ist, besteht darin,
dass dieses das zugrunde liegende `std::mutex`-Objekt jedes Mal sperren kann,
wenn die Bedingungsvariable (`std::condition_variable`) nach einer gültigen Benachrichtigung
aus einer Wartephase aufwacht und einen kritischen Codeabschnitt ausführt.

Das `std::unique_lock`-Objekt entsperrt das zugrunde liegende Mutexobjekt jedes Mal, wenn

  * der Aufruf der `wait`-Methode an der Bedingungsvariablen fälschlicherweise aktiviert wurde, es muss also erneut gewartet werden.
  * bei automatischer Zerstörung des `std::unique_lock`-Objekts. Dies ist der Fall, wenn der kritische Abschnitt ausgeführt und damit abgelaufen ist und der Gültigkeitsbereich des `std::unique_lock`-Objekts verlassen wird.



##### Klasse `std::scoped_lock`

Die Klasse `std::scoped_lock` realisiert einen sehr einfachen Mechanismus für sperrbare Objekte,
ähnlich zur Klasse `std::lock_guard`, aber mit dem Unterschied, dass `std::scoped_lock`
für mehrere Mutexobjekte gleichzeitig verwendet werden kann!

  * Ein `std::scoped_lock`-Objekt umschließt ein oder mehrere Mutexobjekte, genauso wie ein `std::lock_guard`-Objekt,
    aber mit der Ausnahme, dass ein `std::lock_guard`-Objekt jeweils nur **ein** Mutexobjekt umschließen kann!
  * Bei der Erstellung sperrt ein `std::scoped_lock`-Objekt automatisch ein oder mehrere Mutexobjekte.
  * Bei der Zerstörung (Verlassen des Gültigkeitsbereichs) entsperrt ein `std::scoped_lock`-Objekt automatisch alle Mutexobjekte.

  

##### Klassen `std::shared_mutex` und `std::shared_lock`

In C++ kann man mit den beiden Klassen `std::shared_mutex` und `std::shared_lock` ein Synchronisationsprimitiv umsetzen,
das es mehreren Threads ermöglicht, eine gemeinsam genutzte Ressource gleichzeitig zum Lesen zu nutzen
und gleichzeitig exklusiven Schreibzugriff zu gewährleisten.

Dies ist in Situationen hilfreich, in denen viele Threads schreibgeschützten Zugriff
auf dieselbe Datenstruktur benötigen, Schreibvorgänge jedoch nicht häufig verwendet werden.


  * Die Klasse `std::shared_lock` ist für den gemeinsamen Besitz (*shared ownership*) eines Mutexobjekts konzipiert und ermöglicht mehrere Leser.
  * Ermöglicht mehreren Threads den gleichzeitigen Erwerb der Sperre für den gemeinsamen lesenden Zugriff.
  * Gewährleistet Thread-Sicherheit, wenn mehrere Threads auf gemeinsam genutzte Ressourcen lesend zugreifen.
  * Ähnlich wie `std::lock_guard` wird kein manuelles Entsperren oder erneutes Sperren unterstützt.

Eine genauere Beschreibung der beiden Klassen `std::shared_mutex` und `std::shared_lock` wird in dem Entwurfsmuster
[Reader-Writer Lock](../../Programs/23_ReaderWriterLock/Readme.md)
aufgezeigt.




*Zusammenfassung*:

Durch den Einsatz des richtigen Sperrmechanismus können Sie Thread-Sicherheit gewährleisten, *Data Races* verhindern
und effiziente Multithreaded-Programme erstellen.

---

##### Quellcode:

[Examples.cpp](Examples.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Aufsatz

[Understanding Locking Mechanisms in C++](https://medium.com/@elysiumceleste/understanding-locking-mechanisms-in-c-std-lock-guard-std-unique-lock-and-std-shared-lock-a8aac4d575ce)

von Elysium Celeste.

Zum Zweiten sind auch viele Informationen aus dem Artikel

[C++ Mutexes, Concurrency, and Locks](https://gabrielstaples.com/cpp-mutexes-and-locks/)

von Gabriel Staples übernommen worden.

---

[Zurück](../../Readme.md)

---

