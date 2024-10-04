# Die Klasse `std::jthread`

[Zur�ck](../../Readme.md)

---

## Inhalt

  * [Verwendete Werkzeuge](#link1)
  * [Allgemeines](#link2)
  * [Verhalten der Klassen `std::jthread` und `std::jthread` bzgl. des `join`-Aufrufs](#link3)

---

## Verwendete Werkzeuge <a name="link1"></a>

<ins>Klassen</ins>:

  * Klasse `std::thread`
  * Klasse `std::jthread`

---

#### Quellcode

[*JThread.cpp*: `std::jthread`-Objekt und `join()`-Aufruf](JThread_01.cpp).<br />

---

## Allgemeines <a name="link2"></a>

Die Implementierung der `std::jthread`-Klasse basiert auf der bereits vorhandenen Klasse `std::thread`.

Die `std::jthread`-Klasse ist eine Wrapper-Klasse um Klasse `std::thread` herum,
sie stellt dieselben �ffentlichen Methoden bereit, die die Aufrufe einfach weiterleiten.

Es kann folglich jedes `std::thread`-Objekt in ein `std::jthread`-Objekt umgewandelt werden,
dieses verh�lt sich dann genau so, wie sich ein entsprechendes `std::thread`-Objekt verhalten w�rde.

---

#### Verhalten der Klassen `std::jthread` und `std::jthread` bzgl. des `join`-Aufrufs <a name="link3"></a>

Wenn ein `std::thread`-Objekt den Zustand *joinable* besitzt
und dieses auf Grund des Verlassens eines Blocks (G�ltigkeitsbereichs) aufger�umt wird,
wird in seinem Destruktor automatisch `std::terminate` aufgerufen.

Ein `std::thread`-Objekt besitzt den Zustand *joinable*,
wenn am Objekt keine der beiden Methoden `join()` oder `detach()` aufgerufen wurde.

Ein Objekt der Klasse `std::jthread` verh�lt sich hier anders: Der Destruktor des Objekts ruft automatisch `join` auf,
wenn sich der Thread noch im Zustand *joinable* befindet.

---

[Zur�ck](../../Readme.md)

---






