package solution

import (
	"fmt"
	"testing"
)

func formatParcel(p Parcel) string {
	return fmt.Sprintf("Label: %s, Stamps: %d, Sent: %t", p.Label, p.Stamps, p.Sent)
}

func Test(t *testing.T) {
	type valueCase struct {
		input    Parcel
		expected Parcel
		original Parcel
	}

	type pointerCase struct {
		input    Parcel
		expected Parcel
	}

	type countCase struct {
		input    *Parcel
		expected int
		label    string
	}

	runValueCases := []valueCase{
		{
			input:    Parcel{Label: "Maps", Stamps: 1, Sent: false},
			expected: Parcel{Label: "Maps", Stamps: 2, Sent: true},
			original: Parcel{Label: "Maps", Stamps: 1, Sent: false},
		},
	}

	runPointerCases := []pointerCase{
		{
			input:    Parcel{Label: "Seeds", Stamps: 1, Sent: false},
			expected: Parcel{Label: "Seeds", Stamps: 2, Sent: true},
		},
	}

	runCountCases := []countCase{
		{
			input:    &Parcel{Label: "Notes", Stamps: 4, Sent: false},
			expected: 4,
			label:    "non-nil parcel",
		},
	}

	submitValueCases := []valueCase{
		{
			input:    Parcel{Label: "Blank", Stamps: 0, Sent: false},
			expected: Parcel{Label: "Blank", Stamps: 1, Sent: true},
			original: Parcel{Label: "Blank", Stamps: 0, Sent: false},
		},
	}

	submitPointerCases := []pointerCase{
		{
			input:    Parcel{Label: "Posters", Stamps: 3, Sent: false},
			expected: Parcel{Label: "Posters", Stamps: 4, Sent: true},
		},
	}

	submitCountCases := []countCase{
		{
			input:    nil,
			expected: 0,
			label:    "nil parcel",
		},
	}

	valueCases := runValueCases
	pointerCases := runPointerCases
	countCases := runCountCases
	if withSubmit {
		valueCases = append(valueCases, submitValueCases...)
		pointerCases = append(pointerCases, submitPointerCases...)
		countCases = append(countCases, submitCountCases...)
	}

	skipped := (len(submitValueCases) + len(submitPointerCases) + len(submitCountCases)) - ((len(valueCases) - len(runValueCases)) + (len(pointerCases) - len(runPointerCases)) + (len(countCases) - len(runCountCases)))

	passCount := 0
	failCount := 0

	for _, test := range valueCases {
		original := test.input
		result := processByValue(test.input)
		if result != test.expected || original != test.original {
			failCount++
			t.Errorf(`---------------------------------
Input to processByValue:
  %s

Expected returned parcel:
  %s
Actual returned parcel:
  %s

Expected original parcel after call:
  %s
Actual original parcel after call:
  %s
Fail
`, formatParcel(test.input), formatParcel(test.expected), formatParcel(result), formatParcel(test.original), formatParcel(original))
		} else {
			passCount++
			fmt.Printf(`---------------------------------
Input to processByValue:
  %s

Expected returned parcel:
  %s
Actual returned parcel:
  %s

Expected original parcel after call:
  %s
Actual original parcel after call:
  %s
Pass
`, formatParcel(test.input), formatParcel(test.expected), formatParcel(result), formatParcel(test.original), formatParcel(original))
		}
	}

	for _, test := range pointerCases {
		parcel := test.input
		processByPointer(&parcel)
		if parcel != test.expected {
			failCount++
			t.Errorf(`---------------------------------
Input to processByPointer:
  %s

Expected parcel after call:
  %s
Actual parcel after call:
  %s
Fail
`, formatParcel(test.input), formatParcel(test.expected), formatParcel(parcel))
		} else {
			passCount++
			fmt.Printf(`---------------------------------
Input to processByPointer:
  %s

Expected parcel after call:
  %s
Actual parcel after call:
  %s
Pass
`, formatParcel(test.input), formatParcel(test.expected), formatParcel(parcel))
		}
	}

	for _, test := range countCases {
		result := safeStampCount(test.input)
		inputText := test.label
		if test.input != nil {
			inputText = formatParcel(*test.input)
		}
		if result != test.expected {
			failCount++
			t.Errorf(`---------------------------------
Input to safeStampCount:
  %s

Expected stamp count:
  %d
Actual stamp count:
  %d
Fail
`, inputText, test.expected, result)
		} else {
			passCount++
			fmt.Printf(`---------------------------------
Input to safeStampCount:
  %s

Expected stamp count:
  %d
Actual stamp count:
  %d
Pass
`, inputText, test.expected, result)
		}
	}

	fmt.Println("---------------------------------")
	if skipped > 0 {
		fmt.Printf("%d passed, %d failed, %d skipped\n", passCount, failCount, skipped)
	} else {
		fmt.Printf("%d passed, %d failed\n", passCount, failCount)
	}
}

var withSubmit = true
