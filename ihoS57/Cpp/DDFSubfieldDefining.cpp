//
//  DDFSubfieldDefining.cpp
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 23/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "iso8211.h"

#include <cstring>
#include <cstdint>
#include <iostream>

using namespace std;
using namespace Standards;

const long DDFSubfieldDefining::ExtractDataAsLong(const byte *sourceData, const size_t& dataSize, size_t* consumedBytes) const noexcept{
    switch (dataFormat) {
        case 'A':
        case 'I':
        case 'R':
        case 'S':
        case 'C':
            return stol(ExtractDataAsString(sourceData, dataSize, consumedBytes), nullptr, 10);
        case 'B':
            return 0l;
        case 'b': {
            byte binaryData[8];
            if (formatWidth > dataSize || formatWidth >= sizeof(binaryData)) {
                cout << "WARNING: Attempt to extract int subfield " << label << " with format failed as only " << min(dataSize, sizeof(binaryData)) << " bytes available.  Using zero." << endl;

                return 0l;
            }

            if (consumedBytes) {
                *consumedBytes = formatWidth;
            }

            memcpy(binaryData, sourceData, formatWidth);

            switch (binaryFormat) {
                case DDF_BF_UINT:
                    switch (formatWidth) {
                        case 4:
                            return *reinterpret_cast<uint32_t*>(binaryData);
                        case 2:
                            return *reinterpret_cast<uint16_t*>(binaryData);
                        case 1:
                            return *binaryData;
                        default:
                            return 0l;
                    }
                case DDF_BF_SINT:
                    switch (formatWidth) {
                        case 4:
                            return *reinterpret_cast<int32_t*>(binaryData);
                        case 2:
                            return *reinterpret_cast<int16_t*>(binaryData);
                        case 1:
                            return *binaryData;
                        default:
                            return 0l;
                    }
                case DDF_BF_FLOAT_REAL:
                    switch (formatWidth) {
                        case 8:
                            return *reinterpret_cast<double*>(binaryData);
                        case 4:
                            return *reinterpret_cast<float*>(binaryData);
                        default:
                            return 0l;
                    }
                default:
                    return 0l;
            }
        }
        default:
            return 0l;
    }
}

const double DDFSubfieldDefining::ExtractDataAsDouble(const byte *sourceData, const size_t& dataSize, size_t* consumedBytes) const noexcept{
    switch (dataFormat) {
        case 'A':
        case 'I':
        case 'R':
        case 'S':
        case 'C':
            return stod(ExtractDataAsString(sourceData, dataSize, consumedBytes), nullptr);
        case 'B':
            return 0.0;
        case 'b': {
            byte binaryData[8];
            if (formatWidth > dataSize || formatWidth >= sizeof(binaryData)) {
                cout << "WARNING: Attempt to extract float subfield " << label << " with format failed as only " << min(dataSize, sizeof(binaryData)) << " bytes available.  Using zero." << endl;

                return 0.0;
            }

            if (consumedBytes) {
                *consumedBytes = formatWidth;
            }

            memcpy(binaryData, sourceData, formatWidth);

            switch (binaryFormat) {
                case DDF_BF_UINT:
                    switch (formatWidth) {
                        case 4:
                            return *reinterpret_cast<uint32_t*>(binaryData);
                        case 2:
                            return *reinterpret_cast<uint16_t*>(binaryData);
                        case 1:
                            return static_cast<double>(*binaryData);
                        default:
                            return 0.0;
                    }
                case DDF_BF_SINT:
                    switch (formatWidth) {
                        case 4:
                            return *reinterpret_cast<int32_t*>(binaryData);
                        case 2:
                            return *reinterpret_cast<int16_t*>(binaryData);
                        case 1:
                            return static_cast<double>(*binaryData);
                        default:
                            return 0.0;
                    }
                case DDF_BF_FLOAT_REAL:
                    switch (formatWidth) {
                        case 8:
                            return *reinterpret_cast<double*>(binaryData);
                        case 4:
                            return *reinterpret_cast<float*>(binaryData);
                        default:
                            return 0.0;
                    }
                default:
                    return 0.0;
            }
        }
        default:
            return 0.0;
    }
}

const char* DDFSubfieldDefining::ExtractDataAsString(const byte *sourceData, const size_t& dataSize, size_t* consumedBytes) const noexcept{
    auto strLenght = GetDataLenght(sourceData, dataSize, consumedBytes);

    auto destString = new char[strLenght + 1]();
    memcpy(destString, sourceData, strLenght);

    return destString;
}

const byte* DDFSubfieldDefining::ExtractDataAsBinary(const byte *sourceData, const size_t& dataSize, size_t* consumedBytes) const noexcept{
    auto strLenght = GetDataLenght(sourceData, dataSize, consumedBytes);

    auto destString = new byte[strLenght + 1]();
    memcpy(destString, sourceData, strLenght);

    return destString;
}

