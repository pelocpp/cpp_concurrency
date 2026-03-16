# Kooperation von Threads: Klasse `std::condition_variable`

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Klasse `std::condition_variable`](#link3)
  * [Zur Abgrenzung: Klasse `std::atomic<T>`](#link4)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Mutex-Klassen</ins>:

  * Klasse `std::mutex`
  * Klasse `std::condition_variable`
 
<ins>Hüllen-Klassen für Mutexobjekte</ins>:

  * Klasse `std::lock_guard`
  * Klasse `std::unique_lock`


<ins>Thread-Klassen</ins>:

  * Klasse `std::thread`


<ins>Methoden</ins>:

  * Methoden `wait`, `notify_one` und `notify_all`
  * Methode `join` und `detach`
  * Methode `sleep_for`


<ins>Weitere Klassen</ins>:

  * Klasse `std::atomic<T>`

---

#### Quellcode

[*Condition_Variable_01_Simple.cpp*](Condition_Variable_01_Simple.cpp).<br />
[*Condition_Variable_02_Simple.cpp*](Condition_Variable_02_Simple.cpp).

---

## Allgemeines <a name="link2"></a>

*Concurrency* (Nebenläufigkeit, Parallelität) und *Synchronization* (Synchronisation)
sind entscheidende Aspekte der Multithreading-Programmierung.

In C++ stellt die Standardbibliothek mehrere Synchronisierungsprimitive bereit,
wie etwa `std::mutex`, `std::lock_guard`, `std::unique_lock` usw.,
die dazu beitragen, Thread-Sicherheit zu gewährleisten und *Data Races* zu verhindern,
wenn mehrere Threads auf gemeinsam genutzte Ressourcen gleichzeitig zugreifen.

Für das Zusammenspiel von Methoden im Kontext unterschiedlicher Threads
gibt es die Klasse `std::condition_variable`.


## Klasse `std::condition_variable` <a name="link3"></a>

Wir gehen auf die Klasse `std::condition_variable` ein:

Ein `std::unique_lock`-Objekt muss von der &bdquo;empfangenden&rdquo; Seite
(der Seite, die benachrichtigt wird) verwendet werden, um bei Gebrauch eines `std::condition_variable`-Objekts
entsprechende Benachrichtigungen empfangen zu können.

Der Grund, warum ein `std::unique_lock`-Objekt für ein `std::condition_variable`-Objekt erforderlich ist, besteht darin,
dass dieses das zugrunde liegende `std::mutex`-Objekt jedes Mal sperren kann,
wenn die Bedingungsvariable (`std::condition_variable`) nach einer gültigen Benachrichtigung
aus einer Wartephase aufwacht und einen kritischen Codeabschnitt ausführt.

Das `std::unique_lock`-Objekt entsperrt das zugrunde liegende Mutexobjekt jedes Mal, wenn

  * der Aufruf der `wait`-Methode an der Bedingungsvariablen fälschlicherweise aktiviert wurde, es also erneut gewartet werden muss.
  * bei automatischer Zerstörung des `std::unique_lock`-Objekts. Dies ist der Fall, wenn der kritische Abschnitt ausgeführt und schließlich abgelaufen ist und der Gültigkeitsbereich des `std::unique_lock`-Objekts verlassen wird.


#### Erster Hinweis

Siehe das Thema

[Do I have to acquire lock before calling std::condition_variable.notify_one()?](https://stackoverflow.com/questions/17101922/do-i-have-to-acquire-lock-before-calling-condition-variable-notify-one)

Und gleich noch ein zweiter Hinweis:

#### Zweiter Hinweis

Die Funktionsweise der Methode `wait` der Klasse `std::condition_variable` ist wie folgt definiert:

*Definition* von `wait`:

```cpp
template< class Predicate >
void wait(std::unique_lock<std::mutex>& lock, Predicate pred);
```

*Ablauf*:

```cpp
while (!pred()) {
    wait(lock);
}
```

Das heißt inbesondere, dass vor dem ersten eigentlichen Warten das Prädikat ausgewertet wird!

---

## Zur Abgrenzung: Klasse `std::atomic<T>` <a name="link4"></a>

Auch an der (den) Klasse(n) `std::atomic<T>` gibt es die drei Methoden `wait`, `notify_one` und `notify_all`.
Der Unterschied zur Klasse `std::condition_variable` besteht darin, dass die Methode `std::atomic::wait`
auf eine Wertänderung wartet, die `wait`-Methode an einem `std::condition_variable`-Objekt hingegen 
eine komplexere Bedingung hantieren kann. Hierzu ist zusätzlich der Einsatz eines Mutex-Objekts erforderlich,
um die beteiligten Variablen vor dem konkurrierenden Zugriff zu schützen.

**Hauptmerkmale** der `std::condition_variable::wait`-Methode:<br />
  * Erfordert ein `std::mutex`-Objekt
  * Die Wartebedingung kann von mehreren Variablen abhängen
  * Behandelt unerwartete Aktivierungen (&bdquo;Unexpected WakeUps&rdquo;)
  * Typische Einsatzgebiete sind Warteschlangen, Pipelines und Ressourcenpools


**Hauptmerkmale** der `std::atomic::wait`-Methode:<br />
  * Kein `std::mutex`-Objekt erforderlich
  * Wartet auf den Wechsel des Werts einer einzelnen (atomaren) Variable
  * Sehr geringer Overhead
  * Ideal für die Hantierung von Flags und Zustandsübergängen


*Beispiel*:

```cpp
01: std::atomic<bool> g_ready{ false };
02: 
03: static void func_01()
04: {
05:     Logger::log(std::cout, "Before Wait");
06: 
07:     g_ready.wait(false); // blocks, as long  the value is 'false'
08: 
09:     Logger::log(std::cout, "After Wait");
10: }
11: 
12: static void func_02()
13: {
14:     Logger::log(std::cout, "Another Thread");
15: 
16:     std::this_thread::sleep_for(std::chrono::seconds{ 5 });
17: 
18:     Logger::log(std::cout, "Storing value 'false'");
19: 
20:     g_ready = false;
21:     g_ready.notify_one();
22: 
23:     std::this_thread::sleep_for(std::chrono::seconds{ 5 });
24: 
25:     Logger::log(std::cout, "Storing value 'true'");
26: 
27:     g_ready = true;
28:     g_ready.notify_one();
29: 
30:     Logger::log(std::cout, "Done Thread.");
31: }
32: 
33: void test_atomic_variable_01()
34: {
35:     Logger::log(std::cout, "Start:");
36: 
37:     std::thread t1{ func_01 };
38:     std::thread t2{ func_02 };
39: 
40:     t1.join();
41:     t2.join();
42: 
43:     Logger::log(std::cout, "Done.");
44: }
```

*Ausgabe*:

```
[1]:    Start:
[2]:    Before Wait
[3]:    Another Thread
[3]:    Storing value 'false'
[3]:    Storing value 'true'
[2]:    After Wait
[3]:    Done Thread.
[1]:    Done.
```


---

[Zurück](../../Readme.md)

---
