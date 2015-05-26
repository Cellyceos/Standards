//
//  ihoS57.h
//  International Hydrographic Organization
//  IHO Transfer Standard for Digital Hydrographic Data, Publication S-57
//
//  Created by Kirill Bravichev on 22/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#ifndef __ProjectGIS__ihoS57__
#define __ProjectGIS__ihoS57__

#include <map>

#include "vmath.h"
#include "iso8211.h"

namespace Standards {
    /**
     * Predeclarations
     */
    class ENCFeature;
    class ENCPrimitive;
    class ENCEdgeGeometry;
    class ENCPointGeometry;
    class ENCObjectPrimitive;
    class ENCGeometryPrimitive;

    /**
     * Internal data type
     */
    typedef map<const ulong, const ENCFeature*> FeatureMap;

    class ENCChart {
        typedef struct {
            double minX = 0.0;
            double minY = 0.0;
            double maxX = 0.0;
            double maxY = 0.0;

            bool isValid = false;

            /**
             * Extend the bounding box with a point
             * @param x the x coordinate
             * @param y the y coordinate
             */
            inline void Extend(const double x, const double y) noexcept {
                if (isValid) {
                    minX = min(minX, x);
                    minY = min(minY, y);
                    maxX = max(maxX, x);
                    maxY = max(maxY, y);
                } else {
                    minX = maxX = x;
                    minY = maxY = y;
                    isValid = true;
                }
            }

            /**
             * Gets width
             */
            inline const double GetWidth() const noexcept { return std::abs(maxX - minX); }

            /**
             * Gets height
             */
            inline const double GetHeight() const noexcept { return std::abs(maxY - minY); }
        } ENCboundingBox;

        /**
         * Coordinate multiplication factor
         */
        long COMF = 0l;

        /**
         * 3-D (sounding) multiplication factor
         */
        long SOMF = 0l;

        /**
         * Compilation scale of data
         */
        long CSCL = 0l;

        /**
         * NATF lexical level
         */
        long NALL = 0l;

        /**
         * ATTF lexical level
         */
        long AALL = 0l;

        ENCboundingBox boundingRegion = ENCboundingBox();

        //bool isCatalogField = false;
        bool isFileIngested = false;

        /**
         * Data set name
         */
        const char* DSNM = nullptr;
        DDFModule* module = nullptr;

        const string fileName;

        FeatureMap edges;
        //FeatureMap faces;
        FeatureMap features;
        FeatureMap isolatedNodes;
        FeatureMap connectedNodes;

    public:
        /**
         * The constructor
         * @param path The path to the S57 file
         */
        ENCChart(const string path) noexcept : fileName(path), edges()/*, faces()*/, features(), isolatedNodes(), connectedNodes() { }

        /**
         * Open a S57 file (ENC) for reading.
         * @return false if the open fails.
         */
        const bool Open() noexcept;

        /**
         * Read all the records into memory, adding to the appropriate indexes.
         */
        const bool Ingest() noexcept;

        /**
         * Read all the recods into standart output
         */
        const bool View() noexcept;

        /**
         * Find all update files that would appear to apply to this base file.
         */
        const bool FindAndUpplyUpdates() noexcept;

        auto GetConnectedNodes() const noexcept -> const FeatureMap& { return connectedNodes; }
        auto GetIsolatedNodes() const noexcept -> const FeatureMap& { return isolatedNodes; }
        auto GetEdges() const noexcept -> const FeatureMap& { return edges; }
        auto GetFeatures() const noexcept -> const FeatureMap& { return features; }

        /**
         * Close a S57 file (ENC).
         */
        void Close() noexcept;

        /**
         * Gets the bounding region for this chart
         */
        auto GetBoundingBox() const noexcept -> const ENCboundingBox { return boundingRegion; }

        /**
         * The destructor
         */
        ~ENCChart() noexcept;

        /**
         * Delete standart realization
         */
        ENCChart(const ENCChart&) = delete;
        ENCChart(const ENCChart&&) = delete;
        ENCChart &operator = (const ENCChart&) = delete;
        ENCChart &operator = (const ENCChart&&) = delete;

    private:
        const bool ApplyUpdates(DDFModule &updModule, byte &updNumber) noexcept;
        const bool ReadVector(const DDFRecord *&record, const DDFField *&fieldVRID) noexcept;
        const bool ReadFeature(const DDFRecord *&record, const DDFField *&fieldFRID) noexcept;
    };

