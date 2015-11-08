﻿//
//  ENCChart.cpp
//  International Hydrographic Organization
//  IHO Transfer Standard for Digital Hydrographic Data, Publication S-57
//
//  Created by Kirill Bravichev on 22/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#include "ihoS57.h"

#include <iomanip>
#include <iostream>

using namespace std;
using namespace Standards;

const bool ENCChart::Open() noexcept{
    if (module) {
        return true;
    }

    module = new DDFModule;
    if (!module->Open(fileName)) {
        delete module;
        module = nullptr;

        return false;
    }

    if (!module->FindFieldDefining("DSID")/* && !(isCatalogField = module->FindFieldDefining("CATD"))*/) {
        cout << "ERROR: '" << fileName << "' is an ISO8211 file, but not an IHO S-57 data file." << endl;

        delete module;
        module = nullptr;

        return false;
    }

    return true;
}

const bool ENCChart::Ingest() noexcept {
    if (!module || isFileIngested)
        return false;

    const DDFRecord* record = nullptr;
    while ((record = module->ReadRecord())) {
        auto&& field = record->GetField(1ull);
        auto&& fieldDefiningName = field->GetFieldDefining()->GetName();

        if (fieldDefiningName == "DSID") {
            DSNM = field->GetSubfieldAsString("DSNM");

            auto&& dssi = record->GetField(2ull);
            NALL = dssi->GetSubfieldAsLong("NALL");
            AALL = dssi->GetSubfieldAsLong("AALL");
        } else if (fieldDefiningName == "DSPM") {
            COMF = max(1l, field->GetSubfieldAsLong("COMF"));
            SOMF = max(1l, field->GetSubfieldAsLong("SOMF"));
            CSCL = max(1l, field->GetSubfieldAsLong("CSCL"));
        } else if (fieldDefiningName == "VRID") {
            ReadVector(record, field);
        } else if (fieldDefiningName == "FRID") {
            ReadFeature(record, field);
        } else {
            cout << "INFO: Skipping '" << fieldDefiningName << "' record in ihoS57::Ingest()." << endl;
        }
    }

    isFileIngested = true;

    return FindAndUpplyUpdates();
}

