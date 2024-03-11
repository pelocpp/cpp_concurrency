# *Event Loop* &ndash; Ereigniswarteschlange

[Zurück](../../Readme.md)

---

*Kurz gefasst*:


Eine *Event Loop* (Ereigniswarteschlange) kann als Alternative zu einem
Mutex-Objekt betrachtet werden.
Beide serialisieren Zugriffe auf geschützte Objekt, jedoch auf etwas unterschiedliche Weise.

*Motivation*:

Stellen Sie sich vor, Sie haben eine Reihe von Klassen,
die nicht für die Arbeit in einer Multithread-Umgebung ausgelegt sind.

Vielleicht, weil sie von einem alten Teil des Systems geerbt wurden,
oder vielleicht entwerfen Sie gerade neue Klassen und möchten sie nicht mit gleich mit
Synchronisationsmechanismen wie `std:mutex`-Objekten überfrachten.


Sie wollen jedoch von verschiedenen Threads aus auf Objekte dieser Klassen zugreifen können,
um die Dinge so einfach wie möglich zu halten.

*Realisierung*:

std::function ist eine wirklich interessante Sache.

Es besitzt zwei sehr wichtige Eigenschaften,
die es für uns so nützlich machen – Type Erasure und Small-Object Optimization.

Auf Grund von Typ-Erasure können wir alles speichern,
worauf wir den Anrufoperator anwenden können.

Es kann eine C-ähnliche Funktion sein, es kann ein aufrufbares Objekt sein.

Es kann auch ein Lambda sein, einschließlich eines generischen Lambda.

Da ein std::function-Objekt interne Daten enthalten kann,
wie z. B. die Member des Funktors oder den Capture eines Lambdas,
muss std::function auch alle diese Daten speichern.

Um Heap-Zuweisungen zu vermeiden oder zumindest zu minimieren, speichert ein std::function-Objekt sine Daten im Objekt selbst,
wenn es klein genug ist.

Was „klein genug“ ist, hängt von der Umsetzung ab.

Wenn es nicht in den internen Speicher passt, erfolgt die Heap-Zuweisung als Fallback.

Das ist im Wesentlichen die Small-Object-Optimierung.

Wenn Sie das alles wissen, können Sie in den meisten Fällen einen std::vector der std::function verwenden, um sowohl Daten als auch Zeiger auf vtables in einem einzigen Speicherblock zu halten.

Und das ist das erste Mitglied unserer Klasse – std::vector<callable_t> m_writeBuffer;






*std::condition_variable*:

Eine Bedingungsvariable ist ein Synchronisationsprimitiv,
das einen Thread dazu bringt, seine Ausführung über wait() zu verschieben (suspendieren),
bis ein anderer Thread ihn über notify_one() aufweckt.

Was aber, wenn der zweite Thread notify_one() aufgerufen hat, kurz bevor der erste Thread wait() aufruft?

Dazu wird das *std::condition_variable* mit einem Mutex-Obekt komniniert:
Das Mutex ist zu sperren, wenn auf die Zustand (*Bedingung*) des Szenarios zugegriffen wird.

Ist das Mutex-Objekt gesprerrt, möchte std::condition_variable,
dass Sie den Status überprüfen, ob der erste Thread in den Ruhezustand fallen muss
oder die Bedingung bereits erfüllt wurde und der Thread einfach weitermachen muss.

Wenn sich herausstellt, dass der Thread verschoben werden muss, passiert als nächstes etwas Interessantes.

Ist Ihnen aufgefallen, dass die Funktion wait() diesen gesperrten Mutex akzeptiert?

Dies liegt daran, dass die Funktion „wait()“ Ihr Betriebssystem auffordert, Folgendes zu tun:

A) Entsperren Sie den Mutex und verschieben Sie die Ausführung (Suspendiering wird aufrecht erhalten)

B) Sperren Sie den Mutex und setzen Sie die Ausführung fort,
   wenn der Aufruf notify_one() / notify_all() erfolgt.


Wie Sie sehen, benötigen wir einen geschützten Zustand, der durch den Mutex geschützt wird.

Daher sollte der zweite benachrichtigende Thread Folgendes tun:

A) Sperren Sie den Mutex, ändern Sie den freigegebenen Status und entsperren Sie den Mutex.

B) Benachrichtigen Sie den ersten Thread.

 
Bemerkung:
Einige Entwickler rufen notify_one() auf, während sie den Mutex halten.
Das ist nicht falsch, aber es macht das System ineffizient.
Um zusätzliche Synchronisierungen zu vermeiden, stellen Sie einfach sicher, dass Sie notify_one() aufrufen, nachdem Sie den Mutex freigegeben haben.



Double Buffering Technik

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
