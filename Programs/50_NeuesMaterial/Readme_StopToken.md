# Klassen `std::stop_source`, `std::stop_token` und `std::stop_callback`


[Zurück](../../Readme.md)

---

## Verwendete Hilfsmittel:

<ins>Klassen</ins>:

  * Klasse `std::stop_source`
  * Klasse `std::stop_token `
  * Klasse `std::stop_callback `
 
 Diese Klassen arbeiten mit `std::jthread` and `std::condition_variable_any` zusammen.

---

## Allgemeines

Einmal gestartet, kann ein Thread nicht ohne Weiteres beendet werden.
Die einzige Lösung besteht darin, einen &bdquo;seitlichen Kommunikationskanal&rdquo; einzuführen,
der eine Beendigungsanforderung signalisiert.
Dieser Kanal wird durch die drei Klassen `std::stop_source`, `std::stop_token` und `std::stop_callback`
etabliert.

---

## Stop Tokens und Bedingungsvariablen

Wenn die Beendigung (Anhalten, Stoppen) eines Threads angefordert wird, kann es sein,
dass zu genau diesem Zeitpunkt der Thread blockiert ist,
weil er auf die Benachrichtigung für eine Bedingungsvariable wartet (Methode `wait`).

Es gibt für diesen Fall eine neue Überladung der `wait`-Methode:
Sie bekommt neben dem Callable für die Auswertung der Bedingung noch ein `std::stop_token`-Objekt übergeben,
so dass der `wait`-Methodenaufruf unterbrochen werden kann, wenn eine Stopp-Anforderung vorliegt.

Aus technischen Gründen ist für diesen Fall eine Bedingungsvariable vom Typ `std::condition_variable_any` zu verwenden.

*Hinweis*:
In den meisten Fällen sollten ein `std::stop_token`-Objekt per *Value*
übergeben werden, da das Kopieren von `std::stop_token`-Objekten (und `std::stop_source`-Objekten)
relativ günstig ist.

Da außerdem `std::token`-Objjekte normalerweise in einem *Callable* (*Lambda*) gespeichert
und/oder in einem anderen Thread verwendet werden,
können durch die Wertübergabe Probleme mit der Lebensdauer vermieden werden.

*Beispiel*:

```cpp
01: static void test()
02: {
03:     std::queue<std::string>     m_messages;
04:     std::mutex                  m_mutex;
05:     std::condition_variable_any m_condition_variable;
06: 
07:     auto task = [&] (std::stop_token token) {
08:             
09:         while (! token.stop_requested()) {
10: 
11:             std::string msg{};
12: 
13:             std::cout << "Waiting  ..." << std::endl;
14: 
15:             {
16:                 std::unique_lock lock{ m_mutex };
17: 
18:                 // wait for the next message
19:                 bool stopRequested {
20:                     m_condition_variable.wait(
21:                         lock,
22:                         token,
23:                         [&] () { return ! m_messages.empty(); }
24:                     )
25:                 };
26: 
27:                 if (!stopRequested) {
28:                     std::cout << "Stop has been requested!" << std::endl;
29:                     return;
30:                 }
31: 
32:                 // retrieve the next message from the queue
33:                 msg = m_messages.front();
34:                 m_messages.pop();
35:             }
36: 
37:             // print the next message:
38:             std::cout << "Message: " << msg << std::endl;
39:         }
40:     };
41: 
42:     std::jthread t { task };
43: 
44:     std::cout << "Pushing strings into queue" << std::endl;
45: 
46:     // store three messages
47:     for (std::string s : { std::string{ "Tic" }, std::string{ "Tac" }, std::string{ "Toe" }}) {
48: 
49:         // std::scoped_lock lg{ m_mutex };
50:         std::lock_guard guard { m_mutex };
51:         m_messages.push(s);
52:     }
53: 
54:     // notify waiting thread
55:     m_condition_variable.notify_one();
56: 
57:     // after some time, store another message
58:     std::this_thread::sleep_for(3s);
59: 
60:     {
61:         std::lock_guard guard{ m_mutex };
62:         m_messages.push("Done");
63:     }
64: 
65:     // notify waiting thread
66:     m_condition_variable.notify_one();
67: 
68:     // after some time, end program (requests stop, which interrupts wait())
69:     std::this_thread::sleep_for(3s);
70: }
```

