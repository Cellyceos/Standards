//
//  DDFFIeldDefining.cpp
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 23/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "iso8211.h"

#include <cstring>
#include <iostream>

using namespace std;
using namespace Standards;

DDFSubfieldDefining const* DDFFieldDefining::GetSubfieldDefining(const ulong &idx) const noexcept {
    if (idx >= subfieldDefinings.size()) {
        return nullptr;
    }

    return subfieldDefinings[idx];
}

const DDFSubfieldDefining* DDFFieldDefining::FindSubfieldDefining(const string &mnemonicTag) const noexcept {
    const auto &&subfieldDefiningsSize = subfieldDefinings.size();
    
    for (auto idx = 0ul; idx < subfieldDefiningsSize; ++idx) {
        auto &&currentSubfieldName = subfieldDefinings[idx]->GetName();

        if (currentSubfieldName == mnemonicTag) {
            return subfieldDefinings[idx];
        }
    }

    return nullptr;
}

bool DDFFieldDefining::BuildSubfields(const char*& descriptorArray, const size_t& descriptorArrayLenght, const char*& formatControls, const size_t& formatControlsLenght) noexcept {
    if (formatControlsLenght < 2 || formatControls[0] != '(' || formatControls[formatControlsLenght - 1] != ')') {
        cout << "WARNING: Format controls for '" << tagName << "' field missing brackets: " << formatControls << endl;
        return false;
    }

    auto descriptors = descriptorArray;
    const auto controls = formatControls + 1;

    if (*descriptors == '*') {
        isRepitingSubfields = true;
        ++descriptors;
    }

    char* formatString = nullptr;
    auto subDescriptors = strchr(descriptors, '*');

    size_t numberOfRepeat = 0ul;
    size_t formatStringOffset = 0ul;
    size_t mnemonicNameOffset = 0ul;
    size_t formatStringLenght = 0ul;
    size_t mnemonicNameLenght = 0ul;
    size_t subDescriptorLenght = subDescriptors ? strlen(subDescriptors) : 0ul;

    while (mnemonicNameOffset < (descriptorArrayLenght - subDescriptorLenght)) {
        if (!numberOfRepeat && !subDescriptors) {
            formatString = DDFFetchVariable(controls + formatStringOffset, formatControlsLenght - formatStringOffset - 2, &formatStringLenght, ',');
            formatStringOffset += formatStringLenght;

            if (isdigit(*formatString)) {
                size_t endNumberPointer = 0ull;
                numberOfRepeat = stoul(formatString, &endNumberPointer, 10);

                DDFCorrectString(formatString, endNumberPointer, &formatStringLenght);
            }
        } else if (subDescriptors && !formatString) {
            formatString = DDFCloneString(controls, formatControlsLenght - 2);
        }

        auto mnemonicName = DDFFetchVariable(descriptors + mnemonicNameOffset, descriptorArrayLenght - mnemonicNameOffset, &mnemonicNameLenght, '!', '*');
        mnemonicNameOffset += mnemonicNameLenght;

        auto subField = DDFSubfieldDefining::Initialize(mnemonicName, formatString, subDescriptors);

        if (subField) {
            subfieldDefinings.push_back(subField);
        }

        if (numberOfRepeat) {
            --numberOfRepeat;
        }
    }

    for(auto&& subfueld : subfieldDefinings) {
        if(!subfueld->GetWidth()) {
            fixedWidth = 0;
            break;
        } else {
            fixedWidth += subfueld->GetWidth();
        }
    }

    return true;
}

DDFFieldDefining* DDFFieldDefining::Initialize(const ulong& fieldControlLenght, const ulong& sizeFieldTag, char *tagName, const ulong& fieldEntrySize, const char *binaryData) noexcept {
    auto newFieldDefining = new DDFFieldDefining;

    size_t fieldDefiningOffset = fieldControlLenght;
    newFieldDefining->tagName = DDFStringUpper(tagName);

    switch (binaryData[0]) {
        default:
            cout << "WARNING: Unrecognised data structure code '" << binaryData[0] << "'." << endl << "Field " << tagName << " initialization incorrect." << endl;
        case ' ':
        case '0':
            newFieldDefining->dataStructureCode = DDF_DSC_ELEMENTARY;
            break;
        case '1':
            newFieldDefining->dataStructureCode = DDF_DSC_VECTOR;
            break;
        case '2':
            newFieldDefining->dataStructureCode = DDF_DSC_ARRAY;
            break;
        case '3':
            newFieldDefining->dataStructureCode = DDF_DSC_CONCATENATED;
            break;
    }

    switch (binaryData[1]) {
        default:
            cout << "WARNING: Unrecognised data type code '" << binaryData[1] << "'." << endl << "Field " << tagName << " initialization incorrect." << endl;
        case ' ':
        case '0':
            newFieldDefining->dataTypeCode = DDF_DTC_CHAR_STRING;
            break;
        case '1':
            newFieldDefining->dataTypeCode = DDF_DTC_IMPLICIT_POINT;
            break;
        case '2':
            newFieldDefining->dataTypeCode = DDF_DTC_EXPLICIT_POINT;
            break;
        case '3':
            newFieldDefining->dataTypeCode = DDF_DTC_EXPLICIT_POINT_SCALED;
            break;
        case '4':
            newFieldDefining->dataTypeCode = DDF_DTC_CHAR_BIT_STRING;
            break;
        case '5':
            newFieldDefining->dataTypeCode = DDF_DTC_BIT_STRING;
            break;
        case '6':
            newFieldDefining->dataTypeCode = DDF_DTC_MIXED_DATA_TYPE;
            break;
    }

    switch (binaryData[6]) {
        default:
        case ' ':
            newFieldDefining->truncatedEscapeSequence = DDF_TES_LEVEL0;
            break;
        case '-':
            newFieldDefining->truncatedEscapeSequence = DDF_TES_LEVEL1;
            break;
        case '%':
            newFieldDefining->truncatedEscapeSequence = DDF_TES_LEVEL2;
            break;
    }

    size_t fieldNameLenght = 0ull;
    newFieldDefining->fieldName = DDFFetchVariable(binaryData + fieldDefiningOffset, fieldEntrySize - fieldDefiningOffset, &fieldNameLenght);
    fieldDefiningOffset += fieldNameLenght;

    size_t descriptorArrayLenght = 0ull;
    const char* descriptorArray = DDFFetchVariable(binaryData + fieldDefiningOffset, fieldEntrySize - fieldDefiningOffset, &descriptorArrayLenght);
    fieldDefiningOffset += descriptorArrayLenght;

    size_t formatControlsLenght = 0ull;
    const char* formatControls = DDFFetchVariable(binaryData + fieldDefiningOffset, fieldEntrySize - fieldDefiningOffset, &formatControlsLenght);

    if (newFieldDefining->dataStructureCode != DDF_DSC_ELEMENTARY && !newFieldDefining->BuildSubfields(descriptorArray, --descriptorArrayLenght, formatControls, --formatControlsLenght)) {
        delete newFieldDefining;
        newFieldDefining = nullptr;
    }

    delete [] descriptorArray;
    descriptorArray = nullptr;

    delete [] formatControls;
    formatControls = nullptr;

    return newFieldDefining;
}

DDFFieldDefining::~DDFFieldDefining() noexcept {
    if (tagName) {
        delete [] tagName;
        tagName = nullptr;
    }

    if (fieldName) {
        delete [] fieldName;
        fieldName = nullptr;
    }

    if (subfieldDefinings.size()) {
        for (auto &&subField : subfieldDefinings) {
            delete subField;
            subField = nullptr;
        }

        subfieldDefinings.clear();
    }
}
