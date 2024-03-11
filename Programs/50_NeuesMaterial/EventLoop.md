# *Event Loop* &ndash; Ereigniswarteschlange

[Zur�ck](../../Readme.md)

---

*Kurz gefasst*:


Eine *Event Loop* (Ereigniswarteschlange) kann als Alternative zu einem
Mutex-Objekt betrachtet werden.
Beide serialisieren Zugriffe auf gesch�tzte Objekt, jedoch auf etwas unterschiedliche Weise.

*Motivation*:

Stellen Sie sich vor, Sie haben eine Reihe von Klassen,
die nicht f�r die Arbeit in einer Multithread-Umgebung ausgelegt sind.

Vielleicht, weil sie von einem alten Teil des Systems geerbt wurden,
oder vielleicht entwerfen Sie gerade neue Klassen und m�chten sie nicht mit gleich mit
Synchronisationsmechanismen wie `std:mutex`-Objekten �berfrachten.


Sie wollen jedoch von verschiedenen Threads aus auf Objekte dieser Klassen zugreifen k�nnen,
um die Dinge so einfach wie m�glich zu halten.

*Realisierung*:

std::function ist eine wirklich interessante Sache.

Es besitzt zwei sehr wichtige Eigenschaften,
die es f�r uns so n�tzlich machen � Type Erasure und Small-Object Optimization.

Auf Grund von Typ-Erasure k�nnen wir alles speichern,
worauf wir den Anrufoperator anwenden k�nnen.

Es kann eine C-�hnliche Funktion sein, es kann ein aufrufbares Objekt sein.

Es kann auch ein Lambda sein, einschlie�lich eines generischen Lambda.

Da ein std::function-Objekt interne Daten enthalten kann,
wie z. B. die Member des Funktors oder den Capture eines Lambdas,
muss std::function auch alle diese Daten speichern.

Um Heap-Zuweisungen zu vermeiden oder zumindest zu minimieren, speichert ein std::function-Objekt sine Daten im Objekt selbst,
wenn es klein genug ist.

Was �klein genug� ist, h�ngt von der Umsetzung ab.

Wenn es nicht in den internen Speicher passt, erfolgt die Heap-Zuweisung als Fallback.

Das ist im Wesentlichen die Small-Object-Optimierung.

Wenn Sie das alles wissen, k�nnen Sie in den meisten F�llen einen std::vector der std::function verwenden, um sowohl Daten als auch Zeiger auf vtables in einem einzigen Speicherblock zu halten.

Und das ist das erste Mitglied unserer Klasse � std::vector<callable_t> m_writeBuffer;






*std::condition_variable*:

Eine Bedingungsvariable ist ein Synchronisationsprimitiv,
das einen Thread dazu bringt, seine Ausf�hrung �ber wait() zu verschieben (suspendieren),
bis ein anderer Thread ihn �ber notify_one() aufweckt.

Was aber, wenn der zweite Thread notify_one() aufgerufen hat, kurz bevor der erste Thread wait() aufruft?

Dazu wird das *std::condition_variable* mit einem Mutex-Obekt komniniert:
Das Mutex ist zu sperren, wenn auf die Zustand (*Bedingung*) des Szenarios zugegriffen wird.

Ist das Mutex-Objekt gesprerrt, m�chte std::condition_variable,
dass Sie den Status �berpr�fen, ob der erste Thread in den Ruhezustand fallen muss
oder die Bedingung bereits erf�llt wurde und der Thread einfach weitermachen muss.

Wenn sich herausstellt, dass der Thread verschoben werden muss, passiert als n�chstes etwas Interessantes.

Ist Ihnen aufgefallen, dass die Funktion wait() diesen gesperrten Mutex akzeptiert?

Dies liegt daran, dass die Funktion �wait()� Ihr Betriebssystem auffordert, Folgendes zu tun:

A) Entsperren Sie den Mutex und verschieben Sie die Ausf�hrung (Suspendiering wird aufrecht erhalten)

B) Sperren Sie den Mutex und setzen Sie die Ausf�hrung fort,
   wenn der Aufruf notify_one() / notify_all() erfolgt.


Wie Sie sehen, ben�tigen wir einen gesch�tzten Zustand, der durch den Mutex gesch�tzt wird.

Daher sollte der zweite benachrichtigende Thread Folgendes tun:

A) Sperren Sie den Mutex, �ndern Sie den freigegebenen Status und entsperren Sie den Mutex.

B) Benachrichtigen Sie den ersten Thread.

 
Bemerkung:
Einige Entwickler rufen notify_one() auf, w�hrend sie den Mutex halten.
Das ist nicht falsch, aber es macht das System ineffizient.
Um zus�tzliche Synchronisierungen zu vermeiden, stellen Sie einfach sicher, dass Sie notify_one() aufrufen, nachdem Sie den Mutex freigegeben haben.



Double Buffering Technik

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
