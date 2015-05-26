//
//  DDFUtils.cpp
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 28/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include <cstring>
#include "iso8211.h"

char* Standards::DDFStringUpper(char *&string) noexcept {
    auto bufp = string;

    while (*bufp) {
        if ((*bufp >= 'a') && (*bufp <= 'z')) {
            *bufp = 'A' + (*bufp - 'a');
        }

        ++bufp;
    }

    return string;
}

int Standards::DDFScanInt(const char *strSource, byte byteRead) noexcept {
    static char strBuffer[33];

    if (byteRead > 32 || !byteRead) {
        byteRead = 32;
    }

    memcpy(strBuffer, strSource, byteRead);
    strBuffer[byteRead] = '\0';

    return stoi(strBuffer, nullptr, 10);
}

char* Standards::DDFCloneString(const char *strSource, size_t strLenght) noexcept {
    if (!strSource || !strLenght) {
        return nullptr;
    }

    auto newString = new char[strLenght + 1]();

    if (newString) {
        memcpy(newString, strSource, strLenght);
    }

    return newString;
}

const bool Standards::DDFCorrectString(char *&strSource, size_t byteOffset, size_t *strLenght) noexcept {
    if (!strSource || !strLenght) {
        return false;
    }

    *strLenght -= byteOffset;
    auto newCorrectString = DDFCloneString(strSource + byteOffset, *strLenght);

    delete [] strSource;
    strSource = newCorrectString;

    return true;
}

char* Standards::DDFFetchVariable(const char *strRecord, size_t strLenght, size_t *consumedChar, const char delimCharOne, const char delimCharSecond) noexcept {
    size_t idx;
    for (idx = 0ull; idx < strLenght && strRecord[idx] != delimCharOne && strRecord[idx] != delimCharSecond; idx++ );

    auto resultVariable = new char[idx + 1]();
    memcpy(resultVariable, strRecord, idx);

    if (idx < strLenght && (strRecord[idx] == delimCharOne || strRecord[idx] == delimCharSecond)) {
        ++idx;
    }

    if (consumedChar) {
        *consumedChar = idx;
    }

    return resultVariable;
}
