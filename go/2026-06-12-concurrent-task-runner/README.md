# Concurrent Task Runner — Worker Pools in Go

**Date:** 2026-06-12
**Language:** Go
**Source:** boot.dev
**Concepts:** goroutines, channels, worker pools, `sync.Mutex`, `sync.WaitGroup`, generics

## 🎯 The Problem

Implement a generic `RunTasks[T Task](tasks []T, workerCount int) map[int]int` that executes a batch of tasks concurrently using a fixed-size worker pool, and returns each task's result keyed by its ID — without data races.

## 🧩 My First Approach

The first thing I did was to try and understand the problem using a mental model and from the instructions I knew I needed a map to hold my result, so I created the `results` map — I knew that whatever else happened, I needed somewhere to collect each worker's outcome. So I
started from the *output* of the problem and worked backwards: results map
first, then the question became "okay, who fills this map, and how do tasks
reach them?" That led me to the worker pool — a `for` loop spinning up
`workerCount` goroutines — and a channel to carry tasks to them. I have basic understanding of `goroutines`, `channel`, and `loops`. So what was remaining was to understand how the concepts come together to form a system that works.

## 💥 What Broke / Where I Got Stuck

I didn't hit a crash so much as a wall of confusion: I couldn't see how the
workers loop and the channel-receiving loop worked *in tandem*. There are
two loops nested inside each other and they run on completely different
timescales:

- The **outer loop** (`for i := 0; i < workerCount; i++`) runs exactly
  `workerCount` times, instantly. Its only job is to *create* the workers —
  it's done in microseconds.
- The **inner loop** (`for task := range taskCh`) lives inside each worker
  and runs for the whole lifetime of the program, pulling task after task
  off the channel until it's closed.

I was mentally trying to read them as one sequential flow, when really the
outer loop is a *factory* and the inner loop is each worker's *day job*.
The two loops never "run together" line by line — the outer one finished
long ago; only its products (the goroutines) are still alive.

The second thing I got stuck on was **keeping track of my goroutines** in
the workers loop. Once the outer loop finished, I had `workerCount`
goroutines running off on their own, and no way to know when they were all
done. That's what `sync.WaitGroup` turned out to be for: `wg.Add(1)`
registers each worker as it's created, `defer wg.Done()` checks it out when
its inner loop ends, and `wg.Wait()` blocks until the count hits zero. The
WaitGroup is essentially an attendance register for goroutines.

## 💡 The Eureka Moment

Before touching the code again, I needed a mental model I could actually
*see*. So I thought about a restaurant.

Imagine a kitchen with a head chef and several line chefs:

| Restaurant | Go code | Responsibility |
|---|---|---|
| 🧑‍🍳 Line chefs | worker goroutines | Do the actual cooking — grab the next ticket, execute the order, post the result |
| 🎫 The order ticket rail | `taskCh` (the channel) | Carries orders to whichever chef is free — natural load balancing |
| 👨‍🍳 Head chef | `close(taskCh)` | Calls "no more orders tonight!" — chefs finish what's on the rail, then stop |
| 📋 Restaurant manager | `wg.Wait()` | Stands at the door with a clipboard, waits for every chef to clock out before shutting down |
| 📊 The owner | `return results` | Receives the final end-of-day report only after the manager confirms everyone is done |
| 📌 The result board | `results` map + `sync.Mutex` | Where chefs post finished orders — the mutex ensures two chefs never write at the same time |

Each role has one job and one job only. The head chef doesn't wait for
chefs to finish — that's the manager's job. The manager doesn't collect
results — that's the owner's job. The owner doesn't shut anything down —
that's the head chef's job. **No role bleeds into another, and neither does
the code.**

The key insight: `close(taskCh)` and `wg.Wait()` are often mistaken for
doing the same thing because they both appear "at the end." But they answer
completely different questions:

- **Head chef → `close(taskCh)`**: *"Are any new orders coming?"* — No.
  Stop accepting work.
- **Manager → `wg.Wait()`**: *"Has every chef actually finished and left?"*
  — Yes. Now we can close up.

