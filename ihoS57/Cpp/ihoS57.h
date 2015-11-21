//
//  ihoS57.h
//  International Hydrographic Organization
//  IHO Transfer Standard for Digital Hydrographic Data, Publication S-57
//
//  Created by Kirill Bravichev on 22/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#ifndef __Standards__ihoS57__
#define __Standards__ihoS57__

#include <map>
#include <cmath>

#include "iso8211.h"

namespace Standards {
    /**
     * Predeclarations
     */
    class ENCPrimitive;
    class ENCEdgeGeometry;
    class ENCPointGeometry;
    class ENCObjectPrimitive;
    class ENCGeometryPrimitive;

	struct ENCboundingBox {
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
		inline void Extend(const double &x, const double &y) noexcept {
			if (isValid) {
				minX = std::min(minX, x);
				minY = std::min(minY, y);
				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);
			}
			else {
				isValid = true;
				minX = maxX = x;
				minY = maxY = y;
			}
		}

		/**
		* Extend the bounding box with a point
		* @param x the x coordinate
		* @param y the y coordinate
		*/
		inline void Extend(const ENCboundingBox &bbox) noexcept {
			if (isValid) {
				minX = std::min(minX, bbox.minX);
				minY = std::min(minY, bbox.minY);
				maxX = std::max(maxX, bbox.maxX);
				maxY = std::max(maxY, bbox.maxY);
			}
			else {
				isValid = true;
				minX = bbox.minX;
				maxX = bbox.maxX;
				minY = bbox.minX;
				maxY = bbox.maxY;
			}
		}

		/**
		* Gets width
		*/
		inline const double GetWidth() const noexcept { return std::fabs(maxX - minX); }

