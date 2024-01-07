# Gegenseitiger Ausschluss: Klasse `std::mutex`

[Zur�ck](../../Readme.md)

Aus *Wikipedia*:

"Der Begriff "**gegenseitiger Ausschluss**" bzw. *Mutex* (Abk. f�r engl. *Mutual Exclusion*) bezeichnet eine Gruppe von Verfahren,
mit denen das Problem des kritischen Abschnitts gel�st wird.
Mutex-Verfahren verhindern, dass nebenl�ufige Prozesse bzw. Threads gleichzeitig oder zeitlich verschr�nkt
gemeinsam genutzte Datenstrukturen unkoordiniert ver�ndern, wodurch die Datenstrukturen in einen inkonsistenten Zustand geraten k�nnen,
auch wenn die Aktionen jedes einzelnen Prozesses oder Threads f�r sich betrachtet konsistenzerhaltend sind.

Mutex-Verfahren koordinieren den zeitlichen Ablauf nebenl�ufiger Prozesse/Threads derart,
dass andere Prozesse/Threads von der Ausf�hrung kritischer Abschnitte ausgeschlossen sind,
wenn sich bereits ein Prozess/Thread im kritischen Abschnitt befindet (die Datenstruktur ver�ndert)."

---

#### Mehrere *Lock*-Klassen:

  * Klasse `std::lock_guard`:<br/>
    Die Klasse `std::lock_guard` ist seit C++ 11 verf�gbar. Sie wurde �berarbeitet,
    was allerdings zu einer neuen Klasse `std::scoped_lock` f�hrte.
    In neuem Quellcode sollte die Klasse `std::lock_guard` nicht mehr zum Einsatz kommen,
    sie wird nur noch aus Kompatibilit�tsgr�nden unterst�tzt.

  * Klasse `std::scoped_lock`:<br/>
    Die Klasse `std::scoped_lock` kann man als eine strikte Obermenge der Klasse
    `std::lock_guard` ansehen.
    Der einzige und wesentliche Unterschied besteht darin,
    dass `std::scoped_lock` einen variadischen Konstruktor hat,
    der mehr als einen Mutex aufnehmen kann.
    Damit l�sst sich eine gleichzeitige Sperre von mehreren Mutex-Objekten durchf�hren.
    Die Klasse `std::scoped_lock` unterst�tzt keine *Move*-Semantik.

```cpp
{
    std::scoped_lock<std::mutex, std::mutex> raii(mutex1, mutex2);  
    ...   
}
```

  * Klasse `std::unique_lock`:<br/>
    Die Klasse `std::unique_lock` bietet eine Reihe von Funktionalit�ten,
    von denen einige besonders beim gleichzeitigen Sperren mehrerer Mutexe anwendbar sind.
    Dazu z�hlen zum Beispiel das "*Deferred Locking*"  und "*Timeout Locks*".
    Auch unterst�tzt die Klasse `std::unique_lock` die *Move*-Semantik.

  * Klasse `std::shared_lock`:<br/>
    Ein `std::shared_lock`-Objekt kann im Zusammenspiel mit einem oder mehreren
    `std::unique_lock`-Objekten verwendet werden,
    um mehrere *Leser* (Konsument) und einen exklusiven *Schreiber* (Produzent) zuzulassen.

---

#### Das Monitor-Konzept von Djikstra:

<img src="Dijkstra_2.png" width="150">

*Abbildung* 1: Edsger W. Dijkstra, 2002.

<img src="Monitor_01.png" width="600">

*Abbildung* 2: Anschauliche Darstellung eines Monitors: Ein Geb�ude mit drei R�umen, in dem sich beliebig viele Threads nach bestimmten Spielregeln bewegen d�rfen..

<img src="Monitor_02.png" width="600">

*Abbildung* 3: Erste Veranschaulichung eines Monitors: Aspekt des gegenseitigen Ausschlusses.

<img src="Monitor_03.png" width="600">

*Abbildung* 4: Zweite, vollst�ndige Veranschaulichung eines Monitors: Aspekt der Koordination von Threads.

---

#### Quellcode:

  * Elementare Demonstration des `std::mutex` Objekts:<br />
    Die parallele Ausgabe mit `std::cout` kann mit und ohne *Mutex*-Sperre beobachtet werden.<br />
    [Beispiel](Simple_Mutex.cpp).

  * Elementare Demonstration des `std::condition_variable` Objekts:<br />
    [Beispiel](Simple_Condition_Variable_01.cpp).
    [Beispiel](Simple_Condition_Variable_02.cpp).
   
  * Demonstration des klassischen Konsumenten/Verbraucher-Problems:<br />
    [Beispiel einer Klasse `ConsumerProducerOne::ConsumerProducer`](Producer_Consumer_01.cpp).<br />
    [Beispiel einer Klasse `ConsumerProducerTwo::ConsumerProducer`](Producer_Consumer_02.cpp).<br />
    [Beispiel einer Klasse `ConsumerProducerThree::ConsumerProducer`](Producer_Consumer_03.cpp).<br />

---

**Beachte**:

Siehe das Thema

