# Realisierung einer Ereigniswarteschlange (*Event Loop* )

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Klasse `std::function` oder `std::std::move_only_function`](#link3)
  * [Konzeption einer `enqueue`-Methode an der Klasse `EventLoop`](#link4)
  * [Doppelpuffertechnik (*Double Buffering*)](#link5)
  * [Funktionen mit Parametern in der Ereigniswarteschlange](#link6)
  * [Beendigung der Ausführung](#link7)
  * [Literaturhinweise](#link8)

---

## Verwendete Werkzeuge <a name="link1"></a>

  * `std::function` und `std::move_only_function`
  * `std::mutex`
  * `std::lock_guard` und `std::unique_lock`
  * `std::condition_variable`
  * `std::jthread`
  * `std::swap`

---

#### Quellcode

[*EventLoop.h*](./EventLoop.h)<br />
[*EventLoop.cpp*](./EventLoop.cpp)<br />
[*TestEventLoop.cpp*](TestEventLoop.cpp)<br />
[*Program.cpp*](Program.cpp)<br />

---

## Allgemeines <a name="link2"></a>

*Kurz gefasst*:

Eine Ereigniswarteschlange (engl. *Event Loop* ) kann man als Alternative zu einem
Mutex-Objekt betrachten. Beide serialisieren Zugriffe auf kritischen Abschnitte eines Programms,
jedoch auf unterschiedliche Weise:

  * `std:mutex`-Objekte stellen einen Synchronisationsmechanismus dar, es sind zu diesem Zweck die kritischen Abschnitte
  zu identifizieren und mit entsprechenden `lock` bzw. `unlock`-Aufrufen zu schützen.<br />
  *Bemerkung*: In der Praxis kommen hier entsprechende Hüllen-Objekte wie z.B. `std::lock_guard` zum Zuge.
  * Reiht man die kritischen Abschnitte in eine Ereigniswarteschlange ein, kann man auf `std:mutex`-Objekte verzichten.
  Die kritischen Abschnitte müssen zu diesem Zweck aber Funktions-(Methoden-)grenzen haben,
  um sie in eine Ereigniswarteschlange einschleusen zu können.

Generell können die Gründe für den Einsatz dieser Synchronisationsmechanismen unterschiedlicher Natur sein:

  * Möglicherweise wurden die Klassen von einem alten Teil eines Softwaresystems geerbt.
  * Sie entwerfen gerade neue Klassen, möchten diese aber nicht mit gleich mit
Synchronisationsmechanismen wie `std:mutex`-Objekten überfrachten.

Wir stellen in diesem Abschnitt die Realisierung einer Klasse `EventLoop`,
die eine Ereigniswarteschlange darstellt. Es ist möglich, Funktionen ohne als auch mit Parametern 
in dieser Warteschlange einzureihen. Konzeptionell beabsichtigt hingegen ist der Rückgabetyp `void` bei allen Funktion &ndash;
Welchen Sinn sollte es ergeben, zu einem späteren Zeitpunkte einer Funktionsausführung zu erhalten?
Meiner Meinung nach keinen, deshalb dieser Ansatz.

Es folgen einige Hinweise zur Realisierung.


## Klasse `std::function` oder `std::std::move_only_function` <a name="link3"></a>

Um eine Ereigniswarteschlange zu realisieren, benötigt man die Möglichkeit,
&bdquo;Methodenaufrufe&rdquo; zwischenzuspeichern. Gewisse Ähnlichkeiten zum *Command Pattern*
aus dem Umfeld der *Design Pattern* sind hier vorhanden.

Hier kommen zwei Klassen ins Spiel: `std::function` oder `std::std::move_only_function`.
Was ist der Unterschied zwischen diesen beiden Klassen?

| `std::function<void()>` | `std::move_only_function<void()>` |
|:-|:-|
| Kopierbar. | Nur verschiebbar. | 
| Erfordert, dass die gespeicherte Funktion kopierbar ist (es dürfen in der Funktion z. B. keine `std::unique_ptr`-Variablen verwendet werden). | Kann nur verschiebbare Funktionen speichern (z. B. sind `std::unique_ptr`- oder `std::packaged_task`-Objekte erlaubt). | 
| Kann Speicher auf der Halde anlegen. | Vermeidet unnötige Kopien.| 
| Flexibler, aber ressourcenintensiver.| Bessere semantische Eignung für einmalige Aufgaben. | 

*Tabelle* 1: Unterschiede zwischen `std::function<void()>` und `std::move_only_function<void()>`.

Welche Situation liegt bei uns im Kontext der Realisierung einer Ereigniswarteschlange vor?
Alle Ereignisse

  * werden einmalig ausgeführt. 
  * sind sinnvollerweise nicht zu kopieren.
  * werden konzeptionell &bdquo;verarbeitet&rdquo;, sie müssen nicht aufgehoben werden.

Dies entspricht perfekt der *Move-Only*-Semantik.
Damit sollten wir in der Realisierung auf die `std::move_only_function<void()>`-Klasse zurückgreifen:

```cpp
using Event = std::move_only_function<void()>;
```

## Konzeption einer `enqueue`-Methode an der Klasse `EventLoop` <a name="link4"></a>

Wie sieht es mit der Definition der Schnittstelle einer `enqueue`-Methode aus?
Welche der folgenden Schnittstellen würden Sie bevorzugen &ndash; technisch gesehen sind sie alle realisierbar:

```cpp
void enqueue(Event callable);
void enqueue(Event& callable);
void enqueue(const Event& callable);
void enqueue(Event&& callable);
```

Dabei hatten wir den Datentyp `Event` als `std::move_only_function<void()>` definiert.

Wir studieren die Möglichkeiten nun im Detail.

#### Schnittstelle `void enqueue(Event& callable)`

Diese Methode erwartet eine nicht-konstante *LValue*-Referenz.

*Problem*:<br />
  * Die Verschiebe-Semantik geht nicht &ndash; sie ist technisch gesehen möglich, aber es würde auf der Seite des Aufrufers zu Überraschungen kommen.
  * Temporäre Werte (*RValues*) werden nicht akzeptiert.

Damit wären die naheliegendsten Verwendungszwecke nicht möglich, zum Beispiel Aufrufe der Gestalt


```cpp
m_events.enqueue([] () { /* ... */ });  // ERROR: initial value of reference to non-const must be an lvalue
```

#### Schnittstelle `void enqueue(Event&& callable)`

```cpp
void enqueue(Event&& callable);
```

Funktioniert einwandfrei mit temporären Objekten! 
Die Move-Semantik wird damit unterstützt.


```cpp
eventLoop.enqueue([] () { /* ... */ });  // Works !!!
```

*Nachteil*:<br />
Es werden keine *LValues* akzeptiert, es sei denn, diese werden explizit verschoben:


```cpp
Event event = [] { /* ... */ };
eventLoop.enqueue(event);              // ERROR: an rvalue reference cannot be bound to an lvalue
eventLoop.enqueue(std::move(event));   // Works !!!
```

Das ist zwar korrekt und sauber, aber etwas weniger ergonomisch.

#### Zwei Methoden `void enqueue(Event& callable)` und `void enqueue(Event&& callable)`


```cpp
void enqueue(Event& callable);
void enqueue(Event&& callable);
```

Das ist so nicht optimal.

*Warum*?<br />
  * Was würde die `Event&`-Version tun? Kopieren? Bei `std::move_only_function<void()>`-Objekten geht das nicht!
  * Einen *LValue* verschieben? Das ist überraschend und gefährlich.

Dies führt zu verwirrender Semantik und sollte vermieden werden.

#### Schnittstelle `void enqueue(const Event& callable)`

Ein Move-Only Datentyp kann nicht mit `const` qualifiziert werden,
da der Parameter `callable` sonst nicht verschiebbar ist.
Also diese Variante kommt überhaupt nicht in Betracht.


## Bewährte Vorgehensweise: *Pass-by-Value* &ndash; `void enqueue(Event callable)`

Langer Rede, kurzer Sinn: All diese Varianten sind nicht empfehlenswert.
Die idiomatische Lösung in modernem C++ lautet:

```cpp
void enqueue(Event callable);  // Pass by Value
```

Was ist an dieser Lösung so gut?

  * Funktioniert effizient mit RValues:

```cpp
eventLoop.enqueue([] () { /* ... */ });  // Works ! Lambda is first moved into the parameter, 
                                         // then into the std::vector
```


  * Funktioniert mit *LValues* (allerdings ist explizites Verschieben erforderlich)

```cpp
Event event = [] { /* ... */ };
eventLoop.enqueue(std::move(event));   // Works ! Clear Intent
```

*Zusammenfassung*:<br />
  * Einfache und sichere Schnittstelle
  * Keine Überladungen
  * Keine Überraschungen
  * Entspricht modernen C++-Konventionen


*Hinweis*:<br />
Wenn Sie beliebige aufrufbare Funktionen (nicht nur `Event`) zulassen möchten,
können Sie ein Funktionstemplate verwenden:

```cpp
template <typename TFunc>
void enqueue(TFunc&& func) {
    m_events.emplace_back(std::forward<TFunc>(func));
}
```

Dies ermöglicht folgende Aufrufe:

```cpp
eventLoop.enqueue([] { /* ... */ });      // Lambda => Template

std::move_only_function<void()> event{ function };
eventLoop.enqueue(std::move(event));      // Event  => std::move_only_function<void()>;
```

Aber:<br />
  * Geringfügig komplexer
  * Kann unerwartete Typen akzeptieren (normalerweise unproblematisch)


## Doppelpuffertechnik (*Double Buffering*) <a name="link5"></a>

In der Realisierung der Abarbeitung der Nachrichten
finden Sie eine Umsetzung der *Double Buffering Technik* vor.

Es kommen zwei Objekte des Typs `std::vector<std::move_only_function<void()>>` zum Einsatz:

  * Der erste Puffer dient ausschließlich zum Einreihen neuer Nachrichten.
  * Der zweite Puffer dient ausschließlich zum Entnehmen vorhandener Nachrichten.

Das Tauschen der Puffer findet innerhalb einer Mutex-Sperre statt, mit der Funktion `std::swap` werden
die beiden Pufferinhalte möglichst effizient getauscht.

Nach einem Tausch ist der erste Puffer grundsätzlich leer, der zweite Puffer hat den Inhalt des ersten Puffers erhalten
und kann nun bei Bedarf bzw. wenn Rechenzeit vorhanden ist, abgearbeitet werden.

Eine grobe Skizzierung der Realisierung der Verarbeitung der Nachrichten in der Warteschlange
&ndash; inklusive Doppelpuffertechnik &ndash; sieht so aus:

```cpp
01: void event_loop()
02: {
03:     std::vector<std::move_only_function<void()>> events;
04: 
05:     while (true)
06:     {
07:         {
08:             std::unique_lock<std::mutex> guard{ m_mutex };
09: 
10:             m_condition.wait(
11:                 guard,
12:                 [this] () -> bool { return ! m_events.empty() || !m_running; }
13:             );
14: 
15:             if (!m_running && m_events.empty())
16:                 return;
17: 
18:             std::swap(events, m_events);
19:         }
20: 
21:         for (auto& callable : events)
22:         {
23:             callable();
24:         }
25: 
26:         events.clear();  // empty container for next loop
27:     }
28: 
29: }
```

In Zeile 10 finden wir einen Aufruf der `wait`-Methode an einem `m_condition`-Objekt vor.
Hierzu muss es einen korrespondierenden `notify_one`- oder `notify_all`-Aufruf geben:

```cpp
01: void enqueue(std::move_only_function<void()> callable)
02: {
03:     {
04:         std::lock_guard<std::mutex> guard{ m_mutex };
05:         m_events.push_back(std::move(callable));
06:     }
07: 
08:     m_condition.notify_one();
09: }

```

Sinnigerweise ist dieser Aufruf in der Methode `enqueue` vorhanden, wenn neue Nachrichten in der
Warteschlange aufgenommen werden.


## Funktionen mit Parametern in der Ereigniswarteschlange <a name="link6"></a>

Welche Funktionen (Rückgabetyp, Parameter) lassen sich in der Ereigniswarteschlange einreihen?
Es sind dies Funktionen mit beliebig vielen Parametern und auch einem beliebigen Rückgabetyp &ndash; und dies sogar,
ohne an der vorhandenen Realisierung der Klasse `EventLoop` Änderungen vornehmen zu müssen.

Wie könnte dieser Trick aussehen?<br />
Und wie werden die Parameter zwischengespeichert?

Wir greifen auf das C++ Sprachfeature von Lambda-Objekten zurück.
Lambda-Objekte können über die *Capture Clause* auf Variablen der Umgebung zugreifen,
und diese mittels `[=]` in das Lambda-Objekt kopieren!

Ab C++ 14 kann man sogar auf das unnötige Kopieren verzichten,
mit dem so genannten &bdquo;*Generalized Lambda Capture*&rdquo; Feature können die Parameter auch verschoben werden,
also kann die Move-Semantik Anwendung finden!

Der realisierende Quellcode mag nicht ganz einfach zu lesen zu sein, da er mit Hilfe *variadischer Templates*
eine beliebige Anzahl von Parametern unterschiedlichen Datentyps in das Lambda-Objekt aufnimmt:


```cpp
01: template<typename TFunc, typename ... TArgs>
02: void enqueueTask(TFunc&& func, TArgs&& ...args)
03: {
04:     Logger::log(std::cout, "enqueueTask ...");
05: 
06:     // using "Generalized Lambda Capture" to preserve move semantics
07:     auto callable{
08:         [func = std::forward<TFunc>(func),
09:         ... capturedArgs = std::forward<TArgs>(args)]() {
10:             std::invoke(std::move(func), std::move(capturedArgs)...);
11:         } 
12:     };
13: 
14:     {
15:         // RAII guard
16:         std::lock_guard<std::mutex> guard{ m_mutex };
17:         m_events.push_back(std::move(callable));
18:     }
19: 
20:     m_condition.notify_one();
21: }
```

In Zeile 8 des Listings finden wir einen Lambda-Ausdruck vor:
Der Aufruf der Nachricht `func` ist im Rumpf der Lambda-Funktion platziert &ndash; mit `std::invoke`,
das Funktionsobjekt selbst (`func`) wird via `[func = std::forward<TFunc>(func)]` in das Lambda-Objekt verschoben!
Dies gilt genauso für die Parameter der Funktion, nur kommt hier syntaktisch gesehen das so genannte *Variadic Capture* Sprachfeature hinzu:

```cpp
[... args = std::forward<TArgs>(args)]
```

## Beendigung der Ausführung <a name="link7"></a>

Wenn die Abarbeitung der Nachrichten beendet werden soll,
wird dies dadurch erreicht, dass eine spezielle Nachricht in die Warteschlange am Ende eingefügt wird:

```cpp
[this] { m_running = false; }
```

Mit diesem Lambda-Ausdruck wird einfach das Flag `m_running` umgesetzt,
und so die Ausführung der Verarbeitungsprozedur verlassen.

---

## Literaturhinweise <a name="link8"></a>

Die Anregungen zur Klasse `EventLoop` stammen im Wesentlichen aus dem Artikel

[Idiomatic Event Loop in C++](https://habr.com/en/articles/665730/)

von *Anton Vasin*.

---

[Zurück](../../Readme.md)

---
