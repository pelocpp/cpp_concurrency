# Lock-Free Programming

[Zur�ck](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Verwendete Werkzeuge](#link2)
  * [Literaturhinweise](#link3)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::atomic`

---

#### Quellcode

[*LockFreeProgramming.cpp*](LockFreeProgramming.cpp).

---

## CAS / Compare-and-Swap-Idiom <a name="link2"></a>

Die beiden Methoden `compare_exchange_weak` / `compare_exchange_strong` (auch als CAS / Compare-and-Swap-Idiom bezeichnet)
f�hren eine atomare Anweisung aus, die zur Synchronisierung verwendet wird.

Sie vergleicht den Inhalt eines Speicherorts mit einem gegebenen (dem vorherigen) Wert und �ndert den Inhalt dieses Speicherorts
nur bei �bereinstimmung auf einen neuen gegebenen Wert.

Dies geschieht als einzelne atomare Operation.

Die Atomizit�t garantiert, dass der neue Wert auf Basis aktueller Informationen berechnet wird;
w�re der Wert in der Zwischenzeit von einem anderen Thread aktualisiert worden, w�rde der Schreibvorgang fehlschlagen.

Das Ergebnis der Operation, gegeben durch einen boolesche R�ckgabewert, gibt an, ob die Ersetzung durchgef�hrt wurde oder nicht.

---

## Beispiele <a name="link2"></a>

### Erstes Beispiel: Erfolgreiche Ausf�hrung

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

### Zweites Beispiel: Erfolglose Ausf�hrung


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

## Literaturhinweise <a name="link3"></a>

---

[Zur�ck](../../Readme.md)

---
