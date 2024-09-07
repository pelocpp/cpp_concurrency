# Warten und Benachrichtigen

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Klasse `std::condition_variable`](#link3)

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


#### Hinweis

Siehe das Thema

[Do I have to acquire lock before calling std::condition_variable.notify_one()?](https://stackoverflow.com/questions/17101922/do-i-have-to-acquire-lock-before-calling-condition-variable-notify-one)

Und gleich noch ein zweiter Hinweis:

#### Hinweis

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

[Zurück](../../Readme.md)

---