[Do I have to acquire lock before calling std::condition_variable.notify_one()?](https://stackoverflow.com/questions/17101922/do-i-have-to-acquire-lock-before-calling-condition-variable-notify-one)

---

**Beachte**:

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

Das hei�t inbesondere, dass vor dem ersten eigentlichen Warten das Pr�dikat ausgewertet wird!

---

In [Visual C#](https://amazon.de/Objektorientiertes-Programmieren-methodische-Einf�hrung-Fortgeschrittene/dp/3866454066)
gibt es eine ausf�hrliche Beschreibung zum Thema *Mutual Exclusion*, das dort
die Programmiersprache C# zugrunde legt. Die Ausf�hrungen lassen sich ohne Weiteres auch auf andere
Programmiersprachen �bertragen, wie zum Beispiel C++.

---

## Konkurrierender Zugriff auf Objekte 

### Probleme des konkurrierenden Zugriffs auf ein Objekt

Im letzten Kapitel haben wir eine Variable betrachtet, die durch mehrere Threads
schreibend oder lesend in die Mangel genommen wurde.
In diesem Kapitel drehen wir den Spie� um und wenden uns Objekten zu,
verbunden mit der Fragestellung:
D�rfen eine oder unterschiedliche Methoden desselben Objekts jederzeit - ohne Auftreten von Fehlern - von mehreren Threads durchlaufen werden?
Wie im letzten Abschnitt n�hern wir uns der detaillierten Analyse dieser Frage wieder mit einem einfachen Beispiel,
um potentielle kritische Situationen exakt darlegen zu k�nnen.
In
*Listing* 1 stellen wir eine m�glichst einfache - und damit f�r anspruchsvollere Programme auch nicht
sonderlich geeignete - Implementierung eines Stapels bereit
mit der Intention, seine `Push`- und `Pop`-Methode
intensiv einem Belastungstest durch mehrere Threads auszusetzen:
      

```cpp
01: class Stack
02: {
03:     private int[] buf;
04:     private int index;
05: 
06:     public Stack ()
07:     {
08:         this.buf = new int[10];
09:         this.index = 0;
10:     }
11: 
12:     public void Push (int n)
13:     {
14:         if (this.IsFull ())
15:             return;
16: 
17:         this.buf[this.index] = n;
18:         this.index ++;
19:     }
20: 
21:     public int Pop () 
22:     {
23:         if (this.IsEmpty ())
24:             return -1;
25: 
26:         this.index --;
27:         return this.buf[this.index];
28:     }
29: 
30:     public bool IsEmpty () 
31:     {
32:         return this.index == 0;
33:     }
34: 
35:     public bool IsFull () 
36: 
37:     {
38:         return this.index == this.buf.Length;
39:     }
40: }
```

###### Listing 1: Einfache Implementierung eines Stapels.

Die Realisierung des Stapels in
*Listing* 1
basiert ganz schlicht auf einem `int`-Array fester L�nge,
das die abzuspeichernden `int`-Elemente unter Zuhilfenahme eines Positionszeigers verwaltet.
Damit die beiden Operationen
`Push` und `Pop` ohne Verletzung der Arraygrenzen ausgef�hrt werden k�nnen,
besitzt die `Stack`-Klasse zus�tzlich
die Methoden `IsEmpty` und `IsFull`.
Im Mittelpunkt des Stresstests stehen die Methoden `Push` und `Pop`,
wir lassen sie mit den folgenden zwei Stressmethoden
`FillStack` und `EmptyStack` aus der Klasse `StressClient`
zu einem Belastungstest "gegeneinander" antreten
(*Listing* 2):
      

```cpp
01: class StressClient
02: {
03:     public static void FillStack (Object o)
04:     {
05:         Stack s = (Stack) o;
06:         for (int i = 0; i < Int32.MaxValue; i ++)
07:             s.Push (i);
08:     }
09: 
10:     public static void EmptyStack (Object o)
11:     {
12:         Stack s = (Stack) o;
13:         for (int i = 0; i < Int32.MaxValue; i ++)
14:             s.Pop ();
15:     }
16: }
```

###### Listing 2: Die Klasse `StressClient` mit den Stressmethoden `FillStack` und `EmptyStack`


Die Methode `FillStack` versucht ununterbrochen, ein Element auf dem Stapel abzulegen.
Zum Ausgleich tr�gt die Methode `EmptyStack` pausenlos dazu bei, vorhandene Elemente vom Stapel wieder zu entfernen.
Beide Stressmethoden halten sich an die Spielregeln der `Stack`-Klasse, da bei jedem Aufruf von
`Push` bzw. `Pop`
mit dem entsprechenden Aufruf von `IsEmpty` bzw. `IsFull`
die Integrit�t des Stapels �berpr�ft wird.
Vor dem Hintergrund der Fragestellung dieses Kapitels lassen wir die zwei Methoden `FillStack` und
`EmptyStack` in
*Listing* 3
nebenl�ufig im Kontext zweier Threads laufen:
      

```cpp
01: class Program
02: {
03:     public static void Main()
04:     {
05:         ParameterizedThreadStart ps1 =
06:             new ParameterizedThreadStart(StressClient.FillStack);
07:         ParameterizedThreadStart ps2 =
08:             new ParameterizedThreadStart(StressClient.EmptyStack);
09: 
10:         Thread t0 = new Thread(ps1);
11:         Thread t1 = new Thread(ps2);
12: 
13:         Stack s = new Stack();
14: 
15:         t0.Start(s);
16:         t1.Start(s);
17: 
18:         t0.Join();
19:         t1.Join();
20:     }
21: }
```

###### Listing 3: Zwei Threads testen die Klasse `Stack`.

Einige Programml�ufe der `StressTest`-Methode aus
*Listing* 3
verursachen keine Auff�lligkeiten, andere hingegen beenden die Programmausf�hrung vorzeitig mit
folgenden Fehlermeldungen in der Konsole:
      
```
Unhandled Exception: System.IndexOutOfRangeException:
Index was outside the bounds of the array.
   at Stack.Pop() in d:\Synchronisation.cs:line 27
   at StressClient.EmptyStack() in d:\Synchronisation.cs:line 103
```

Wir erkennen an den Ausnahmen, dass der Zugriff auf das `buf`-Array mit einem unzul�ssigen Index erfolgte.
Auf der anderen Seite ist jeder Aufruf von `Push` oder `Pop` mit einer entsprechenden
Vorsichtsma�nahme (Aufruf von `IsEmpty` bzw. `IsFull`) gesch�tzt.
Wie kann es trotzdem zu Ausnahmen kommen? Wir analysieren zu diesem Zweck eine ganz bestimmte Folge
von `Push`- und `Pop`-Aufrufen,
wie sie in der Praxis auftreten k�nnte und garnieren das Ganze nat�rlich geschickt mit zwei Threadwechseln.
Unsere Ausgangssituation finden Sie in
*Abbildung* 1
vor: Der aktive Thread T<sub>1</sub> hat die `Pop`-Methode betreten und
den Positionszeiger bereits dekrementiert. Dabei spielt es keine Rolle, an welcher Position im Stapel
wir uns augenblicklich befinden.
      

<img src="Sync_Study_01.png" width="400">

*Abbildung* 1: Die Ausf�hrung der `Pop`-Methode wird im Kontext von Thread T<sub>1</sub> nach dem Dekrementieren des Positionszeigers unterbrochen.

Wie Sie vermutlich erwarten, verdr�ngen wir nun Thread T<sub>1</sub>, sprich die noch ausstehende Anweisung
      
```
return buf[index];
```

in der `Pop`-Methode kommt erst zu einem sp�teren Zeitpunkt zum Zuge.
Bitte beachten Sie: Wir verletzen bereits zum jetzigen Zeitpunkt die Integrit�t des Stapelobjekts,
da eine nun folgende `Push`-Operation genau das Element im Stapel �berschreibt, das
von der zuvor unterbrochenen `Pop`-Methode noch zur�ckzuliefern ist.
Wenn gleich bereits dieser Fehler inakzeptabel ist, f�hrt er zumindest noch nicht zu einem Absturz der
`Push`-Methode. Wir treiben die Analyse deshalb noch ein St�ck voran,
um auch noch dem zweiten Fehler auf die Spur zu kommen.
      
An den Thread T<sub>2</sub>, der nun das Stapelobjekt betritt und eine Reihe von `Push`-Operationen durchf�hrt,
stellen wir keine besonderen Anspr�che - von einer Ausnahme abgesehen:
Vor der erneuten Zuteilung der Programmausf�hrung
an Thread T<sub>1</sub> muss der Stapel voll sein. Der Positionszeiger `index` muss folglich den Wert 5 besitzen,
siehe *Abbildung* 2:
      

<img src="Sync_Study_02.png" width="400">

*Abbildung* 2: Nach mehreren `Push`-Aufrufen im Kontext von Thread T<sub>2</sub> ist das Stapelobjekt voll, der Positionszeiger besitzt den Wert 5..

Wir kehren zur�ck zu Thread T<sub>1</sub>.
Dieser hatte beim letzten Threadwechsel die `Pop`-Methode noch nicht vollst�ndig abgearbeitet,
der Zugriff auf das `int`-Array in der Anweisung
      
```
return buf[index];
```
      
mit Positionszeiger 5 f�hrt nun zum Eintreten
einer `IndexOutOfRangeException`-Ausnahme im Stapelobjekt
(*Abbildung* 3):
      

<img src="Sync_Study_03.png" width="400">

*Abbildung* 3: Das Wiederaufsetzen in der `Pop`-Methode f�hrt zu einem Absturz, da ein Zugriff auf das Datenarray `buf` mit Index 5 nicht zul�ssig ist.

Wir erkennen an diesem Beispiel, dass die Unterbrechung der `Pop`-Methode
im Zusammenspiel mit einem Threadwechsel dazu f�hrt, dass der interne Zustand des Stapelobjekts fehlerhaft wird.
Die Integrit�t des Stapelobjekts wird auf der anderen Seite aber nur dann verletzt, wenn sich zwei Ausf�hrungen
der `Push` und `Pop`-Methode im Kontext *unterschiedlicher* Threads �berlappen.
Wir sind bei einem der elementarsten Probleme der Parallelprogrammierung angelangt,
dem Wunsch nach dem *gegenseitigen Ausschluss* (engl. *mutual exclusion*) zweier Aktionen.
      

Um den .NET-Entwickler bei solch grundlegenden Problemen nicht im Regen stehen zu lassen, gibt es ab dem .NET-Framework einen neuen
Namensraum `System.Collections.Concurrent`,
der unter anderem eine thread-sichere Stapelklasse `ConcurrentStack<>` enth�lt.

Mit ihrem Einsatz k�nnen wir die mit viel M�he konzipierte Klasse `Stack` aus
*Listing* 3 getrost zur Seite legen.

Sollten Sie noch nicht das endg�ltige Vertrauen in die Thread-Sicherheit dieser Klasse haben,
k�nnen Sie ja den Testrahmen aus *Listing* 4
anpassen und stattdessen ein `ConcurrentStack<>`-Objekt testen:
      

```cpp
01: class StressClient
02: {
03:     public static void FillStack (Object o)
04:     {
05:         ConcurrentStack<int> s = (ConcurrentStack<int>)o;
06:         for (int i = 0; i < Int32.MaxValue; i ++)
07:             s.Push (i);
08:     }
09: 
10:     public static void EmptyStack (Object o)
11:     {
12:         ConcurrentStack<int> s = (ConcurrentStack<int>)o;
13:         int data;
14:         for (int i = 0; i < Int32.MaxValue; i++)
15:             s.TryPop(out data);
16:     }
17: }
18: 
19: class Program
20: {
21:     public static void Main()
22:     {
23:         ParameterizedThreadStart ps1 =
24:             new ParameterizedThreadStart(StressClient.FillStack);
25:         ParameterizedThreadStart ps2 =
26:             new ParameterizedThreadStart(StressClient.EmptyStack);
27: 
28:         Thread t0 = new Thread(ps1);
29:         Thread t1 = new Thread(ps2);
30: 
31:         ConcurrentStack<int> s = new ConcurrentStack<int>();
32: 
33:         t0.Start(s);
34:         t1.Start(s);
35: 
36:         t0.Join();
37:         t1.Join();
38:     }
39: }
```

###### Listing 4: Die thread-sichere Klasse `ConcurrentStack<>`


### Kritische Abschnitte und das Monitorprinzip

In der Betriebssystemprogrammierung bezeichnet man Codesequenzen, die sich w�hrend ihrer Ausf�hrung nicht �berschneiden d�rfen,
als *kritischen Abschnitt* (engl. *critical section*).

Die Behandlung des gegenseitigen Ausschlusses erfolgt in .NET mit dem so genannten *Monitorprinzip*
Dieses Konzept geht urspr�nglich von Edsger Wybe Dijkstra konzipiert
und 1965 in seinem Artikel "Cooperating sequential processes" vorgestellt.

Dijkstra war ein niederl�ndischer Informatiker, unter anderem Wegbereiter der strukturierten Programmierung,
Erfinder eines nach ihm benannten Algorithmus zur Berechnung eines k�rzesten Weges in einem Graphen
und an der Einf�hrung des Moitorkonzepts zur Synchronisation von Threads beteiligt.

<img src="Dijkstra_2.png" width="200">

*Abbildung* 4: Edsger W. Dijkstra, 2002.

      
Anschaulich betrachtet kann man sich unter einem Monitor ein Geb�ude vorstellen, das aus drei R�umen besteht
(siehe *Abbildung* 5).
Im Mittelpunkt dieses Geb�udes befindet sich der *Behandlungsraum*,
ein spezieller Raum, der zu einem bestimmten Zeitpunkt nur von einem Thread
betreten werden darf. In der Regel erfolgt in diesem Raum der Zugriff auf die sensiblen Daten eines Objekts,
dessen Integrit�t durch den Monitor zu gew�hrleisten ist.
Da in einem Umfeld paralleler Aktivit�ten stets mehrere Threads um den Zugang zum Monitor konkurrieren k�nnen,
kann der Behandlungsraum nur �ber ein *Wartezimmer* betreten werden.
Im Wartezimmer k�nnen sich nat�rlich mehrere Threads aufhalten,
in den Behandlungsraum kann aber immer nur genau ein Thread eintreten.
In der Zeit, die sich vom Betreten des Behandlungsraums bis hin zum Verlassen erstreckt,
hat dieser Thread exklusiven Zugriff zu allen Daten des Objekts.
Vom Behandlungszimmer verzweigt noch eine T�re zu einem *Interimsraum*.
Dieses Zimmer ist f�r die Zusammenarbeit von mehreren Threads von Bedeutung, auf die wir noch
zu sprechen kommen.
      

<img src="Monitor_01.png" width="600">

*Abbildung* 5: Anschauliche Darstellung eines Monitors: Ein Geb�ude mit drei R�umen, in dem sich beliebig viele Threads nach bestimmten Spielregeln bewegen d�rfen..

Einige Hinweise zur Begriffsbildung in diesem Zusammenhang:
Das Betreten des Monitorgeb�udes bezeichnet man auch als "den Monitor betreten" (engl. *entering the monitor*),
das Betreten des Behandlungsraums wird als "den Monitor belegen" (engl. *acquiring the monitor*) bezeichnet.
Der einzige Thread, der sich zu einem bestimmten
Zeitpunkt im Behandlungsraum aufh�lt, "besitzt den Monitor" (engl. *owning the monitor*).
Dieser Thread wird auch der "aktive Thread" des Monitors genannt (engl. *active thread*).
Das Verlassen des Behandlungsraums nennt man "den Monitor freigeben" (engl. *releasing the monitor*),
das Verlassen des gesamten Monitorgeb�udes
rangiert unter der Formulierung "den Monitor verlassen" (engl. *exiting the monitor*).
    
Neben den Daten, die ein Monitor sch�tzt, ordnet man einem Monitor auch eine Reihe von Operationen zu,
die wir im folgenden als *Monitoroperationen* bezeichnen wollen. Eine Monitoroperation bezeichnet eine Folge von Anweisungen,
die (in Bezug auf einen bestimmten Monitor) als unteilbare Operation auszuf�hren sind.
Anders ausgedr�ckt: Ein Thread muss
in der Lage sein, eine Monitoroperation vom Anfang bis zum Ende ausf�hren zu k�nnen, ohne dass ein anderer Thread zur selben
Zeit ebenfalls eine Operation desselben Monitors ausf�hrt.
Ein Monitor erzwingt eine
"*one-thread-at-a-time*"-Ausf�hrung aller seiner Monitoroperationen.

Die einzige M�glichkeit, um einen Monitor betreten zu k�nnen, besteht darin,
dass ein Thread in seinem Kontext eine der Monitoroperationen aufruft.
Zu diesem Zweck bedarf es einer speziellen Anweisung am Anfang der Monitoroperation,
die dem Thread den Eintritt in das Wartezimmer des Monitors erm�glicht.
Technisch formuliert hei�t das, dass der Thread blockiert wird und seine
Referenz in einer Warteschlange abgelegt wird.
Diese Queue bezeichnen wir als *Eingangswarteschlange* (engl. *entry queue*),
sie ist nat�rlich mit der Eingangshalle des Monitorgeb�udes gleichzusetzen,
die wir in *Abbildung* 5
zur Veranschaulichung des Monitorprinzips herangezogen haben.
Um in der Monitoroperation weiter voranschreiten zu k�nnen, muss der Monitor nun belegt werden.
Wenn in der Eingangswarteschlange kein weiterer Thread eingetragen ist und auch kein Thread den Monitor belegt,
kann dieser Thread sofort den Monitor belegen und damit in der Ausf�hrung der Monitoroperation fortfahren.
Ist die Eingangswarteschlange
nicht leer, muss der neu angekommene Thread sich damit begn�gen,
solange blockiert zu bleiben, bis der Thread, der den Monitor belegt, diesen freigibt und verl�sst.
Nun obliegt es der Verwaltung des Monitors, einem der wartenden Threads aus der Eingangswarteschlange
die Belegung des Monitors zu gew�hren.
    

<img src="Monitor_02.png" width="600">

*Abbildung* 6: Erste Veranschaulichung eines Monitors: Aspekt des gegenseitigen Ausschlusses.

In 
*Abbildung* 6
haben wir das Prinzip des Monitors aus dem Blickwinkel seiner technischen Realisierung veranschaulicht:
In der linken H�lfte finden wir die Warteschlange aller wartenden Threads vor, es sind dort aktuell die Threads
T<sub>1</sub>, T<sub>2</sub> und T<sub>3</sub> abgelegt.
Der Zugang zum Monitor erfolgt durch �ffnung Nummer 1, auf die technischen Details des Zutritts
kommen wir noch zu sprechen.
Besitzer eines Monitors kann zu einem Zeitpunkt immer nur ein Thread sein,
dieser schreitet nach dem Austragen seiner Referenz aus der Eingangswarteschlange durch �ffnung Nummer 2
und wird in
*Abbildung* 6
in der rechten H�lfte als *Owner* dargestellt (hier: Thread T<sub>4</sub>).
Gibt der Besitzer den Monitor frei, wird dieser durch �ffnung Nummer 3 verlassen.
    

### Betreten und Verlassen eines Monitors

Monitore werden im .NET Framework durch die Klasse `Monitor` aus dem
Namensraum `System.Threading` zur Verf�gung gestellt.
Die beiden Methoden zum Betreten und Verlassen des Monitors wurden mit den Bezeichnungen
`Enter` und `Exit` versehen, siehe *Tabelle* 1:



| Methode | Beschreibung |
|:-------------- |----|
| Methode ``Enter`` | ```public static void Enter (Object obj);```<br/> Betritt und belegt das durch `obj` spezifizierte Monitorobjekt. Durch das Belegen des Monitors wird insbesondere ein kritischer Abschnitt eingeleitet, der verhindert, dass andere Monitoroperationen zur Ausf�hrung gelangen k�nnen. |
| Methode ``Exit`` | ```public static void Exit (Object obj);```<br/> Freigabe und Verlassen des durch `obj` spezifizierten Monitorobjekts. Insbesondere ist der kritische Abschnitt der aktuellen Monitoroperation beendet und andere Threads k�nnen nun den Monitor belegen. |

*Tabelle* 1: `Enter`- und `Exit`-Methode der Klasse `Monitor`.

Unter Einbeziehung der `Monitor`-Klasse sieht eine thread-sichere Implementierung von `Push` oder `Pop` nun so aus:
      

```cpp
01: public void Push (int n)
02: {
03:     Monitor.Enter (this);
04:     if (this.IsFull ())
05:     {
06:         Monitor.Exit (this);
07:         return;
08:     }
09: 
10:     this.buf[this.index] = n;
11:     this.index ++;
12:     Monitor.Exit (this);
13: }
14: 
15: public int Pop () 
16: {
17:     Monitor.Enter (this);
18:     if (this.IsEmpty ())
19:     {
20:         Monitor.Exit (this);
21:         return -1;
22:     }
23: 
24:     this.index --;
25:     int val = this.buf[index];
26:     Monitor.Exit (this);
27:     return val;
28: }
```

###### Listing 5: Der gegenseitige Ausschluss der `Push`- und `Pop`-Methoden mit Hilfe eines Monitors

In den Zeilen 3, 6, 12, 17, 20 und 26 von
*Listing* 5 f�llt auf,
dass die beiden Methoden `Enter` und `Exit`
genau dann zu synchronisieren sind, wenn sie sich auf dasselbe Stapelobjekt beziehen, deshalb die
Verwendung der `this`-Referenz.
      

*Hinweis*:
*Listing* 5 stellt ein gutes Beispiel dar, um auf eine der h�ufigsten und fatalsten Fehlerquellen im Umfeld der Programmierung
mit Monitoren hinzuweisen. Wenn wir einen kritischen Abschnitt
(mit `Enter`) betreten,
d�rfen wir beim Verlassen nicht vergessen, die korrespondierende `Exit`-Methode aufzurufen.
Ein fl�chtiger Blick auf 
*Listing* 5 
zeigt, dass die entsprechenden Methodenaufrufe in den Zeilen 12 bzw. 26 platziert sind.
Vorsicht: Die
`Push`- und `Pop`-Methode besitzen jeweils einen zweiten Ausgang (falls der Stapel voll oder leer ist).
Der kritische Abschnitt muss f�r diesen Fall ebenfalls korrekt mit einem Aufruf der `Exit`-Methode verlassen werden,
wie in den Zeilen 6 und 20 erkennbar ist. Eine fehlerhafte Implementierung der `Push`-Methode, etwa der Gestalt
        
```
public void Push (int n)
{
    Monitor.Enter (this);
    if (this.IsFull ())
        return;

    this.buf[this.index] = n;
    this.index ++;
    Monitor.Exit (this);
}
```
        
h�tte zur Folge, dass alle nachfolgenden Aufrufe von `Push` und `Pop` (im Kontext anderer Threads)
blockiert werden - in vielen F�llen resultiert daraus ein Stillstand der gesamten Anwendung!
Zur Vermeidung derartiger Fehlerquellen gibt es in C# das `lock`-Sprachelement, das
        
  * das Betreten eines Monitors,
  * die Ausf�hrung eines Anweisungsblocks,
  * und das (garantierte) Verlassen des Monitors

in einer Anweisung zusammenfasst:
       
```
lock (expression)
    statement_block;
```
        
Auf unser Beispiel mit der Klasse `Stack` bezogen, k�nnten die beiden
Methoden `Push` und `Pop` damit alternativ auch wie in
*Listing* 6 implementiert werden:
        

```cpp
01: public void Push (int n)
02: {  
03:     lock (this)
04:     {
05:         if (this.IsFull ())
06:             return;
07: 
08:         this.buf[this.index] = n;
09:         this.index ++;
10:     }
11: }
12: 
13: public int Pop () 
14: {
15:     lock (this)
16:     {
17:         if (this.IsEmpty ())
18:             return -1;
19: 
20:         this.index --;
21:         return this.buf[this.index];
22:     }            
23: }
```

###### Listing 6: Alternative Implementierung der `Push`- und `Pop`-Methode mit `lock`: 

Nat�rlich wird das `lock`-Sprachelement vom C#-Compiler wiederum auf die statischen Methoden
`Enter` und `Exit` der `Monitor`-Klasse abgebildet.
Die Umsetzung sieht schematisch betrachtet unter Verwendung eines Bezugsobjekts `obj` so aus:
        
```
Monitor.Enter (obj);
try {
   ...
}
finally {
   Monitor.Exit (obj);
}
```
        
�hnlich wie beim Problem der atomaren Operation
m�ssen wir leider auch bei der nicht vermeidbaren Verwendung von Monitoren zur Kenntnis nehmen,
dass die Laufzeit der Anwendung zu leiden beginnt. Um ein Gef�hl f�r das Ma� der Verschlechterung zu erlangen,
erg�nzen wir das Beispiel des Stresstests 
um entsprechende Laufzeitmessungen. Mit einem Wiederholungsfaktor von 10000000
`Push`- und `Pop`-Methodenaufrufen messen wir eine Laufzeit von ca. 6 Sekunden
im Gegensatz zu 1 Sekunde ohne Einsatz eines Monitors. Dieser Grad der Verschlechterung ist dennoch nicht
repr�sentativ, da der Stresstest ausschlie�lich ununterbrochen die beiden kritischen Methoden aufruft, eine Situation,
die in einer realen Anwendung in dieser Form nicht gegeben ist!
      

## Zusammenarbeit von Threads

Eine Anwendung mit einer grafischen Oberfl�che kann beispielsweise eine umfangreiche Berechnung an einen Thread
delegieren. Die Anwendung bleibt dadurch reaktionsf�hig, der Benutzer kann jederzeit durch entsprechende Bedienungen
(Maus-, Tastaturereignisse) mit der Anwendung kommunizieren.
Wenn der Thread mit seinen Berechnungen fertig ist, kann die Oberfl�che aktualisiert werden.
Nat�rlich ist die Aktualisierung erst dann vorzunehmen, wenn der Thread seine Berechnungen erfolgreich abgeschlossen hat
und eine entsprechende Benachrichtigung (an einen m�glicherweise wartenden Ausgabethread) �bertragen hat.
Es ist nicht zuf�llig, dass in den letzten S�tzen die W�rter *warten* und *benachrichtigen* verwendet wurden.
Sie bringen das f�r die Thread-Kooperation zentrale Konzept zum Ausdruck:
Ein Thread *wartet* darauf, dass bestimmte Voraussetzungen geschaffen werden,
andere Threads f�hren tempor�re Dienstleistungen aus und *benachrichtigen* wartende Threads mit ihren Resultaten.
    


### Erweiterung des Monitorprinzips: Der Interimsraum

Mit den bislang
beschriebenen Mechanismen des Monitorprinzips erreichten wir das Ziel
des gegenseitigen Ausschlusses eines kritischen Abschnitts im Umfeld eines Multithreadingszenarios.
Zu einem bestimmten Zeitpunkt kann nur ein Thread eine Monitoroperation (eines bestimmten Monitors) ausf�hren.
Dieses Verfahren ist dann von Bedeutung, wenn mehrere Threads auf gemeinsame Daten zugreifen.
Das zweite Thema bez�glich der Synchronisierung von Threads ist ihre Koordination.
Sie unterst�tzt mehrere Threads beim Erreichen eines gemeinsamen Ziels.
Die Koordination von Threads ist dann von Bedeutung, wenn ein Thread Daten in einem bestimmten Zustand ben�tigt und
ein anderer Thread daf�r verantwortlich ist, die Daten in genau diesem Zustand zur Verf�gung zu stellen.
Zum Beispiel k�nnen wir uns einen Thread vorstellen ("*Reader-Thread*"), der Daten aus einem Puffer
auslesen m�chte, der von einem zweiten Thread ("*Writer-Thread*") gef�llt wird.
Der Reader-Thread ben�tigt den Puffer im Zustand "*not empty*", bevor er Daten lesen kann.
Wenn der Reader-Thread feststellt, dass der Puffer leer ist, muss er warten (Zustand "*empty*").
Der Writer-Thread ist daf�r zust�ndig, den Puffer zu f�llen und ggf. wartende Threads zu benachrichtigen,
dass der Puffer im Zustand "*not empty*" ist.
      
Wir sind nun bei einer Funktionalit�t des Monitors angekommen, die im Jargon der Betriebssystemtheorie
als "*Signal and Continue*"-Monitor bezeichnet wird
oder mit Blick auf .NET-Namensgebungen auch "*Wait and Pulse*"-Monitor hei�en k�nnte.
In diesem Typus Monitor kann sich ein Thread, der den Monitor gerade belegt, suspendieren,
indem er eine *Wait*-Aktion anst��t.
Wenn ein Thread die *Wait*-Aktion ausf�hrt, gibt er den Monitor frei und reiht sich in einer zweiten Warteschlange
des Monitorgeb�udes ein, der so genannten *Bedingungswarteschlange* (engl. *condition queue*),
in der angels�chsischen Fachliteratur auch als *wait queue* bezeichnet. 
Das Einreihen in der Bedingungswarteschlange bedeutet einfach, dass die Daten des Monitors f�r diesen Thread gerade
in einem unpassenden Zustand sind. Der Thread wartet somit, bis auf Grund einer Zustands�nderung der Daten
eine bestimmte Bedingung erf�llt ist, die ein Weiterarbeiten wieder sinnvoll macht.
Ein Thread muss in der Bedingungswarteschlange solange warten, bis ein anderer Thread, der den Monitor belegt,
eine *Pulse*-Aktion
ausf�hrt. Mit dieser Aktion gibt dieser Thread kund, dass die Daten des Monitors ihren Zustand ge�ndert haben.
Wenn ein Thread eine *Pulse*-Aktion ausf�hrt, muss er deswegen den Monitor nicht sofort verlassen.
Er f�hrt in der Regel seine Monitoroperation bis zum Ende aus und verl�sst erst dann den Monitor.
Wenn der benachrichtigende Thread
den Monitor freigegeben hat, wird einer der blockierten Threads aufgeweckt, um den Monitor erneut zu belegen.
      

<img src="Monitor_03.png" width="600">

*Abbildung* 7: Zweite, vollst�ndige Veranschaulichung eines Monitors: Aspekt der Koordination von Threads.

In 
*Abbildung* 7 erkennen wir 
einige Erg�nzungen, die dem Aspekt der Koordination von Threads dienen.
Stellt beispielsweise der aktive Thread des Monitors fest, dass er die Monitoroperation auf Grund eines ungeeigneten
Datenzustands nicht beenden kann, gibt er den Monitor mit dem Aufruf einer *Wait*-Aktion frei
und begibt sich durch �ffnung 4 in den Interimsraum des Monitors.
Diesen Begriff haben wir gew�hlt, um damit die Bedingungswarteschlange des Monitors zu veranschaulichen.
Sie nimmt alle Threads auf, die auf ein
bestimmtes Ereignis warten und deshalb den Monitor noch nicht verlassen m�chten.
Irgendwann gibt es einen aktiven Thread im Monitor, der die Daten geeignet �ndert
und in Folge dessen eine *Pulse*-Aktion ausl�st.
Damit kann einer der Threads aus der Bedingungswarteschlange entnommen werden
und den Monitor durch �ffnung 5 erneut belegen.
      
 Mit Hilfe von *Abbildung* 7 k�nnen wir den internen Aufbau eines Monitors exakter beschreiben. Er enth�lt
      
  * ein Threadobjekt, das den Monitor im Augenblick belegt,
  * eine *Entry Queue*, die alle Threads enth�lt, die den Monitor belegen wollen und
  * eine *Condition Queue*, die alle Threads enth�lt, die auf Benachrichtigungen warten, die auf einen Zustandswechsel in den Daten des zu sch�tzenden Objekts hindeuten.

Wir kommen noch einmal auf das Beispiel mit dem Reader- und dem Writer-Thread zur�ck
und legen zu Grunde, dass zwei Operationen "*Read Data*" und "*Write Data*"
unter der Kontrolle eines Monitors ablaufen.
Betritt ein Reader-Thread den Monitor, �berpr�ft er zun�chst den Zustand des Datenpuffers, oder etwas allgemeiner formuliert:
Es wertet die Bedingung "*buffer empty*" aus. Ist der Puffer nicht leer, kann der Reader-Thread 
mit der "*Read Data*"-Operation Daten aus dem Puffer entnehmen und anschlie�end den Monitor verlassen.
Ist der Puffer jedoch leer, f�hrt der Reader-Thread eine *Wait*-Aktion aus.
In diesem Fall gibt der Reader-Thread den Monitor frei, suspendiert sich und wird in der Bedingungswarteschlange des Monitors
eingetragen.
Zu einem sp�teren Zeitpunkt betritt der Writer-Thread den Monitor, legt einige Daten im Puffer ab, f�hrt eine *Pulse*-Aktion
aus und verl�sst schlie�lich den Monitor.
Bei der Ausf�hrung des *Pulse*-Kommandos wird der blockierte Zustand des Reader-Threads aufgehoben,
er kann beim n�chsten Threadwechsel wieder die Kontrolle �bertragen bekommen.
Da die Bedingungswarteschlange aber mehrere Threads enthalten kann, die alle Daten aus dem Puffer entnehmen wollen,
muss der Reader-Thread beim Belegen des Monitors zun�chst �berpr�fen, ob der Puffer auch tats�chlich im Zustand
"*not empty*" vorliegt.
Ein *Pulse*-Kommando kann folglich nur als *Hinweis* f�r wartende Threads verstanden werden,
dass die Daten im gew�nschten Zustand vorliegen.
Sind im Puffer Daten abgelegt, kann der Reader-Thread diese entnehmen und danach
den Monitor wieder verlassen.

Zum Abschluss dieser Betrachtungen werfen wir in
*Abbildung* 8 einen Blick auf die Zust�nde,
die ein Thread bei seinem Parcour durch das Monitorgeb�ude einnehmen kann.
Wir erkennen, dass ein Thread im Wartezimmer zun�chst den Zustand `Ready` besitzt.
Die Belegung des Monitors bedeutet, dass der Thread in den Zustand `Running` �bergeht.
Nat�rlich kann auch der Besitzer eines Monitors in seiner Arbeit unterbrochen werden.
Beispielsweise zu Gunsten von anderen Threads im System,
die vor�bergehend eine h�here Priorit�t besitzen und mit dem Monitor nicht in Zusammenhang stehen.
In dieser Situation nimmt der Besitzer des Monitors den Zustand `Ready` ein,
ohne dabei allerdings wieder im Wartezimmer Platz zu nehmen.
Solange der Besitzer des Monitors diesen nicht freigibt,
kann kein anderer Thread aus dem Wartezimmer in die Rolle des Besitzers schl�pfen.
M�chte der Besitzer des Monitors diesen freigeben, um sich im Sinne der Zusammenarbeit mit anderen Threads aus dem
Wartezimmer zu koordinieren, kann er im Interimsraum des Monitors Platz nehmen - er nimmt dazu den Zustand `Blocked` an.
Blockierte Threads k�nnen - entweder alle oder nur einer - den Interimsraum wieder verlassen,
sie m�ssen dann mit einem kleinen Umweg �ber das Wartezimmer
(Zustand `Ready`) darauf warten, bis sie wieder den Monitor belegen d�rfen.
      
<img src="Monitor_04.png" width="400">

*Abbildung* 8: Die Zust�nde, die ein Thread in einem Monitor annehmen kann.

*Hinweis*:
Halten sich im Interimsraum eines Monitors mehrere Threads auf, ist es nicht m�glich, gezielt einen bestimmten Thread
aus seiner Blockade aufzuwecken. Entweder es k�nnen alle Threads auf einmal in das Wartezimmer gehen,
oder aber es wird ein Thread von der Verwaltungsinstanz des Monitors nach dem Zufallsprinzip bestimmt - beziehungsweise nach Kriterien,
die f�r den Monitor deterministisch erscheinen m�gen, aber eben von au�en nicht vorhersehbar sind.


### Benachrichtigungen �ber Zustands�nderungen eines Monitorobjekts

Zur Koordination von Threads stehen in der `Monitor`-Klasse die drei Methoden
`Pulse`-, `PulseAll`- und `Wait`-Methode zur Verf�gung (*Tabelle* 2).
Mit `Wait` versetzt sich ein Thread solange in einen inaktiven Zustand,
bis im Monitorobjekt eine geeignete Zustands�nderung eingetreten ist.
Zur Reaktivierung wartender Threads gibt es die beiden Methoden `Pulse` und `PulseAll`:

| Methode | Beschreibung |
|:-------------- |----|
| Methode ``Wait`` | ```public static bool Wait (Object obj);```<br/>Gibt das durch `obj` spezifizierte Monitorobjekt frei, so dass andere Threads dieses belegen k�nnen. Der aktuelle Thread geht in einen blockierten Zustand �ber. Mit Hilfe von `Pulse`-Methodenaufrufen aus dem Kontext anderer Threads kann ein Wechsel im Zustand des aktuellen Objekts signalisiert werden und der zuletzt aktive Thread wieder reaktiviert werden. |
| Methode ``Pulse`` | ```public static void Pulse (Object obj);```<br/>Sendet ein Signal an einen (`Pulse`) oder alle (`PulseAll`) blockierten Threads des durch `obj` spezifizierten Monitorobjekts. Mit diesem Signal ist zus�tzlich ein Wechsel im Zustand des zu sch�tzenden Objekts verbunden. In der Regel gibt der aktuelle Inhaber des Monitors diesen in K�rze frei, da das *Pulse*-Signal als Aufforderung f�r andere Threads zu verstehen ist, ihre Arbeit fortzusetzen. |

*Tabelle* 2: Die `Pulse`-, `PulseAll`- und `Wait`-Methode der Klasse `Monitor`.

Damit kommen wir zur den Formulierungen einer Monitoroperation in C#.
Die �berpr�fung des Datenzustandes k�nnen wir uns mit einer Bedingungsvariablen `condition` vom
Typ `bool` verkn�pft vorstellen.
Die Bedingung muss solange wahr bleiben, wie der Thread fortgesetzt werden soll.
Ist die Bedingung nicht mehr erf�llt, muss der Thread warten, bis sie wieder `true` ist,
um fortgesetzt zu werden.
In C# formuliert man dieses Szenario - beispielsweise in einer Methode `DoWhenCondition` - so:    

```cpp
01: void DoWhenCondition ()
02: {
03:     Monitor.Enter (this);  // acquire a lock for this object
04: 
05:     while (! condition)
06:     {
07:         Monitor.Wait (this);  // wait until the state
08:                               // of this object has changed
09:     }
10: 
11:     /* do what needs to be done       */
12:     /* when the condition is true ... */
13: 
14:     Monitor.Exit (this);  // release the lock on this object
15: }
```

###### Listing 7: Aussehen einer Monitoroperation mit passivem Charakter: Verwendung der Methode `Wait`. 

Zuerst wird in der Methode `DoWhenCondition` in *Listing* 7 
gepr�ft, ob die gew�nschte Bedingung
bereits `true` ist.
Ist die
Bedingungsvariable `true`, muss nicht gewartet werden. Ist sie noch nicht `true`,
wird die `Wait`-Methode aufgerufen.
Wenn der Aufruf der Methode `Wait` zur�ckkehrt, wird die Bedingung erneut gepr�ft,
um sicherzustellen, dass sie jetzt wahr ist. Zus�tzlich haben wir das Betreten und Verlassen des Monitors
mit den beiden Methoden `Enter` bzw. `Exit` ber�cksichtigt.
Das Gegenst�ck zur `DoWhenCondition`-Methode sieht in Gestalt
einer exemplarischen `ChangeCondition`-Methode so aus (*Listing* 8):

```cpp
01: void ChangeCondition ()
02: {
03:     Monitor.Enter (this);  // acquire a lock for this object
04: 
05:     /* change some value(s)         */
06:     /* used in a condition test ... */
07: 
08:     Monitor.Pulse (this);  // notify waiting threads that the state
09:                            // of this object has changed
10: 
11:     Monitor.Exit  (this);  // release the lock on this object
12: }
```

###### Listing 8: Aussehen einer Monitoroperation mit aktivem Charakter: Verwendung der Methode `Pulse`. 

*Achtung*:
Wir weisen noch einmal auf die wichtigste Passage im Codefragment der `DoWhenCondition`-Methode hin:
Die �berpr�fung des Datenzustandes ist in einer `while`-Anweisung platziert, da nach
jeder Wiederbelebung eines blockierten Threads der tats�chliche Zustand der Daten zu betrachten ist.

---

[Zur�ck](../../Readme.md)

---
