# Mutexe und Locking Mechanismen

[Zur�ck](../../Readme.md)

---

## Verwendete Hilfsmittel:

<ins>Mutex-Klassen</ins>:

  * Klasse `std::mutex`
  * Klasse `std::recursive_mutex`
  * Klasse `std::shared_mutex`
 
<ins>H�llen-Klassen f�r Mutexobjekte</ins>:

  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::scoped_lock`
  * Klasse `std::shared_lock`

<ins>Sperrstrategien (*Locking Strategies*)</ins>:

  * Klasse `std::defer_lock`
  * Klasse `std::std::adopt_lock`

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

##### Klasse `std::shared_mutex`

In C++ kann man mit den beiden Klassen `std::shared_mutex` und `std::shared_lock` ein Synchronisationsprimitiv umsetzen,
das es mehreren Threads erm�glicht, eine gemeinsam genutzte Ressource gleichzeitig zum Lesen zu nutzen
und gleichzeitig exklusiven Schreibzugriff zu gew�hrleisten.
Weitere Informationen siehe [Klasse `std::shared_lock`].

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


##### Klasse `std::shared_lock`

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

#### Sperrstrategien (*Locking Strategies*)

In manchen Situationen muss ein Thread zwei Sperren gleichzeitig halten
und diese nach dem Zugriff auf die gemeinsam genutzten Daten freigeben.
Wenn ein Thread mehr als eine Sperre hat, besteht die Gefahr eines Deadlocks.
Um dies zu vermeiden, gibt es mehrere Strategien in C++:


##### Strategie `std::adopt_lock`

Die Strategie `std::adopt_lock` geht davon aus, dass der aufrufende Thread die Sperre bereits besitzt.
Ein Mutex-H�llenobjekt sollte den Besitz des Mutex �bernehmen und ihn freigeben,
wenn die Kontrolle den G�ltigkeitsbereich verl�sst.

```cpp
01: std::lock(g_mutex1, g_mutex2);
02: 
03: std::lock_guard<std::mutex> lock1(g_mutex1, std::adopt_lock);
04: std::lock_guard<std::mutex> lock2(g_mutex2, std::adopt_lock);
```

##### Strategie `std::defer_lock`

Die Strategie `std::defer_lock` erwirbt *nicht* den Besitz des Mutex und geht davon aus,
dass der aufrufende Thread `lock` aufrufen wird,
um den Mutex zu erwerben.
Das Mutex-H�llenobjekt gibt die Sperre frei, wenn die Kontrolle den G�ltigkeitsbereich verl�sst.

```cpp
01: std::unique_lock<std::mutex> lock1(g_mutex1, std::defer_lock);
02: std::unique_lock<std::mutex> lock2(g_mutex2, std::defer_lock);
03: 
04: std::lock(lock1, lock2);
```

##### Unterschiede im Gebrauch von `std::lock_guard` und `std::unique_lock`

  * `std::lock_guard` mit der Strategie `std::adopt_lock` geht davon aus, dass der Mutex bereits erworben wurde.
  * `std::unique_lock` mit der `std::defer_lock`-Strategie geht davon aus, dass der Mutex nicht bei der Konstruktion erfasst wird, sondern explizit gesperrt wird.

---

## Weitere Beispiele

##### Beispiel zur Klasse `std::recursive_mutex`


Betrachten Sie das folgende Beispiel genau:

  * Welche Mutex-Klassen kommen zum Einsatz?
  * Studieren Sie die beiden Methoden `reserve` und `allocate`: Wann kommt es zum Gebrauch eines Mutex-H�llenobjekts und wann nicht?
  * Begr�nden Sie Ihre Beobachtungen.


```cpp
01: constexpr size_t BucketSize = 4;
02: 
03: class NonRecursive
04: {
05: private:
06:     std::mutex m_mutex;
07:     std::unique_ptr<int[]> m_data;
08:     size_t m_size;
09:     size_t m_capacity;
10: 
11: public:
12:     NonRecursive() : m_size{}, m_capacity{} {}
13: 
14:     void push_back(int value) {
15: 
16:         std::unique_lock lock{ m_mutex };
17: 
18:         if (m_size == m_capacity) {
19:             allocate(m_capacity == 0 ? BucketSize : m_capacity * 2);
20:         }
21: 
22:         m_data[m_size++] = value;
23:     }
24: 
25:     void reserve(size_t capacity) {
26: 
27:         std::unique_lock lock{ m_mutex };
28:         allocate(capacity);
29:     }
30: 
31: private:
32:     void allocate(size_t capacity) {
33: 
34:         std::unique_ptr<int[]> data{ std::make_unique<int[]>(capacity) };
35:         size_t newSize{ std::min(m_size, capacity) };
36: 
37:         std::copy(
38:             m_data.get(),
39:             m_data.get() + newSize,
40:             data.get()
41:         );
42: 
43:         m_data = std::move(data);
44:         m_capacity = capacity;
45:         m_size = newSize;
46:     }
47: };
48: 
49: class Recursive
50: {
51: private:
52:     std::recursive_mutex m_mutex;
53:     std::unique_ptr<int[]> m_data;
54:     size_t m_size;
55:     size_t m_capacity;
56: 
57: public:
58:     Recursive() : m_size{}, m_capacity{} {}
59: 
60:     void push_back(int value) {
61: 
62:         std::unique_lock lock{ m_mutex };
63: 
64:         if (m_size == m_capacity) {
65:             reserve(m_capacity == 0 ? BucketSize : m_capacity * 2);
66:         }
67: 
68:         m_data[m_size++] = value;
69:     }
70: 
71:     void reserve(size_t capacity) {
72: 
73:         std::unique_lock lock{ m_mutex };
74: 
75:         std::unique_ptr<int[]> data{ std::make_unique<int[]>(capacity) };
76:         size_t newSize{ std::min(m_size, capacity) };
77: 
78:         std::copy(
79:             m_data.get(),
80:             m_data.get() + newSize,
81:             data.get()
82:         );
83: 
84:         m_data = std::move(data);
85:         m_capacity = capacity;
86:         m_size = newSize;
87:     }
88: };
```

Folgende wichtige Passagen in dem Beispiel sind anzusprechen:

  * Klasse `NonRecursive` verwendet ein Mutexobjekt des Typs `std::mutex`.

  * In der `push_back`-Methode wird bei Bedarf die Methode `allocate` aufgerufen.
    Diese darf *keine* erneute Sperre des `std::mutex`-Objekts ausl�sen, da es sonst zu einem Absturz kommt.
    Aus diesem Grund ist die `allocate`-Methode privat deklariert, damit sie nicht von au�en aus aufgerufen werden kann!

  * Die Methode `reserve` ist �ffentlich zug�nglich &ndash; und ihre Ausf�hrung ist durch ein Mutexobjekt gesch�tzt.
    Auch sie ruft intern Methode `allocate` auf &ndash; mit derselben Beobachtung, dass die komplette Ausf�hrung
    von `allocate` f�r den �konkurrierenden Zugriff gesch�tzt ist.

  * Klasse `Recursive` verwendet ein Mutexobjekts des Typs `std::recursive`.

  * Desweiteren besitzt diese Klasse zwei �ffentliche Methoden `push_back` und `reserve`.
    Beide Methoden sperren das Mutexobjekt.

  * *Achtung*: In Methode `push_back` kommt es (bei Bedarf) zu einem Aufruf der `reserve` Methode.
    Dies f�hrt *nicht* zu einem Absturz, da das Mutexobjekt vom Typ `std::recursive` ist!
    Eine separate Methode `allocate` ist in dieser Realisierung �berfl�ssig!


---

##### Quellcode:

[Examples.cpp](Examples.cpp).
[Examples_RecursiveMutex.cpp](Examples_RecursiveMutex.cpp).

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Aufsatz

[Understanding Locking Mechanisms in C++](https://medium.com/@elysiumceleste/understanding-locking-mechanisms-in-c-std-lock-guard-std-unique-lock-and-std-shared-lock-a8aac4d575ce)

von Elysium Celeste.

Zum Zweiten sind auch viele Informationen aus dem Artikel

[C++ Mutexes, Concurrency, and Locks](https://gabrielstaples.com/cpp-mutexes-and-locks/)

von Gabriel Staples �bernommen worden.

Das Beispiel zu rekursiven Mutex-Objekten habe ich in einem Aufsatz von Simon Toth gefunden:

[Daily bit(e) of C++: `std::recursive_mutex`](https://medium.com/@simontoth/daily-bit-e-of-c-std-recursive-mutex-dd9b84f38f8d)

Die Beschreibung der Sperrstrategien lehnt sich an

[C++ 11 Locking Strategy: `adopt_lock` and `defer_lock`](https://medium.com/@back_to_basics/c-11-locking-strategy-adopt-lock-and-defer-lock-eeedf76a2689)

an.


---

[Zur�ck](../../Readme.md)

---

