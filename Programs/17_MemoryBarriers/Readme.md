# Speicherbarrieren (Memory Barriers)

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Speicherbarierre *Relaxed* - `std::memory_order_relaxed`](#link3)
  * [Behebung des Problems: Speicherbarierre *Acquire/Release* - `std::memory_order_acquire`/`std::memory_order_release`](#lin4k)
  * [Speicherbarierre *Relaxed*: Real-World Beispiel](#link5)
  * [Speicherbarierre *Acquire/Release*: Real-World Beispiel](#link6)
  * [Beispiel aus Buch von Anthony Williams](#link7)
  * [Literaturhinweise](#link8)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::thread`
  * Klasse `std::atomic<>`

<ins>Konstanten</ins>:

 * `std::memory_order_relaxed`
 * `std::memory_order_consume`
 * `std::memory_order_acquire`
 * `std::memory_order_release`
 * `std::memory_order_acq_rel`
 * `std::memory_order_seq_cst`

---

#### Quellcode

[*MemoryBarriers.cpp*](./MemoryBarriers.cpp).<br />

---

## Allgemeines <a name="link2"></a>

Was sind Speicherbarrieren?

Eine Speicherbarriere ist eine Anweisung an den Compiler oder die CPU, die sicherstellt,
dass alle Anweisungen vor der Barriereanweisung vor und alle Anweisungen nach der Barriereanweisung danach ausgeführt werden.

Warum benötigen wir Speicherbarrieren? Moderne Compiler und CPUs können Anweisungen zur Optimierung in beliebiger Reihenfolge ausführen,
solange dies die eigentliche Programmausführung nicht beeinträchtigt.
Dies kann jedoch in Multithreading-Programmen zu *Race Conditions* oder *Unexpected Behavior* führen.

Um dieses unerwartete Verhalten zu verhindern, werden Speicherbarrieren eingesetzt.

---

## Speicherbarierre *Relaxed* - `std::memory_order_relaxed` <a name="link3"></a>

Was versteht man unter der Speicherbarierre `std::memory_order_relaxed`?

  * Die Speicherbarierre `std::memory_order_relaxed` unterstützt den atomaren Zugriff auf die korrespondierende Variable.
  * Es ist aber nicht festgelegt, wann die anderen Anweisungen relativ zur Anweisung des atomaren Zugriffs dazu geschehen.
    In Bezug auf diese Anweisungen liegt keine Synchronisierung vor.


Schauen wir uns ein klassisches, fehlerhaftes Beispiel an und beheben wir es dann in einer zweiten Betrachtung
mit einer weiteren *Acquire/Release* Speicherbarierre.
Wir betrachten zu diesem Zweck ein vereinfachtes Produzenten/Konsumenten-Problem:

*Thread* 1 / Produzent:<br />

```cpp
01: std::size_t       g_data{};
02: std::atomic<bool> g_ready{ false };
03: 
04: void producer()
05: {
06:     g_data = 123;                                        // (1) write data
07:     g_ready.store(true, std::memory_order_relaxed);      // (2) publish flag
08: }
```

*Thread* 2 / Konsument:<br />

```cpp
01: void consumer()
02: {
03:     if (g_ready.load(std::memory_order_relaxed)) {       // (3) consume flag
04:         std::println("[{}] Data: {}", tid, g_data);      // (4) read data
05:     }
06: }
```

Welche Ausgabe erwarten wir?

Wenn der Konsument erkennt, dass `g_ready` &bdquo;wahr&rdquo; ist, dann sicherlich

```
Data: 123
```

Was kann aber auch tatsächlich passieren? Eine Ausgabe 

```
Data: 0
```

Warum kann das passieren? Weil keine Reihenfolgegarantie besteht zwischen den beiden Anweisungen

  * (1) `g_data = 123;` und
  * (2) `g_ready.store(...);`

Mit der Speicherbarierre `std::memory_order_relaxed` darf sich das System so verhalten,
als ob die Ausführung (aus Sicht anderer Threads) neu geordnet wäre, zum Beispiel auch so:

```cpp
g_ready.store(true, std::memory_order_relaxed);          // happens "first"
g_data = 123;                                            // happens "later"
```

Der Verbraucher kann also Folgendes beobachten:

  * `g_ready == true`
  * er sieht aber weiterhin den alten Wert `g_data == 0`


## Behebung des Problems: Speicherbarierre *Acquire/Release* - `std::memory_order_acquire`/`std::memory_order_release` <a name="link4"></a>

Nun fügen wir die Synchronisation hinzu:

*Thread* 1 / Produzent:<br />

```cpp
01: std::size_t       g_data{};
02: std::atomic<bool> g_ready{ false };
03: 
04: void producer()
05: {
06:     g_data = 123;                                        // (1) write data
07:     g_ready.store(true, std::memory_order_release);      // (2) 'release'
08: }
```

*Thread* 2 / Konsument:<br />

```cpp
01: void consumer()
02: {
03:     if (g_ready.load(std::memory_order_acquire)) {       // (3) 'acquire'
04:         std::println("[{}] Data: {}", tid, g_data);      // (4) read data
05:     }
06: }
```

Was hat sich geändert? Mit Hilfe der Speicherbarierre *Acquire/Release*
können wir eine &bdquo;Vorher&rdquo;-Beziehung erschaffen.

  * &bdquo;Freigeben&rdquo; stellt sicher, dass alle vorherigen Schreibvorgänge (z. B. `g_data = 123`) sichtbar werden.
  * &bdquo;Erwerben&rdquo; stellt sicher, dass alle nachfolgenden Lesevorgänge diese Schreibvorgänge sehen.


Mit der *Acquire/Release*-Speicherbarierre haben wir so etwas wie eine &bdquo;unsichtbare Barriere&rdquo; geschaffen.
Stellen Sie sich es so vor:

Mit `release`:

> &bdquo;Alles, was davor passiert, muss sichtbar sein, bevor das Flag sichtbar wird.&rdquo;

Mit `acquire`:

> &bdquo;Sobald ich das Flag sehe, muss ich auch alles sehen, was davor passiert ist.&rdquo;

Zusammenfassung:<br />
  * `std::memory_order_relaxed` &RightArrow;<br />Nur atomarer Zugriff, keine Reihenfolge, keine Synchronisierung. Hohe Performanz, keine Einschränkungen in der Reihenfolge der Hardware-Befehle (Optimizer).
  * `std::memory_order_release` und `std::memory_order_acquire`: &RightArrow;<br />Legt eine threadübergreifende Reihenfolge fest (&bdquo;*Cross-Thread Ordering*&rdquo;).


## Speicherbarierre *Relaxed*: Real-World Beispiel <a name="link5"></a>

*Beispiel*:

Ein klassisches Beispiel für die Speicherbarierre `std::memory_order_relaxed` ist ein globaler Statistik-Zähler
(z. B. ein *Request*-Counter in einem Webserver).

Das Szenario könnte ein Thread Pool mit 100 verfügbaren Threads sein,
die gleichzeitig eingehenden Anfragen sind (quasi)-parallel zu verarbeiten.
Die Aufgabe besteht nun darin, einfach nur zählen, wie viele Anfragen insgesamt reingekommen sind.

```cpp
01: std::atomic<int> requestCount{};
02: constexpr std::size_t NumRequests{ 10'000 };
03: 
04: void handleRequest()
05: {
06:     // get the job done...
07:     requestCount.fetch_add(1, std::memory_order_relaxed);
08: }
09: 
10: void test()
11: {
12:     std::vector<std::thread> threads;
13:     threads.reserve(NumRequests);
14: 
15:     for (std::size_t i{}; i != NumRequests; ++i) {
16:         threads.emplace_back(handleRequest);
17:     }
18:         
19:     for (auto& t : threads) { 
20:         t.join(); 
21:     }
22: 
23:     Logger::log(std::cout, "Total requests: ", requestCount.load(std::memory_order_relaxed));
24: }
```

*Ausgabe*:

```
Total requests: 10000
```


Eigenschaften des vorgestellten Beispiels:

**Performance**:<br />
Auf Architekturen wie ARM oder PowerPC erzwingen strengere Reihenfolgen (wie `std::memory_order_seq_cst`)
teure &bdquo;Memory Barriers&rdquo; (Hardware-Befehle, die den CPU-Pipeline-Fluss stoppen, um Cache-Konsistenz zu erzwingen).
`std::memory_order_relaxed` lässt die CPU einfach mit voller Geschwindigkeit weiterlaufen.

**Keine Logik-Abhängigkeit**:<br />
Der Zähler ist völlig unabhängig von anderen Daten. Wenn Thread A den Zähler erhöht, muss Thread B nicht sofort wissen,
welche Daten Thread A vorher geschrieben hat. Es zählt nur das Endergebnis.

**Garantie**:<br />
Trotz der Speicherbarierre `std::memory_order_relaxed` ist die Operation atomar.
Es geht niemals ein Inkrement verloren. Es finden keine *Race Conditions* beim Schreiben statt,
was bei Verwendung einer normalen `int`-Variablen mit einer `++`-Operation der Fall wäre.

## Speicherbarierre *Acquire/Release*: Real-World Beispiel <a name="link6"></a>

Man kann sich das Prinzip von *Acquire/Release* wie eine Stafettenübergabe beim Stafffellaufen vorstellen:
Es geht nicht nur darum, dass der Stab (die atomare Variable) ankommt,
sondern dass auch alle anderen Informationen (die restlichen Daten) sicher mit übergeben werden.

Hier ist das Standard-Beispiel: Ein Daten-Producer und ein Daten-Consumer.

```cpp
01: std::atomic<bool> g_ready{ false };
02: std::string       g_data{ "<empty>" }; // normal, non-atomare variable
03: 
04: void producer()
05: {
06:     std::thread::id tid{ std::this_thread::get_id() };
07: 
08:     Logger::log(std::cout, "Producer: data = [", g_data, "]");
09: 
10:     std::this_thread::sleep_for(std::chrono::seconds{ 3 });
11: 
12:     Logger::log(std::cout, "Producer: writing data now ...");
13: 
14:     // (1) Write at first data, non synchronized
15:     g_data = "<secret password>";
16: 
17:     // (2) Everything I did before this point,
18:     // must be visible to anyone reading 'g_ready' with ACQUIRE
19:     g_ready.store(true, std::memory_order_release);
20: 
21:     Logger::log(std::cout, "Producer: Done.");
22: }
23: 
24: void consumer()
25: {
26:     std::thread::id tid{ std::this_thread::get_id() };
27: 
28:     Logger::log(std::cout, "Consumer: data = [", g_data, "]");
29: 
30:     // (3) ACQUIRE: I wait until 'ready' is true. Once that happens,
31:     // I guarantee that I will also see all previous write accesses (such as 'data').
32:     while (!g_ready.load(std::memory_order_acquire))
33:         ;
34: 
35:     // (4) Secure access: data is guaranteed "Secret password"
36:     Logger::log(std::cout, "Consumer: received data [", g_data, "]");
37: }
```

*Ausgabe*:

```
[1]:    Producer: data = [<empty>]
[2]:    Consumer: data = [<empty>]
[1]:    Producer: writing data now ...
[2]:    Consumer: received data [<secret password>]
[1]:    Producer: Done.
```


Warum reicht die Speicherbarierre *Relaxed* hier nicht?

Würde man hier `std::memory_order_relaxed` verwenden, könnte folgendes passieren:

  * CPU-Optimierung:<br />Die CPU oder der Compiler könnten die Zeilen im Producer vertauschen (zuerst `ready = true`, dann `data = ...`).

  * Sichtbarkeit:<br />Der Consumer-Thread sieht zwar, dass `ready` auf `true` springt,
    aber sein lokaler CPU-Cache hat vielleicht noch den alten, leeren Wert von `data`.

## Beispiel aus Buch von Anthony Williams <a name="link7"></a>

Im Buch &bdquo;C++ Concurrency in Action&rdquo; von Anthony Williams sind in Kapitel 5.4
einige weitere, vertiefende Beispiele zu dieser Thematik:

```cpp
01: void write_x()
02: {
03:     x.store(true, order);
04: }
05: 
06: void write_y()
07: {
08:     y.store(true, order);
09: }
10: 
11: void read_x_then_y()
12: {
13:     while (!x.load(order));
14:     if (y.load(order))
15:         ++z;
16: }
17: 
18: void read_y_then_x()
19: {
20:     while (!y.load(order));
21:     if (x.load(order))
22:         ++z;
23: }
```

Welchen Wert hat `z` nach Ausführung aller vier Funktion im Kontext von vier Threads?

  * Die beiden Funktionen `read_x_then_y` und `read_y_then_x` blockieren zunächst, da `x` und `y` mit dem 0 vorbelegt sind.
  * Irgendwann gelangt eine der beiden Funktionen `write_x` oder `write_y` zur Ausführung, sagen wir `write_x`.
  * `x` wird auf den Wert `true` gesetzt.
  * Nun endet die `while`-Endlosschleife in Funktion `read_x_then_y`. 
  * Da `write_y` noch nicht ausgeführt worden sein muss, bleibt der Wert von `z` auf 0 stehen.
  * Für diesen Fall bleibt die `while`-Endlosschleife in Funktion `read_y_then_x` immer noch aktiv, da `y` ja den Wert 0 hat. 
  * Irgendwann gelangt die Funktione `write_y` zur Ausführung.
  * Nur wird `y` auf den Wert `true` gesetzt.
  * Damit blockiert die `while`-Schleife in Funktion `read_y_then_x` nicht mehr.
  * Auf Grund der vorherigen Überlegungen wurde `write_x` aber schon ausgeführt.
  * Damit wird in Funktion `read_y_then_x` der `if`-Zweig ausgeführt und der Wert von `z` inkrementiert.

---

## Literaturhinweise <a name="link8"></a>

Die Ideen zu den Beispielen aus diesem Abschnitt stammen aus diesen Artikeln:

[*Memory Barriers in C++*](https://medium.com/@harshadaggarwal1999/memory-barriers-in-c-171bf4f03ff4).

[*`std::atomic`, Explained Properly: Memory Ordering Without the Hand-Waving*](https://towardsdev.com/cpp-atomic-memory-ordering-explained-2a5a69ed025b).

[*Lock-Free Programming in C++: Compare-And-Swap Without the Magic*](https://towardsdev.com/lock-free-programming-in-c-compare-and-swap-without-the-magic-4e8a8f278d90).


---

[Zurück](../../Readme.md)

---
