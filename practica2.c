#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>


void reverse_string(wchar_t *string) {
    wchar_t *start = string;
    wchar_t *end = string;

    while (*end != L'\0') {
        end++;
    }
    end--;

    while (start < end) {
        wchar_t aux = *start;
        *start = *end;
        *end = aux;
        start++;
        end--;
    }

    wprintf(L"%ls ", string);
}


int isAlphabet(wchar_t c) {
    return (iswalpha(c) || c == L' ' || 
            c == L'á' || c == L'é' || c == L'í' || c == L'ó' || c == L'ú' ||
            c == L'Á' || c == L'É' || c == L'Í' || c == L'Ó' || c == L'Ú');
}


int checklen_and_alphabet(wchar_t *string) {
    wchar_t *start = string;
    int length = 0;
    int hasAlphabet = 0;

    while (*start != L'\0') {
        if (isAlphabet(*start)) {
            hasAlphabet = 1;
        }
        length++;
        start++;
    }

    if (length == 0) {
        wprintf(L"The string is empty. Exiting....\n");
        return 1;
    }

    if (length > 100) {
        wprintf(L"The length %d of the string exceeds 100. Exiting....\n", length);
        return 1;
    }

    if (!hasAlphabet) {
        wprintf(L"The string contains no alphabetic characters. Exiting....\n");
        return 1;
    }

    return 0;
}


void charFrecuencies(wchar_t *string, int *vowelCounter, int *consonantCounter, int vowel[5]) {
    *vowelCounter = 0;
    *consonantCounter = 0;

    while (*string != L'\0') {
        wchar_t ch = towlower(*string);

        if (ch == L'a' || ch == L'á') { (*vowelCounter)++; vowel[0]++; }
        else if (ch == L'e' || ch == L'é') { (*vowelCounter)++; vowel[1]++; }
        else if (ch == L'i' || ch == L'í') { (*vowelCounter)++; vowel[2]++; }
        else if (ch == L'o' || ch == L'ó') { (*vowelCounter)++; vowel[3]++; }
        else if (ch == L'u' || ch == L'ú') { (*vowelCounter)++; vowel[4]++; }
        else if (iswalpha(ch)) { (*consonantCounter)++; }

        string++;
    }
}


wchar_t* filterSpaces(wchar_t *string) {
    wchar_t *pointer = string;
    while (*pointer != L'\0') {
        if (*pointer == L' ') {
            *pointer = L'_';
        }
        pointer++;
    }
    return string;
}


void filterNonAlphabetic(wchar_t *string) {
    wchar_t *read = string;
    wchar_t *write = string;

    while (*read != L'\0') {
        if (isAlphabet(*read)) {
            *write = *read;
            write++;
        }
        read++;
    }

    *write = L'\0'; 
}


void filterFrecuencies(int vowel[5]) {
    for (int i = 0; i < 5; i++) {
        if (vowel[i] != 0) {
            wprintf(L"%d ", vowel[i]);
        }
    }
}

int main() {
    setlocale(LC_ALL, ""); 

    wchar_t string[1000];
    int vowelCount;
    int consonantCount;
    int vowels[5] = {0};

    wprintf(L"Type your string (Max: 100 chars):\n");
    fgetws(string, sizeof(string) / sizeof(wchar_t), stdin);

    int len = checklen_and_alphabet(string);
    if (len == 1) {
        return 1;
    }

    filterNonAlphabetic(string);
    reverse_string(string);
    filterSpaces(string);
    charFrecuencies(string, &vowelCount, &consonantCount, vowels);
    filterFrecuencies(vowels);
    wprintf(L"%d %d %ls", vowelCount, consonantCount, string);

    return 0;
}
