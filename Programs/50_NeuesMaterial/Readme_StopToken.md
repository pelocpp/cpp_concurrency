# Klassen `std::stop_token`, `std::stop_callback` und `std::stop_source`


[Zurück](../../Readme.md)

---


## Verwendete Hilfsmittel:

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

## Stop Sources und Stop Tokens


---

[Zurück](../../Readme.md)

---
