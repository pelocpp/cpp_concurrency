# Die Klasse `std::packaged_task`

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::packaged_task`
  * Klasse `std::future`
  * Klasse `std::promise`

---

## `std::packaged_task`

Das charakteristische Merkmal der Klasse `std::packaged_task` ist, dass die betroffene Task (Thread)
nicht von selbst startet, sondern explizit zu starten ist.

Für das Arbeiten mit `std::packaged_task`-Objekten sind typischerweise vier Schritte notwendig:

  * Die Aufgabe in einem `std::packaged_task`-Objekt verpacken bzw. aufbereiten.
  * Ein `std::future`-Objekt erzeugen.
  * Die Berechnung (mit dem `std::packaged_task`-Objekt) explizit anstoßen.
  * Das Ergebnis (mit `get` am `std::future`-Objekt) abholen.

---

## Ein einfaches Beispiel

Die soeben beschriebenen vier Schritte im Umgang mit der Klasse `std::packaged_task`
demonstriert folgendes Beispiel:


```cpp
01: void test() {
02: 
03:     // create packaged_task object
04:     std::packaged_task<int(void)> task { 
05:         [] () {
06:             std::this_thread::sleep_for(std::chrono::seconds{ 1 });
07:             return 123;
08:         }
09:     };
10: 
11:     // retrieve future object from task
12:     std::future<int> future{ task.get_future() };
13:         
14:     // create a thread with this task
15:     std::thread thread{ std::move(task) };
16: 
17:     // retrieve result from future object
18:     int result{ future.get() };
19:     std::cout << "Result: " << result << std::endl;
20: 
21:     thread.join();
22: }
```

*Ausgabe*:

```
Result: 123
```

*Hinweis*:<br />
Die beiden Klassen `std::packaged_task` und `std::function` besitzen Gemeinsamkeiten.

Die Klasse `std::packaged_task` erzeugt &bdquo;Callable Wrapper&rdquo; Objekte,
ähnlich wie die Klasse `std::function`,
nur mit dem Unterschied, dass die Klasse `std::packaged_task`
einen direkten Zugriff auf ein korrespondierendes `std::future`-Objekt bietet.

Die Klasse `std::packaged_task` bietet folglich einen natürlicheren und einfacheren Arbeitsablauf
als das manuelle Einrichten und Durchschleusen eines `std::promise`-Objekts,
wie wir an folgendem Vergleichsbeispiel betrachten können:


```cpp
01: static void test() {
02: 
03:     std::promise<int> promise;
04: 
05:     std::future<int> future{ promise.get_future() };
06: 
07:     std::function<void(std::promise<int>&&)> function {
08:         [] (std::promise<int>&& promise) {
09:             std::this_thread::sleep_for(std::chrono::seconds{ 1 });
10:             promise.set_value(123);
11:         }
12:     };
13: 
14:     // create a thread with this function
15:     std::thread thread{ std::move(function), std::move(promise) };
16: 
17:     // retrieve result from future object
18:     int result = future.get();
19:     std::cout << "Result: " << result << std::endl;
20: 
21:     thread.join();
22: }
```

*Ausgabe*:

```
Result: 123
```

#### Quellcode:

[Siehe hier](PackagedTask_01.cpp).


---

## Ein zweites Beispiel

Ein einfaches Beispiel skizziert den Ablauf eines Szenarios mit vier `std::packaged_task`-Objekten.

Es ist die Summe der ersten 400 natürlichen Zahlen von 1 bis einschließlich 400 - ohne den Algorithmus von Gauss - zu berechnen.
Dabei kommen vier `std::packaged_task`-Objekte zum Einsatz.
Insbesondere betrachte man, dass sowohl `std::packaged_task`- als auch `std::future`-Objekte
in einem `std::deque`-Objekt ablegt werden können.

*Hinweis*:<br />
Die *Tasks* können sowohl sequentiell als auch parallel ausgeführt werden.
Studieren Sie im Quellcode die entsprechenden Abschnitte,
wo eine *Task* entweder synchron oder asynchron abgearbeitet wird.


#### Quellcode:

[Siehe hier](PackagedTask_02.cpp).

---

[Zurück](../../Readme.md)

---
