# Lock-Free Programming

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [CAS / Compare-and-Swap-Idiom](#link2)
  * [Zwei Beispiele](#link3)
  * [Ein Vergleich: Atomares Inkrement versus CAS](#link4)
  * [Literaturhinweise](#link5)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::atomic`

---

#### Quellcode

[*LockFreeProgramming01.cpp*](LockFreeProgramming01.cpp)<br />
[*LockFreeProgramming02.cpp*](LockFreeProgramming02.cpp).

---

## CAS / Compare-and-Swap-Idiom <a name="link2"></a>

Die beiden Methoden `compare_exchange_weak` / `compare_exchange_strong` (auch als *CAS* / *Compare-and-Swap*-Idiom bezeichnet)
führen eine atomare Anweisung aus, die zur Synchronisierung verwendet wird.

Sie vergleicht den Inhalt eines Speicherorts mit einem gegebenen (dem vorherigen) Wert und ändert den Inhalt dieses Speicherorts
nur bei Übereinstimmung auf einen neuen gegebenen Wert.

Dies geschieht als einzelne atomare Operation.

Die Atomizität garantiert, dass der neue Wert auf Basis aktueller Informationen berechnet wird.
Wäre der Wert in der Zwischenzeit von einem anderen Thread aktualisiert worden, würde der Schreibvorgang fehlschlagen.

Das Ergebnis der Operation, gegeben durch einen booleschen Rückgabewert, gibt an, ob die Ersetzung durchgeführt wurde oder nicht.

---

## Zwei Beispiele <a name="link3"></a>

### Erstes Beispiel: Erfolgreiche Ausführung

```cpp
01: void test ()
02: {
03:     std::atomic<int> value{ 123 };
04: 
05:     int expectedValue = 123;
06: 
07:     std::println("Previous expected value: {}", expectedValue);
08: 
09:     bool b = value.compare_exchange_weak(expectedValue, 456);
10: 
11:     std::println("Return Value:            {}", b);
12:     std::println("Current expected Value:  {}", expectedValue);
13:     std::println("Current Value:           {}", value.load());
14: }
```

Ausgabe:

```
Previous expected value: 123
Return Value:            true
Current expected Value:  123
Current Value:           456
```

### Zweites Beispiel: Erfolglose Ausführung


```cpp
01: void test()
02: {
03:     std::atomic<int> value{ 123 };
04: 
05:     int expectedValue = 124;
06: 
07:     std::println("Previous expected value: {}", expectedValue);
08: 
09:     bool b = value.compare_exchange_weak(expectedValue, 456);
10: 
11:     std::println("Return Value:            {}", b);
12:     std::println("Current expected Value:  {}", expectedValue);
13:     std::println("Current Value:           {}", value.load());
14:     std::println();
15: }
```

Ausgabe:

```
Previous expected value: 124
Return Value:            false
Current expected Value:  123             // <=== note this value !!!
Current Value:           123
```

---

## Ein Vergleich: Atomares Inkrement versus CAS <a name="link4"></a>

Die atomare Inkrement-Operation kann man auch mit einer CAS-Anweisung formulieren.
Die folgenden beiden Code-Fragment sind bzgl. ihrer Funktionalität identisch:

```cpp
std::atomic<long> count{};
...
++count;
```

und 


```cpp
std::atomic<long> count{};
...
T value{ count.load(std::memory_order_relaxed) };

while (!count.compare_exchange_weak(
    value,
    value + 1,
    std::memory_order_release,
    std::memory_order_relaxed))
{
}
```

---

## Literaturhinweise <a name="link5"></a>

Einen weiterführenden Artikel mit dem Thema &bdquo;*An Introduction to Lock-Free Programming*&rdquo;
findet man [hier](https://preshing.com/20120612/an-introduction-to-lock-free-programming).

---

[Zurück](../../Readme.md)

---
