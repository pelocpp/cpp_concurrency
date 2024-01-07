# Mutexe und Sperrmechanismen

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Mutex-Klassen</ins>:

  * Klasse `std::mutex`
  * Klasse `std::recursive_mutex`
  * Klasse `std::shared_mutex`
 
<ins>Hüllen-Klassen für Mutexobjekte</ins>:

  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`
  * Klasse `std::scoped_lock`
  * Klasse `std::shared_lock`

<ins>Sperrstrategien (*Locking Strategies*)</ins>:

  * Klasse `std::defer_lock`
  * Klasse `std::std::adopt_lock`

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
öffentlichen Schnittstelle von unterschiedlichen Ebenen aus aufrufen,
um auf die kritischen Abschnitte bzw. Daten der Datenstruktur zuzugreifen.

##### Klasse `std::shared_mutex`

In C++ kann man mit den beiden Klassen `std::shared_mutex` und `std::shared_lock` ein Synchronisationsprimitiv umsetzen,
das es mehreren Threads ermöglicht, eine gemeinsam genutzte Ressource gleichzeitig zum Lesen zu nutzen
und gleichzeitig exklusiven Schreibzugriff zu gewährleisten.
Weitere Informationen siehe [hier](#Klasse-std::shared_lock).

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


##### Klasse std::shared_lock

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

#### Sperrstrategien (*Locking Strategies*)

In manchen Situationen muss ein Thread zwei Sperren gleichzeitig halten
und diese nach dem Zugriff auf die gemeinsam genutzten Daten freigeben.
Wenn ein Thread mehr als eine Sperre hat, besteht die Gefahr eines Deadlocks.
Um dies zu vermeiden, gibt es mehrere Strategien in C++:


##### Strategie `std::adopt_lock`

Die Strategie `std::adopt_lock` geht davon aus, dass der aufrufende Thread die Sperre bereits besitzt.
Ein Mutex-Hüllenobjekt sollte den Besitz des Mutex übernehmen und ihn freigeben,
wenn die Kontrolle den Gültigkeitsbereich verlässt.

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
Das Mutex-Hüllenobjekt gibt die Sperre frei, wenn die Kontrolle den Gültigkeitsbereich verlässt.

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

##### Beispiel zur Klasse `std::scoped_lock`

Wir betrachten in diesem Beispiel ein Würfelspiel. Es sind 10 Spieler beteiligt.
Jeder Spieler würfelt gegen alle anderen Spieler, und dies erfolgt pro Spieler in einem separaten Thread.

Um das Aktualisieren der Punktezahlen akkurat durchzuführen, werden bei einem Würfelspiel zweier Spieler
die Datenbereich *beider* Spieler gesperrt. 

Studieren Sie den Quellcode des Beispiels genau.

  * Wieviele Mutexobjekte werden in Methode `playWith` gesperrt?
  * Wann werden diese Mutexobjekte freigegeben?

```cpp
001: class Player
002: {
003: private:
004:     std::string_view m_name;
005:     std::mutex m_mutex;
006:     std::default_random_engine m_random_engine;
007:     std::uniform_int_distribution<int> m_dist;
008:     int m_score;
009: 
010: public:
011:     // c'tor
012:     Player(std::string_view name, int seed)
013:         : m_name(std::move(name)), m_random_engine(seed), m_dist(1, 6), m_score{}
014:     {}
015: 
016:     // getter
017:     std::string_view name() const { return m_name; }
018: 
019:     int getScore() const { return m_score; }
020: 
021:     void incrementScore(int points) {
022: 
023:         m_score += points;
024:         Logger::log(std::cout, name(), " got point => ", m_score);
025:     }
026: 
027:     // public interface
028:     void playWith(Player& other) {
029: 
030:         if (&other == this)
031:             return;
032: 
033:         // retrieve our lock and the lock of the oponent
034:         std::scoped_lock lock{ m_mutex, other.m_mutex };
035: 
036:         Logger::log(std::cout, name(), " plays against ", other.name());
037: 
038:         // roll a dice until one player wins,
039:         // then increase the score of the winner
040:         int points{};
041:         int otherPoints{};
042: 
043:         while (points == otherPoints)
044:         {
045:             points = roll();
046:             otherPoints = other.roll();
047:         }
048: 
049:         if (points > otherPoints) {
050:             incrementScore(points);
051:         }
052:         else {
053:             other.incrementScore(otherPoints);
054:         }
055:     }
056: 
057: private:
058:     // roll a six-sided dice
059:     int roll() { return m_dist(m_random_engine); }
060: };
061: 
062: void examples_scoped_lock()
063: {
064:     std::random_device device;
065: 
066:     std::vector<std::unique_ptr<Player>> players;
067: 
068:     std::initializer_list<std::string_view> names =
069:     {
070:         "Player1", "Player2", "Player3", "Player4", "Player5",
071:         "Player6", "Player7", "Player8", "Player9"
072:     };
073: 
074:     // generate players from the names using transform algorithm
075:     std::transform(
076:         std::begin(names),
077:         std::end(names),
078:         std::back_inserter(players),
079:         [&] (std::string_view name) {
080:             return std::make_unique<Player>(name, device());
081:         }
082:     );
083: 
084:     std::vector<std::jthread> rounds;
085: 
086:     // run the game: each player plays
087:     // against all other players in parallel
088:     for (auto& player : players) {
089:         
090:         auto round = [&] () {
091:             for (auto& oponent : players) {
092:                 player->playWith(*oponent);
093:             }
094:         };
095:         
096:         rounds.push_back(std::jthread { std::move(round) });
097:     }
098: 
099:     // join all the threads
100:     std::for_each(
101:         rounds.begin(),
102:         rounds.end(),
103:         std::mem_fn(&std::jthread::join)
104:     );
105: 
106:     // sort
107:     std::sort(
108:         players.begin(),
109:         players.end(),
110:         [] (const auto& elem1, const auto& elem2) {
111:             return elem1->getScore() > elem2->getScore();
112:         }
113:     );
114: 
115:     // print
116:     std::cout << std::endl << "Final score:" << std::endl;
117:     for (const auto& player : players) {
118:         std::cout 
119:             << player->name() << ":\t" << player->getScore()
120:             << " points." << std::endl;
121:     }
122: }
```

*Ausgabe*:

```
[1]:    Player1 plays against Player2
[1]:    Player2 got point => 3
[1]:    Player1 plays against Player3
[1]:    Player3 got point => 5
[1]:    Player1 plays against Player4
[1]:    Player1 got point => 5
[1]:    Player1 plays against Player5
[1]:    Player5 got point => 3
[1]:    Player1 plays against Player6
[1]:    Player6 got point => 2
[1]:    Player1 plays against Player7
[1]:    Player1 got point => 11
[1]:    Player1 plays against Player8
[1]:    Player1 got point => 16
[1]:    Player1 plays against Player9
[1]:    Player1 got point => 21
[2]:    Player5 plays against Player1
[2]:    Player5 got point => 6
[2]:    Player5 plays against Player2
[3]:    Player3 plays against Player1
[2]:    Player2 got point => 5
[3]:    Player3 got point => 8
[3]:    Player3 plays against Player2
[4]:    Player7 plays against Player1
[3]:    Player3 got point => 14
[4]:    Player7 got point => 5
......

