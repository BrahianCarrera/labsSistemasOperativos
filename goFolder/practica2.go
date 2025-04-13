package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
	"unicode"
)

func reverseString(runes []rune) []rune {
	reversed := make([]rune, len(runes))
	copy(reversed, runes)
	for i, j := 0, len(reversed)-1; i < j; i, j = i+1, j-1 {
		reversed[i], reversed[j] = reversed[j], reversed[i]
	}
	return reversed
}

func isAlphabet(c rune) bool {
	return unicode.IsLetter(c) || c == ' ' ||
		c == 'á' || c == 'é' || c == 'í' || c == 'ó' || c == 'ú' ||
		c == 'Á' || c == 'É' || c == 'Í' || c == 'Ó' || c == 'Ú'
}

func checkLenAndAlphabet(runes []rune) bool {
	hasAlphabet := false

	for _, r := range runes {
		if isAlphabet(r) {
			hasAlphabet = true
			break
		}
	}

	if len(runes) == 0 {
		fmt.Println("The string is empty. Exiting....")
		return false
	}

	if len(runes) > 100 {
		fmt.Printf("The length %d of the string exceeds 100. Exiting....\n", len(runes))
		return false
	}

	if !hasAlphabet {
		fmt.Println("The string contains no alphabetic characters. Exiting....")
		return false
	}

	return true
}

func charFrequencies(runes []rune) (int, int, [5]int) {
	var vowelCount, consonantCount int
	var vowels [5]int

	for _, r := range runes {
		ch := unicode.ToLower(r)
		switch ch {
		case 'a', 'á':
			vowelCount++
			vowels[0]++
		case 'e', 'é':
			vowelCount++
			vowels[1]++
		case 'i', 'í':
			vowelCount++
			vowels[2]++
		case 'o', 'ó':
			vowelCount++
			vowels[3]++
		case 'u', 'ú':
			vowelCount++
			vowels[4]++
		default:
			if unicode.IsLetter(ch) {
				consonantCount++
			}
		}
	}

	return vowelCount, consonantCount, vowels
}

func filterSpaces(runes []rune) []rune {
	for i, r := range runes {
		if r == ' ' {
			runes[i] = '_'
		}
	}
	return runes
}

func filterNonAlphabetic(runes []rune) []rune {
	var filtered []rune
	for _, r := range runes {
		if isAlphabet(r) {
			filtered = append(filtered, r)
		}
	}
	return filtered
}

func filterFrequencies(vowels [5]int) {
	for _, v := range vowels {
		if v != 0 {
			fmt.Printf("%d ", v)
		}
	}

}

func main() {
	fmt.Println("Type your string (Max: 100 chars):")
	reader := bufio.NewReader(os.Stdin)
	input, _ := reader.ReadString('\n')
	input = strings.TrimSpace(input)
	runes := []rune(input)

	if !checkLenAndAlphabet(runes) {
		return
	}

	runes = filterNonAlphabetic(runes)

	reversed := reverseString(runes)
	fmt.Printf("%s ", string(reversed)) // 1. Imprime la cadena invertida original

	vowelCount, consonantCount, vowels := charFrequencies(reversed)
	filterFrequencies(vowels) // 2. Frecuencias de vocales individuales
	fmt.Printf("%d %d ", vowelCount, consonantCount)

	reversedWithUnderscores := filterSpaces(append([]rune(nil), reversed...))
	fmt.Printf("%s\n", string(reversedWithUnderscores)) // 3. Cadena invertida con espacios como guiones bajos
}
