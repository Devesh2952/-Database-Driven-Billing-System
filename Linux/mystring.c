#include<stdio.h>
#include<stdlib.h>

char* getString() //Dynamically allocates string (Input from Terminal)
{
    char *str = NULL;
    while(getchar()!='\n');
    int i=0;
    do
    {
        str = realloc(str,(i+1)*sizeof(char));
        str[i] = getchar();
    }while(str[i++]!='\n');
    str[i-1] = '\0';
    return str;
}

char* str_Alloc(char *str) //Dynamically Allocates a GIVEN Static String 
{
    char *buf = NULL;
    int i=0;
    do
    {
        buf = realloc(buf,(i+1)*sizeof(char));
        buf[i] = str[i];
    }while(buf[i++]);
    return buf;
}