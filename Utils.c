#include "Utils.h"

int statusCodeEqualsTo(int statusCode,int code){
    return (((UINTN)(statusCode)) == ((UINTN)(code)));
}