    typedef enum : byte {
        /**
         * No object information
         */
        ENC_RCNM_NO = 0u,
        /**
         * Data Set General Information
         */
        ENC_RCNM_DS = 10u,
        /**
         * Data Set Geographic Reference
         */
        ENC_RCNM_DP = 20u,
        /**
         * Data Set History
         */
        ENC_RCNM_DH = 30u,
        /**
         * Data Set Accuracy
         */
        ENC_RCNM_DA = 40u,
        /**
         * Catalogue Cross Reference
         */
        ENC_RCNM_CR = 60u,
        /**
         * Data Dictionary Definition
         */
        ENC_RCNM_ID = 70u,
        /**
         * Data Dictionary Domain
         */
        ENC_RCNM_IO = 80u,
        /**
         * Data Dictionary Schema
         */
        ENC_RCNM_IS = 90u,
        /**
         * Feature record
         */
        ENC_RCNM_FE = 100u,
        /**
         * Isolated Node vector
         */
        ENC_RCNM_VI = 110u,
        /**
         * Connected Node vector
         */
        ENC_RCNM_VC = 120u,
        /**
         * Edge vector
         */
        ENC_RCNM_VE = 130u,
        /**
         * Face vector
         */
        ENC_RCNM_VF = 140u
    } ENCrecordName;

    typedef enum : byte {
        /**
         * No instruction set
         */
        ENC_RUIN_N = 0u,

        /**
         * Insert
         */
        ENC_RUIN_I = 1u,
        /**
         * Delete
         */
        ENC_RUIN_D = 2u,
        /**
         * Modify
         */
        ENC_RUIN_M = 3u
    } ENCrecordUpdateInstruction;

    class ENCFeature {
        /**
         * Record name
         */
        ENCrecordName RCNM = ENC_RCNM_NO;

        /**
         * Record identification number
         */
        ulong RCID = 0ul;

        /**
         * Record version
         */
        ulong RVER = 0ul;

        /**
         * Record update instruction
         */
        ENCrecordUpdateInstruction RUIN = ENC_RUIN_N;

    protected:
        /**
         * The constructor.
         */
        ENCFeature() noexcept = default;

    public:
        /**
         * Gets the record name code
         */
        auto GetRCNM() const noexcept -> const ENCrecordName& { return RCNM; }

        /**
         * Gets the record identification number
         */
        auto GetRCID() const noexcept -> const ulong& { return RCID; }

        /**
         * Gets the record version number
         */
        auto GetRVER() const noexcept -> const ulong& { return RVER; }

        /**
         * Gets the record update instruction code
         */
        auto GetRUIN() const noexcept -> const ENCrecordUpdateInstruction& { return RUIN; }

        /**
         * The destructor.
         */
        virtual ~ENCFeature() noexcept = default;

