# C++ Concurrency

## Werkzeuge

### [1. Nebenläufige Ausführung: Klasse `std::thread`](Programs/01_Thread/Readme.md)

### [2. Atomare Operationen: Klassen `std::atomic<T>`](Programs/02_Atomic/Readme.md)

### [3. Mutexe und Sperrmechanismen](Programs/03_MutualExclusion/Readme.md)

### [4. Kooperation von Threads: Klasse `std::condition_variable`](Programs/04_ConditionVariable/Readme.md)

### [5. Nochmals nebenläufige Ausführung: Funktion `std::async`](Programs/05_Async/Readme.md)

### [6. Futures und Promises: Klassen `std::future` und `std::promise`](Programs/06_FuturesPromises/Readme.md)

### [7. Funktionen verpacken: Klasse `std::packaged_task`](Programs/07_PackagedTask/Readme.md)

### [8. Exception Handling](Programs/08_ExceptionHandling/Readme.md)

### [9. STL und parallele Algorithmen](Programs/09_STL_ParallelAlgorithms/Readme.md)

### [10. Nochmals nebenläufige Ausführung: Klasse `std::jthread`](Programs/10_JThread/Readme.md)

### [11. Die Klassen `std::stop_source`, `std::stop_token`, `std::stop_callback` und `std::condition_variable_any`](Programs/11_StopToken/Readme.md)

### [12. Verriegelungen und Barrieren: Klassen `std::latch` und `std::barrier`](Programs/12_LatchesBarriers/Readme.md)

### [13. Semaphore: Klassen `std::binary_semaphore` und `std::counting_semaphore`](Programs/13_Semaphore/Readme.md)

### [14. Spinlocks](Programs/14_SpinLock/Readme.md)

### [15. Thread Local Storage](Programs/15_Thread_Local_Storage/Readme.md)

---

## Entwurfsmuster

### [1. Strategisches Locking (Strategized Locking)](Programs/20_StrategizedLocking/Readme.md)

### [2. Reader-Writer Lock (Klassen `std::shared_mutex` und `std::shared_lock`)](Programs/21_ReaderWriterLock/Readme.md)

### [3. Das Erzeuger-Verbraucher Problem (*Producer Consumer Pattern*)](Programs/22_ProducerConsumerProblem/Readme.md)

### [4. Das *Active Object* Pattern](Programs/23_ActiveObject/Readme.md)

---

## Beispiele

### [1. Threadsicherer Stapel (Threadsafe Stack)](Programs/30_Threadsafe_Stack/Readme.md)

### [2. Threadsichere Warteschlange (Threadsafe Queue)](Programs/31_Threadsafe_Queue/Readme.md)

### [3. Realisierung einer `parallel_for` Wiederholungsschleife](Programs/32_ParallelFor/Readme.md)

### [4. Realisierung einer Ereigniswarteschlange (*Event Loop* )](Programs/33_EventLoop/Readme.md)

### [5. Realisierung eines Thread Pools](Programs/34_ThreadPool/Readme.md)

### [6. Das Problem der dinierenden Philosophen](Programs/50_DiningPhilosophers/Readme.md)

---

## Aufgaben

### [1. Aufgabe: Zeitvergleich (`std::thread` vs. `std::async`)](Programs/40_Excercises/Readme.md)

### [2. Aufgabe: Simulation eines Parkplatzes (`std::counting_semaphore`)](Programs/40_Excercises/Readme.md)

---

## Literatur

### [Einige Literaturhinweise](./Resources/Literature.md)


---

