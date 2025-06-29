#  Thread Local Storage

[Zurück](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Beispiel](#link2)
  * [Literaturhinweise](#link3)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Schlüsselwort</ins>:

  * `thread_local`

<ins>Klassen</ins>:

  * Klasse `std::jthread`
  * Klasse `std::mutex>`
  * Klasse `std::lock_guard`

---

#### Quellcode

[*ThreadLocalStorage.cpp*](ThreadLocalStorage.cpp).

---

## Allgemeines <a name="link1"></a>

Seit C++ 11 gibt es das Schlüsselwort `thread_local`,
um die spezielle Speicherung einer Variable anzugeben.

Ein `thread_local`-Objekt kann lokal oder global deklariert werden.

In beiden Fällen ist seine Initialisierung an einen Thread gebunden,
und die Speicherung erfolgt im *Thread Local Storage*.

Jeder Thread, der dieses Objekt verwendet, erstellt eine Kopie davon.

---

## Beispiel <a name="link2"></a>

```cpp
01: std::mutex mutex{};
02: 
03: thread_local int x{};
04: 
05: void function()
06: {
07:     thread_local int y{};
08: 
09:     std::lock_guard<std::mutex> guard{ mutex };
10: 
11:     std::println("TID:  {} ", std::this_thread::get_id());
12:     std::println("  &x: {:#010x} ", reinterpret_cast<intptr_t>(&x));
13:     std::println("  &y: {:#010x} ", reinterpret_cast<intptr_t>(&y));
14: }
15: 
16: void test() {
17: 
18:     std::println("Main: {} ", std::this_thread::get_id());
19:     std::println("  &x: {:#010x} ", reinterpret_cast<intptr_t>(&x));
20: 
21:     std::jthread worker1{ function };
22:     function();
23:     std::jthread worker2{ function };
24:     function();
25: }
```


Ausgabe:

```
Main: 53640
  &x: 0x1feaa883348
TID:  53640
  &x: 0x1feaa883348
  &y: 0x1feaa883344
TID:  53656
  &x: 0x1feaa8990e8
  &y: 0x1feaa8990e4
TID:  53640
  &x: 0x1feaa883348
  &y: 0x1feaa883344
TID:  53660
  &x: 0x1feaa89aaf8
  &y: 0x1feaa89aaf4
```

---

## Literaturhinweise <a name="link3"></a>

Die Idee zu dem Beispiel stammt aus dem Artikel

[Storage duration and Non-Local Objects in C++](https://www.cppstories.com/2023/storage-init-cpp/)

von Bartlomiej Filipek.

---

[Zurück](../../Readme.md)

---
