package solution

import (
	"fmt"
	"testing"
)

type addTask struct {
	id int
	a  int
	b  int
}

func (t addTask) ID() int {
	return t.id
}

func (t addTask) Execute() int {
	return t.a + t.b
}

func Test(t *testing.T) {
	type testCase struct {
		name        string
		tasks       []addTask
		workerCount int
		expected    map[int]int
	}

	runCases := []testCase{
		{
			name: "small set, single worker",
			tasks: []addTask{
				{1, 2, 3},
				{2, 10, -5},
			},
			workerCount: 1,
			expected: map[int]int{
				1: 5,
				2: 5,
			},
		},
		{
			name: "three tasks, many workers",
			tasks: []addTask{
				{10, 100, 200},
				{11, -3, -7},
				{12, 42, 0},
			},
			workerCount: 5,
			expected: map[int]int{
				10: 300,
				11: -10,
				12: 42,
			},
		},
	}

	submitCases := append(runCases, []testCase{
		{
			name: "no tasks",
			tasks:       []addTask{},
			workerCount: 4,
			expected:    map[int]int{},
		},
		{
			name: "many tasks, worker limit smaller than tasks",
			tasks: []addTask{
				{1, 1, 1},
				{2, 2, 2},
				{3, 3, 3},
				{4, 4, 4},
				{5, 5, 5},
				{6, -10, 7},
				{7, 1000, -1},
			},
			workerCount: 3,
			expected: map[int]int{
				1: 2,
				2: 4,
				3: 6,
				4: 8,
				5: 10,
				6: -3,
				7: 999,
			},
		},
		{
			name: "unordered IDs, more workers than tasks",
			tasks: []addTask{
				{100, 7, 8},
				{2, 50, 50},
				{42, -100, 1},
			},
			workerCount: 10,
			expected: map[int]int{
				100: 15,
				2:   100,
				42:  -99,
			},
		},
	}...)

	testCases := runCases
	if withSubmit {
		testCases = submitCases
	}

	skipped := len(submitCases) - len(testCases)

	passCount := 0
	failCount := 0

	for _, test := range testCases {
		result := RunTasks[addTask](test.tasks, test.workerCount)

		if !mapsEqual(result, test.expected) {
			failCount++
			fmtTasks := formatTasks(test.tasks)
			t.Errorf(`---------------------------------
Test: %s
Worker count: %d
Input tasks:
%s
Expected results: %v
Actual results:   %v
Fail
`, test.name, test.workerCount, fmtTasks, test.expected, result)
		} else {
			passCount++
			fmtTasks := formatTasks(test.tasks)
			fmt.Printf(`---------------------------------
Test: %s
Worker count: %d
Input tasks:
%s
Expected results: %v
Actual results:   %v
Pass
`, test.name, test.workerCount, fmtTasks, test.expected, result)
		}
	}

	fmt.Println("---------------------------------")
	if skipped > 0 {
		fmt.Printf("%d passed, %d failed, %d skipped\n", passCount, failCount, skipped)
	} else {
		fmt.Printf("%d passed, %d failed\n", passCount, failCount)
	}
}

func mapsEqual(a, b map[int]int) bool {
	if len(a) != len(b) {
		return false
	}
	for k, v := range a {
		if b[k] != v {
			return false
		}
	}
	return true
}

func formatTasks(tasks []addTask) string {
	if len(tasks) == 0 {
		return "  (no tasks)\n"
	}

	result := ""
	for _, task := range tasks {
		result += fmt.Sprintf("  * ID: %d, a: %d, b: %d\n", task.id, task.a, task.b)
	}
	return result
}

// withSubmit is set at compile time depending
// on which button is used to run the tests
var withSubmit = true
