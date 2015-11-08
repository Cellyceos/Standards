//
//  DDFModule.cpp
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 22/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "iso8211.h"

#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;
using namespace Standards;

const bool DDFModule::Open(const string& fileName, const bool testOpen) noexcept {

    if (DDFfileBuf.tellp()) {
        Close();
    }

    fstream fileDDF(fileName, ios::binary | ios::in);

    if (!fileDDF.is_open()) {
        if (!testOpen) {
            cout << "WARNING: Unable to open DDF file '" << fileName << "'." << endl;
        }

        return false;
    }

    char strBuffer[DDF_LEADER_SIZE];
    DDFfileBuf << fileDDF.rdbuf();
    fileDDF.close();

    if (!DDFfileBuf.read(strBuffer, DDF_LEADER_SIZE)) {
        cout << "WARNING: Leader is short on DDF file '" << fileName << "'." << endl;

        Close();

        return false;
    }

    recordLenght = DDFScanInt(strBuffer, 5);
    interchangeLevel = strBuffer[5];

    leaderIdentifier = strBuffer[6];
    inlineCodeExrensionIndicator = strBuffer[7];
    versionNumber = strBuffer[8];
    applicationIndicator = strBuffer[9];

    fieldControlLenght = DDFScanInt(strBuffer + 10, 2);
    fieldAreaStart = DDFScanInt(strBuffer + 12, 5);

    memcpy(extendedCharSet, strBuffer + 17, 3);

    sizeFieldLenght = DDFScanInt(strBuffer + 20, 1);
    sizeFieldPosition = DDFScanInt(strBuffer + 21, 1);
    sizeFieldTag = DDFScanInt(strBuffer + 23, 1);

    if (recordLenght < DDF_LEADER_SIZE || !fieldControlLenght || fieldAreaStart < 24 || !sizeFieldLenght || !sizeFieldPosition || !sizeFieldTag) {
        cout << "WARNING: File '" << fileName << "' does not appear to have a valid ISO 8211 header." << endl;

        Close();

        return false;
    }

    const ulong pachRecordLenght = recordLenght - DDF_LEADER_SIZE;
    auto pachRecord = new char[pachRecordLenght + 1]();

    if (!DDFfileBuf.read(pachRecord, pachRecordLenght)) {
        cout << "WARNING: Header record is short on DDF file '" << fileName << "'." << endl;

        Close();

        return false;
    }

    const ulong fieldEntryWidth = sizeFieldLenght + sizeFieldPosition + sizeFieldTag;
    for (auto idx = 0ul; idx < pachRecordLenght && pachRecord[idx] != DDF_FIELD_TERMINATOR; idx += fieldEntryWidth) {
        auto entryOffset = idx;

        auto tagName = new char[sizeFieldTag + 1]();
        memcpy(tagName, pachRecord + entryOffset, sizeFieldTag);

        entryOffset += sizeFieldTag;
        auto fieldLenght = static_cast<ulong>(DDFScanInt(pachRecord + entryOffset, sizeFieldLenght));

        entryOffset += sizeFieldLenght;
        auto fieldPosition = DDFScanInt(pachRecord + entryOffset, sizeFieldPosition);

        const auto fieldOffset = fieldAreaStart + fieldPosition - DDF_LEADER_SIZE;
        if ((recordLenght - fieldOffset) < fieldLenght) {
            cout << "WARNING: Header record invalid on DDF file '" << fileName << "'." << endl;

            Close();

            delete [] pachRecord;
            pachRecord = nullptr;

            return false;
        }

        auto field = DDFFieldDefining::Initialize(fieldControlLenght, sizeFieldTag, tagName, fieldLenght, pachRecord + fieldOffset);

        if (field) {
            fieldDefinings.push_back(field);
        }
    }

    firstRecordOffset = DDFfileBuf.tellg();

    delete [] pachRecord;
    pachRecord = nullptr;

    return true;
}

const DDFRecord *DDFModule::ReadRecord() noexcept {
    if (!currentRecord) {
        currentRecord = new DDFRecord(*this);
    }

    return currentRecord->Read() ? currentRecord : nullptr;
}
            
void DDFModule::Rewind() noexcept {
    DDFfileBuf.seekg(firstRecordOffset, ios::beg);
}

const DDFFieldDefining *DDFModule::GetFieldDefining(const ulong &idx) const noexcept {
    if (idx >= fieldDefinings.size()) {
        return nullptr;
    }

    return fieldDefinings[idx];
}

const DDFFieldDefining *DDFModule::FindFieldDefining(const string& fieldName) const noexcept {
    for (auto idx = 0ul; idx < fieldDefinings.size(); idx++) {
        auto currentFieldName = fieldDefinings[idx]->GetName();

        if (fieldName == currentFieldName) {
            return fieldDefinings[idx];
        }
    }

    return nullptr;
}

void DDFModule::Close() noexcept {
    if (DDFfileBuf.tellp()) {
        DDFfileBuf.clear();
    }

    if (fieldDefinings.size()) {
        for (auto&& field : fieldDefinings) {
            delete field;
            field = nullptr;
        }

        fieldDefinings.clear();
    }
}

DDFModule::~DDFModule() noexcept {
    Close();
}
