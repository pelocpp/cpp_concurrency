# Reader-Writer Lock<br />(Klassen `std::shared_mutex` und `std::shared_lock`)

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

  * Klasse `std::shared_mutex`

  * Klasse `std::unique_lock`
  * Klasse `std::shared_lock`

  * Klassen `std::jthread` und `std::stop_token`

---

### Allgemeines

Das *Reader-Writer Lock*&ndash;Entwurfsmuster dient der
Synchronisation mehrerer Threads auf eine gemeinsam genutzte Ressource.

Eine Besonderheit dieses Patterns besteht darin,
dass das Pattern entweder den Zugriff einem einzelnen Schreiber
oder mehreren Lesern ermöglicht, wodurch es auf diese Weise zu
einem besseren Durchsatz im Programmablauf kommt.

### Beteiligte Klassen und Objekte

Um das *Reader-Writer Lock*&ndash;Entwurfsmuster umzusetzen,
werden folgende Klassen und Objekte benötigt:

  * Leser und Schreiber: Instanz von `std::shared_mutex`, z.B.<br />
    `std::shared_mutex m_mutex;`
    
  * Schreiber: Instanz von `std::unique_lock`, z.B.<br />
    `std::unique_lock<std::shared_mutex> lock{ m_mutex };`

  * Leser: Instanz von `std::shared_lock`, z.B.<br />
    `std::shared_lock<std::shared_mutex> lock{ m_mutex };`


### Beispiel

Wir betrachten ein Beispiel, dass ein Feld mit Daten befüllt.
Der Befüller, also der Schreiber, benötigt exklusiven Zugriff auf das Feld.
Daneben gibt es zwei Leser, die gleichzeitig lesend auf den kritischen Bereich
(Feld) zugreifen.

Der kritische Bereich wird durch eine Klasse `Snapshots` gekapselt:

```cpp
01: struct Data
02: {
03:     int m_data;
04: };
05: 
06: template <size_t TSize = 64>
07: class Snapshots
08: {
09: private:
10:     std::array<Data, TSize>   m_buffer;
11:     size_t                    m_offset;
12:     mutable std::shared_mutex m_mutex;
13: 
14: public:
15:     Snapshots() : m_buffer{}, m_offset{} {}
16: 
17:     void push(const Data& data) {
18: 
19:         // we are about to modify the data, therefore we need a unique_lock
20:         std::unique_lock<std::shared_mutex> lock{ m_mutex };
21:         m_buffer[m_offset % TSize] = data;
22:         ++m_offset;
23:     }
24: 
25:     std::optional<Data> get(size_t index) const {
26: 
27:         // we only read, but need to prevent concurrent writes,
28:         // therefore we are using a shared_lock
29:         std::shared_lock<std::shared_mutex> lock{ m_mutex };
30: 
31:         if (index >= TSize) {
32:             std::string msg{ 
33:                 std::string{ "Wrong Index:" } +
34:                 std::to_string(index) +
35:                 std::string{ " !" }
36:             };
37:                 
38:             throw std::out_of_range{ msg };
39:         }
40: 
41:         if (m_buffer[index].m_data != 0) {
42:             return m_buffer[index];
43:         }
44:         else {
45:             return std::nullopt;
46:         }
47:     }
48: };
```

Die beiden Leser erhalten Zugriff auf ein Objekt des Typs `Snapshots`
und lesen gleichzeitig Daten aus dem Feld aus:

```cpp
01: static void test_reader_writer_lock_01()
02: {
03:     constexpr size_t Size{ 64 };
04: 
05:     Snapshots<Size> snapshots{};
06: 
07:     std::jthread thread { 
08:         [&] (std::stop_token stop) {
09:             int count{};
10:             while (!stop.stop_requested()) {
11:                 std::this_thread::sleep_for(100ms);
12:                 ++count;
13:                 Logger::log(std::cout, "Writer:    pushing ", count);
14:                 snapshots.push(Data{ count });
15:             }
16:         }
17:     };
18: 
19:     // run next while loop for two seconds
20:     std::chrono::system_clock::time_point deadline{
21:         std::chrono::system_clock::now() + 4s 
22:     };
23: 
24:     std::chrono::system_clock::duration pause{ 15ms };
25: 
26:     while (true) {
27: 
28:         if (std::chrono::system_clock::now() > deadline)
29:             break;
30: 
31:         // start two readers reading concurrently snapshots
32:         int counter1{};
33:         int counter2{};
34: 
35:         std::jthread reader1 { 
36:             [&] () {
37:                 for (size_t i{}; i != Size; i++) {
38:                     if (snapshots.get(i).has_value()) {
39:                         ++ counter1;
40:                     }
41:                 }
42:             } 
43:         };
44: 
45:         std::jthread reader2{
46:             [&] () {
47:                 for (size_t i{}; i != Size; i++) {
48:                     if (snapshots.get(i).has_value()) {
49:                         ++counter2;
50:                     }
51:                 }
52:             }
53:         };
54: 
55:         reader1.join();
56:         reader2.join();
57: 
58:         Logger::log(std::cout, "1. Reader: ", counter1, " snapshots");
59:         Logger::log(std::cout, "2. Reader: ", counter2, " snapshots");
60: 
61:         std::this_thread::sleep_for(pause);
62:     }
63: 
64:     // stop the writer thread
65:     thread.request_stop();
66: }
```

