# *Event Loop* &ndash; Ereigniswarteschlange

[Zurück](../../Readme.md)

---


## Motivation

Stellen Sie sich vor, Sie haben eine Reihe von Klassen,
die nicht für die Arbeit in einer Multithreading-Umgebung ausgelegt sind.

Die Gründe hierfür können unterschiedlicher Natur sein:

  * Möglicherweise wurden die Klassen von einem alten Teil eines Softwaresystems geerbt.
  * Sie entwerfen gerade neue Klassen, möchten diese aber nicht mit gleich mit
Synchronisationsmechanismen wie `std:mutex`-Objekten überfrachten.

Sie wollen jedoch von verschiedenen Threads aus auf Objekte dieser Klassen zugreifen können,
um die Dinge so einfach wie möglich zu halten.

*Kurz gefasst*:


Eine *Event Loop* (Ereigniswarteschlange) kann man als Alternative zu einem
Mutex-Objekt betrachtet.
Beide serialisieren Zugriffe auf geschützte Objekt, jedoch auf unterschiedliche Weise.

## Realisierung

### Klasse `std::function`

Um eine Ereigniswarteschlange zu realisieren, benötigt man die Möglichkeit,
&bdquo;Methodenaufrufe&rdquo; zwischenzuspeichern. Gewisse Ähnlichkeiten zum *Command Pattern*
aus dem Umfeld der *Design Pattern* sind hier vorhanden.

Hierfür kommt uns die Klasse `std::function` zur Hilfe.
Diese Klasse zeichnet sich durch zwei besondere Eigenschaften aus:

  * *Type Erasure*<br />
Auf Grund des *Type Erasure* Features kann man in einem `std::function`-Objekt alles speichern,
was man in C++ &bdquo;aufrufen&rdquo; kann:
  * C-Funktionen
  * Methoden eines Objekts
  * Aufrufbare Objekte
  * Lambdas
  * Also alles, worauf man den Anrufoperator `operator()` anwenden kann.


In den internen Daten des `std::function`-Objekts wird alles abgelegt,
was pro *Funktion* notwendig ist, wie zum Beispiel die Referenz eines Objekts,
dessen Methode aufgerufen werden soll oder die Erfassungsklausel (*Capture Clause*) eines Lambda-Objekts.

Dies geht einher mit der so genannten *Small-Object*-Optimierung:

  * *Small-Object*-Optimierung<br />
  * Um Heap-Zuweisungen zu vermeiden oder zumindest zu minimieren, speichert ein `std::function`-Objekt seine Daten im Objekt selbst ab, wenn es klein genug ist.
  * Was &bdquo;klein genug&rdquo; ist, hängt von der Umsetzung ab.
  * Was nicht unmittelbar in das `std::function`-Objekt passt, wird auf dem Heap abgelegt.

Zusammenfassend kann man nun sagen, dass *Aufrufe* bzw. *Nachrichten* in C++
in einem `std::vector`-Objekt abgelegt werden können, zum Beispiel so:


```cpp
std::vector<std::function<void()>> m_writeBuffer;
```

In diesem Beispiel handelt es sich um *Callables*, die keine Parameter entgegennehmen und auch nichts zurückliefern,
also die Schnittstelle `void()` besitzen.

Das Manko der fehlenden Parameter werden wir in unserer Realisierung abstellen.

Eine Rückgabe von Daten erscheint mir in einer Ereigniswarteschlangenrealisierung weniger relevant,
da diese letzten Endes doch &bdquo;Nachrichten abarbeitet&rdquo;, aber nicht vordergründig zur Berechnung von *Ergebnissen*
konzipiert ist.

### Klasse `std::condition_variable`

Eine *Bedingungsvariable* ist ein Synchronisationsprimitiv,
das einen Thread dazu bringt, seine Ausführung über `wait()` zu verschieben (suspendieren),
bis ein anderer Thread ihn über `notify_one()` aufweckt.

Was aber, wenn der zweite Thread `notify_one()` aufgerufen hat, kurz bevor der erste Thread `wait()` aufruft?

