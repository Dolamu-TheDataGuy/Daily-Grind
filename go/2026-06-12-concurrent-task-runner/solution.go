package solution

import "sync"

// Task is the interface provided by the challenge.
type Task interface {
	ID() int
	Execute() int
}

// RunTasks runs a batch of tasks concurrently using a fixed-size
// worker pool of workerCount goroutines and returns a map of
// task ID -> Execute() result.
func RunTasks[T Task](tasks []T, workerCount int) map[int]int {
	results := make(map[int]int)

	var mu sync.Mutex      // protects results from concurrent writes
	var wg sync.WaitGroup  // lets us wait for all workers to finish

	taskCh := make(chan T) // workers pull tasks from this channel

	// Start exactly workerCount workers.
	for i := 0; i < workerCount; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			// range over the channel: receives tasks until
			// the channel is closed AND drained.
			for task := range taskCh {
				result := task.Execute() // do the work OUTSIDE the lock

				mu.Lock()
				results[task.ID()] = result
				mu.Unlock()
			}
		}()
	}

	// Feed every task into the channel.
	for _, task := range tasks {
		taskCh <- task
	}

	// Closing the channel signals workers there's no more work,
	// so their `for range` loops exit.
	close(taskCh)

	// Block until every worker has called wg.Done().
	wg.Wait()

	return results
}
