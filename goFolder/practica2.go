package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
	"unicode"
)

func reverseString(s []rune) {
	start := 0
	end := len(s) - 1
	for start < end {
		s[start], s[end] = s[end], s[start]
		start++
		end--
	}
	fmt.Print(string(s) + " ")
}

func checkLen(s string) bool {
	length := len(s)
	if length > 100 || length == 0 {
		fmt.Printf("The length %d of the string is not correct. Exiting ....\n", length)
		return false
	}
	return true
}

func charFrequencies(s string, vowelCounter *int, consonantCounter *int, vowels []int) {
	for _, ch := range s {
		c := unicode.ToLower(ch)
		switch c {
		case 'a':
			*vowelCounter++
			vowels[0]++
		case 'e':
			*vowelCounter++
			vowels[1]++
		case 'i':
			*vowelCounter++
			vowels[2]++
		case 'o':
			*vowelCounter++
			vowels[3]++
		case 'u':
			*vowelCounter++
			vowels[4]++
		default:
			if unicode.IsLetter(c) {
				*consonantCounter++
			}
		}
	}
}

func filterSpaces(s string) string {
	return strings.ReplaceAll(s, " ", "_")
}

func isAlphabet(c rune) bool {
	return unicode.IsLetter(c) || c == ' '
}

func filterNonAlphabetic(s string) string {
	var filtered strings.Builder
	for _, ch := range s {
		if isAlphabet(ch) {
			filtered.WriteRune(ch)
		}
	}
	return filtered.String()
}

func filterFrequencies(vowels []int) {
	for v := range vowels {
		if vowels[v] != 0 {
			fmt.Printf("%d ", vowels[v])
		}
	}
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Type your string (Max: 100 chars):")
	input, _ := reader.ReadString('\n')
	input = strings.TrimSpace(input)

	if !checkLen(input) {
		return
	}

	filtered := filterNonAlphabetic(input)
	reversed := []rune(filtered)
	reverseString(reversed)

	withUnderscores := filterSpaces(string(reversed))

	var vowelCount, consonantCount int
	vowels := make([]int, 5)
	charFrequencies(string(reversed), &vowelCount, &consonantCount, vowels)

	filterFrequencies(vowels)
	fmt.Printf("%d %d %s", vowelCount, consonantCount, withUnderscores)
}
