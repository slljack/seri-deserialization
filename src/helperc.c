#include"helperc.h"
#include <stdio.h>

int new_strcmp(char *str1, char *str2){

    int i;
    char cha1 = *str1;
    char cha2 = *str2;
    while(cha1 != '\0' || cha2 != '\0'){

        if(cha1 == cha2){
            str1 = str1 + 1;
            str2 = str2 + 1;

            cha1 = *str1;
            cha2 = *str2;

            continue;
        }
        else
            break;
    }

    if(cha1 == cha2)
        return 0;

    if((cha1 | 32) < (cha2 | 32))
        return -1;

    return 1;
}

int is_arg(char *str1){
    char startSign = *str1;
    if(startSign == '-')
        return -1;
    else
        return 0;
}

int new_strlen(char *str1){
    int answer = 0;
    char char1 = *str1;
    while(char1 != '\0'){
        str1 = str1 + 1;
        char1 = *str1;
        answer++;
    }
    answer++;
    return answer;
}