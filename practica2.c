#include <stdio.h>
#include <string.h>
#include <ctype.h>

void reverse_string(char * string){

    char * start = string;
    char * end = string;
    
    while( *end != '\0'){

        end++; 
    }
    end--;
    
    while(start < end){

        char aux  = *start;
        *start = *end;
        *end = aux;

        start++;
        end--;

    }
    
    printf("%s ", string);
};

int isAlphabet(char c) {
    return ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') );
}

int checklen_and_alphabet(char * string) {
    char *start = string;
    int length = 0;
    int hasAlphabet = 0;

    while (*start != '\0') {
        if (isAlphabet(*start)) {
            hasAlphabet = 1;
        }
        length++;
        start++;
    }

    if (length == 0) {
        printf("The string is empty. Exiting....\n");
        return 1;
    }

    if (length > 100) {
        printf("The length %d of the string exceeds 100. Exiting....\n", length);
        return 1;
    }

    if (!hasAlphabet) {
        printf("The string contains no alphabetic characters. Exiting....\n");
        return 1;
    }

    return 0;
}

void charFrecuencies(char * string, int *vowelCounter, int *consonantsCounter, int vowel[5] ){
    char *start = string;
    


    *vowelCounter = 0;
    *consonantsCounter = 0;

    while (*start != '\0') {
        char ch = tolower(*start);
        if (ch == 'a') { 
            (*vowelCounter)++;
             vowel[0]++; }
        else if (ch == 'e') { 
            (*vowelCounter)++; 
            vowel[1]++; }
        else if (ch == 'i') { 
            (*vowelCounter)++;
             vowel[2]++; }
        else if (ch == 'o') { 
            (*vowelCounter)++;
             vowel[3]++; }
        else if (ch == 'u') { 
            (*vowelCounter)++;
             vowel[4]++; }
        else if (isalpha(ch)) { 
            (*consonantsCounter)++; }
        start++;
    }
}

char * filterSpaces(char * string){

    char *pointer = string;
    while(*pointer !='\0' ){
        if (*pointer == ' '){
            *pointer = '_';
        }
        pointer++;
    }
    return string;
}



void filterNonAlphabetic(char *string) {
    char *read = string;
    char *write = string;

    while (*read != '\0') {
        if (isAlphabet(*read)) {
            *write = *read;
            write++;
        }
        read++;
    }

    *write = '\0'; // Cierra la cadena limpiamente
}

void filterFrecuencies(int vowel[5]){
    for (int i = 0; i < 5; i++ ){

        if(vowel[i] != 0){
            printf("%d ", vowel[i]);
        }
    
    } 
}

int main() {
    char string[1000];
    int vowelCount;
    int consonantCount;
    int vowels[5] = {0};


    
    printf("Type your string (Max: 100 chars) \n" );
    fgets(string, sizeof(string), stdin);

    int len = checklen_and_alphabet(string);
    if (len == 1){
        return 1;
    }
    
    filterNonAlphabetic(string);
    reverse_string(string);
    char * spaces = filterSpaces(string);
    charFrecuencies(string, &vowelCount,&consonantCount,vowels);
    filterFrecuencies(vowels);
    printf("%d %d %s", vowelCount, consonantCount, spaces );

    return 0;
}





