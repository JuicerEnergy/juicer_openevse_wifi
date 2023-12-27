#include <Arduino.h>

char* releaseAndAllocateString(char* orgString, const char* newString){
    if (orgString){
        free(orgString);
        orgString = NULL ;
    }

    return strdup(newString);
}