		/**
		* Gets height
		*/
		inline const double GetHeight() const noexcept { return std::fabs(maxY - minY); }
	};

	class Vector3 {
	public:
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;

		/**
		 * Creates and sets to (0,0,0)
		 */
		Vector3() noexcept = default;

		/**
		 * Creates and sets to (x,y,z)
		 * @param nx initial x-coordinate value
		 * @param ny initial y-coordinate value
		 * @param nz initial z-coordinate value
		 */
		Vector3(const double &nx, const double &ny,const double &nz = 0.0) noexcept : x(nx), y(ny), z(nz) { }

		/**
		 * Copy constructor.
		 * @param src Source of data for new created Vector3 instance.
		 */
		Vector3(const Vector3 &src) noexcept : x(src.x), y(src.y), z(src.z) { }

		/**
		 * Copy operator
		 * @param rhs Right hand side argument of binary operator.
		 */
		Vector3 &operator = (const Vector3 &rhs) noexcept {
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}

		/**
		 * The destructor
		 */
		~Vector3() noexcept = default;
	};
    
    /**
     * Data structure code,
     * see 3.1 and part 2 Theoretical Data Model
     */
    enum ENCdataStructure : byte {
        /**
         * Cartographic spaghetti
         */
        ENC_DSTR_CS = 1u,
        /**
         * Chain-node
         */
        ENC_DSTR_CN = 2u,
        /**
         * Planar graph
         */
        ENC_DSTR_PG = 3u,
        /**
         * Full topology
         */
        ENC_DSTR_FT = 4u,
        /**
         * Topology is not relevant
         */
        ENC_DSTR_NO = 255u
    };

	class ENCChart {
        /**
         * Data structure, see 3.1 and part 2 Theoretical Data Model
         * @see ENCdataStructure
         */
        ENCdataStructure DSTR = ENC_DSTR_NO;
            
        /**
         * Lexical level used for the NATF fields (see 2.4)
         */
        long NALL = 0l;
            
        /**
         * Lexical level used for the ATTF fields (see 2.4)
         */
        long AALL = 0l;
            
        /**
         * Floating-point to integer multiplication factor
         * for coordinate values (see 3.2.1)
         */
        double COMF = 0.0;

        /**
         * Floating point to integer multiplication factor
         * for 3-D (sounding) values (see 3.3)
         */
        double SOMF = 0.0;

        /**
         * The modulus of the compilation scale.
         * For example, a scale of 1:25000 is encoded as 25000
         */
        long CSCL = 0l;

        //bool isCatalogField = false;
        bool isFileIngested = false;

        /**
         * A string indicating the data set name 
         * (see Appendix B - Product Specifications)
         */
        const char* DSNM = nullptr;
        DDFModule* module = nullptr;

        ENCboundingBox boundingRegion = ENCboundingBox();

        const string fileName;

        //FeatureMap faces;
        map<ulong, ENCEdgeGeometry*> edges;
        map<ulong, ENCGeometryPrimitive*> features;
        map<ulong, ENCPointGeometry*> isolatedNodes;
        map<ulong, ENCPointGeometry*> connectedNodes;

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

        /**
         * Gets the connected nodes map
         * @see ENCPointGeometry
         */
        inline const map<ulong, ENCPointGeometry*> &GetConnectedNodes() const noexcept { return connectedNodes; }
        
        /**
         * Gets the isolated nodes map
         * @see ENCPointGeometry
         */
        inline const map<ulong, ENCPointGeometry*> &GetIsolatedNodes() const noexcept { return isolatedNodes; }
        
        /**
         * Gets the edge map
         * @see ENCEdgeGeometry
         */
        inline const map<ulong, ENCEdgeGeometry*> &GetEdges() const noexcept { return edges; }
        
        /**
         * Gets the feature map
         * @see ENCGeometryPrimitive
         */
        inline const map<ulong, ENCGeometryPrimitive*> &GetFeatures() const noexcept { return features; }

        /**
         * Gets the bounding region for this chart
         * @see ENCboundingBox
         */
        inline const ENCboundingBox &GetBoundingBox() const noexcept { return boundingRegion; }

        /**
         * Close a S57 file (ENC).
         */
        void Close() noexcept;

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

    /**
     * Record name codes (see 2.2.1)
     */
    enum ENCrecordName : byte {
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
    };

    /**
     * Record update instruction codes 
     * (see 8.4.2.2 and 8.4.3.2)
     */
    enum ENCrecordUpdateInstruction : byte {
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
    };

    class ENCFeature {
        /**
         * Record name (see 2.2.1)
         * @see ENCrecordName
         */
        ENCrecordName RCNM = ENC_RCNM_NO;

        /**
         * Record identification number (see 2.2.2)
         */
        ulong RCID = 0ul;

        /**
         * Record version, contains the serial number 
         * of the record edition (see 8.4.3.1)
         */
        ulong RVER = 0ul;

        /**
         * Record update instruction (see 8.4.2.2 and 8.4.3.2)
         * @see ENCrecordUpdateInstruction
         */
        ENCrecordUpdateInstruction RUIN = ENC_RUIN_N;

    protected:
        /**
         * The default constructor.
         */
        ENCFeature() noexcept = default;
        
        /**
         * The constructor.
         */
        ENCFeature(const ulong &rcnm, const ulong &rcid, const ulong &rver, const ulong &ruin) noexcept : RCID(rcid), RVER(rver) {
            RUIN = static_cast<ENCrecordUpdateInstruction>(ruin);
            RCNM = static_cast<ENCrecordName>(rcnm);
        }

    public:
        /**
         * Gets the record name code (see 2.2.1)
         * @see ENCrecordName
         */
        inline const ENCrecordName &GetRCNM() const noexcept { return RCNM; }

        /**
         * Gets the record identification number (see 2.2.2)
         */
        inline const ulong &GetRCID() const noexcept { return RCID; }

        /**
         * Gets the record version, contains the serial number 
         * of the record edition (see 8.4.2.1 and 8.4.3.1)
         */
        inline const ulong &GetRVER() const noexcept { return RVER; }

        /**
         * Gets the record update instruction code (see 8.4.2.2 and 8.4.3.2)
         * @see ENCrecordUpdateInstruction
         */
        inline const ENCrecordUpdateInstruction &GetRUIN() const noexcept { return RUIN; }

        /**
         * The destructor.
         */
        virtual ~ENCFeature() noexcept = default;

        /**
         * Delete standart realization
         */
        ENCFeature(const ENCFeature&) = delete;
        ENCFeature(const ENCFeature&&) = delete;
        ENCFeature &operator = (const ENCFeature&) = delete;
        ENCFeature &operator = (const ENCFeature&&) = delete;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    class ENCPointGeometry final : public ENCFeature {
        Vector3 point = Vector3();

    public:
        /**
         * The constructor
         */
        ENCPointGeometry(const ulong &rcnm, const ulong &rcid, const ulong &rver, const ulong &ruin) noexcept : ENCFeature(rcnm, rcid, rver, ruin) { }

        /**
         * Gets the point reference
         */
        inline const Vector3 &GetPointRef() const noexcept { return point; }

        /**
         * The destructor
         */
        ~ENCPointGeometry() noexcept = default;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    enum ENCorientation : byte {
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
    };

    /**
     * Masking indicator (see 4.7.3.4)
     */
    enum ENCmaskingIndicator : byte {
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
    };

    /**
     * Usage indicator (see 4.7.3.3)
     */
    enum ENCusageIndicator : byte {
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
    };

    enum ENCtopologyIndicator : byte {
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
    };

    struct ENCvectorRecordPointer {
        /**
         * Record identification number
         */
        ulong RCID = 0ul;
        /**
         * Record name
         * @see ENCrecordName
         */
        ENCrecordName RCNM = ENC_RCNM_NO;
        /**
         * Orientation
         * @see ENCorientation
         */
        ENCorientation ORNT = ENC_ORNT_N;
        /**
         * Usage indicator (see 4.7.3.3)
         * @see ENCusageIndicator
         */
        ENCusageIndicator USAG = ENC_USAG_N;
        /**
         * Masking indicator (see 4.7.3.4)
         * @see ENCmaskingIndicator
         */
        ENCmaskingIndicator MASK = ENC_MASK_N;
        /**
         * Topology indicator
         * @see ENCtopologyIndicator
         */
        ENCtopologyIndicator TOPI = ENC_TOPI_N;
    };

    class ENCEdgeGeometry final : public ENCFeature {
        ENCvectorRecordPointer beginNode = ENCvectorRecordPointer();
        ENCvectorRecordPointer endNode = ENCvectorRecordPointer();

        vector<Vector3> points;
    public:
        /**
         * The constructor.
         */
        ENCEdgeGeometry(const ulong &rcnm, const ulong &rcid, const ulong &rver, const ulong &ruin) noexcept : ENCFeature(rcnm, rcid, rver, ruin), points() { }

        /**
         * Gets reference for the begibing node.
         * @see ENCvectorRecordPointer
         */
        inline const ENCvectorRecordPointer &GetBeginNode() const noexcept { return beginNode; }

        /**
         * Gets reference for the end node.
         * @see ENCvectorRecordPointer
         */
        inline const ENCvectorRecordPointer &GetEndNode() const noexcept { return endNode; }

        /**
         * Gets reference for the points array
         */
        inline const vector<Vector3>& GetPointsRef() const noexcept { return points; }

        /**
         * The destructor.
         */
        ~ENCEdgeGeometry() noexcept = default;

        /**
         * Friend classes
         */
        friend class ENCChart;
    };

    enum ENCgeometricPrimitive : byte {
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
    };

	enum ENCobjectAcronymCodes: uint {
		/**
		 * Unknown
		 */
		ENC_OBJL_UNKNOWN = 0u,
		/**
		 * Administration Area(Named)
		 */
		ENC_OBJL_ADMARE = 1u,
		/**
		 * Airport / airfield
		 */
		ENC_OBJL_AIRARE = 2u,
		/**
		 * Anchor berth
		 */
		ENC_OBJL_ACHBRT = 3u,
		/**
		 * Anchorage area
		 */
		ENC_OBJL_ACHARE = 4u,
		/**
		 * Beacon, cardinal
		 */
		ENC_OBJL_BCNCAR = 5u,
		/**
		 * Beacon, isolated danger
		 */
		ENC_OBJL_BCNISD = 6u,
		/**
		 * Beacon, lateral
		 */
		ENC_OBJL_BCNLAT = 7u,
		/**
		 * Beacon, safe water
		 */
		ENC_OBJL_BCNSAW = 8u,
		/**
		 * Beacon, special purpose / general
		 */
		ENC_OBJL_BCNSPP = 9u,
		/**
		 * Berth
		 */
		ENC_OBJL_BERTHS = 10u,
		/**
		 * Bridge
		 */
		ENC_OBJL_BRIDGE = 11u,
		/**
		 * Building, single
		 */
		ENC_OBJL_BUISGL = 12u,
		/**
		 * Built - up area
		 */
		ENC_OBJL_BUAARE = 13u,
		/**
		 * Buoy, cardinal
		 */
		ENC_OBJL_BOYCAR = 14u,
		/**
		 * Buoy, installation
		 */
		ENC_OBJL_BOYINB = 15u,
		/**
		 * Buoy, isolated danger
		 */
		ENC_OBJL_BOYISD = 16u,
		/**
		 * Buoy, lateral
		 */
		ENC_OBJL_BOYLAT = 17u,
		/**
		 * Buoy, safe water
		 */
		ENC_OBJL_BOYSAW = 18u,
		/**
		 * Buoy, special purpose / general
		 */
		ENC_OBJL_BOYSPP = 19u,
		/**
		 * Cable area
		 */
		ENC_OBJL_CBLARE = 20u,
		/**
		 * Cable, overhead
		 */
		ENC_OBJL_CBLOHD = 21u,
		/**
		 * Cable, submarine
		 */
		ENC_OBJL_CBLSUB = 22u,
		/**
		 * Canal
		 */
		ENC_OBJL_CANALS = 23u,
		/**
		 * Canal bank
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_CANBNK = 24u,
		/**
		 * Cargo transhipment area
		 */
		ENC_OBJL_CTSARE = 25u,
		/**
		 * Causeway
		 */
		ENC_OBJL_CAUSWY = 26u,
		/**
		 * Caution area
		 */
		ENC_OBJL_CTNARE = 27u,
		/**
         * Checkpoint
         */
		ENC_OBJL_CHKPNT = 28u,
		/**
		 * Coastguard station
		 */
		ENC_OBJL_CGUSTA = 29u,
		/**
		 * Coastline
		 */
		ENC_OBJL_COALNE = 30u,
		/**
		 * Contiguous zone
		 */
		ENC_OBJL_CONZNE = 31u,
		/**
		 * Continental shelf area
		 */
		ENC_OBJL_COSARE = 32u,
		/**
		 * Control point
		 */
		ENC_OBJL_CTRPNT = 33u,
		/**
		 * Conveyor
		 */
		ENC_OBJL_CONVYR = 34u,
		/**
		 * Crane
		 */
		ENC_OBJL_CRANES = 35u,
		/**
		 *Current - non - gravitational
		 */
		ENC_OBJL_CURENT = 36u,
		/**
		 * Custom zone
		 */
		ENC_OBJL_CUSZNE = 37u,
		/**
		 * Dam
		 */
		ENC_OBJL_DAMCON = 38u,
		/**
		 * Daymark
		 */
		ENC_OBJL_DAYMAR = 39u,
		/**
		 * Deep water route centerline
		 */
		ENC_OBJL_DWRTCL = 40u,
		/**
		 * Deep water route part
		 */
		ENC_OBJL_DWRTPT = 41u,
		/**
		 * Depth area
		 */
		ENC_OBJL_DEPARE = 42u,
		/**
		 * Depth contour
		 */
		ENC_OBJL_DEPCNT = 43u,
		/**
		 * Distance mark
		 */
		ENC_OBJL_DISMAR = 44u,
		/**
		 * Dock area
		 */
		ENC_OBJL_DOCARE = 45u,
		/**
		 * Dredged area
		 */
		ENC_OBJL_DRGARE = 46u,
		/**
		 * Dry dock
		 */
		ENC_OBJL_DRYDOC = 47u,
		/**
		 * Dumping ground
		 */
		ENC_OBJL_DMPGRD = 48u,
		/**
		 * Dyke
		 */
		ENC_OBJL_DYKCON = 49u,
		/**
		 * Exclusive economic zone
		 */
		ENC_OBJL_EXEZNE = 50u,
		/**
		 * Fairway
		 */
		ENC_OBJL_FAIRWY = 51u,
		/**
		 * Fence / wall
		 */
		ENC_OBJL_FNCLNE = 52u,
		/**
		 * Ferry route
		 */
		ENC_OBJL_FERYRT = 53u,
		/**
		 * Fishery zone
		 */
		ENC_OBJL_FSHZNE = 54u,
		/**
		 * Fishing facility
		 */
		ENC_OBJL_FSHFAC = 55u,
		/**
		 * Fishing ground
		 */
		ENC_OBJL_FSHGRD = 56u,
		/**
		 * Floating dock
		 */
		ENC_OBJL_FLODOC = 57u,
		/**
		 * Fog signal
		 */
		ENC_OBJL_FOGSIG = 58u,
        /**
		 * Fortified structure
		 */
		ENC_OBJL_FORSTC = 59u,
		/**
		 * Free port area
		 */
		ENC_OBJL_FRPARE = 60u,
		/**
		 * Gate
		 */
		ENC_OBJL_GATCON = 61u,
		/**
		 * Gridiron
		 */
		ENC_OBJL_GRIDRN = 62u,
		/**
		 * Harbour area(administrative)
		 */
		ENC_OBJL_HRBARE = 63u,
		/**
		 * Harbour facility
		 */
		ENC_OBJL_HRBFAC = 64u,
		/**
		 * Hulk
		 */
		ENC_OBJL_HULKES = 65u,
		/**
		 * Ice area
		 */
		ENC_OBJL_ICEARE = 66u,
		/**
		 * Incineration area
		 */
		ENC_OBJL_ICNARE = 67u,
		/**
		 * Inshore traffic zone
		 */
		ENC_OBJL_ISTZNE = 68u,
		/**
		 * Lake
		 */
		ENC_OBJL_LAKARE = 69u,
		/**
		 * Lake shore
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_LAKSHR = 70u,
		/**
		 * Land area
		 */
		ENC_OBJL_LNDARE = 71u,
		/**
		 * Land elevation
		 */
		ENC_OBJL_LNDELV = 72u,
		/**
		 * Land region
		 */
		ENC_OBJL_LNDRGN = 73u,
		/**
		 * Landmark
		 */
		ENC_OBJL_LNDMRK = 74u,
		/**
		 * Light
		 */
		ENC_OBJL_LIGHTS = 75u,
		/**
		 * Light float
		 */
		ENC_OBJL_LITFLT = 76u,
		/**
		 * Light vessel
		 */
		ENC_OBJL_LITVES = 77u,
		/**
		 * Local magnetic anomaly
		 */
		ENC_OBJL_LOCMAG = 78u,
		/**
		 * Lock basin
		 */
		ENC_OBJL_LOKBSN = 79u,
		/**
		 * Log pond
		 */
		ENC_OBJL_LOGPON = 80u,
		/**
		 * Magnetic variation
		 */
		ENC_OBJL_MAGVAR = 81u,
		/**
		 * Marine farm / culture
		 */
		ENC_OBJL_MARCUL = 82u,
		/**
		 * Military practice area
		 */
		ENC_OBJL_MIPARE = 83u,
		/**
		 * Mooring / Warping facility
		 */
		ENC_OBJL_MORFAC = 84u,
		/**
		 * Navigation line
		 */
		ENC_OBJL_NAVLNE = 85u,
		/**
		 * Obstruction
		 */
		ENC_OBJL_OBSTRN = 86u,
		/**
		 * Offshore platform
		 */
		ENC_OBJL_OFSPLF = 87u,
		/**
		 * Offshore production area
		 */
		ENC_OBJL_OSPARE = 88u,
		/**
		 * Oil barrier
		 */
		ENC_OBJL_OILBAR = 89u,
		/**
		 * Pile
		 */
		ENC_OBJL_PILPNT = 90u,
		/**
		 * Pilot boarding place
		 */
		ENC_OBJL_PILBOP = 91u,
		/**
		 * Pipeline area
		 */
		ENC_OBJL_PIPARE = 92u,
		/**
		 * Pipeline, overhead
		 */
		ENC_OBJL_PIPOHD = 93u,
		/**
		 * Pipeline, submarine / on land
		 */
		ENC_OBJL_PIPSOL = 94u,
		/**
		 * Pontoon
		 */
		ENC_OBJL_PONTON = 95u,
		/**
		 * Precautionary area
		 */
		ENC_OBJL_PRCARE = 96u,
		/**
		 * Production / storage area
		 */
		ENC_OBJL_PRDARE = 97u,
		/**
		 * Pylon / bridge support
		 */
		ENC_OBJL_PYLONS = 98u,
		/**
		 * Radar line
		 */
		ENC_OBJL_RADLNE = 99u,
		/**
		 * Radar range
		 */
		ENC_OBJL_RADRNG = 100u,
		/**
		 * Radar reflector
		 */
		ENC_OBJL_RADRFL = 101u,
		/**
		 * Radar station
		 */
		ENC_OBJL_RADSTA = 102u,
		/**
		 * Radar transponder beacon
		 */
		ENC_OBJL_RTPBCN = 103u,
		/**
		 * Radio calling - in point
		 */
		ENC_OBJL_RDOCAL = 104u,
		/**
		 * Radio station
		 */
		ENC_OBJL_RDOSTA = 105u,
		/**
		 * Railway
		 */
		ENC_OBJL_RAILWY = 106u,
		/**
		 * Rapids
		 */
		ENC_OBJL_RAPIDS = 107u,
		/**
		 * Recommended route centerline
		 */
		ENC_OBJL_RCRTCL = 108u,
		/**
		 * Recommended track
		 */
		ENC_OBJL_RECTRC = 109u,
		/**
		 * Recommended traffic lane part
		 */
		ENC_OBJL_RCTLPT = 110u,
		/**
		 * Rescue station
		 */
		ENC_OBJL_RSCSTA = 111u,
		/**
		 * Restricted area
		 */
		ENC_OBJL_RESARE = 112u,
		/**
		 * Retro - reflector
		 */
		ENC_OBJL_RETRFL = 113u,
		/**
		 * River
		 */
		ENC_OBJL_RIVERS = 114u,
		/**
		 * River bank
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_RIVBNK = 115u,
		/**
		 * Road
		 */
		ENC_OBJL_ROADWY = 116u,
		/**
		 * Runway
		 */
		ENC_OBJL_RUNWAY = 117u,
		/**
		 * Sand waves
		 */
		ENC_OBJL_SNDWAV = 118u,
		/**
		 * Sea area / named water area
		 */
		ENC_OBJL_SEAARE = 119u,
		/**
		 * Sea - plane landing area
		 */
		ENC_OBJL_SPLARE = 120u,
		/**
		 * Seabed area
		 */
		ENC_OBJL_SBDARE = 121u,
		/**
		 * Shoreline construction
		 */
		ENC_OBJL_SLCONS = 122u,
		/**
		 * Signal station, traffic
		 */
		ENC_OBJL_SISTAT = 123u,
		/**
		 * Signal station, warning
		 */
		ENC_OBJL_SISTAW = 124u,
		/**
		 * Silo / tank
		 */
		ENC_OBJL_SILTNK = 125u,
		/**
		 * Slope topline
		 */
		ENC_OBJL_SLOTOP = 126u,
		/**
		 * Sloping ground
		 */
		ENC_OBJL_SLOGRD = 127u,
		/**
		 * Small craft facility
		 */
		ENC_OBJL_SMCFAC = 128u,
		/**
		 * Sounding
		 */
		ENC_OBJL_SOUNDG = 129u,
		/**
		 * Spring
		 */
		ENC_OBJL_SPRING = 130u,
		/**
		 * Square
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_SQUARE = 131u,
		/**
		 * Straight territorial sea baseline
		 */
		ENC_OBJL_STSLNE = 132u,
		/**
		 * Submarine transit lane
		 */
		ENC_OBJL_SUBTLN = 133u,
		/**
		 * Swept Area
		 */
		ENC_OBJL_SWPARE = 134u,
		/**
		 * Territorial sea area
		 */
		ENC_OBJL_TESARE = 135u,
		/**
		 * Tidal stream - flood / ebb
		 */
		ENC_OBJL_TS_FEB = 160u,
		/**
		 * Tidal stream - harmonic prediction
		 */
		ENC_OBJL_TS_PRH = 136u,
		/**
		 * Tidal stream - non - harmonic prediction
		 */
		ENC_OBJL_TS_PNH = 137u,
		/**
		 * Tidal stream panel data
		 */
		ENC_OBJL_TS_PAD = 138u,
		/**
		 * Tidal stream - time series
		 */
		ENC_OBJL_TS_TIS = 139u,
		/**
		 * Tide - harmonic prediction
		 */
		ENC_OBJL_T_HMON = 140u,
		/**
		 * Tide - non - harmonic prediction
		 */
		ENC_OBJL_T_NHMN = 141u,
		/**
		 * Tide - time series
		 */
		ENC_OBJL_T_TIMS = 142u,
		/**
		 * Tideway
		 */
		ENC_OBJL_TIDEWY = 143u,
		/**
		 * Topmark
		 */
		ENC_OBJL_TOPMAR = 144u,
		/**
		 * Traffic separation line
		 */
		ENC_OBJL_TSELNE = 145u,
		/**
		 * Traffic separation scheme boundary
		 */
		ENC_OBJL_TSSBND = 146u,
		/**
		 * Traffic separation scheme crossing
		 */
		ENC_OBJL_TSSCRS = 147u,
		/**
		 * Traffic separation scheme lane part
		 */
		ENC_OBJL_TSSLPT = 148u,
		/**
		 * Traffic separation scheme roundabout
		 */
		ENC_OBJL_TSSRON = 149u,
		/**
		 * Traffic separation zone
		 */
		ENC_OBJL_TSEZNE = 150u,
		/**
		 * Tunnel
		 */
		ENC_OBJL_TUNNEL = 151u,
		/**
		 * Two - way route part
		 */
		ENC_OBJL_TWRTPT = 152u,
		/**
		 * Underwater / awash rock
		 */
		ENC_OBJL_UWTROC = 153u,
		/**
		 * Unsurveyed area
		 */
		ENC_OBJL_UNSARE = 154u,
		/**
		 * Vegetation
		 */
		ENC_OBJL_VEGATN = 155u,
		/**
		 * Water turbulence
		 */
		ENC_OBJL_WATTUR = 156u,
		/**
		 * Waterfall
		 */
		ENC_OBJL_WATFAL = 157u,
		/**
		 * Weed / Kelp
		 */
		ENC_OBJL_WEDKLP = 158u,
		/**
		 * Wreck
		 */
		ENC_OBJL_WRECKS = 159u,
		/**
		 * Archipelagic Sea Lane
		 */
		ENC_OBJL_ARCSLN = 161u,
		/**
		 * Archipelagic Sea Lane axis
		 */
		ENC_OBJL_ASLXIS = 162u,
        /**
		 * New object
		 */
		ENC_OBJL_NEWOBJ = 163u,
		/**
		 * Accuracy of data
		 */
		ENC_OBJL_M_ACCY = 300u,
		/**
		 * Compilation scale of data
		 */
		ENC_OBJL_M_CSCL = 301u,
		/**
		 * Coverage
		 */
		ENC_OBJL_M_COVR = 302u,
		/**
		 * Horizontal datum of data
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_M_HDAT = 303u,
		/**
		 * Horizontal datum shift parameters
		 */
		ENC_OBJL_M_HOPA = 304u,
		/**
		 * Nautical publication information
		 */
		ENC_OBJL_M_NPUB = 305u,
		/**
		 * Navigational system of marks
		 */
		ENC_OBJL_M_NSYS = 306u,
		/**
		 * Production information
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_M_PROD = 307u,
		/**
		 * Quality of data
		 */
		ENC_OBJL_M_QUAL = 308u,
		/**
		 * Sounding datum
		 */
		ENC_OBJL_M_SDAT = 309u,
		/**
		 * Survey reliability
		 */
		ENC_OBJL_M_SREL = 310u,
		/**
		 * Units of measurement of data
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_M_UNIT = 311u,
		/**
		 * Vertical datum of data
		 */
		ENC_OBJL_M_VDAT = 312u,
		/**
		 * Aggregation
		 */
		ENC_OBJL_C_AGGR = 400u,
		/**
		 * Association
		 */
		ENC_OBJL_C_ASSO = 401u,
		/**
		 * Stacked on / stacked under
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_C_STAC = 402u,
		/**
		 * Cartographic area
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_SAREAS = 500u,
		/**
		 * Cartographic line
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_SLINES = 501u,
		/**
		 * Cartographic symbol
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_SCSYMB = 502u,
		/**
		 * Compass
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_SCOMPS = 503u,
		/**
		 * Text
		 * @warning PROHIBITED FOR USE IN ENC
		 */
		ENC_OBJL_STEXTS = 504u
	};

    #define ENC_GROUP_NO 255u
    class ENCPrimitive : public ENCFeature {
        /**
         * Object label/code
         * @see ENCobjectAcronymCodes
         */
		ENCobjectAcronymCodes OBJL = ENC_OBJL_UNKNOWN;
        /**
         * Group, 255 - No group 
         * (see Appendix B - Product Specifications)
         */
        byte GRUP = ENC_GROUP_NO;
        /**
         * Object geometric primitive
         * @see ENCgeometricPrimitive
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
         * The default constructor
         */
        ENCPrimitive() noexcept = default;

        /**
         * The constructor
         */
        ENCPrimitive(const ulong &prim, const ulong &rcnm, const ulong &rcid, const ulong &rver, const ulong &ruin) noexcept : ENCFeature(rcnm, rcid, rver, ruin) {
            PRIM = static_cast<ENCgeometricPrimitive>(prim);
        }
    public:
        /**
         * Gets the object geometric primitive code
         * @see ENCgeometricPrimitive
         */
        inline const ENCgeometricPrimitive &GetPRIM() const noexcept { return PRIM; }

        /**
         * Gets the group, 255 - No group
         * (see Appendix B - Product Specifications)
         */
        inline const byte &GetGRUP() const noexcept { return GRUP; }

        /**
         * Gets the object label / code
         * @see ENCobjectAcronymCodes
         */
        inline const ENCobjectAcronymCodes &GetOBJL() const noexcept { return OBJL; }

        /**
         * Gets the producing agency code
         */
        inline const ulong &GetAGEN() const noexcept { return AGEN; }

        /**
         * Gets the feature identification number
         */
        inline const ulong &GetFIDN() const noexcept { return FIDN; }

        /**
         * Gets the feature identification subdivision
         */
        inline const ulong &GetFIDS() const noexcept { return FIDS; }

        /**
         * The destructor
         */
        virtual ~ENCPrimitive() noexcept = default;
        
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

    struct ENCspatialRecordPointer {
        /**
         * Record identification number
         */
        ulong RCID = 0ul;
        /**
         * Record name
         * @see ENCrecordName
         */
        ENCrecordName RCNM = ENC_RCNM_NO;
        /**
         * Orientation
         * @see ENCorientation
         */
        ENCorientation ORNT = ENC_ORNT_N;
        /**
         * Usage indicator
         * @see ENCusageIndicator
         */
        ENCusageIndicator USAG = ENC_USAG_N;
        /**
         * Masking indicator
         * @see ENCmaskingIndicator
         */
        ENCmaskingIndicator MASK = ENC_MASK_N;
    };

    class ENCGeometryPrimitive final : public ENCPrimitive {
        vector<ENCspatialRecordPointer> FSPTObjects;

    public:
        /**
         * The constructor
         */
        ENCGeometryPrimitive(const ulong &prim, const ulong &rcnm, const ulong &rcid, const ulong &rver, const ulong &ruin) noexcept : ENCPrimitive(prim, rcnm, rcid, rver, ruin), FSPTObjects() { };

        /**
         * Gets the vector of the feature record to spatial record pointers
         * @see ENCspatialRecordPointer
         */
        inline const vector<ENCspatialRecordPointer> &GetFSPTObjects() const noexcept { return FSPTObjects; }

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
#endif /* defined(__Standards__ihoS57__) */
