# PA2 analysis — < Kiernan Flieh, Cyril Tabaranza, Nathan Nguyen, Jared Fabian, Nick Pieratos >

Group members (all names, or write "working alone"):
Anyone outside your group you discussed this with:
AI assistance (tool, and what you used it for; write "none" if none): none

---

## Part C — can the kernel see your threads?

**System 1**

- Machine and OS: WSL Ubuntu (Linux)
- Command used to count threads: ./tcount --threads 4 & PID=$!; sleep 1; echo "before: $(ls /proc/$PID/task | wc -l)"; sleep 4; echo "after: $(ls /proc/$PID/task | wc -l)"; wait
- Threads before the pthread_create calls: 1
- Threads after: 5
- Difference: 4
- Does the difference match the number you asked for?: I requested 4 threads, and got a difference of 4.

**System 2** (optional but recommended — the course server and your laptop)

- Machine and OS: CSUSM Server (Linux)
- Command used: ./tcount --threads 4 & PID=$!; sleep 1; echo "before: $(ls /proc/$PID/task | wc -l)"; sleep 4; echo "after: $(ls /proc/$PID/task | wc -l)"; wait
- Before / after / difference: 1 / 5 / 4 

**One-to-one or many-to-one?** Two sentences. Say what you would have seen
instead if the mapping were many-to-one.

: If it was many-to-one mapping, the kernal would only be able to see one thread. This is because the user threads would be mapped to a single kernel thread.
---

## Part D — measurements

Machine used for the timings: Windows PC running Ubuntu through WSL2 
Core count, and the command you got it from: 16, nproc
Plugged in, or on battery: Plugged in

### Speedup table

`--n 20000000 --reps 3`. Speedup is against your own 1-thread run.

| threads | fill_s | sum_s | wall_s | sum-phase speedup | whole-program speedup |
|---|---|---|---|---|---|
| 1 | 0.1318 | 0.4915 | 0.6233 | 1.00 | 1.00 |
| 2 | 0.1342 | 0.2550 | 0.3892 | 1.93 | 1.60 |
| 4 | 0.1306 | 0.1331 | 0.2647 | 3.69 | 2.36 |
| 8 | 0.1308 | 0.0785 | 0.2092 | 6.26 | 2.98 |
| 16 (cores) | 0.1310 | 0.0563 | 0.1872 | 8.73 | 3.33 |

### The serial fraction

- S = fill_s / wall_s at one thread =
- Amdahl ceiling = 1 / S =

### The four questions

**1. Your measured S, and the ceiling it implies.**

: it was s = 0.1318/0.6233 = 0.2115 or 21.15%. So 1/0.2115 = 4.73 times.

**2. What whole-program speedup did you actually reach at the core count? How
far below the ceiling, and one reason for the gap.**

: With 16 I reached 3.33 times faster which is about 1.40 below the ceiling. I believe most of the difference is from the creation and management of multiple threads. 

**3. Why is the sum-phase speedup so much better than the whole-program
speedup? Which one would a user of this program notice?**

: Sum-phase has a much greater speedup because the work id divided between the threads, while the filling only runs on one. The user would only ever notice the whole program speedup because its what measures the amount of time needed to finish 

**4. Why does `total` change in its last digits when the thread count changes,
and why is that not a bug?**

: Floating-point addition often has small rounding differences. The difference in the number of threads changes the order in which the sums are added, meaning small differences is what we expect. 

---

## Stretch (only if you did it)