[9]:    Player9 got point => 38
[7]:    Player9 plays against Player7
[7]:    Player9 got point => 42
[6]:    Player4 plays against Player8
[6]:    Player4 got point => 36
[6]:    Player4 plays against Player9
[6]:    Player9 got point => 47
[7]:    Player9 plays against Player8
[7]:    Player8 got point => 46

Final score:
Player3:        50 points.
Player9:        47 points.
Player8:        46 points.
Player1:        38 points.
Player4:        36 points.
Player6:        35 points.
Player7:        33 points.
Player5:        31 points.
Player2:        19 points.
```

##### Beispiel zur Klasse `std::recursive_mutex`

Betrachten Sie das folgende Beispiel genau:

  * Welche Mutex-Klassen kommen zum Einsatz?
  * Studieren Sie die beiden Methoden `reserve` und `allocate`: Wann kommt es zum Gebrauch eines Mutex-Hüllenobjekts und wann nicht?
  * Begründen Sie Ihre Beobachtungen.


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
    Diese darf *keine* erneute Sperre des `std::mutex`-Objekts auslösen, da es sonst zu einem Absturz kommt.
    Aus diesem Grund ist die `allocate`-Methode privat deklariert, damit sie nicht von außen aus aufgerufen werden kann!

  * Die Methode `reserve` ist öffentlich zugänglich &ndash; und ihre Ausführung ist durch ein Mutexobjekt geschützt.
    Auch sie ruft intern Methode `allocate` auf &ndash; mit derselben Beobachtung, dass die komplette Ausführung
    von `allocate` für den ´konkurrierenden Zugriff geschützt ist.

  * Klasse `Recursive` verwendet ein Mutexobjekts des Typs `std::recursive`.

  * Desweiteren besitzt diese Klasse zwei öffentliche Methoden `push_back` und `reserve`.
    Beide Methoden sperren das Mutexobjekt.

  * *Achtung*: In Methode `push_back` kommt es (bei Bedarf) zu einem Aufruf der `reserve` Methode.
    Dies führt *nicht* zu einem Absturz, da das Mutexobjekt vom Typ `std::recursive` ist!
    Eine separate Methode `allocate` ist in dieser Realisierung überflüssig!


---

##### Quellcode:

[Examples.cpp](Examples.cpp).<br />
[Examples_LockingStrategies.cpp](Examples_LockingStrategies.cpp).<br />
[Examples_RecursiveMutex.cpp](Examples_RecursiveMutex.cpp).<br />
[Examples_ScopedLock.cpp](Examples_ScopedLock.cpp).<br />

---

## Literaturhinweise

Die Anregungen zur Klasse stammen im Wesentlichen aus dem Aufsatz

[Understanding Locking Mechanisms in C++](https://medium.com/@elysiumceleste/understanding-locking-mechanisms-in-c-std-lock-guard-std-unique-lock-and-std-shared-lock-a8aac4d575ce)

von Elysium Celeste.

Zum Zweiten sind auch viele Informationen aus dem Artikel

[C++ Mutexes, Concurrency, and Locks](https://gabrielstaples.com/cpp-mutexes-and-locks/)

von Gabriel Staples übernommen worden.

Das Beispiel zu rekursiven Mutex-Objekten habe ich in einem Aufsatz von Simon Tóth gefunden:

[Daily bit(e) of C++: `std::recursive_mutex`](https://medium.com/@simontoth/daily-bit-e-of-c-std-recursive-mutex-dd9b84f38f8d)

Die Beschreibung der Sperrstrategien lehnt sich an

[C++ 11 Locking Strategy: `adopt_lock` and `defer_lock`](https://medium.com/@back_to_basics/c-11-locking-strategy-adopt-lock-and-defer-lock-eeedf76a2689)

an.

Das Würfelbeispiel (Klasse `scoped_lock`) stammt wiederum von Simon Tóth und kann
[hier](https://medium.com/@simontoth/daily-bit-e-of-c-std-scoped-lock-9cab4142f9d4) nachgelesen werden.

---

[Zurück](../../Readme.md)

---