const size_t DDFSubfieldDefining::GetDataLenght(const byte *sourceData, const size_t &dataSize, size_t* consumedBytes) const noexcept {
    size_t dataLenght = 0ull;

    if (!isVariable) {
        if (formatWidth > dataSize) {
            cout << "WARNING: Only " << dataSize << " bytes avaulable for subfield " << label << endl;

            dataLenght = dataSize;
        } else {
            dataLenght = formatWidth;
        }

        if (consumedBytes) {
            *consumedBytes = dataLenght;
        }
    } else {
        long extraConsumedBytes = 0l;
        bool isAsciiField = !(dataSize > 1 && (sourceData[dataSize - 2] == DDF_UNIT_TERMINATOR || sourceData[dataSize - 2] == DDF_UNIT_TERMINATOR) && !sourceData[dataSize - 1]);

        while (dataLenght < dataSize) {
            if (isAsciiField && (sourceData[dataLenght] == DDF_UNIT_TERMINATOR || sourceData[dataLenght] == DDF_UNIT_TERMINATOR)) {
                break;
            } else if (dataLenght > 0 && (sourceData[dataLenght - 1] == DDF_UNIT_TERMINATOR || sourceData[dataLenght - 1] == DDF_UNIT_TERMINATOR) && !sourceData[dataLenght]) {
                if ((dataLenght + 1) < dataSize && sourceData[dataLenght + 1] == DDF_FIELD_TERMINATOR) {
                    ++extraConsumedBytes;
                }

                break;
            }

            ++dataLenght;
        }

        if (consumedBytes) {
            *consumedBytes = dataLenght + extraConsumedBytes + 1;
        }
    }

    return dataLenght;
}

DDFSubfieldDefining* DDFSubfieldDefining::Initialize(char* tagName,const char* formatString, const char* pachFieldArea) noexcept {
    auto newSubfieldDefining = new DDFSubfieldDefining;

    newSubfieldDefining->label = DDFStringUpper(tagName);

    if (pachFieldArea) {
        if (!newSubfieldDefining->BuildSubfields(pachFieldArea, formatString)) {
            delete newSubfieldDefining;
            newSubfieldDefining = nullptr;
        }
    } else {
        newSubfieldDefining->ApplyFormat(formatString);
    }

    return newSubfieldDefining;
}

bool DDFSubfieldDefining::ApplyFormat(const char*& stringFormat) noexcept {
    if (*(stringFormat + 1) == '(') {
        formatWidth = stol(stringFormat + 2, nullptr, 10);
        isVariable = !formatWidth;
    }

    dataFormat = *stringFormat;

    switch (dataFormat) {
        case 'A':
            dataType = DDF_DT_STRING;
            break;
        case 'I':
            dataType = DDF_DT_INT;
            break;
        case 'R':
            dataType = DDF_DT_FLOAT;
            break;
        case 'B':
            isVariable = false;
            dataType = DDF_DT_BINARY_STRING;

            formatWidth /= 8;
            break;
        case 'b':
            isVariable = false;

            binaryFormat = static_cast<DDFbinaryFormat>(*(stringFormat + 1) - '0');
            formatWidth = stol(stringFormat + 2, nullptr, 10);

            if (binaryFormat != DDF_BF_SINT && binaryFormat != DDF_BF_UINT) {
                dataType = DDF_DT_FLOAT;
            }
            break;
        default:
            cout << "WARNING: Format type of '" << dataFormat << "' not recognised" << endl;
            return false;
    }

    return true;
}

bool DDFSubfieldDefining::BuildSubfields(const char* pachFieldArea, const char*& stringFormat) noexcept {
    if (*pachFieldArea == '*') {
        ++pachFieldArea;
    }

    char* subFormatString = nullptr;
    const auto descriptorArrayLenght = strlen(pachFieldArea);
    const auto formatControlsLenght = strlen(stringFormat);

    size_t numberOfRepeat = 0ull;
    size_t formatStringOffset = 0ull;
    size_t formatStringLenght = 0ull;
    size_t mnemonicNameOffset = 0ull;
    size_t mnemonicNameLenght = 0ull;

    while (mnemonicNameOffset < descriptorArrayLenght) {
        if (!numberOfRepeat) {
            subFormatString = DDFFetchVariable(stringFormat + formatStringOffset, formatControlsLenght - formatStringOffset, &formatStringLenght, ',');
            formatStringOffset += formatStringLenght;

            if (isdigit(*stringFormat)) {
                size_t endNumberPointer = 0ull;
                numberOfRepeat = stoul(subFormatString, &endNumberPointer, 10);

                DDFCorrectString(subFormatString, endNumberPointer, &formatStringLenght);
            }
        } else {
            subFormatString = DDFCloneString(subFormatString, formatStringLenght);
        }

        auto mnemonicName = DDFFetchVariable(pachFieldArea + mnemonicNameOffset, descriptorArrayLenght - mnemonicNameOffset, &mnemonicNameLenght, '!');
        mnemonicNameOffset += mnemonicNameLenght;

        auto subField = DDFSubfieldDefining::Initialize(mnemonicName, subFormatString, nullptr);

        if (subField) {
            subfieldDefinings.push_back(subField);
        }

        if (numberOfRepeat) {
            --numberOfRepeat;
        }
    }

    return true;
}

DDFSubfieldDefining::~DDFSubfieldDefining() noexcept {
    if (label) {
        delete [] label;
        label = nullptr;
    }

    if (subfieldDefinings.size()) {
        for (auto&& subField : subfieldDefinings) {
            delete subField;
            subField = nullptr;
        }

        subfieldDefinings.clear();
    }
}