const bool ENCChart::View() noexcept {
    ulong recordNumber = 0ul;

    const DDFRecord *record = nullptr;
    while ((record = module->ReadRecord())) {
        cout << "Record " << ++recordNumber << " (" << record->GetDataSize() << " bytes)" << endl;

        for (auto fieldIdx = 0ul; fieldIdx < record->GetFieldCount(); ++fieldIdx) {
            auto &&field = record->GetField(fieldIdx);
            auto &&fieldDefining = field->GetFieldDefining();

            size_t bytesConsumed = 0ul;
            auto dataSize = field->GetDataSize();
            auto binaryData = field->GetBinaryData();

            for (auto fieldIdx = 0ul; fieldIdx < field->GetRepeatCount(); ++fieldIdx) {
                cout << "Field " << fieldDefining->GetName() << ": " << fieldDefining->GetDescription() << endl;

                for (auto subfieldIdx = 0ul; subfieldIdx < fieldDefining->GetSubfieldCount(); ++subfieldIdx) {
                    auto &&subfieldDefining = fieldDefining->GetSubfieldDefining(subfieldIdx);
                    auto &&subfieldName = subfieldDefining->GetName();

                    switch (subfieldDefining->GetDataType()) {
                    case DDF_DT_INT:
                        if (subfieldDefining->GetBinaryFormat() == DDF_BF_UINT) {
                            cout << setw(8) << subfieldName << " = " << static_cast<ulong>(subfieldDefining->ExtractDataAsLong(binaryData, dataSize, &bytesConsumed)) << endl;
                        } else {
                            cout << setw(8) << subfieldName << " = " << subfieldDefining->ExtractDataAsLong(binaryData, dataSize, &bytesConsumed) << endl;
                        }
                        break;
                    case DDF_DT_FLOAT:
                        cout << setw(8) << subfieldName << " = " << subfieldDefining->ExtractDataAsDouble(binaryData, dataSize, &bytesConsumed) << endl;
                        break;
                    case DDF_DT_STRING: {
                        auto &&str = subfieldDefining->ExtractDataAsString(binaryData, dataSize, &bytesConsumed);
                        cout << setw(8) << subfieldName << " = " << str << endl;

                        delete[] str;
                        str = nullptr;
                    }
                        break;
                    case DDF_DT_BINARY_STRING: {
                        auto &&binaryString = subfieldDefining->ExtractDataAsBinary(binaryData, dataSize, &bytesConsumed);
                        cout << setw(8) << subfieldName << " = ";

                        cout << hex;
                        for (auto idx = 0ul; idx < bytesConsumed; ++idx) {
                            cout << static_cast<short>(binaryString[idx]);
                        }

                        cout << dec << endl;
                        if (subfieldName == "NAME") {
                            cout << setw(24) << "VRID RCNM = " << static_cast<short>(binaryString[0]) << endl;
                            cout << setw(24) << "RCID = " << binaryString[1] + binaryString[2] * 256u + binaryString[3] * 65536u + binaryString[4] * 16777216u << endl;
                        } else if (subfieldName == "LNAM") {
                            cout << setw(24) << "FOID AGEN = " << binaryString[0] + binaryString[1] * 256u << endl;
                            cout << setw(24) << "FIDN = " << binaryString[2] + binaryString[3] * 256u + binaryString[4] * 65536u + binaryString[5] * 16777216u << endl;
                            cout << setw(24) << "FIDS = " << binaryString[6] + binaryString[7] * 256u << endl;
                        }

                        delete [] binaryString;
                        binaryString = nullptr;
                    }
                        break;
                    default:
                        cout << "WARNING: Format type of '" << subfieldDefining->GetDataType() << "' not recognised" << endl;
                    }

                    dataSize -= bytesConsumed;
                    binaryData += bytesConsumed;
                }
            }
        }
    }
    
    module->Rewind();

    return true;
}

const bool ENCChart::FindAndUpplyUpdates() noexcept {
    static char updExtension[5];

    const auto foundIndex = fileName.find_last_of('.');

    if (foundIndex == string::npos) {
        return false;
    }

    DDFModule updModule;
    byte updNumber = 1u;

    while(true) {
        sprintf(updExtension, ".%03hhu", updNumber);
        if(!updModule.Open(fileName.substr(0u, foundIndex) + updExtension, true))
            break;

        if (!ApplyUpdates(updModule, updNumber)) {
            return false;
        }

        ++updNumber;
    }

    return true;
}

const bool ENCChart::ApplyUpdates(DDFModule &updModule, byte &updNumber) noexcept{
    return true;
}