Studieren Sie in dem Beispiel den Aufruf der `wait`-Methode (Zeile 22):
Wir übergeben neben weiteren Parametern auch ein `std::stop_token`-Objekt.
Somit kann das Warten jetzt aus einem von zwei Gründen enden:

  * Es gab eine Benachrichtigung mit `notify_one` bzw. `notify_all`, dass die Warteschlange nicht mehr leer ist.
  * Es wurde ein Stopp angefordert.


*Ausgabe*:

```
Pushing strings into queue
Waiting  ...
Message: Tic
Waiting  ...
Message: Tac
Waiting  ...
Message: Toe
Waiting  ...
Message: Done
Waiting  ...
Stop has been requested!
```


---

### Stop Callbacks

Ein *Stop Callback* ist ein Objekt vom Typ `std::stop_callback` mit RAII-Verhaltensweise.
Der Konstruktor registriert ein *Callable*
(Funktion, Funktionsobjekt oder Lambda), das aufgerufen werden soll,
wenn ein Stopp für ein angegebenes Stop Token angefordert wird.

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
11: } // unregisters callback is unregistered
```

---

## Stop Sources und Stop Tokens

`std::jthread`-Objekte verfügen über ein integriertes `std::stop_source`-Objekt,
das automatisch einem Token zugeordnet wird,
wenn das an `std::jthread` übergebene *Callable* ein `std::stop_token`-Objekt als Parameter besitzt.

Optional, wenn es gewünscht ist, Code bei Anforderungs eines Stopps auszuführen, 
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
03:     Logger::log(std::cout, "Task");
04: 
05:     auto id{ std::this_thread::get_id() };
06: 
07:     // register a stop callback
08:     std::stop_callback cb{
09:         token, 
10:         [=] {
11:             auto currentId{ std::this_thread::get_id() };
12:             if (currentId == id) {
13:                 Logger::log(std::cout, "Task: Stop requested - Thread Context = Task");
14:             }
15:             else {
16:                 Logger::log(std::cout, "Task: Stop requested - Thread Context = Main");
17:             }
18:         }
19:     };
20: 
21:     std::this_thread::sleep_for(std::chrono::seconds(4));
22: 
23:     Logger::log(std::cout, "Done Task");
24: }
25: 
26: static void test_02()
27: {
28:     Logger::log(std::cout, "Main");
29: 
30:     // create stop source and stop token
31:     std::stop_source source;
32:     std::stop_token token{ source.get_token() };
33: 
34:     // register callback
35:     std::stop_callback cb{ 
36:         token,
37:         [] {
38:             Logger::log(std::cout, "Main: Stop requested");
39:         } 
40:     };
41: 
42:     // request stop before task has been created
43:     // source.request_stop();
44: 
45:     std::future<void> future {
46:         std::async(std::launch::async, [token] { task(token, 123); })
47:     };
48: 
49:     std::this_thread::sleep_for(std::chrono::seconds(2));
50: 
51:     // request stop after task has been created
52:     // (runs any associated callbacks on this thread)
53:     source.request_stop();
54: 
55:     Logger::log(std::cout, "Done Main");
56: }
```

Je nachdem, wann wir die `request_stop`-Methode an einem `std::stop_source`-Objekt aufrufen,
können wir an den Ausgaben erkennen, in welchem Thread-Kontext das Callable eines `std::stop_callback`-Objekts 
ausgeführt wird. Folgende Aufgaben lassen sich mit dem letzten Beispiel erzielen:

*Ausgabe*:

```
[1]:    Main
[2]:    Task
[1]:    Task: Stop requested - Thread Context = Main
[1]:    Main: Stop requested
[1]:    Done Main
[2]:    Done Task
```
oder auch

```
[1]:    Main
[1]:    Main: Stop requested
[2]:    Task
[2]:    Task: Stop requested - Thread Context = Task
[1]:    Done Main
[2]:    Done Task
```

---

## Literaturhinweise

Die Anregungen zu den Klasse `std::stop_source`, `std::stop_token` und `std::stop_callback` stammen im Wesentlichen aus dem Buch

[C++ &ndash; The Complete Guide](https://www.josuttis.com/cppstd20/index.html) von Nicolai M. Josuttis.

Kleinere Ergänzungen wurden in Abstimmung mit der Unterlage

[`std::stop_source`, `std::stop_token` und `std::stop_callback`](https://medium.com/@simontoth/daily-bit-e-of-c-std-stop-source-std-stop-token-std-stop-callback-d69d3ebe8e36)

von Simon Tóth vorgenommen.

---


[Zurück](../../Readme.md)

---