Dazu wird das *std::condition_variable* mit einem Mutex-Objekt kombiniert:
Das Mutex-Objekt ist zu sperren, wenn auf den Zustand (*die Bedingung*) des Szenarios zugegriffen wird.

Ist das Mutex-Objekt gesprerrt, erwartet ein `std::condition_variable`-Objekt,
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
  * Den ersten Thread mit `notify_one()` / `notify_all()` benachrichtigen


 
*Bemerkung*:<br />
Einige Entwickler rufen `notify_one()` auf, während sie das Mutex-Objekt sperren.
Das ist nicht falsch, aber es macht das System ineffizient.
Um zusätzliche Synchronisierungen zu vermeiden, stellen Sie einfach sicher, dass Sie notify_one() aufrufen, nachdem Sie den Mutex freigegeben haben.


## Double Buffering Technik

Was diese Implementierung wirklich besonders effizient macht, ist, dass wir hier zwei Puffer haben.

Möglicherweise ist Ihnen aufgefallen, dass wir readBuffer und m_writeBuffer vertauschen.

Und wir tun dies, während der Mutex gesperrt wird. std::swap tauscht einfach die Zeiger innerhalb dieser beiden Vektoren aus, was eine extrem schnelle Operation ist.

Daher lassen wir m_writeBuffer leer und bereit, erneut gefüllt zu werden.

Nach std::swap endet der Scope, der Mutex wird entsperrt.

Jetzt haben wir eine Situation, in der der Schreibpuffer gefüllt wird, während der Lesepuffer verarbeitet wird.

Jetzt können diese beiden Prozesse gleichzeitig ablaufen, ohne dass es Schnittpunkte gibt!

Wenn die Verarbeitung beendet ist, löschen wir den Lesepuffer.

Das Löschen von std::vector führt nicht zur zugrunde liegenden Speicherfreigabe.

Wenn wir also diese beiden Puffer schnell wieder austauschen, wird dieser zugrunde liegende Speicher als Schreibpuffer wieder aufgefüllt.





Wenn Sie enqueue() aufrufen und Ihr callable_t klein genug ist, berühren Sie den Heap beim Erstellen nicht einmal.

Das Einfügen dieses callable_t in den Vektor, der nach dem Verarbeitungsschritt bereits über etwas Speicherplatz verfügt, kostet fast nichts!

Wie wäre es mit dem Sperren des Mutex?

Es stellt sich heraus, dass moderne Mutexe als ersten Schritt atomare Spinlocks verwenden und nach mehreren Iterationen das Betriebssystem auffordern, den Thread zu verschieben.

Wenn Sie also zwischen lock() und unlock() etwas wirklich Schnelles tun, wird das Betriebssystem nicht einmal gestört.

notify_one() ist außerdem so konzipiert, dass es schnell ist, falls Sie es aufrufen, während der Mutex entsperrt ist.

Sie sollten sich hier also nicht darum kümmern.

Es macht enqueue() im Durchschnitt extrem schnell.

Es macht auch wait() schnell, da wir einfach prüfen, ob m_writeBuffer nicht leer ist, und es austauschen.


*"noexcept"*

---


Hinweise zur REalisierung:

Ich möchte den Rückgabetyp einer Funktion als auto kennzeichen.

Damit kann ich aber diese Funktion nicht ohne weiteres über einen Header-Datei 
bekannt machen, da ich sonst mit der Fehlermedlung 

```
A function that returns 'auto' cannot be used before it is defined
```


konfrontiert bin.

Es gibt da nur 2 Möglichkeiten: Auf auto verzichten
oder mit einem Trailing Return Type den Rückgabetyp explzit defnieren

Das Wort explizit ist hier etwas skurul. da ich ja nach wie vor 
mit der generischen Programmierung , also mich in einem Template Kontext aufhalte

Aber mit Typwandlungen der Geslta std::invoke_result lässt sich dieses Problem
auch lösen !!!

---

Man beachte in den Ausgaben:

Wenn der Event Pool beendet wird ,werden soll,
wird ein extra Ebent eingeschleut ... das erklärt die zusätzliche Ausführung eines Events.



---



[Zurück](../../Readme.md)

---