const bool ENCChart::ReadVector(const DDFRecord *&record, const DDFField *&fieldVRID) noexcept{
    auto &&RCNM = fieldVRID->GetSubfieldAsLong("RCNM");
    auto &&RCID = fieldVRID->GetSubfieldAsLong("RCID");

    if (RCNM < ENC_RCNM_VI || RCNM > ENC_RCNM_VF) {
        cout << "ERROR: Unrecognised record name code '" << RCNM << "'." << endl << "Field initialization incorrect." << endl;
        return false;
    }

    const DDFField *field = nullptr;
    ENCFeature *geomFeature = nullptr;
    if (RCNM == ENC_RCNM_VI || RCNM == ENC_RCNM_VC) {
        auto &&pointFeature = static_cast<ENCPointGeometry*>(geomFeature = new ENCPointGeometry);

        if ((field = record->FindField("SG2D"))) {
            pointFeature->point.x = field->GetSubfieldAsLong("XCOO") / static_cast<double>(COMF);
            pointFeature->point.y = field->GetSubfieldAsLong("YCOO") / static_cast<double>(COMF);
        } else if ((field = record->FindField("SG3D"))){
            pointFeature->point.x = field->GetSubfieldAsLong("XCOO") / static_cast<double>(COMF);
            pointFeature->point.y = field->GetSubfieldAsLong("YCOO") / static_cast<double>(COMF);
            pointFeature->point.z = field->GetSubfieldAsLong("VE3D") / static_cast<double>(SOMF);
        }

        boundingRegion.Extend(pointFeature->point.x, pointFeature->point.y);
    } else if (RCNM == ENC_RCNM_VE) {
        auto &&edgeFeature = static_cast<ENCEdgeGeometry*>(geomFeature = new ENCEdgeGeometry);

        if ((field = record->FindField("SG2D"))) {
            auto &&count = field->GetRepeatCount();
            for (auto idx = 0ul; idx < count; idx++) {
                auto &&x = field->GetSubfieldAsLong("XCOO", idx) / static_cast<double>(COMF);
                auto &&y = field->GetSubfieldAsLong("YCOO", idx) / static_cast<double>(COMF);

                boundingRegion.Extend(x, y);
				edgeFeature->points.push_back({ x, y, 0.0 });
            }
        }

        if ((field = record->FindField("VRPT"))) {
            auto binaryString = field->GetSubfieldAsBinary("NAME");

            edgeFeature->beginNode.RCNM = static_cast<ENCrecordName>(binaryString[0]);
            edgeFeature->beginNode.ORNT = static_cast<ENCorientation>(field->GetSubfieldAsLong("ORNT"));
            edgeFeature->beginNode.USAG = static_cast<ENCusageIndicator>(field->GetSubfieldAsLong("USAG"));
            edgeFeature->beginNode.MASK = static_cast<ENCmaskingIndicator>(field->GetSubfieldAsLong("MASK"));
            edgeFeature->beginNode.TOPI = static_cast<ENCtopologyIndicator>(field->GetSubfieldAsLong("TOPI"));
            edgeFeature->beginNode.RCID = binaryString[1] + (binaryString[2] * 256u) + (binaryString[3] * 65536u) + (binaryString[4] * 16777216u);

            if (field->GetRepeatCount() > 1) {
                binaryString = field->GetSubfieldAsBinary("NAME", 1ul);

                edgeFeature->endNode.RCNM = static_cast<ENCrecordName>(binaryString[0]);
                edgeFeature->endNode.ORNT = static_cast<ENCorientation>(field->GetSubfieldAsLong("ORNT", 1ul));
                edgeFeature->endNode.USAG = static_cast<ENCusageIndicator>(field->GetSubfieldAsLong("USAG", 1ul));
                edgeFeature->endNode.MASK = static_cast<ENCmaskingIndicator>(field->GetSubfieldAsLong("MASK", 1ul));
                edgeFeature->endNode.TOPI = static_cast<ENCtopologyIndicator>(field->GetSubfieldAsLong("TOPI", 1ul));
                edgeFeature->endNode.RCID = binaryString[1] + (binaryString[2] * 256u) + (binaryString[3] * 65536u) + (binaryString[4] * 16777216u);
            }
        }
    } /*else if (RCNM == ENC_RCNM_VF) {

    }*/

    geomFeature->RCID = RCID;
    geomFeature->RCNM = static_cast<ENCrecordName>(RCNM);
    geomFeature->RVER = fieldVRID->GetSubfieldAsLong("RVER");
    geomFeature->RUIN = static_cast<ENCrecordUpdateInstruction>(fieldVRID->GetSubfieldAsLong("RUIN"));

    switch (RCNM) {
        case ENC_RCNM_VC:
            connectedNodes[RCID] = geomFeature;
            break;
        case ENC_RCNM_VE:
            edges[RCID] = geomFeature;
            break;
        case ENC_RCNM_VI:
            isolatedNodes[RCID] = geomFeature;
            break;
        /*case ENC_RCNM_VF:
            faces[RCID] = geomFeature;
            break;*/
        default:
            return false;
    }

    return true;
}

