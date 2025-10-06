# Thread Local Storage

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

Jeder Thread, der dieses Objekt verwendet, besitzt eine eigene Instanz des originalen Objekts.

---

Der Begriff des thread-lokalen Speichers ist ein Feature, 
das sich auf Daten bezieht, die scheinbar global oder statisch gespeichert sind (aus Sicht der sie verwendenden Funktionen),
die tatsächlich aber nur in Gestalt einer Kopie jeweils einmal pro Thread vorhanden sind.

Das Feature ergänzt folgende bestehenden Optionen:

  * *automatisch*: Daten existieren während eines Blocks oder einer Funktion (Schlüsselwort `auto`).
  * *statisch*: Daten existieren für die gesamte Programmdauer (Schlüsselwort `static` / *File-Scope*).
  * *global*: Daten existieren für die gesamte Programmdauer (*Global-Scope*).
  * *dynamisch*: Daten existieren auf dem Heap in zeitlicher Existenz zwischen Reservierung und Freigabe (Operatoren `new` und `delete`).

Threadlokale Daten werden bei der Thread-Erstellung erstellt und nach Beendigung des Threads gelöscht.

---

## Beispiel <a name="link2"></a>

```cpp
01: std::mutex mutex{};
02: 
03: thread_local int x{};
04: 
05: static void function()
06: {
07:     thread_local int y{};
08: 
09:     {
10:         std::lock_guard<std::mutex> guard{ mutex };
11: 
12:         x++;
13:         y++;
14: 
15:         std::println("TID:  {} ", std::this_thread::get_id());
16:         std::println("  &x: {:#010x} => {}", reinterpret_cast<intptr_t>(&x), x);
17:         std::println("  &y: {:#010x} => {}", reinterpret_cast<intptr_t>(&y), y);
18:     }
19: }
20: 
21: void test() {
22: 
23:     using namespace ThreadLocalStorage;
24: 
25:     std::println("Main: {} ", std::this_thread::get_id());
26:     std::println("  &x: {:#010x} => {}", reinterpret_cast<intptr_t>(&x), x);
27: 
28:     function();
29:     std::jthread worker1{ function };
30:     function();
31:     std::jthread worker2{ function };
32:     function();
33: }
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
