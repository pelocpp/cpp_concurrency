# Placement New

Es ist &ndash; wenngleich auch eher selten benutzt &ndash; in C++ möglich,
die Speicherallokation von der Objektkonstruktion zu trennen.

Man kann zum Beispiel mit `std::malloc()` ein Byte-Array reservieren
und in diesem Speicherbereich ein neues Objekt erstellen.

Betrachte den folgenden Codeausschnitt:

```cpp
auto* memory = std::malloc(sizeof(User));
auto* user = ::new (memory) User("John");
```

Die vielleicht ungewohnte Syntax

```cpp
::new (memory)
```

heißt *Placement new*.

Dieser Aufruf des new-Operators reserviert keinen Speicher, sondern konstruiert auf bereits vorhandenem Speicher ein neues Objekt.

Der Doppelpunkt (::) vor new stellt sicher, dass die Suche nach `new` aus dem globalen Namensraum erfolgt,
um zu vermeiden, dass eine überladene Version des `new`-Operators erfasst wird.

Im vorherigen Beispiel wird ein `User`-Objekt an einem vorgegebenen Speicherort konstruiert.

Es gibt kein *Placement delete*. Um das Objekt zu zerstören und den von ihm benutzten Speicher freizugeben,
Wir müssen den Destruktor explizit aufrufen und dann den Speicher freigeben.

```cpp
user->~User();
std::free(memory);
```

---