const bool ENCChart::ReadFeature(const DDFRecord *&record, const DDFField *&fieldFRID) noexcept{
    auto &&PRIM = fieldFRID->GetSubfieldAsLong("PRIM");
    auto &&RCID = fieldFRID->GetSubfieldAsLong("RCID");

    if (PRIM > ENC_PRIM_A/* && PRIM != ENC_PRIM_N */) {
        cout << "ERROR: Unrecognised geometric object primitive code '" << PRIM << "'." << endl << "Field initialization incorrect." << endl;
        return false;
    }

    const DDFField *field = nullptr;
    auto &&primFeature = new ENCGeometryPrimitive;

    primFeature->RCID = RCID;
    primFeature->RVER = fieldFRID->GetSubfieldAsLong("RVER");
    primFeature->PRIM = static_cast<ENCgeometricPrimitive>(PRIM);
    primFeature->GRUP = static_cast<byte>(fieldFRID->GetSubfieldAsLong("GRUP"));
    primFeature->RCNM = static_cast<ENCrecordName>(fieldFRID->GetSubfieldAsLong("RCNM"));
	primFeature->OBJL = static_cast<ENCobjectsAcronymCode>(fieldFRID->GetSubfieldAsLong("OBJL"));
	primFeature->RUIN = static_cast<ENCrecordUpdateInstruction>(fieldFRID->GetSubfieldAsLong("RUIN"));

    if ((field = record->FindField("FOID"))) {
        primFeature->AGEN = field->GetSubfieldAsLong("AGEN");
        primFeature->FIDN = field->GetSubfieldAsLong("FIDN");
        primFeature->FIDS = field->GetSubfieldAsLong("FIDS");
    }

    if ((field = record->FindField("FSPT"))) {
        auto&& count = field->GetRepeatCount();

        ENCGeometryPrimitive::ENCspatialRecordPointer record;
        for (auto idx = 0ul; idx < count; idx++) {
            auto binaryString = field->GetSubfieldAsBinary("NAME", idx);

            record.RCNM = static_cast<ENCrecordName>(binaryString[0]);
            record.ORNT = static_cast<ENCorientation>(field->GetSubfieldAsLong("ORNT", idx));
            record.USAG = static_cast<ENCusageIndicator>(field->GetSubfieldAsLong("USAG", idx));
            record.MASK = static_cast<ENCmaskingIndicator>(field->GetSubfieldAsLong("MASK", idx));
            record.RCID = binaryString[1] + (binaryString[2] * 256u) + (binaryString[3] * 65536u) + (binaryString[4] * 16777216u);

            primFeature->FSPTObjects.push_back(record);
        }
    }

    /*if ((field = record->FindField("ATTF"))) {

    }*/

    features[RCID] = primFeature;

    return true;
}

void ENCChart::Close() noexcept{
    if (module) {
        if (edges.size()) {
            for (auto &&edge : edges) {
                delete edge.second;
                edge.second = nullptr;
            }

            edges.clear();
        }

        /*if (faces.size()) {
            for (auto &&face : faces) {
                delete face.second;
                face.second = nullptr;
            }

            faces.clear();
        }*/

        if (features.size()) {
            for (auto &&feature : features) {
                delete feature.second;
                feature.second = nullptr;
            }

            features.clear();
        }

        if (isolatedNodes.size()) {
            for (auto &&isolatedNode : isolatedNodes) {
                delete isolatedNode.second;
                isolatedNode.second = nullptr;
            }

            isolatedNodes.clear();
        }

        if (connectedNodes.size()) {
            for (auto &&connectedNode : connectedNodes) {
                delete connectedNode.second;
                connectedNode.second = nullptr;
            }

            connectedNodes.clear();
        }

        delete module;
        module = nullptr;
    }

    if (DSNM) {
        delete [] DSNM;
        DSNM = nullptr;
    }
}

ENCChart::~ENCChart() noexcept{
    Close();
}
