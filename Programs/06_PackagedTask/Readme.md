# Die Klasse `std::packaged_task`

[Zur�ck](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Ein einfaches Beispiel](#link3)
  * [Die beiden Klassen `std::packaged_task` und `std::function` im Vergleich](#link4)
  * [Ein zweites Beispiel](#link5)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::packaged_task`
  * Klasse `std::future`
  * Klasse `std::promise`
  * Klasse `std::function`

---

#### Quellcode

[*PackagedTask_01.cpp*](PackagedTask_01.cpp).<br />
[*PackagedTask_02.cpp*](PackagedTask_02.cpp).

---

## Allgemeines <a name="link2"></a>

Das charakteristische Merkmal der Klasse `std::packaged_task` ist seine F�higkeit,
ein aufrufbares Objekt (*Callable*) zu umschlie�en.

Ein auf diese Weise verpacktes Objekt wird *nicht* von alleine gestartet:
Man muss den Aufruf explizit ansto�en &ndash; dies kann synchron im aktuellen Thread
oder asynchron durch einen separaten Thread erfolgen.

Der R�ckgabewert wird in einem `std::future`-Objekt abgelegt.

F�r das Arbeiten mit `std::packaged_task`-Objekten sind typischerweise vier Schritte notwendig:

  * Die Aufgabe in einem `std::packaged_task`-Objekt verpacken bzw. aufbereiten.
  * Ein `std::future`-Objekt erzeugen.
  * Die Berechnung (mit dem `std::packaged_task`-Objekt) explizit ansto�en.
  * Das Ergebnis (mit `get` am `std::future`-Objekt) abholen.

---

## Ein einfaches Beispiel <a name="link3"></a>

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
15:     std::thread t{ std::move(task) };
16: 
17:     // retrieve result from future object
18:     int result{ future.get() };
19:     std::cout << "Result: " << result << std::endl;
20: 
21:     t.join();
22: }
```

*Ausgabe*:

```
Result: 123
```

In diesem Beispiel wurde die *Task* asynchron in einem separaten Thread ausgef�hrt.

Es ginge aber auch synchron im aktuellen Thread, siehe dazu das n�chste Beispiel:


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
14:     // execute task
15:     task();
16: 
17:     // retrieve result from future object
18:     int result{ future.get() };
19:     std::cout << "Result: " << result << std::endl;
20: }
```

---

## Die beiden Klassen `std::packaged_task` und `std::function` im Vergleich <a name="link4"></a>

Die beiden Klassen `std::packaged_task` und `std::function` besitzen �hnlichkeiten.

Die Klasse `std::packaged_task` erzeugt &bdquo;Callable Wrapper&rdquo; Objekte,
�hnlich wie die Klasse `std::function`,
nur mit dem Unterschied, dass die Klasse `std::packaged_task`
einen direkten Zugriff auf ein korrespondierendes `std::future`-Objekt bietet.

Die Klasse `std::packaged_task` bietet folglich einen nat�rlicheren und einfacheren Arbeitsablauf
als das manuelle Einrichten und Durchschleusen eines `std::promise`-Objekts,
wie wir an folgendem Vergleichsbeispiel betrachten k�nnen:


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
15:     std::thread t{ std::move(function), std::move(promise) };
16: 
17:     // retrieve result from future object
18:     int result = future.get();
19:     std::cout << "Result: " << result << std::endl;
20: 
21:     t.join();
22: }
```

*Ausgabe*:

```
Result: 123
```

---

## Ein zweites Beispiel <a name="link5"></a>

Ein einfaches Beispiel skizziert den Ablauf eines Szenarios mit vier `std::packaged_task`-Objekten.

Es ist die Summe der ersten 400 nat�rlichen Zahlen von 1 bis einschlie�lich 400 - ohne den Algorithmus von Gauss - zu berechnen.
Dabei kommen vier `std::packaged_task`-Objekte zum Einsatz.
Insbesondere betrachte man, dass sowohl `std::packaged_task`- als auch `std::future`-Objekte
in einem `std::deque`-Objekt ablegt werden k�nnen.

*Hinweis*:<br />
Die *Tasks* k�nnen sowohl sequentiell als auch parallel ausgef�hrt werden.
Studieren Sie im Quellcode die entsprechenden Abschnitte,
wo eine *Task* entweder synchron oder asynchron abgearbeitet wird.

---

[Zur�ck](../../Readme.md)

---
