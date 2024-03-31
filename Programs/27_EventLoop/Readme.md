# Realisierung einer Ereigniswarteschlange (*Event Loop* )

[Zurück](../../Readme.md)

---

## Verwendete Werkzeuge

  * `std::function`
  * `std::mutex`
  * `std::lock_guard` und `std::unique_lock`
  * `std::condition_variable`
  * `std::jthread`
  * `std::swap`

---

## Allgemeines

*Kurz gefasst*:

Eine Ereigniswarteschlange (engl. *Event Loop* ) kann man als Alternative zu einem
Mutex-Objekt betrachten. Beide serialisieren Zugriffe auf geschützte Objekt bzw. deren Methoden,
jedoch auf unterschiedliche Weise:

  * `std:mutex`-Objekte stellen einen Synchronisationsmechanismus dar, es sind zu diesem Zweck die kritischen Abschnitte
  zu identifizieren und mit entsprechenden `lock` bzw. `unlock`-Aufrufen zu schützen.<br />
  *Bemerkung*: In der Praxis kommen hier entsprechende Hüllen-Objekte wie z.B. `std::lock_guard` zum Zuge.
  * Reiht man die kritischen Abschnitte in eine Ereigniswarteschlange ein, kann man auf `std:mutex`-Objekte verzichten.
  Die kritischen Abschnitte müssen zu diesem Zweck aber Funktions-(Methoden-)grenzen haben,
  um die in eine Ereigniswarteschlange einschleusen zu können.

Generell können die Gründe für den Einsatz dieser Synchronisationsmechanismus unterschiedlicher Natur sein:

  * Möglicherweise wurden die Klassen von einem alten Teil eines Softwaresystems geerbt.
  * Sie entwerfen gerade neue Klassen, möchten diese aber nicht mit gleich mit
Synchronisationsmechanismen wie `std:mutex`-Objekten überfrachten.


## Realisierung

### Klasse `std::function`

Um eine Ereigniswarteschlange zu realisieren, benötigt man die Möglichkeit,
&bdquo;Methodenaufrufe&rdquo; zwischenzuspeichern. Gewisse Ähnlichkeiten zum *Command Pattern*
aus dem Umfeld der *Design Pattern* sind hier vorhanden.

Hierzu kommt uns die Klasse `std::function` zur Hilfe.
Diese Klasse zeichnet sich durch zwei besondere Eigenschaften aus:

#### *Type Erasure*

Auf Grund des *Type Erasure* Features kann man in einem `std::function`-Objekt alles speichern,
was man in C++ &bdquo;aufrufen&rdquo; kann:

  * C-Funktionen
  * Methoden eines Objekts
  * Aufrufbare Objekte (`operator()`)
  * Lambdas
 
In den internen Daten des `std::function`-Objekts wird alles abgelegt,
was pro *Funktion* notwendig ist, wie zum Beispiel die Referenz eines Objekts,
dessen Methode aufgerufen werden soll oder die Erfassungsklausel (*Capture Clause*) eines Lambda-Objekts.


#### *Small-Object*-Optimierung

Dies geht einher mit der so genannten *Small-Object*-Optimierung:

  * *Small-Object*-Optimierung<br />
  * Um Heap-Zuweisungen zu vermeiden oder zumindest zu minimieren, speichert ein `std::function`-Objekt seine Daten im Objekt selbst ab, wenn es klein genug ist.
  * Was &bdquo;klein genug&rdquo; ist, hängt von der Umsetzung ab.
  * Was nicht unmittelbar in das `std::function`-Objekt passt, wird auf dem Heap abgelegt.

Zusammenfassend kann man nun sagen, dass *Aufrufe* bzw. *Nachrichten* in C++
in einem `std::vector`-Objekt abgelegt werden können, zum Beispiel so:

```cpp
std::vector<std::function<void()>> m_queue;
```
In diesem Beispiel handelt es sich um *Callables*, die keine Parameter entgegennehmen und auch nichts zurückliefern,
also die Schnittstelle `void()` besitzen.

Das Manko der fehlenden Parameter werden wir in unserer Realisierung abstellen.

