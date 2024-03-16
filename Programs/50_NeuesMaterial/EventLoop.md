# *Event Loop* &ndash; Ereigniswarteschlange

[Zur�ck](../../Readme.md)

---


## Motivation

Stellen Sie sich vor, Sie haben eine Reihe von Klassen,
die nicht f�r die Arbeit in einer Multithreading-Umgebung ausgelegt sind.

Die Gr�nde hierf�r k�nnen unterschiedlicher Natur sein:

  * M�glicherweise wurden die Klassen von einem alten Teil eines Softwaresystems geerbt.
  * Sie entwerfen gerade neue Klassen, m�chten diese aber nicht mit gleich mit
Synchronisationsmechanismen wie `std:mutex`-Objekten �berfrachten.

Sie wollen jedoch von verschiedenen Threads aus auf Objekte dieser Klassen zugreifen k�nnen,
um die Dinge so einfach wie m�glich zu halten.

*Kurz gefasst*:


Eine *Event Loop* (Ereigniswarteschlange) kann man als Alternative zu einem
Mutex-Objekt betrachtet.
Beide serialisieren Zugriffe auf gesch�tzte Objekt, jedoch auf unterschiedliche Weise.

## Realisierung

### Klasse `std::function`

Um eine Ereigniswarteschlange zu realisieren, ben�tigt man die M�glichkeit,
&bdquo;Methodenaufrufe&rdquo; zwischenzuspeichern. Gewisse �hnlichkeiten zum *Command Pattern*
aus dem Umfeld der *Design Pattern* sind hier vorhanden.

Hierf�r kommt uns die Klasse `std::function` zur Hilfe.
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
  * Was &bdquo;klein genug&rdquo; ist, h�ngt von der Umsetzung ab.
  * Was nicht unmittelbar in das `std::function`-Objekt passt, wird auf dem Heap abgelegt.

Zusammenfassend kann man nun sagen, dass *Aufrufe* bzw. *Nachrichten* in C++
in einem `std::vector`-Objekt abgelegt werden k�nnen, zum Beispiel so:


```cpp
std::vector<std::function<void()>> m_writeBuffer;
```

In diesem Beispiel handelt es sich um *Callables*, die keine Parameter entgegennehmen und auch nichts zur�ckliefern,
also die Schnittstelle `void()` besitzen.

Das Manko der fehlenden Parameter werden wir in unserer Realisierung abstellen.

Eine R�ckgabe von Daten erscheint mir in einer Ereigniswarteschlangenrealisierung weniger relevant,
da diese letzten Endes doch &bdquo;Nachrichten abarbeitet&rdquo;, aber nicht vordergr�ndig zur Berechnung von *Ergebnissen*
konzipiert ist.

### Klasse `std::condition_variable`

Eine *Bedingungsvariable* ist ein Synchronisationsprimitiv,
das einen Thread dazu bringt, seine Ausf�hrung �ber `wait()` zu verschieben (suspendieren),
bis ein anderer Thread ihn �ber `notify_one()` aufweckt.

Was aber, wenn der zweite Thread `notify_one()` aufgerufen hat, kurz bevor der erste Thread `wait()` aufruft?

Dazu wird das *std::condition_variable* mit einem Mutex-Objekt kombiniert:
Das Mutex-Objekt ist zu sperren, wenn auf den Zustand (*die Bedingung*) des Szenarios zugegriffen wird.

Ist das Mutex-Objekt gesprerrt, erwartet ein `std::condition_variable`-Objekt,
dass der Status (die Bedingung) �berpr�ft wird, ob der erste Thread in den Ruhezustand fallen muss
oder die Bedingung bereits erf�llt wurde und der Thread einfach weiterarbeiten kann.

Wenn sich herausstellt, dass der Thread zu blockieren ist, tritt folgender Ablauf ein:

Die `wait()`-Methode bekommt ein gesperrtes Mutex-Objekt als Parameter �bergeben:

Mit diesem Mutex-Objekt geht folgende Arbeitsweise einher:

  * Das Mutex-Objekt wird im Kontext der `wait()`-Methode entsperrt, damit die Ausf�hrung anderer Threads weiter erfolgen kann.
  * Es wird zu bestimmten Zeitpunkten eine &bdquo;Kontrollfunktion&rdquo; aufgerufen, die �berpr�ft, ob die Suspendierung des wartenden Threads weiter aufrecht zu erhalten ist oder nicht.
  * Zu diesem Zweck wird das Mutex-Objekt gesperrt und nach dem Aufruf der Kontrollfunktion wieder entsperrt.
  * Kommt die Kontrollfunktion zu der Erkenntnis, dass die Bedingung f�r eine Weiterarbeit gegeben ist, sperrt sie das Mutex-Objekt wieder und setzt die Ausf�hrung fort.
  Dies geht im Regelfall damit einher, dass ein Aufruf von `notify_one()` / `notify_all()` erfolgte.


