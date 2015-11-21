//
//  DDFRecord.cpp
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 28/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "iso8211.h"

#include <cstring>
#include <iostream>

using namespace std;
using namespace Standards;

const DDFField* DDFRecord::GetField(const ulong &idx) const {
    if (idx > fields.size()) {
        return nullptr;
    }

    return fields[idx];
}

const DDFField *DDFRecord::FindField(const string &fieldName) const {
    const auto &&fieldsSize = fields.size();
    
    for (auto idx = 0ul; idx < fieldsSize; ++idx) {
        auto currentFieldName = fields[idx]->GetFieldDefining()->GetName();

        if (fieldName == currentFieldName) {
            return fields[idx];
        }
    }

    return nullptr;
}

bool DDFRecord::Read() {
    if (!isReuseHeader) {
        return ReadHeader();
    }

    return false;
}

bool DDFRecord::ReadHeader() {
    Clear();

    if (parentModule.DDFfileBuf.peek() == EOF) {
        parentModule.DDFfileBuf.clear();

        return false;
    }

    char strBuffer[DDF_LEADER_SIZE];

    //auto pos = parentModule.fileDDF.tellg();
    if (!parentModule.DDFfileBuf.read(strBuffer, DDF_LEADER_SIZE)) {
        cout << "WARNING: DR leader is short on DDF file." << endl;
        return false;
    }

    isReuseHeader = strBuffer[6] == 'R';

    auto recordLenght = DDFScanInt(strBuffer, 5);
    auto fieldAreaStart = DDFScanInt(strBuffer + 12, 5);

    auto sizeFieldLenght = DDFScanInt(strBuffer + 20, 1);
    auto sizeFieldPosition = DDFScanInt(strBuffer + 21, 1);
    auto sizeFieldTag = DDFScanInt(strBuffer + 23, 1);

    if (recordLenght < DDF_LEADER_SIZE || fieldAreaStart < DDF_LEADER_SIZE || !sizeFieldLenght || !sizeFieldPosition || !sizeFieldTag) {
        cout << "WARNING: Data record appears to be corrupt on DDF file." << endl << "-- ensure that the files were uncompressed without modifying carriage return/linefeeds (by default WINZIP does this)." << endl;

        return false;
    }

    dataSize = recordLenght - DDF_LEADER_SIZE;
    binaryData = new byte[dataSize + 1]();

    if (!parentModule.DDFfileBuf.read(reinterpret_cast<char*>(binaryData), dataSize)) {
        cout << "WARNING: Data record is short on DDF file." << endl;

        return false;
    }

    auto tagName = new char[sizeFieldTag + 1]();
    const long fieldEntryWidth = sizeFieldLenght + sizeFieldPosition + sizeFieldTag;
    for (auto idx = 0ul; idx < dataSize && binaryData[idx] != DDF_FIELD_TERMINATOR; idx += fieldEntryWidth) {
        auto entryOffset = idx;

        memcpy(tagName, binaryData + entryOffset, sizeFieldTag);

        entryOffset += sizeFieldTag;
        auto fieldLenght = DDFScanInt(reinterpret_cast<char*>(binaryData + entryOffset), sizeFieldLenght);

        entryOffset += sizeFieldLenght;
        auto fieldPosition = DDFScanInt(reinterpret_cast<char*>(binaryData + entryOffset), sizeFieldPosition);

        auto fieldDefinition = parentModule[tagName];

        if (!fieldDefinition) {
            cout << "WARNING: Undefined field '" << tagName << "' encoutered in data record" << endl;

            delete [] tagName;
            tagName = nullptr;

            return false;
        }

        const auto fieldOffset = fieldAreaStart + fieldPosition - DDF_LEADER_SIZE;
        if ((recordLenght - fieldOffset) < fieldLenght) {
            cout << "WARNING: Not enough byte to initialize field '" << tagName << "'." << endl;

            delete [] tagName;
            tagName = nullptr;

            return false;
        }

        fields.push_back(new DDFField(fieldDefinition, fieldLenght, binaryData + fieldOffset));
    }

    delete [] tagName;
    tagName = nullptr;

    return true;
}

void DDFRecord::Clear() {
    if (fields.size()) {
        for (auto &&field : fields) {
            delete field;
            field = nullptr;
        }

        fields.clear();
    }

    if (binaryData) {
        delete [] binaryData;
        binaryData = nullptr;
        dataSize = 0;
    }

    isClone = false;
    isReuseHeader = false;
}

DDFRecord* DDFRecord::Clone() const noexcept {
    auto cloneRecord = new DDFRecord(parentModule);

    cloneRecord->dataSize = dataSize;
    cloneRecord->isClone = true;

    cloneRecord->binaryData = new byte[dataSize + 1]();
    memcpy(cloneRecord->binaryData, binaryData, dataSize);

    for(auto&& field : fields) {
        auto fieldOffset = field->GetBinaryData() - binaryData;
        cloneRecord->fields.push_back(new DDFField(field->GetFieldDefining(), field->GetDataSize(), cloneRecord->binaryData + fieldOffset));
    }

    return cloneRecord;
}

DDFRecord::~DDFRecord() {
    Clear();
}