Eine Rückgabe von Daten erscheint mir in einer Ereigniswarteschlangenrealisierung weniger relevant,
da diese letzten Endes doch &bdquo;Nachrichten abarbeitet&rdquo;, aber nicht vordergründig zur Berechnung von *Ergebnissen*
konzipiert ist.


### Klasse `std::condition_variable`

Eine *Bedingungsvariable* ist ein Synchronisationsprimitiv,
das einen Thread dazu bringt, seine Ausführung über `wait()` zu verschieben (*suspend*),
bis ein anderer Thread ihn über `notify_one()` aufweckt (*resume*).

Was aber, wenn der zweite Thread `notify_one()` aufgerufen hat, kurz bevor der erste Thread `wait()` aufruft?

Dazu wird das `std::condition_variable`-Objekt mit einem Mutex-Objekt kombiniert:
Das Mutex-Objekt ist zu sperren, wenn auf den Zustand (*die Bedingung*) des Szenarios zugegriffen wird.

Ist das Mutex-Objekt gesperrt, erwartet ein `std::condition_variable`-Objekt,
dass der Status (die Bedingung) überprüft wird, ob der erste Thread in den Ruhezustand fallen muss
oder die Bedingung bereits erfüllt wurde und der Thread einfach weiterarbeiten kann.

Wenn sich herausstellt, dass der Thread zu blockieren ist, tritt folgender Ablauf ein:

Die `wait()`-Methode bekommt ein gesperrtes Mutex-Objekt als Parameter übergeben:

Mit diesem Mutex-Objekt geht folgende Arbeitsweise einher:

  * Das Mutex-Objekt wird im Kontext der `wait()`-Methode entsperrt, damit die Ausführung anderer Threads weiter erfolgen kann.
  * Es wird zu bestimmten Zeitpunkten eine &bdquo;Kontrollfunktion&rdquo; aufgerufen, die überprüft, ob die Suspendierung des wartenden Threads weiter aufrecht zu erhalten ist oder nicht.
  * Zu diesem Zweck wird das Mutex-Objekt gesperrt und nach dem Aufruf der Kontrollfunktion wieder entsperrt.
  * Kommt die Kontrollfunktion zu der Erkenntnis, dass die Bedingung für eine Weiterarbeit gegeben ist, sperrt sie das Mutex-Objekt wieder und setzt die Ausführung fort.
  Dies geht im Regelfall damit einher, dass ein Aufruf von `notify_one()` / `notify_all()` erfolgte.


Es gibt also einen gemeinsamen Zustand, der durch das Mutex-Objekt geschützt wird.

Daher sollte der zweite, benachrichtigende Thread Folgendes tun:

  * Das Mutex-Objekt sperren, den gemeinsamen Status ändern und das Mutex-Objekt wieder entsperren.
  * Den ersten Thread mit `notify_one()` / `notify_all()` benachrichtigen.


*Bemerkung*:<br />
Einige Entwickler rufen `notify_one()` auf, während sie das Mutex-Objekt sperren.
Das ist nicht falsch, aber es macht das System ineffizient.
Um zusätzliche Synchronisierungen zu vermeiden, stellen Sie einfach sicher, dass Sie `notify_one()` aufrufen,
nachdem Sie den Mutex freigegeben haben.


### Doppelpuffertechnik (*Double Buffering*)

In der Realisierung der Abarbeitung der Nachrichten
finden Sie eine Umsetzung der *Double Buffering Technik* vor.

Es kommen zwei Objekte des Typs `std::vector<std::function<void()>>` zum Einsatz:

  * Der erste Puffer dient ausschließlich zum Einreihen neuer Nachrichten.
  * Der zweite Puffer dient ausschließlich zum Entnehmen vorhandener Nachrichten.

Das Tauschen der Puffer findet innerhalb einer Mutex-Sperre statt, mit der Funktion `std::swap` werden
die beiden Pufferinhalte möglichst effizient getauscht.

Nach einem Tausch ist der erste Puffer grundsätzlich leer, der zweite Puffer hat den Inhalt des ersten Puffers erhalten
und kann nun bei Bedarf bzw. wenn Rechenzeit vorhanden ist, abgearbeitet werden.

