# Spinlocks

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Literaturhinweise](#link3)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::atomic_flag`
  * Klasse `std::atomic<>`
  * Klasse `std::mutex`

<ins>Aufzählungstypen</ins>:

  * Aufzählungstyp `enum class memory_order`

<ins>Konstanten</ins>:

    * `std::memory_order_relaxed`
    * `std::memory_order_consume`
    * `std::memory_order_acquire`
    * `std::memory_order_release`
    * `std::memory_order_acq_rel`
    * `std::memory_order_seq_cst`

---

#### Quellcode

[*SpinLock.cpp*](./SpinLock.cpp).<br />

---

## Allgemeines <a name="link2"></a>

Wenn ein Thread versucht, ein Mutex-Objekt zu sperren (Aufruf von `lock`) und das Mutex-Objekt bereits gesperrt ist,
wird der aktuelle Thread in einen Ruhezustand versetzt (*suspend*),
sodass ein anderer Thread ausgeführt werden kann (*resume*).

Der Zustand des suspendierten Threads ändert sich nur, wenn dieser Thread aufgeweckt wird,
und dies geschieht nur, wenn das Mutex-Objekt von dem Thread entsperrt wird, der die Sperre verursacht hat (Aufruf von `unlock`).

Wenn ein Thread hingegen ein Spinlock-Objekt sperrt und dies fehlschlägt,
versucht er kontinuierlich erneut, dieses zu sperren, bis das Vorhaben gelingt.

Es kommt also nicht zu einem Thread-Wechsel!

Natürlich gibt es hier eine Ausnahme: Das Betriebssystem wechselt dann zu einem anderen Thread,
wenn das CPU-Laufzeitkontingent des aktuellen Threads überschritten wurde.

Bewirkt ein Mutex-Objekt einen Zustandswechsel eines Threads in den Ruhezustand,
zieht dies einen Kontextwechsel auf Betriebssystemebene nach sich, der zu Leistungseinbußen führt.

Wenn Sie diese Einbußen vermeiden und eine kurze Sperrzeit haben möchten,
ist der Einsatz eines *Spinlock*-Objekts möglicherweise eine gute Alternative.

In diesem Abschnitt demonstrieren wir die Implementierung einer *Spinlock*-Klasse.

---

## Literaturhinweise <a name="link3"></a>

Die Ideen zu den Beispielen aus diesem Abschnitt stammen aus diesen Artikeln:

[*Implementing a spinlock in C++*](https://medium.com/@amishav/implementing-a-spinlock-in-c-8078ec584efc).

[*Spin lock in Modern C++ with atomics, memory barriers and exponential back-off*](https://medium.com/@joao_vaz/spin-lock-in-modern-c-with-atomics-memory-barriers-and-exponential-back-off-522798aca817).

[*Correctly implementing a spinlock in C++*](https://rigtorp.se/spinlock/).

---

[Zurück](../../Readme.md)

---
