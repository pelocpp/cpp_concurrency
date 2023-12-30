# Mutexe und Locking Mechanismen (*Locking Mechanisms*)

[Zur�ck](../../Readme.md)

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

*Concurrency* (Nebenl�ufigkeit, Parallelit�t) und *Synchronization* (Synchronisation)
sind entscheidende Aspekte der Multithreading-Programmierung.
In C++ stellt die Standardbibliothek mehrere Synchronisierungsprimitive bereit,
wie etwa `std::mutex`, `std::lock_guard`, `std::unique_lock` usw.,
die dazu beitragen, Thread-Sicherheit zu gew�hrleisten und *Data Races* zu verhindern,
wenn mehrere Threads auf gemeinsam genutzte Ressourcen gleichzeitig zugreifen.


### Beteiligte Klassen und Objekte

Das Verst�ndnis der Unterschiede und Anwendungsf�lle dieser Sperrmechanismen ist
f�r das Schreiben effizienter und korrekter nebenl�ufiger Programme von entscheidender Bedeutung.
Folgende Klassen werden hierzu von der C++&ndash;Standardbibliothek bereitgestellt:


#### Mutex-Klassen

##### Klasse `std::mutex`

Die Klasse `std::mutex` steht f�r sperrbare Objekte. Darunter versteht man Objekte, die zwei
Methoden `lock` und `unlock` besitzen, die verhindern, dass kritische Codeabschnitte von anderen Threads
zum selben Zeitpunkt ausgef�hrt werden k�nnen und auf dieselben Speicherorte wiederum zum selben Zeitpunkt
zugegriffen werden kann.

  * Mit einem `std::mutex`-Objekt kann man *Race Conditions* zwischen mehreren Threads verhindern, indem man den Zugriff auf eine gemeinsam genutzte Ressource explizit sperren (`lock`) und entsperren (`unlock`) kann.
  * Nachteil: Kommt es &ndash; aus welchen Gr�nden auch immer &ndash; nicht zum Aufruf von `unlock` nach einem `lock`-Aufruf,
    ger�t die Programmausf�hrung in den Zustand &bdquo;UB&rdquo; (*Undefined Behaviour*).


##### Klasse `std::recursive_mutex`

Die `std::recursive_mutex`-Klasse ist eine Variante der `std::mutex`-Klasse,
die es einem Thread erm�glicht, mehrere Sperren zu halten.
Erst wenn alle Sperren aufgehoben sind, k�nnen andere Threads dieses Mutexobjekt erwerben.

Ein Anwendungsfall f�r die `std::recursive_mutex`-Klasse wird in dem Entwurfsmuster
[Strategisches Locking (Strategized Locking)](../../Programs/22_StrategizedLocking/Readme.md)
aufgezeigt.

Die Klasse `std::recursive_mutex` ist dann erforderlich, wenn Sie zum einen
threadsichere Datenstrukturen entwerfen und zum anderen die Methoden der
�ffentlichen Schnittstelle von unterschiedlichen Ebenen aus
auf die kritischen Abschnitte bzw. Daten der Datenstruktur zugreifen.

#### H�llen-Klassen f�r Mutexobjekte

##### Klasse `std::lock_guard`

Leichtgewichtige Realisierung (*lightweight implementation*) eines einfachen Sperrmechanismus.
Bietet exklusiven Besitz eines `std::mutex`-Objekts f�r einen begrenzten Zeitraum (&bdquo;Scope&rdquo;, Block, Bereich).

  * Ein `std::lock_guard`-Objekt umschlie�t ein `std::mutex`-Objekt.
  * Bei der Erstellung eines `std::lock_guard`-Objekts wird das Mutexobjekt automatisch gesperrt.
  * Bei der Zerst�rung (Verlassen des G�ltigkeitsbereichs, *Scope*) wird das Mutexobjekt automatisch entsperrt.
  * Ein manuelles Entsperren oder erneutes Sperren des Mutexobjekts wird nicht unterst�tzt.
  * Ein `std::lock_guard`-Objekt kann ein bereits gesperrtes Mutexobjekt &bdquo;�bernehmen&rdquo; (siehe in den Beispielen Parameter `std::adopt_lock`).
  * Ideal f�r einfache, kurzzeitige Sperren, die eine automatische Freigabe beim Verlassen des Blocks gew�hrleisten.


##### Klasse `std::unique_lock`