        /**
         * Delete standart realization
         */
        ENCFeature(const ENCFeature&) = delete;
        ENCFeature(const ENCFeature&&) = delete;
        ENCFeature& operator = (const ENCFeature&) = delete;
        ENCFeature& operator = (const ENCFeature&&) = delete;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    class ENCPointGeometry final : public ENCFeature {
        Vector3d point = Vector3d();

    public:
        /**
         * The constructor
         */
        ENCPointGeometry() = default;

        /**
         * Gets the point reference
         */
        auto GetPointRef() const noexcept -> const Vector3d& { return point; }

        /**
         * The destructor
         */
        ~ENCPointGeometry() noexcept = default;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    typedef enum : byte {
        /**
         * Forward
         */
        ENC_ORNT_F = 1u,
        /**
         * Reverse
         */
        ENC_ORNT_R = 2u,
        /**
         * Object does not directly reference any orientation
         */
        ENC_ORNT_N = 255u
    } ENCorientation;

    typedef enum : byte {
        /**
         * Mask
         */
        ENC_MASK_M = 1u,
        /**
         * Show
         */
        ENC_MASK_S = 2u,
        /**
         * Object does not directly reference any masking indicator.
         */
        ENC_MASK_N = 255u
    } ENCmaskingIndicator;

    typedef enum : byte {
        /**
         * Exterior
         */
        ENC_USAG_E = 1u,
        /**
         * Interior
         */
        ENC_USAG_I = 2u,
        /**
         * Exterior boundary truncated by the data limit
         */
        ENC_USAG_C = 3u,
        /**
         * Object does not directly reference any usage indicator.
         */
        ENC_USAG_N = 255u
    } ENCusageIndicator;

    typedef enum : byte {
        /**
         * Beginning node
         */
        ENC_TOPI_B = 1u,
        /**
         * End node
         */
        ENC_TOPI_E = 2u,
        /**
         * Left face
         */
        ENC_TOPI_S = 3u,
        /**
         * Right face
         */
        ENC_TOPI_D = 4u,
        /**
         * Containing face
         */
        ENC_TOPI_F = 5u,
        /**
         * Object does not directly reference any topology
         */
        ENC_TOPI_N = 255u
    } ENCtopologyIndicator;

    class ENCEdgeGeometry final : public ENCFeature {
        typedef struct {
            /**
             * Record identification number
             */
            ulong RCID = 0ul;
            /**
             * Record name
             */
            ENCrecordName RCNM = ENC_RCNM_NO;
            /**
             * Orientation
             */
            ENCorientation ORNT = ENC_ORNT_N;
            /**
             * Usage indicator
             */
            ENCusageIndicator USAG = ENC_USAG_N;
            /**
             * Masking indicator
             */
            ENCmaskingIndicator MASK = ENC_MASK_N;
            /**
             * Topology indicator
             */
            ENCtopologyIndicator TOPI = ENC_TOPI_N;
        } ENCvectorRecordPointer;

        ENCvectorRecordPointer beginNode = ENCvectorRecordPointer();
        ENCvectorRecordPointer endNode = ENCvectorRecordPointer();

        vector<Vector3d> points;
    public:
        /**
         * The constructor.
         */
        ENCEdgeGeometry() noexcept : points() { }

        /**
         * Gets reference for the begibing node.
         */
        auto GetBeginNode() const noexcept -> const ENCvectorRecordPointer& { return beginNode; }

        /**
         * Gets reference for the end node.
         */
        auto GetEndNode() const noexcept -> const ENCvectorRecordPointer& { return endNode; }

        /**
         * Gets reference for the points array
         */
        auto GetPointsRef() const noexcept -> const vector<Vector3d>& { return points; }

        /**
         * The destructor.
         */
        ~ENCEdgeGeometry() noexcept = default;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    typedef enum : byte {
        /**
         * Point primitive
         */
        ENC_PRIM_P = 1u,
        /**
         * Line primitive
         */
        ENC_PRIM_L = 2u,
        /**
         * Area primitive
         */
        ENC_PRIM_A = 3u,
        /**
         * Object does not directly reference any geometry
         */
        ENC_PRIM_N = 255u
    } ENCgeometricPrimitive;

    #define ENC_NO_GROUP 255u
    class ENCPrimitive : public ENCFeature {
        /**
         * Object label/code
         */
        ulong OBJL = 0ul;

        /**
         * Group
         */
        byte GRUP = ENC_NO_GROUP;

        /**
         * Object geometric primitive
         */
        ENCgeometricPrimitive PRIM = ENC_PRIM_N;

        /**
         * Producing agency
         */
        ulong AGEN = 0ul;

        /**
         * Feature identification number
         */
        ulong FIDN = 0ul;

        /**
         * Feature identification subdivision
         */
        ulong FIDS = 0ul;

    protected:
        /**
         * The constructor
         */
        ENCPrimitive() noexcept = default;

    public:
        /**
         * The destructor
         */
        virtual ~ENCPrimitive() noexcept = default;

        /**
         * Gets the object geometric primitive code
         */
        auto GetPRIM() const noexcept -> const ENCgeometricPrimitive& { return PRIM; }

        /**
         * Gets the group code
         */
        auto GetGRUP() const noexcept -> const byte& { return GRUP; }

        /**
         * Gets the object label / code
         */
        auto GetOBJL() const noexcept -> const ulong& { return OBJL; }

        /**
         * Gets the producing agency code
         */
        auto GetAGEN() const noexcept -> const ulong& { return AGEN; }

        /**
         * Gets the feature identification number
         */
        auto GetFIDN() const noexcept -> const ulong& { return FIDN; }

        /**
         * Gets the feature identification subdivision
         */
        auto GetFIDS() const noexcept -> const ulong& { return FIDS; }

        /**
         * Delete standart realization
         */
        ENCPrimitive(const ENCPrimitive&) = delete;
        ENCPrimitive(const ENCPrimitive&&) = delete;
        ENCPrimitive &operator = (const ENCPrimitive&) = delete;
        ENCPrimitive &operator = (const ENCPrimitive&&) = delete;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    class ENCGeometryPrimitive final : public ENCPrimitive {
        typedef struct {
            /**
             * Record identification number
             */
            ulong RCID = 0ul;
            /**
             * Record name
             */
            ENCrecordName RCNM = ENC_RCNM_NO;
            /**
             * Orientation
             */
            ENCorientation ORNT = ENC_ORNT_N;
            /**
             * Usage indicator
             */
            ENCusageIndicator USAG = ENC_USAG_N;
            /**
             * Masking indicator
             */
            ENCmaskingIndicator MASK = ENC_MASK_N;
        } ENCspatialRecordPointer;

        vector<ENCspatialRecordPointer> FSPTObjects;

    public:
        /**
         * The constructor
         */
        ENCGeometryPrimitive() noexcept : FSPTObjects() { };

        /**
         * Gets the vector of the feature record to spatial record pointers
         */
        auto GetFSPTObjects() const noexcept -> const vector<ENCspatialRecordPointer>& { return FSPTObjects; }

        /**
         * The destructor
         */
        ~ENCGeometryPrimitive() noexcept = default;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    class ENCObjectPrimitive final : public ENCPrimitive {

    public:
        /**
         * The constructor
         */
        ENCObjectPrimitive() noexcept = default;

        /**
         * The destructor
         */
        ~ENCObjectPrimitive() noexcept = default;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };
}

#endif /* defined(__ProjectGIS__ihoS57__) */