You need both because one *signals* the end and the other *confirms* it.
Calling `close()` without `wg.Wait()` is like the head chef locking the
front door while chefs are still cooking inside — the owner gets an
incomplete report. Calling `wg.Wait()` without `close()` is like the
manager waiting forever at the door for chefs who were never told to stop.

## ✅ How the Final Solution Works

```go
func RunTasks[T Task](tasks []T, workerCount int) map[int]int {
	results := make(map[int]int)
	var mu sync.Mutex
	var wg sync.WaitGroup
	taskCh := make(chan T)

	for i := 0; i < workerCount; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for task := range taskCh {
				value := task.Execute()
        id := task.ID()
				mu.Lock()
				results[id] = value
				mu.Unlock()
			}
		}()
	}

	for _, task := range tasks {
		taskCh <- task
	}
	close(taskCh)
	wg.Wait()
	return results
}
```

Step by step:

First we need 2 goroutines:

**Main goroutine**:

  1. Spawns workerCount goroutines (they wait on the channel)
  2. Sends tasks into the channel
  3. Closes the channel
  4. Waits (wg.Wait()) for all workers to finish

**Worker goroutines** (running concurrently):

  - Pick up tasks from the channel as they arrive
  - Execute them and write results to the map

We also need a map that collects the result of each worker, a `taskCh` channel that allows the goroutines to communicate with each other. A mutex that allows us to lock access to our map data.

So from the code, we define our `result` map, the Mutex, and also the WaitGroup counter that must be incremented before the goroutine starts, otherwise `wg.Wait()` could run before any woker registers itself and return too early.

The loop starts *exactly* `workerCount` times. In the first iteration, we increment the WaitGroup counter, then start the worker goroutine in that loop. Since there is no job yet it remains idle waiting for a task. The same happens in the remaining iterations. So now we have exactly `workerCount` goroutines running idly in the background waiting for task. At this point the workerCount loop is done.

We then move to the next loop in the main goroutine. This loop is responsible from extracting task from the `tasks` slice using `range` and sending it to our `taskCh` channel. Once the first task is sent on the channel, one of our worker goroutine is signalled and it receives the task from the `taskCh` channel. The goroutine then goes ahead to run `task.Execute()` and `task.ID()` storing them into the `value` and `id`. We then lock our `result` map to avoid data races when we carrying out append operation on our `result` map. Then `defer wg.Done()` is executed to decrement the `WaitGroup` counter and tell the program that particular goroutine is done. This happens until the second sending loop in the main goroutine ends and tasks on the `taskCh` channel for any worker goroutine to process. Then the `close(taskCh)` is executed to signify to the receiver goroutine that there are no longer task to work on. Finally, once our `WaitGroup` counter becomes `0`, this means all the worker goroutines have been executed. Then `wg.Wait()` unblocks, main continues and the program returns our `result` map as the output.

## Running Test Code
```
cd daily-grind/go/2026-06-12-concurrent-task-runner/

go test -race -v ./...

```
## Result

## 📚 Lessons Learnt

- **A channel + `range` + `close` is Go's idiomatic "job queue."** No index
  juggling, no atomic counters — the channel distributes work and signals
  completion in one mechanism.
- **An unbuffered channel does not close itself automatically.** It stays opens forever, waiting of more values. The rule is the `sender` is responsible for closing the channel, the `receiver` uses `range` and stops automatically when the channel is closed.
- **Mutexes should guard the smallest possible critical section.** Lock
  around the map write, not around the work.
- **`close()` and `wg.Wait()` solve different problems.** Close = "no more
  input" (worker-facing). Wait = "all output done" (caller-facing).
  Forgetting either one produces a deadlock or an incomplete result map.
- **`wg.Add` goes before the `go` statement, always.**

## 🔗 Related Problems / Further Reading

- [Go Tour — Concurrency](https://go.dev/tour/concurrency/1)
- [Go by Example — Worker Pools](https://gobyexample.com/worker-pools)
- Next variation to try: same problem but collect results through a
  **results channel** instead of a mutex-guarded map, with a single
  collector goroutine — the "share memory by communicating" style.
