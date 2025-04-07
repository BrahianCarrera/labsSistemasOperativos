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
    
    printf("%s" , string);
};

int checklen(char * string){
    char *start = string;
    int counter = 0;
    while(*start !='\0' ){
        counter++;
        start++;
    }
    if (counter > 100 || counter == 0 ) {
        printf("the lenght of the string is not correct. Exiting ....");
        return 1;
    }
    else{
        return 0;
    }
};

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

void filterSpaces(char * string){

    char *pointer = string;
    while(*pointer !='\0' ){
        if (*pointer == ' '){
            *pointer = '_';
        }
        pointer++;
    }

    printf(string);

}


int isAlphabet(char c) {
    
    return ((c >= 'A' && c <= 'Z')|| c =='_') || ((c >= 'a' && c <= 'z') || c ==' ');
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






int main() {
    char string[100];
    int vowelCount;
    int consonantCount;
    int vowels[5] = {0};


    
    printf("Type your string (Max: 100 chars) \n" );
    fgets(string, sizeof(string), stdin);

    int len = checklen(string);
    if (len == 1){
        return 1;
    }
    filterSpaces(string);
    filterNonAlphabetic(string);
    reverse_string(string);
    charFrecuencies(string, &vowelCount,&consonantCount,vowels);

    printf("\n  number of vowels: %d\n", vowelCount);
    printf("number of consonants: %d\n", consonantCount);
    printf("%d", vowels[1]);

    return 0;
}





