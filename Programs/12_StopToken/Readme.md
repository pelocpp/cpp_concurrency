# Die Klassen `std::stop_source`, `std::stop_token`, `std::stop_callback` und `std::condition_variable_any`

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Ein `std::jthread`-Objekt ist kooperativ unterbrechbar](#link3)
  * [Stopp Tokens und Bedingungsvariablen (`std::condition_variable_any`)](#link4)
  * [Stopp Callbacks (`std::stop_callback`)](#link5)
  * [Stopp Quellen und Stopp Tokens](#link6)
  * [Literaturhinweise](#link7)
  
---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::jthread`
  * Klasse `std::stop_source`
  * Klasse `std::stop_token`
  * Klasse `std::stop_callback`
  * Klasse `std::stop_source`
  * Klasse `std::atomic<bool>`
  * Klasse `std::condition_variable_any`

---

#### Quellcode

[*01_RequestStop.cpp*: Kooperative Unterbrechung eines Threads](01_RequestStop.cpp).<br />
[*02_ConditionVariableAny.cpp*: `std::condition_variable_any`-Objekt und `wait()`-Aufruf](02_ConditionVariableAny.cpp).<br />
[*03_StopCallback.cpp*: `std::condition_variable_any`-Objekt und `wait()`-Aufruf](03_StopCallback.cpp).<br />

---

## Allgemeines <a name="link2"></a>

Einmal gestartet, kann ein Thread nicht ohne Weiteres beendet werden.
Die einzige Lösung besteht darin, einen &bdquo;seitlichen Kommunikationskanal&rdquo; einzuführen,
der eine Beendigungsanforderung signalisiert.
Dieser Kanal wird durch die drei Klassen `std::stop_source`, `std::stop_token` und `std::stop_callback`
etabliert.

---

## Ein `std::jthread`-Objekt ist kooperativ unterbrechbar <a name="link3"></a>

Wie die Überschrift vermuten lässt, ist ein `std::jthread`-Objekt unterbrechbar,
es gibt also eine Möglichkeit, den Thread von außen zu stoppen.

Wir legen dabei Wert auf die Beobachtung, dass der Thread *kooperativ* unterbrechbar ist.
Der beste Weg, dies zu verstehen, besteht darin, einen Blick auf die Funktion `request_stop()` zu werfen:
Der Name ist sehr sorgfältig ausgewählt,
wir betrachten dazu im Quellcode eine Reihe von Beispielen:

  * *Szenario* 1:
   Der Hauptthread erzeugt einen neuen Thread,
   der jede Sekunde wiederholend etwas tut (eine Ausgabe in der Konsole).
   Der Hauptthread fährt dann mit einem 5-Sekunden-Job fort und wartet anschließend
   auf den Abschluss des anderen Threads. Da dieser nie fertig wird, wartet auch der Hauptthread ewig.<br /><br />
  * *Szenario* 2:
    Wie Szenario 1, es wurde von der Klasse `std::thread` zur Klasse `std::jthread` gewechselt.<br /><br />
  * *Szenario* 3:
     Nach 5 Sekunden erfolgt ein Aufruf von `request_stop()`, dieser ändert aber nichts am Ablauf des
     Programms: Man kann nicht von &bdquo;außen&rdquo; einen Stopp beantragen, der Thread selbst hat das letzte Wort.<br /><br />
  * *Szenario* 4:
   Im Kontext des Threads ist nun ein `std::stop_token`-Objekt verfügbar:
   Dieses besitzt eine Methode `stop_requested` &ndash; im Zusammenspiel mit `request_stop()` kann nun
   kooperativ ein Ende des Threads veranlasst werden.<br /><br />
  * *Szenario* 5:
   Im Kontext des Threads ist nun ein `std::stop_token`-Objekt verfügbar:
   Mit diesem Objekt kann man ein `std::stop_callback`-Objekt erzeugen, welches aufgerufen wird, wenn wiederum
   die `request_stop()`-Methode aufgerufen wird.<br /><br />
  * *Szenario* 6:
   Dieses Szenario ist vergleichbar zum letzten Szenario mit dem Unterschied, 
   dass aufgezeigt wird, dass das `std::stop_source`-Objekt auch über die Instanz eines `std::jthread`-Objekts
   abgerufen werden kann.<br /><br />
     

*Bemerkung*:<br />
In den *Szenarien* 5 und 6 wird auf Grund des konkurrierenden Zugriffs
zum Schutze einer `bool`-Variablen die `std::atomic<bool>`-Klasse verwendet.
Für die häufig gestellte Frage &bdquo;*ist das wirklich erforderlich*&rdquo; möchte ich &ndash; mit dieser [Unterstützung](https://stackoverflow.com/questions/16320838/when-do-i-really-need-to-use-atomicbool-instead-of-bool) &ndash;
so antworten:

> No data type in C++ is &bdquo;Atomic by Nature&rdquo; unless it is an object of kind `std::atomic<T>`.
  **That's because the standard says so!**

---

## Stopp Tokens und Bedingungsvariablen (`std::condition_variable_any`) <a name="link4"></a>

Wenn die Beendigung (Anhalten, Stoppen) eines Threads angefordert wird, kann es sein,
dass zu genau diesem Zeitpunkt der Thread blockiert ist,
weil er auf die Benachrichtigung für eine Bedingungsvariable (`std::condition_variable_any`) wartet (Methode `wait`).

Es gibt für diesen Fall eine neue Überladung der `wait`-Methode:
Sie bekommt neben dem *Callable* für die Auswertung der Bedingung noch ein `std::stop_token`-Objekt übergeben,
so dass der `wait`-Methodenaufruf unterbrochen werden kann, wenn eine Stopp-Anforderung vorliegt.

Aus technischen Gründen ist für diesen Fall eine Bedingungsvariable vom Typ `std::condition_variable_any` zu verwenden.

*Hinweis*:
In den meisten Fällen sollte ein `std::stop_token`-Objekt per *Value*
übergeben werden, da das Kopieren von `std::stop_token`-Objekten (und `std::stop_source`-Objekten)
relativ günstig ist.

Da außerdem `std::token`-Objekte normalerweise in einem *Callable* (*Lambda*) gespeichert
und/oder in einem anderen Thread verwendet werden,
können durch die Wertübergabe Probleme mit der Lebensdauer vermieden werden.

*Beispiel*:

```cpp
01: void test()
02: {
03:     std::queue<std::string>     m_messages;
04:     std::mutex                  m_mutex;
05:     std::condition_variable_any m_condition_variable;
06: 
07:     auto task = [&](std::stop_token token) {
08: 
09:         while (!token.stop_requested()) {
10: 
11:             std::string msg{};
12: 
13:             Logger::log(std::cout, "Waiting  ...");
14: 
15:             {
16:                 std::unique_lock lock{ m_mutex };
17: 
18:                 // wait for the next message
19:                 bool stopRequested {
20:                     m_condition_variable.wait(
21:                         lock,
22:                         token,
23:                             [&]() {
24:                             bool b { !m_messages.empty()};
25:                             Logger::log(std::cout, "Wait: Queue is empty: ", b ? "false" : "true");
26:                             return b;
27:                         }
28: 
29:                     )
30:                 };
31: 
32:                 if (!stopRequested) {
33:                     Logger::log(std::cout, "Stop has been requested!");
34:                     break;
35:                 }
36: 
37:                 // retrieve the next message from the queue
38:                 msg = m_messages.front();
39:                 m_messages.pop();
40:             }
41: 
42:             // print the next message:
43:             Logger::log(std::cout, "Message: ", msg);
44:         }
45: 
46:         Logger::log(std::cout, "Leaving JThread");
47:     };
48: 
49:     Logger::log(std::cout, "Pushing strings into queue ...");
50: 
51:     // store three messages
52:     for (const auto& s : { "Tic" , "Tac", "Toe" }) {
53: 
54:         std::lock_guard guard{ m_mutex };
55:         m_messages.push(s);
56:     }
57: 
58:     Logger::log(std::cout, "Starting JThread");
59: 
60:     std::jthread t{ task };
61: 
62:     std::this_thread::sleep_for(std::chrono::seconds{ 5 });
63: 
64:     {
65:         // after some time, store another message
66:         std::lock_guard guard{ m_mutex };
67:         m_messages.push("Tic-Tac-Toe Done");
68:     }
69: 
70:     // notify waiting thread
71:     m_condition_variable.notify_one();
72: 
73:     // after some time, end program (requests stop, which interrupts wait())
74:     std::this_thread::sleep_for(std::chrono::seconds{ 5 });
75: 
76:     Logger::log(std::cout, "Main Thread: calling request_stop");
77: 
78:     t.request_stop();
79: 
80:     std::this_thread::sleep_for(std::chrono::seconds{ 2 });
81: 
82:     Logger::log(std::cout, "Leaving Main");
83: }
```

Studieren Sie in dem Beispiel den Aufruf der `wait`-Methode (Zeile 20):
Wir übergeben neben weiteren Parametern auch ein `std::stop_token`-Objekt.
Somit kann das Warten jetzt aus einem von zwei Gründen enden:

  * Es gab eine Benachrichtigung mit `notify_one` bzw. `notify_all`, um anzuzeigen, dass die Warteschlange nicht mehr leer ist.
  * Es wurde ein Stopp angefordert.

*Ausgabe*:

```
[1]:    Pushing strings into queue ...
[1]:    Starting JThread
[2]:    Waiting  ...
[2]:    Wait: Queue is empty: false
[2]:    Message: Tic
[2]:    Waiting  ...
[2]:    Wait: Queue is empty: false
[2]:    Message: Tac
[2]:    Waiting  ...
[2]:    Wait: Queue is empty: false
[2]:    Message: Toe
[2]:    Waiting  ...
[2]:    Wait: Queue is empty: true
[2]:    Wait: Queue is empty: false
[2]:    Message: Tic-Tac-Toe Done
[2]:    Waiting  ...
[2]:    Wait: Queue is empty: true
[1]:    Main Thread: calling request_stop
[2]:    Wait: Queue is empty: true
[2]:    Wait: Queue is empty: true
[2]:    Stop has been requested!
[2]:    Leaving JThread
[1]:    Leaving Main
```

---

## Stopp Callbacks (`std::stop_callback`) <a name="link5"></a>

Ein *Stop Callback* ist ein Objekt vom Typ `std::stop_callback` mit RAII-Verhaltensweise.
Der Konstruktor registriert ein *Callable*
(Funktion, Funktionsobjekt oder Lambda), das aufgerufen werden soll,
wenn ein Stopp für ein angegebenes Stopp Token angefordert wird.

Verlässt das *Stop Callback* den Gültigkeitsbereich, wird das *Callable* abgemeldet.

```cpp
01: void task(std::stop_token token, int num)
02: {
03:     // register temporary callback
04:     std::stop_callback cb {
05:         token,
06:         [] { std::cout << "stop requested"; }
07:     };
08: 
09:     // ...
10: 
11: } // unregisters registered callback
```

---

## Stopp Quellen und Stopp Tokens <a name="link6"></a>

`std::jthread`-Objekte verfügen über ein integriertes `std::stop_source`-Objekt.
Dieses ist automatisch einem Token zugeordnet,
wenn das an `std::jthread` übergebene *Callable* ein `std::stop_token`-Objekt als Parameter besitzt.

Optional, wenn es gewünscht ist, Code bei Anforderung eines Stopps auszuführen, 
kann das `std::stop_token`-Objekt mit einer Stopp-Callbackmethode verknüpft werden.
Derartige Aufrufe sind jedoch mit einer Einschränkung verbunden:
Der Rückruf wird

  * auf dem Thread ausgeführt, der den Stopp anfordert, oder
  * sofort auf dem Thread ausgeführt, der das Callable registriert hat, wenn der Stopp bereits angefordert wurde.

---

### Beispiel

```cpp
01: static void task(std::stop_token token, int num)
02: {
03:     Logger::log(std::cout, "Enter Task");
04: 
05:     auto id{ std::this_thread::get_id() };
06: 
07:     // register a stop callback
08:     std::stop_callback cb{
09:         token,
10:         [=] {
11:             auto currentId{ std::this_thread::get_id() };
12: 
13:             if (currentId == id) {
14:                 Logger::log(std::cout, "Task: Stop requested - Thread Context = Task");
15:             }
16:             else {
17:                 Logger::log(std::cout, "Task: Stop requested - Thread Context = Main");
18:             }
19:         }
20:     };
21: 
22:     std::this_thread::sleep_for(std::chrono::seconds{ 3 });
23: 
24:     Logger::log(std::cout, "Done Task");
25: }
26: 
27: static void test_02()
28: {
29:     Logger::log(std::cout, "Main");
30: 
31:     // create stop source and stop token
32:     std::stop_source source;
33:     std::stop_token token{ source.get_token() };
34: 
35:     // A) request stop before task has been created
36:     source.request_stop();                   // put either this line into comment ...
37: 
38:     std::future<void> future{
39:         std::async(std::launch::async, [token] { task(token, 123); })
40:     };
41: 
42:     std::this_thread::sleep_for(std::chrono::seconds{ 2 });
43: 
44:     // B) request stop after task has been created
45:     // (runs any associated callbacks on this thread)
46:     // source.request_stop();                  // or put this line into comment
47: 
48:     Logger::log(std::cout, "Done Main");
49: }
```

Je nachdem, wann wir die `request_stop`-Methode an einem `std::stop_source`-Objekt aufrufen,
können wir an den Ausgaben erkennen, in welchem Thread-Kontext das Callable eines `std::stop_callback`-Objekts 
ausgeführt wird. Folgende Aufgaben lassen sich mit dem letzten Beispiel erzielen:

*Ausgabe*:

```
[1]:    Main
[2]:    Enter Task
[2]:    Task: Stop requested - Thread Context = Task
[1]:    Done Main
[2]:    Done Task
```
oder auch

```
[1]:    Main
[2]:    Enter Task
[1]:    Task: Stop requested - Thread Context = Main
[1]:    Done Main
[2]:    Done Task
```

---

## Literaturhinweise <a name="link7"></a>

Die Anregungen zu den Klasse `std::stop_source`, `std::stop_token` und `std::stop_callback` stammen im Wesentlichen aus dem Buch

[C++ &ndash; The Complete Guide](https://www.josuttis.com/cppstd20/index.html) von Nicolai M. Josuttis.

Kleinere Ergänzungen wurden in Abstimmung mit der Unterlage

[`std::stop_source`, `std::stop_token` und `std::stop_callback`](https://medium.com/@simontoth/daily-bit-e-of-c-std-stop-source-std-stop-token-std-stop-callback-d69d3ebe8e36)

von Simon Tóth vorgenommen.

---

[Zurück](../../Readme.md)

---
