# Das Erzeuger-Verbraucher Problem (*Producer Consumer Pattern*)

[Zur�ck](../../Readme.md)

---

## Verwendete Werkzeuge

<ins>Klassen</ins>:

  * Klassen `std::mutex`

  * Klassen `std::counting_semaphore` 
  * Klassen `std::lock_guard`


  * Klassen `std::condition_variable`
  * Klassen `std::unique_lock`

---

### Allgemeines

Das Erzeuger-Verbraucher Problem entsteht, wenn ein Erzeuger Daten produziert und diese
irgendwo ablegen muss, bis ein Verbraucher in der Lage ist, diese zu verarbeiten.

Sind keine Daten vorhanden, kann ein Verbraucher nicht arbeiten.

Das Nadel�hr zwischen dem Erzeuger und dem Verbraucher ist ein Puffer.
Dieser stellt das Bindeglied zwischen Erzeuger und Verbraucher dar.

Die L�nge eines solchen Puffers kann bei seiner Erzeugung m�glicherweise variabel eingestellt werden,
letzten Endes haben wir aber immer die Situation vorliegen, dass ein solcher Puffer eine maximale L�nge besitzt.


<img src="Producer_Consumer_Problem.png" width="600">

*Abbildung* 1: Das Erzeuger-Verbraucher Problem

  * Der Erzeuger speichert Informationen in einem l�ngenm��ig begrenzter Puffer.
  * Der Verbraucher liest Informationen aus diesem Puffer.


In der Realisierung steht man nun vor zwei Problemen:

a) Der Puffer ist leer:<br />Wenn der Puffer leer ist, muss der Verbraucher
warten, bis der Erzeuger eine Information im
Puffer abgelegt hat, und erst dann weiter
arbeiten.
  
b) Der Puffer ist voll:<br />Wenn der Puffer voll ist, muss der Erzeuger
warten, bis der Verbraucher eine Information
aus dem Puffer abgeholt hat, und erst dann
weiter arbeiten.

Da im Regelfall der Erzeuger und der Verbraucher im Kontext unterschiedlicher Threads arbeiten,
ben�tigen wir zustzlich die Werkzeuge der nebenl�ufigen Programmierung.

F�r das Erzeuger-Verbraucher Problem gibt es vor diesem Hintergrund betrachtet
mehrere L�sungsans�tze:

  * Mit Bedingungsvariablen (std::counting_semaphore)
  * Mit Semaphoren (std::condition_variable)

In jedem Fall wollen wir vermeiden, dass wir f�r das �berpr�fen eines Pufferzustands
zuviel Rechenzeit verwenden. Wir differenzieren hier zwischen zwei Ans�tzen:

  * Aktives Warten (*Busy Waiting*)<br />Diese Variante des Wartens kann man im Regelfall
in Wiederholungsschleifen beobachten. Eine solche Schleife wird solange ausgef�hrt,
bis eine Variable einen bestimmten Wert angenommen hat. Dies hat unter anderen zur Folge,
dass ein Thread aktiv sein muss und die CPU solange belegt, bis die betrachtete Variable
ihren Wert ge�nder hat.

  * Passives Warten<br />In dieser Variante wird Thread blockiert (suspendiert)
und wartet auf ein Ereignis,
das ihn wieder in den Zustand bereit versetzt. Der blockierte Thread besitzt keine CPU-Rechenzeit.
Ein anderer Thread muss das Eintreten eines Ereignisses bewirken.
Beim Eintreten des Ereignisses wird der blockierte Thread geweckt.
Zur Umsetzung dieses Ansatzes ben�tigt man Hilfsmittel des unterlagerten Betriebssystems,
rein mit den  Mitteln einer g�ngigen Programmiersprache l�sst sich dies nicht realisieren.

Im Quellcode zu diesem Abschnitt finden Sie zwei Realisierung des 
Erzeuger-Verbraucher Problems vor:

a Erzeuger-Verbraucher-Problem mit Bedingungsvariablen (wait / notify)
b Erzeuger-Verbraucher-Problem mit Semaphoren (acquire / release)



---

#### Quellcode:

[Erzeuger-Verbraucher-Problem mit Bedingungsvariablen](BlockingQueue.h).<br />
[Erzeuger-Verbraucher-Problem mit Semaphoren](BlockingQueueEx.h).<br />

---

## Literaturhinweise

Die Idee zu dem Beispiel mit der Druckerwarteschlange stammt aus dem Artikel

[Binary Semaphore Tutorial and Example](https://howtodoinjava.com/java/multi-threading/binary-semaphore-tutorial-and-example/).

Der Quellcode dieses Artikels ist zwar in Java geschrieben, l�sst sich aber sehr
einfach nach C++ portieren.

Die Anregungen zum dritten Beispiel stammen im Wesentlichen aus dem Buch

[C++ &ndash; The Complete Guide](https://www.josuttis.com/cppstd20/index.html) von Nicolai M. Josuttis.


---

[Zur�ck](../../Readme.md)

---