Eine grobe Skizzierung der Realisierung der Verarbeitung der Nachrichten in der Warteschlange
&ndash; inklusive Doppelpuffertechnik &ndash; sieht so aus:

```cpp
01: void event_loop_procedure()
02: {
03:     std::vector<Event> events;
04: 
05:     while (m_running)
06:     {
07:         {
08:             std::unique_lock<std::mutex> guard(m_mutex);
09: 
10:             m_condition.wait(
11:                 guard,
12:                 [this] () -> bool { return ! m_events.empty(); }
13:             );
14: 
15:             std::swap(events, m_events);
16:         }
17: 
18:         for (const Event& callable : events)
19:         {
20:             callable();
21:         }
22: 
23:         events.clear();
24:     }
25: }
```

In Zeile 10 finden wir einen Aufruf der `wait`-Methode an einem `m_condition`-Objekt vor.
Hierzu muss es einen korrespondierenden `notify_one`- oder `notify_all`-Aufruf geben:

```cpp
01: void enqueue (const std::function<void()>& callable)
02: {
03:     {
04:         std::lock_guard<std::mutex> guard(m_mutex);
05:         m_events.emplace_back(callable);
06:     }
07: 
08:     m_condition.notify_one();
09: }
```

Sinnigerweise ist dieser in der Methode `enqueue` vorhanden, wenn neue Nachrichten in der
Warteschlange aufgenommen werden.


### Funktionen mit Parametern in der Ereigniswarteschlange

Es lassen sich auch Funktionen mit Parametern in die Ereigniswarteschlange einreihen &ndash; in dies sogar,
ohne an der vorhandenen Realisierung der Klasse `EventLoop` Änderungen vornehmen zu müssen.

Wie könnte dieser Trick aussehen?<br />
Und wie werden die Parameter zwischengespeichert?

Wir greifen auf das C++ Sprachfeature von Lambda-Objekten zurück.
Lambda-Objekte können über die *Capture Clause* auf Variablen der Umgebung zugreifen,
und diese mittels `[=]` in das Lambda-Objekt kopieren!

Der realisierende Quellcode mag noch etwas schwerer zu lesen sein, da er mit Hilfe *variadischer Templates*
eine beliebige Anzahl von Parametern unterschiedlichen Datentyps in das Lambda-Objekt aufnehmen kann.
Eine vereinfachende Realisierung könnte so aussehen:


```cpp
01: template<typename TFunc, typename ... TArgs>
02: void enqueueTask(TFunc&& callable, TArgs&& ...args)
03: {
04:     {
05:         std::lock_guard<std::mutex> guard(m_mutex);
06: 
07:         m_events.emplace_back( [=] () mutable { callable (args ...); } );
08:     }
09: 
10:     m_condition.notify_one();
11: }
```

In Zeile 7 des Listings finden wir einen Lambda-Ausdruck vor:
Der Aufruf der Nachricht `callable` ist im Rumpf der Lambda-Funktion plaziert,
die Parameter `args` werden via `[=]` in das Lambda-Objekt kopiert!


### Beendigung der Ausführung

Wenn die Abarbeitung der Nachrichten beendet werden soll,
wird dies dadurch erreicht, dass eine spezielle Nachricht in die Warteschlange am Ende eingefügt wird:

```cpp
[this] { m_running = false; }
```

Mit diesem Lambda-Ausdruck wird einfach das Flag `m_running` umgesetzt,
und so die Ausführung der Verarbeitungsprozedur verlassen.

---

#### Quellcode:

[*EventLoop.h*](./EventLoop.h)<br />
[*EventLoop.cpp*](./EventLoop.cpp)<br />
[*TestEventLoop.cpp*](TestEventLoop.cpp)<br />
[*Program.cpp*](Program.cpp)<br />

---

## Literaturhinweise

Die Anregungen zur Klasse `EventLoop` stammen im Wesentlichen aus dem Artikel

[Idiomatic Event Loop in C++](https://habr.com/en/articles/665730/)

von *Anton Vasin*.

---

[Zurück](../../Readme.md)

---
