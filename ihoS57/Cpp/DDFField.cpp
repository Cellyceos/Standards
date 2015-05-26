//
//  DDFField.cpp
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 28/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "iso8211.h"

using namespace Standards;

const long DDFField::GetSubfieldAsLong(const string &sufieldName, const ulong fieldIdx) const noexcept {
    auto&& subfield = fieldDefining->FindSubfieldDefining(sufieldName);
    
    if (!subfield)
        return 0l;
    
    size_t bytesLeft = 0ull;
    auto&& subfieldData = GetBinaryData(subfield, fieldIdx, &bytesLeft);
    
    return subfield->ExtractDataAsLong(subfieldData, bytesLeft);
}

const double DDFField::GetSubfieldAsDouble(const string &sufieldName, const ulong fieldIdx) const noexcept {
    auto&& subfield = fieldDefining->FindSubfieldDefining(sufieldName);

    if (!subfield)
        return 0.0;
    
    size_t bytesLeft = 0ull;
    auto&& subfieldData = GetBinaryData(subfield, fieldIdx, &bytesLeft);
    
    return subfield->ExtractDataAsDouble(subfieldData, bytesLeft);
}

const char* DDFField::GetSubfieldAsString(const string &sufieldName, const ulong fieldIdx) const noexcept {
    auto&& subfield = fieldDefining->FindSubfieldDefining(sufieldName);

    if (!subfield)
        return nullptr;
    
    size_t bytesLeft = 0ull;
    auto&& subfieldData = GetBinaryData(subfield, fieldIdx, &bytesLeft);
    
    return subfield->ExtractDataAsString(subfieldData, bytesLeft);
}

const byte* DDFField::GetSubfieldAsBinary(const string &sufieldName, const ulong fieldIdx) const noexcept {
    auto&& subfield = fieldDefining->FindSubfieldDefining(sufieldName);
    
    if (!subfield)
        return nullptr;
    
    return GetBinaryData(subfield, fieldIdx);
}

const byte* DDFField::GetBinaryData(const DDFSubfieldDefining *&subfieldDef, const ulong& fieldIdx, size_t *byteLeft) const noexcept {
    size_t consumedBytes = 0ull;
    size_t offset = 0ull;
    
    if (fieldIdx && fieldDefining->GetFixedWidth()) {
        offset = fieldDefining->GetFixedWidth() * fieldIdx;
    }
    
    for (auto idx = 0ul; fieldDefining->GetSubfieldCount(); idx++) {
        auto&& subfield = fieldDefining->GetSubfieldDefining(idx);
        
        if (subfield == subfieldDef) {
            if (byteLeft) {
                *byteLeft = dataSize - offset;
            }
            
            return binaryData + offset;
        }
        
        subfield->GetDataLenght(binaryData + offset, dataSize - offset, &consumedBytes);
        offset += consumedBytes;
    }
    
    return nullptr;
}

const size_t DDFField::GetRepeatCount() const noexcept {
    if (!fieldDefining->isRepiting()) {
        return 1ul;
    }
    
    if (fieldDefining->GetFixedWidth()) {
        return dataSize / fieldDefining->GetFixedWidth();
    }
    
    return 1ul;
}