Es gibt also einen gemeinsamen Zustand, der durch das Mutex-Objekt gesch�tzt wird.

Daher sollte der zweite, benachrichtigende Thread Folgendes tun:

  * Das Mutex-Objekt sperren, den gemeinsamen Status �ndern und das Mutex-Objekt wieder entsperren.
  * Den ersten Thread mit `notify_one()` / `notify_all()` benachrichtigen


 
*Bemerkung*:<br />
Einige Entwickler rufen `notify_one()` auf, w�hrend sie das Mutex-Objekt sperren.
Das ist nicht falsch, aber es macht das System ineffizient.
Um zus�tzliche Synchronisierungen zu vermeiden, stellen Sie einfach sicher, dass Sie notify_one() aufrufen, nachdem Sie den Mutex freigegeben haben.


## Double Buffering Technik

Was diese Implementierung wirklich besonders effizient macht, ist, dass wir hier zwei Puffer haben.

M�glicherweise ist Ihnen aufgefallen, dass wir readBuffer und m_writeBuffer vertauschen.

Und wir tun dies, w�hrend der Mutex gesperrt wird. std::swap tauscht einfach die Zeiger innerhalb dieser beiden Vektoren aus, was eine extrem schnelle Operation ist.

Daher lassen wir m_writeBuffer leer und bereit, erneut gef�llt zu werden.

Nach std::swap endet der Scope, der Mutex wird entsperrt.

Jetzt haben wir eine Situation, in der der Schreibpuffer gef�llt wird, w�hrend der Lesepuffer verarbeitet wird.

Jetzt k�nnen diese beiden Prozesse gleichzeitig ablaufen, ohne dass es Schnittpunkte gibt!

Wenn die Verarbeitung beendet ist, l�schen wir den Lesepuffer.

Das L�schen von std::vector f�hrt nicht zur zugrunde liegenden Speicherfreigabe.

Wenn wir also diese beiden Puffer schnell wieder austauschen, wird dieser zugrunde liegende Speicher als Schreibpuffer wieder aufgef�llt.





Wenn Sie enqueue() aufrufen und Ihr callable_t klein genug ist, ber�hren Sie den Heap beim Erstellen nicht einmal.

Das Einf�gen dieses callable_t in den Vektor, der nach dem Verarbeitungsschritt bereits �ber etwas Speicherplatz verf�gt, kostet fast nichts!

Wie w�re es mit dem Sperren des Mutex?

Es stellt sich heraus, dass moderne Mutexe als ersten Schritt atomare Spinlocks verwenden und nach mehreren Iterationen das Betriebssystem auffordern, den Thread zu verschieben.

Wenn Sie also zwischen lock() und unlock() etwas wirklich Schnelles tun, wird das Betriebssystem nicht einmal gest�rt.

notify_one() ist au�erdem so konzipiert, dass es schnell ist, falls Sie es aufrufen, w�hrend der Mutex entsperrt ist.

Sie sollten sich hier also nicht darum k�mmern.

Es macht enqueue() im Durchschnitt extrem schnell.

Es macht auch wait() schnell, da wir einfach pr�fen, ob m_writeBuffer nicht leer ist, und es austauschen.


*"noexcept"*

---


Hinweise zur REalisierung:

Ich m�chte den R�ckgabetyp einer Funktion als auto kennzeichen.

Damit kann ich aber diese Funktion nicht ohne weiteres �ber einen Header-Datei 
bekannt machen, da ich sonst mit der Fehlermedlung 

```
A function that returns 'auto' cannot be used before it is defined
```


konfrontiert bin.

Es gibt da nur 2 M�glichkeiten: Auf auto verzichten
oder mit einem Trailing Return Type den R�ckgabetyp explzit defnieren

Das Wort explizit ist hier etwas skurul. da ich ja nach wie vor 
mit der generischen Programmierung , also mich in einem Template Kontext aufhalte

Aber mit Typwandlungen der Geslta std::invoke_result l�sst sich dieses Problem
auch l�sen !!!

---

Man beachte in den Ausgaben:

Wenn der Event Pool beendet wird ,werden soll,
wird ein extra Ebent eingeschleut ... das erkl�rt die zus�tzliche Ausf�hrung eines Events.



---



[Zur�ck](../../Readme.md)

---
