# Realisierung eines Thread Pools

[Zur�ck](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klasse `std::atomic_bool`
  * Klasse `std::function`
  * Klasse `std::thread`
  * Eine Klasse f�r eine *ThreadsafeQueue*

<ins>Funktionen</ins>:

  * Funktion `std::thread::hardware_concurrency`

---

## Allgemeines

Ein *Thread Pool* erm�glicht es, Threads wiederzuverwenden.
Auf diese Weise wird verhindert, dass zur Laufzeit neue Threads erstellt werden m�ssen.
Das Erstellen neuer Threads ist zeit- und ressourcenintensiv. 

Wir stellen in diesem Projekt einige Thread Pool Realisierungen vor.

---

## Eine sehr einfache Thread Pool Realisierung

> Literaturhinweise: Diese Realisierung stammt aus dem Buch &bdquo;Concurrency in Action - 2nd Edition&rdquo; von
Anthony Williams, Kapitel 9.1.

In dieser Realisierung besteht der Thread Pool aus einer festen Anzahl von Worker Threads.
Typischerweise wird diese Anzahl von der Funltion `std::thread::hardware_concurrency()` festgelegt.

Steht eine Aufgabe (*Task*) zur Ausf�hrung an, gibt es am Thread Pool eine Methode (hier: `submit`),
die diese Methode/Funktion in die Warteschlange aller noch ausstehenden Tasks am Ende hinzuf�gt.

Jeder Worker Thread entnimmt, wenn er nichts zu tun hat, eine Task vom Anfang dieser Warteschlange und f�hrt die Funktion aus.
Nach Ausf�hrung der Funktion entnimmt der Worker Thread die n�chste Taks aus der Warteschlange
oder er begibt sich in einen *Idle*-Zustand, wenn die Warteschlange leer ist.

In der aktuellen Realisierung haben die Tasks in der Warteschlange alle den R�ckgabetyp `void`,
es gibt also keine direkte M�glichkeit, ein Ergebnis zur�ckzuliefern.

Auch gibt es keine M�glichkeit, auf das Ende der Ausf�hrung einer Task zu warten.

Der gr��te Nachteil in dieser ersten Realisierung besteht jedoch darin,
dass die Worker Threads, die sich im *Idle*-Zustand befinden, aktiv den Zustand der Warteschlange �berpr�fen.
Wir haben es also mit dem so genannten &bdquo;*Busy Polling*&rdquo; zu tun.
Diesen Nachteil werden wir in einer nachfolgenden Variation beheben.

---

### Weitere Hinweise zur Realisierung

Die `worker_thread`-Funktion selbst ist recht einfach:
Sie befindet sich in einer Wiederholungsschleife und wartet, bis das `m_done`-Flag gesetzt ist,
entnimmt Tasks aus der Warteschlange und f�hrt sie in der Zwischenzeit aus.

Wenn sich keine Tasks in der Warteschlange befinden, ruft die Funktion `std::this_thread::yield()` auf,
um zumindest eine kleine Pause einzulegen und einem anderen Thread die M�glichkeit zu geben,
etwas Arbeit in die Warteschlange zu stellen, bevor er beim n�chsten Mal wieder versucht, etwas zu entnehmen.

Beachte, dass die Reihenfolge der Deklarationen der Instanzvariablen von Klasse `ThreadPool` wichtig ist:

```cpp
01: class ThreadPool
02: {
03: private:
04:     std::atomic_bool                        m_done;
05:     ThreadsafeQueue<std::function<void()>>  m_workQueue;
06:     std::vector<std::thread>                m_threads;
07:     JoinThreads                             m_joiner;
08: 
09: ...
```

Sowohl das `m_done`-Flag als auch das Objekt `m_workQueue` m�ssen vor dem Vektor der Threads `m_threads` deklariert werden,
der wiederum vor dem Objekts des Typs `JoinThreads` deklariert werden muss:


Dadurch wird sichergestellt, dass die Mitglieder in der richtigen Reihenfolge zerst�rt werden.
Das `m_joiner`-Objekt ist in seinem Destruktor daf�r verantwortlich, auf das Ende aller Threads zu warten.
Erst danach kann man die beiden Objekte mit den Threads und den Tasks sicher zerst�ren,
wenn alle Worker Threads gestoppt worden sind.

---

## Literaturhinweise

Das erste Beispiel ist aus dem Buch &bdquo;Concurrency in Action - 2nd Edition&rdquo; von
Anthony Williams, Kapitel 9.1, entnommen.

---


[Zur�ck](../../Readme.md)

---