Wir erkennen, dass der schreibende und lesende Zugriff
korrekt ausgeführt wird:

*Ausgabe*:

```
[1]:    1. Reader: 0 snapshots
[1]:    2. Reader: 0 snapshots
[1]:    1. Reader: 0 snapshots
[1]:    2. Reader: 0 snapshots
[1]:    1. Reader: 0 snapshots
[1]:    2. Reader: 0 snapshots
[1]:    1. Reader: 0 snapshots
[1]:    2. Reader: 0 snapshots
[1]:    1. Reader: 0 snapshots
[1]:    2. Reader: 0 snapshots
[1]:    1. Reader: 0 snapshots
[1]:    2. Reader: 0 snapshots
[2]:    Writer:    pushing 1
[1]:    1. Reader: 1 snapshots
[1]:    2. Reader: 1 snapshots
[1]:    1. Reader: 1 snapshots
[1]:    2. Reader: 1 snapshots
[1]:    1. Reader: 1 snapshots
[1]:    2. Reader: 1 snapshots
[1]:    1. Reader: 1 snapshots
[1]:    2. Reader: 1 snapshots
[1]:    1. Reader: 1 snapshots
[1]:    2. Reader: 1 snapshots
[2]:    Writer:    pushing 2
[1]:    1. Reader: 2 snapshots
[1]:    2. Reader: 2 snapshots
[1]:    1. Reader: 2 snapshots
[1]:    2. Reader: 2 snapshots
[1]:    1. Reader: 2 snapshots
[1]:    2. Reader: 2 snapshots
[1]:    1. Reader: 2 snapshots
[1]:    2. Reader: 2 snapshots
[2]:    Writer:    pushing 3
[1]:    1. Reader: 3 snapshots
[1]:    2. Reader: 3 snapshots
[1]:    1. Reader: 3 snapshots

...

[2]:    Writer:    pushing 33
[1]:    1. Reader: 33 snapshots
[1]:    2. Reader: 33 snapshots
[1]:    1. Reader: 33 snapshots
[1]:    2. Reader: 33 snapshots
[1]:    1. Reader: 33 snapshots
[1]:    2. Reader: 33 snapshots
[2]:    Writer:    pushing 34
[1]:    1. Reader: 34 snapshots
[1]:    2. Reader: 34 snapshots
[1]:    1. Reader: 34 snapshots
[1]:    2. Reader: 34 snapshots
[1]:    1. Reader: 34 snapshots
[1]:    2. Reader: 34 snapshots
[1]:    1. Reader: 34 snapshots
[1]:    2. Reader: 34 snapshots
[2]:    Writer:    pushing 35
[1]:    1. Reader: 35 snapshots
[1]:    2. Reader: 35 snapshots
[1]:    1. Reader: 35 snapshots
[1]:    2. Reader: 35 snapshots
[1]:    1. Reader: 35 snapshots
[1]:    2. Reader: 35 snapshots
[2]:    Writer:    pushing 36
[1]:    1. Reader: 36 snapshots
[1]:    2. Reader: 36 snapshots
[2]:    Writer:    pushing 37
```
---

#### Quellcode

[*ReaderWriterLock.cpp*](ReaderWriterLock.cpp).

---

## Literaturhinweise

Die Anregungen zu diesen Erläuterungen stammen im Wesentlichen  aus dem Aufsatz

[Daily bit(e) of C++ | std::shared_mutex](https://medium.com/@simontoth/daily-bit-e-of-c-std-shared-mutex-ebe7477a7589) von Šimon Tóth.

---

[Zurück](../../Readme.md)

---

