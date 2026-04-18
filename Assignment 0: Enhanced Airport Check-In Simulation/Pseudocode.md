# Pseudocode / Plan of Action

## Assignment 0: Enhanced Airport Check-In Simulation

**Autor:** Gabriel Coto Fernández (C4E540)

---

# 1. Global Overview

We simulate an airport check-in system with:

* **N passengers**
* **M counters (3 types)**
* **Multiple queues (FIFO per class)**
* **Dynamic behavior:**

  * Counters go on break
  * Balancer redistributes load
  * Supervisor reopens counters

### Termination Condition

The system ends when:

```text
All N passengers have been served exactly once
```

We maintain:

```c
int total_served = 0;
```

Protected by a mutex.

---

# 2. Global Timer

At program start:

```c
start_time = now();
```

At program end:

```c
end_time = now();
total_time = end_time - start_time;
```

Also track per-passenger:

* arrival time
* service start time
* service end time

---

# 3. Passenger Model

### Structure

```c
struct Passenger {
    int id;
    string passengerClass; // Economy, Business, International
    bool wasServed;

    long arrival_time;
    long service_start_time;
    long service_end_time;

    bool priority_bumped; // for Business passengers
};
```

---

# 4. Queues (Core of Synchronization)

We define **three FIFO queues**:

```c
Queue economyQueue;
Queue businessQueue;
Queue internationalQueue;
```

Each queue must be:

* Thread-safe
* FIFO-preserving

### Each queue has:

```c
pthread_mutex_t mutex;
pthread_cond_t not_empty;
```

---

# 5. Counter Model

### Structure

```c
enum CounterState {
    OPEN,
    SERVING,
    ON_BREAK
};

struct Counter {
    int id;
    string type; // Economy, Business, International

    CounterState state;

    int passengers_served_since_break;
    int K_limit; // randomly drawn

    pthread_t thread;
};
```

---

# 6. Counter Lifecycle

Each counter runs a loop:

```text
WHILE simulation not finished:

    WAIT until:
        - there is a passenger in its queue
        - AND counter is OPEN

    TAKE passenger from queue

    SET state = SERVING

    PROCESS passenger (sleep random 10–100 ms)

    MARK passenger as served

    INCREMENT passengers_served_since_break

    IF passengers_served_since_break == K_limit:
        GO TO BREAK

    SET state = OPEN
```

---

# 7. Break Logic

### Drawing K

At initialization:

```c
K_limit = random(K_min, K_max);
```

### When K is reached:

```text
state = ON_BREAK
notify Supervisor
sleep(break_duration)
```

Counter must **NOT serve passengers during break**.

---

# 8. Supervisor Thread

Responsible for reopening counters.

### Loop:

```text
WHILE simulation not finished:

    WAIT for counters on break

    FOR each counter on break:
        IF break time finished:
            set state = OPEN
            reset passengers_served_since_break
            redraw K_limit
            signal counter thread
```

---

# 9. Balancer Thread

Runs continuously and manages:

---

## 9.1 Overflow Rule

If:

```text
queue_size(class) > Q
```

Then:

```text
move excess passengers → internationalQueue
```

---

## 9.2 Priority Bump (Business)

If:

```text
Business passenger wait_time > T_max
```

Then:

```text
move passenger → FRONT of internationalQueue
mark priority_bumped = true
```

---

## Starvation Constraint

Ensure:

```text
Economy passengers are NOT starved
```

Possible strategy:

* Limit how many priority bumps happen consecutively
* Or ensure international queue still respects fairness

---

# 10. Thread Structure

### Threads:

| Thread Type | Count |
| ----------- | ----- |
| Counters    | M     |
| Balancer    | 1     |
| Supervisor  | 1     |

---

# 11. Synchronization Strategy

## Key Rules

### 1. No two threads serve same passenger

* Protected via queue mutex

### 2. FIFO per class

* Queue operations strictly ordered

### 3. No busy waiting

* So we use:

```c
pthread_cond_wait()
pthread_cond_signal()
```

---

## Suggested Lock Ordering (YAPPING)

Possible solution to avoid deadlocks:

```text
1. economyQueue mutex
2. businessQueue mutex
3. internationalQueue mutex
4. counter mutex (if any)
```

---

# 12. Counter Selection Logic

Instead of naive wakeups, we implement:

```text
Check queue → then signal specific counter
```

OR

Counters wait on their specific queue condition variable.

---

# 13. Output Requirements

We must print:

### Per passenger:

```text
Passenger X served by Counter Y
Class: ...
Priority bumped: yes/no
```

### Events:

```text
Counter X going on break
Counter X reopened
```

### Final stats:

```text
- Total passengers per class
- Average wait time
- Average service time
- Per-counter stats
```

---

# 14. Clean Termination

When:

```text
total_served == N
```

We must:

* Wake all threads
* Join all threads
* Destroy all mutexes and condvars
* Free memory

---

# 15. Configurable Parameters

From CLI:

```text
N
M_eco
M_biz
M_intl
K_min
K_max
T_max
Q
```

---

# 16. Implementation Order (Step-by-Step Plan)

### Phase 1 — Core

* [ ] Passenger struct
* [ ] Thread-safe queues
* [ ] Basic counter threads (no breaks)

### Phase 2 — Synchronization

* [ ] Condition variables for queues
* [ ] Proper FIFO serving

### Phase 3 — Break System

* [ ] K logic
* [ ] Supervisor thread

### Phase 4 — Balancer

* [ ] Overflow redistribution
* [ ] Priority bump logic

### Phase 5 — Metrics

* [ ] Timing per passenger
* [ ] Final stats

### Phase 6 — Safety

* [ ] Deadlock check
* [ ] Valgrind (memory + helgrind)

---