Die Klasse `std::unique_lock` bietet einen vielseitigen Sperrmechanismus
mit einer umfangreicheren Funktionalit�t als Klasse `std::lock_guard`.

  * Ein `std::unique_lock`-Objekt umschlie�t ein `std::mutex`-Objekt.
  * Im Gegensatz zu einem `std::lock_guard`-Objekt kann ein `std::unique_lock`-Objekt nach der Erstellung explizit gesperrt und entsperrt werden, es erm�glicht also sowohl manuelles Entsperren als auch erneutes Sperren.
  * Sie k�nnen immer ein `std::unique_lock`-Objekt anstelle eines `std::lock_guard`-Objekts verwenden, aber nicht umgekehrt.
  * Ein `std::unique_lock`-Objekt kann erzeugt werden, ohne dass zum Erzeugungszeitpunkt das Mutexobjekt gesperrt ist (siehe Gebrauch des Parameters `std::defer_lock`).
  * Es wird sowohl der exklusive Besitz (*exclusive ownership*) als auch der gemeinsame Besitz (*shared ownership*) eines Mutexobjekts unterst�tzt.
  * Unterst�tzt Verschiebe-Semantik.
  * Unterst�tzt zeitgesteuertes Sperren sowie Bedingungsvariablen (`std::condition_variable`).


Wir gehen noch auf die Klasse `std::condition_variable` ein:

Ein `std::unique_lock`-Objekt muss von der &bdquo;empfangenden&rdquo; Seite
(der Seite, die benachrichtigt wird) verwendet werden, um bei Gebrauch eines `std::condition_variable`-Objekts
entsprechende Benachrichtigungen empfangen zu k�nnen.

Der Grund, warum ein `std::unique_lock` f�r eine `std::condition_variable` erforderlich ist, besteht darin,
dass dieses das zugrunde liegende `std::mutex`-Objekt jedes Mal sperren kann,
wenn die Bedingungsvariable (`std::condition_variable`) nach einer g�ltigen Benachrichtigung
aus einer Wartephase aufwacht und einen kritischen Codeabschnitt ausf�hrt.

Das `std::unique_lock`-Objekt entsperrt das zugrunde liegende Mutexobjekt jedes Mal, wenn

  * der Aufruf der `wait`-Methode an der Bedingungsvariablen f�lschlicherweise aktiviert wurde, es muss also erneut gewartet werden.
  * bei automatischer Zerst�rung des `std::unique_lock`-Objekts. Dies ist der Fall, wenn der kritische Abschnitt ausgef�hrt und damit abgelaufen ist und der G�ltigkeitsbereich des `std::unique_lock`-Objekts verlassen wird.



##### Klasse `std::scoped_lock`

Die Klasse `std::scoped_lock` realisiert einen sehr einfachen Mechanismus f�r sperrbare Objekte,
�hnlich zur Klasse `std::lock_guard`, aber mit dem Unterschied, dass `std::scoped_lock`
f�r mehrere Mutexobjekte gleichzeitig verwendet werden kann!

  * Ein `std::scoped_lock`-Objekt umschlie�t ein oder mehrere Mutexobjekte, genauso wie ein `std::lock_guard`-Objekt,
    aber mit der Ausnahme, dass ein `std::lock_guard`-Objekt jeweils nur **ein** Mutexobjekt umschlie�en kann!
  * Bei der Erstellung sperrt ein `std::scoped_lock`-Objekt automatisch ein oder mehrere Mutexobjekte.
  * Bei der Zerst�rung (Verlassen des G�ltigkeitsbereichs) entsperrt ein `std::scoped_lock`-Objekt automatisch alle Mutexobjekte.

  

##### Klassen `std::shared_mutex` und `std::shared_lock`

In C++ kann man mit den beiden Klassen `std::shared_mutex` und `std::shared_lock` ein Synchronisationsprimitiv umsetzen,
das es mehreren Threads erm�glicht, eine gemeinsam genutzte Ressource gleichzeitig zum Lesen zu nutzen
und gleichzeitig exklusiven Schreibzugriff zu gew�hrleisten.

Dies ist in Situationen hilfreich, in denen viele Threads schreibgesch�tzten Zugriff
auf dieselbe Datenstruktur ben�tigen, Schreibvorg�nge jedoch nicht h�ufig verwendet werden.


  * Die Klasse `std::shared_lock` ist f�r den gemeinsamen Besitz (*shared ownership*) eines Mutexobjekts konzipiert und erm�glicht mehrere Leser.
  * Erm�glicht mehreren Threads den gleichzeitigen Erwerb der Sperre f�r den gemeinsamen lesenden Zugriff.
  * Gew�hrleistet Thread-Sicherheit, wenn mehrere Threads auf gemeinsam genutzte Ressourcen lesend zugreifen.
  * �hnlich wie `std::lock_guard` wird kein manuelles Entsperren oder erneutes Sperren unterst�tzt.

Eine genauere Beschreibung der beiden Klassen `std::shared_mutex` und `std::shared_lock` wird in dem Entwurfsmuster
[Reader-Writer Lock](../../Programs/23_ReaderWriterLock/Readme.md)
aufgezeigt.




*Zusammenfassung*:

Durch den Einsatz des richtigen Sperrmechanismus k�nnen Sie Thread-Sicherheit gew�hrleisten, *Data Races* verhindern
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

von Gabriel Staples �bernommen worden.

---

[Zur�ck](../../Readme.md)

---

