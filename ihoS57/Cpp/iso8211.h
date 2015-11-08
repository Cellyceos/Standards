//
//  iso8211.h
//  ISO/IEC 8211 - Specification for a data descriptive file for information interchange
//
//  Created by Kirill Bravichev on 22/02/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

#ifndef __Standards__iso8211__
#define __Standards__iso8211__

#define DDF_FIELD_TERMINATOR    0x1E
#define DDF_UNIT_TERMINATOR     0x1F
#define DDF_LEADER_SIZE         0x18

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned long ulong;

using namespace std;

namespace Standards {
    /**
     * Predeclarations
     */
    class DDFField;
    class DDFRecord;
    class DDFFieldDefining;
    class DDFSubfieldDefining;

    /**
     * Convert a string into upper case.
     * @param string Points to the string whose letters are to be put into upper case.
     */
    char* DDFStringUpper(char *&string) noexcept;

    /**
     * Read up to byteRead from the passed string, and interpret as an long.
     * @param strSource source string.
     * @param byteRead number of bytes to read. Default value is 32 bytes.
     * @return the resulting number
     */
    int DDFScanInt(const char *strSource, byte byteRead = 32u) noexcept;

    /**
     * Clone string
     * @param strSource source string.
     * @param byteClone number of bytes to need clone.
     * @return pointer to new string.
     */
    char* DDFCloneString(const char *strSource, const size_t &byteClone) noexcept;

    /**
     * Correct string
     * @param strSource source string
     * @param byteOffset number of bytes to offset.
     * @param strLenght string lenght. Well done, contain new lenght for corrected string.
     * @return false is corection operation is fails.
     */
    const bool DDFCorrectString(char *&strSource, size_t byteOffset, size_t *strLenght) noexcept;

    /**
     * Fetch a variable length string from a record, and allocate it as a new string.
     * @param strRecord source string to be fetched.
     * @param strLenght source string lenght.
     * @param consumedChar output variable, contain number of reading simbols from source string.
     * @param delimCharOne first delim char. Default value is DDF_UNIT_TERMINATOR.
     * @param delimCharSecond second delim char. Default value is DDF_FIELD_TERMINATOR.
     * @return Fetched string
     */
    char* DDFFetchVariable(const char* strRecord, size_t strLenght, size_t* consumedChar, const char delimCharOne = DDF_UNIT_TERMINATOR, const char delimCharSecond = DDF_FIELD_TERMINATOR) noexcept;

    /**
     * The primary class for reading ISO 8211 (DDF) files.  This class contains all
     * the information read from the DDR record, and is used to read records
     * from the file.
     */
    class DDFModule {
        stringstream DDFfileBuf;
        size_t firstRecordOffset = 0ul;

        char interchangeLevel = 0;
        char inlineCodeExrensionIndicator = 0;
        char versionNumber = 0;
        char applicationIndicator = 0;
        uint fieldControlLenght = 0u;

        uint sizeFieldTag = 0u;
        uint recordLenght = 0u;
        uint fieldAreaStart = 0u;
        uint sizeFieldLenght = 0u;
        uint sizeFieldPosition = 0u;
        char leaderIdentifier = 0;

        DDFRecord *currentRecord = nullptr;
        vector<DDFFieldDefining*> fieldDefinings;
		char extendedCharSet[4] = { '\0', '\0', '\0', '\0' };

    public:
        /**
         * The constructor.
         */
        DDFModule() noexcept : DDFfileBuf(), fieldDefinings() {}

        /**
         * Open a ISO 8211 (DDF) file for reading.
         *
         * If the open succeeds the data descriptive record (DDR) will have been
         * read, and all the field and subfield definitions will be available.
         *
         * @param fileName The name of the file to open.
         * @return false if the open fails.
         */
        const bool Open(const string &fileName, const bool testOpen = false) noexcept;

        /**
         * Read one record from the file.
         * @warning The returned record is owned by the module, and should not be deleted
         * by the application.  The record is only valid untill the next ReadRecord()
         * at which point it is overwritten.
         * @return A pointer to a DDFRecord object is returned, or nullptr if a read
         * error, or end of file occurs.
         */
        const DDFRecord *ReadRecord() noexcept;

        /**
         * Return to the first record.
         *
         * @warning The next call to ReadRecord() will read the first data record in the file.
         */
        void Rewind() noexcept;

        /**
         * Fetch a field definition by index.
         * @param idx (from 0 to GetFieldCount() - 1).
         * @return the returned field pointer or nullptr if the index is out of range.
         */
        const DDFFieldDefining *GetFieldDefining(const ulong &idx) const noexcept;

        /**
         * Fetch the definition of the named field.
         * @param fieldName The name of the field to search for.  The comparison is case insensitive.
         * @return A pointer to the request DDFSubfieldDefining object is returned, or nullptr
         * if none matching the name are found.  The return object remains owned by
         * the DDFModule, and should not be deleted by application code.
         */
        const DDFFieldDefining *FindFieldDefining(const string &fieldName) const noexcept;

        /**
         * Close an ISO 8211 (DDF) file.
         */
        void Close() noexcept;

        /**
         * Fetch the number of defined fields.
         */
        const size_t GetFieldCount() const noexcept { return fieldDefinings.size(); }

        /**
         * Fetch a field definition by index.
         * @param idx (from 0 to GetFieldCount() - 1).
         * @return the returned field pointer or nullptr if the index is out of range.
         */
        const DDFFieldDefining *operator[] (const ulong &idx) const noexcept { return GetFieldDefining(idx); }

        /**
         * Fetch the definition of the named field.
         * @param fieldName The name of the field to search for.  The comparison is case insensitive.
         * @return A pointer to the request DDFSubfieldDefining object is returned, or nullptr
         * if none matching the name are found.  The return object remains owned by
         * the DDFModule, and should not be deleted by application code.
         */
        const DDFFieldDefining *operator[] (const string &fieldName) const noexcept { return FindFieldDefining(fieldName); }

        /**
         * The destructor.
         */
        ~DDFModule() noexcept;

        /**
         * Delete standart realization
         */
         DDFModule(const DDFModule&) = delete;
         DDFModule(const DDFModule&&) = delete;
         DDFModule& operator = (const DDFModule&) = delete;
         DDFModule& operator = (const DDFModule&&) = delete;

        /**
         * Friend classes
         */
        friend class DDFRecord;
    };

    typedef enum : byte {
        DDF_DSC_ELEMENTARY = 0u,
        DDF_DSC_VECTOR,
        DDF_DSC_ARRAY,
        DDF_DSC_CONCATENATED
    } DDFdataStructureCode;

    typedef enum : byte {
        DDF_DTC_CHAR_STRING = 0u,
        DDF_DTC_IMPLICIT_POINT,
        DDF_DTC_EXPLICIT_POINT,
        DDF_DTC_EXPLICIT_POINT_SCALED,
        DDF_DTC_CHAR_BIT_STRING,
        DDF_DTC_BIT_STRING,
        DDF_DTC_MIXED_DATA_TYPE
    } DDFdataTypeCode;

    typedef enum : byte {
        DDF_TES_LEVEL0 = 0u,
        DDF_TES_LEVEL1,
        DDF_TES_LEVEL2
    } DDFtruncatedEscapeSequence;

    /**
     * Information from the DDR defining one field.  Note that just because
     * a field is defined for a DDFModule doesn't mean that it actually occurs
     * on any records in the module.  DDFFIeldDefinings are normally just significant
     * as containers of the DDFSubfieldDefining.
     */
    class DDFFieldDefining {
        char* tagName = nullptr;
        char* fieldName = nullptr;

        DDFdataTypeCode dataTypeCode = DDF_DTC_MIXED_DATA_TYPE;
        DDFdataStructureCode dataStructureCode = DDF_DSC_ELEMENTARY;
        DDFtruncatedEscapeSequence truncatedEscapeSequence = DDF_TES_LEVEL0;

        size_t fixedWidth = 0ull;
        bool isRepitingSubfields = false;

        vector<DDFSubfieldDefining*> subfieldDefinings;

    public:
        /**
         * The constructor.
         */
        DDFFieldDefining() noexcept : subfieldDefinings(0) {}

        /**
         * Fetch a subfield by index.
         * @param idx The index subfield index. (Between 0 and GetSubfieldCount()-1)
         * @return The subfield pointer, or nullptr if the index is out of range.
         */
        const DDFSubfieldDefining* GetSubfieldDefining(const ulong& idx) const noexcept;

        /**
         * Find a subfield definition by it's mnemonic tag.
         * @param mnemonicTag The name of the field.
         * @return The subfield pointer, or nullptr if there isn't any such subfield.
         */
        const DDFSubfieldDefining* FindSubfieldDefining(const string& mnemonicTag) const noexcept;

        /**
         * Gets a pointer to the field name (tag).
         * @warning this is an internal copy and shouldn't be freed.
         */
        const string GetName() const noexcept { return tagName; }

        /**
         * Gets a longer description of this field.
         * @warning this is an internal copy and shouldn't be freed.
         */
        const string GetDescription() const noexcept { return fieldName; }

        /**
         * Get the number of subfields.
         */
        const size_t GetSubfieldCount() const noexcept { return subfieldDefinings.size(); }

        /**
         * Fetch repeating flag.
         * @see DDFField::GetRepeatCount()
         * @return true if the field is marked as repeating.
         */
        const bool isRepiting() const noexcept { return isRepitingSubfields; }

        /**
         * Get the width of this field.  This function isn't normally used
         * by applications.
         *
         * @return The width of the field in bytes, or zero if the field is not
         * apparently of a fixed width.
         */
        const size_t GetFixedWidth() const noexcept { return fixedWidth; }

        /**
         * Fetch a subfield by index.
         * @param idx The index subfield index. (Between 0 and GetSubfieldCount()-1)
         * @return The subfield pointer, or nullptr if the index is out of range.
         */
        const DDFSubfieldDefining* operator[] (const ulong& idx) const noexcept { return GetSubfieldDefining(idx); }

        /**
         * Find a subfield definition by it's mnemonic tag.
         * @param mnemonicTag The name of the field.
         * @return The subfield pointer, or nullptr if there isn't any such subfield.
         */
        const DDFSubfieldDefining* operator[] (const string& mnemonicTag) const noexcept { return FindSubfieldDefining(mnemonicTag); }

        /**
         * Initialize the field definition from the information in the DDR record.
         * @param fieldControlLenght
         * @param sizeFieldTag
         * @param tagName
         * @param fieldEntrySize
         * @param pachFieldArea
         * @return nullptr if the initialization fails or a pointer of the create DDFFIeldDefining object.
         */
        static DDFFieldDefining* Initialize(const ulong& fieldControlLenght, const ulong& sizeFieldTag, char* tagName, const ulong& fieldEntrySize, const char* pachFieldArea) noexcept;

        /**
         * The destructor.
         */
        ~DDFFieldDefining() noexcept;

        /**
         * Delete standart realization
         */
         DDFFieldDefining(const DDFFieldDefining&) = delete;
         DDFFieldDefining(const DDFFieldDefining&&) = delete;
         DDFFieldDefining& operator = (const DDFFieldDefining&) = delete;
         DDFFieldDefining& operator = (const DDFFieldDefining&&) = delete;

    private:
        bool BuildSubfields(const char*& descriptorArray, const size_t& descriptorArrayLenght, const char*& formatControls, const size_t& formatControlsLenght) noexcept;
    };

    typedef enum : byte {
        DDF_BF_NOT_BINARY = 0u,
        DDF_BF_UINT,
        DDF_BF_SINT,
        DDF_BF_FP_REAL,
        DDF_BF_FLOAT_REAL,
        DDF_BF_FLOAT_COMPLEX
    } DDFbinaryFormat;

    typedef enum : byte {
        DDF_DT_INT = 0u,
        DDF_DT_FLOAT,
        DDF_DT_STRING,
        DDF_DT_BINARY_STRING
    } DDFdataType;

    /**
     * Information from the DDR record describing one subfield of a DDFFIeldDefining.
     * All subfields of a field will occur in each occurance of that field
     * (as a DDFField) in a DDFRecord.  Subfield's actually contain formatted
     * data (as instances within a record).
     */
    class DDFSubfieldDefining {
        bool isVariable = true;

        char* label = nullptr;

        byte dataFormat = 0;
        ulong formatWidth = 0ul;
        DDFdataType dataType = DDF_DT_INT;
        DDFbinaryFormat binaryFormat = DDF_BF_NOT_BINARY;

        vector<DDFSubfieldDefining*> subfieldDefinings;

    public:
        /**
         * The constructor.
         */
        DDFSubfieldDefining() noexcept : subfieldDefinings() {}

        /**
         * Extract a subfield value as an integer.  Given a pointer to the data
         * for this subfield (from within a DDFRecord) this method will return the
         * int data for this subfield.  The number of bytes consumed as part of this
         * field can also be fetched.  This method may be called for any type of
         * subfield, and will return zero if the subfield is not numeric.
         *
         * @param pachSourceData The pointer to the raw data for this field.  This
         * may have come from DDFRecord::GetData(), taking into account skip factors
         * over previous subfields data.
         * @param dataSize The maximum number of bytes that are accessable after
         * pachSourceData.
         * @param consumedBytes Pointer to an integer into which the number of bytes
         * consumed by this field should be written.  May be nullptr to ignore. This
         * is used as a skip factor to increment pachSourceData to point to the next
         * subfields data.
         *
         * @return The subfield's numeric value (or zero if it isn't numeric).
         * @see ExtractFloatData, ExtractDataAsString, ExtractDataAsBinary
         */
        const long ExtractDataAsLong(const byte* sourceData, const size_t& dataSize, size_t* consumedBytes = nullptr) const noexcept;

        /**
         * Extract a subfield value as a float.  Given a pointer to the data
         * for this subfield (from within a DDFRecord) this method will return the
         * floating point data for this subfield.  The number of bytes
         * consumed as part of this field can also be fetched.  This method may be
         * called for any type of subfield, and will return zero if the subfield is
         * not numeric.
         *
         * @param sourceData The pointer to the raw data for this field.  This
         * may have come from DDFRecord::GetData(), taking into account skip factors
         * over previous subfields data.
         * @param dataSize The maximum number of bytes that are accessable after
         * pachSourceData.
         * @param consumedBytes Pointer to an integer into which the number of
         * bytes consumed by this field should be written.  May be nullptr to ignore.
         * This is used as a skip factor to increment pachSourceData to point to the
         * next subfields data.
         *
         * @return The subfield's numeric value (or zero if it isn't numeric).
         * @see ExtractDataAsLong, ExtractDataAsString, ExtractDataAsBinary
         */
        const double ExtractDataAsDouble(const byte* sourceData, const size_t& dataSize, size_t* consumedBytes = nullptr) const noexcept;

        /**
         * Extract a zero terminated string containing the data for this subfield.
         * Given a pointer to the data for this subfield (from within a DDFRecord)
         * this method will return the data for this subfield.  The number of bytes
         * consumed as part of this field can also be fetched.  This number may
         * be one longer than the string length if there is a terminator character
         * used.<p>
         *
         * @param sourceData The pointer to the raw data for this field.  This
         * may have come from DDFRecord::GetData(), taking into account skip factors
         * over previous subfields data.
         * @param dataSize The maximum number of bytes that are accessable after
         * sourceData.
         * @param consumedBytes Pointer to an integer into which the number of
         * bytes consumed by this field should be written.  May be nullptr to ignore.
         * This is used as a skip factor to increment pachSourceData to point to the
         * next subfields data.
         *
         * @return A pointer to a buffer containing the data for this field.
         * @see ExtractDataAsLong, ExtractDataAsDouble, ExtractDataAsBinary
         */
        const char* ExtractDataAsString(const byte* sourceData, const size_t& dataSize, size_t* consumedBytes = nullptr) const noexcept;

        /**
         * Extract a zero terminated string containing the data for this subfield.
         * Given a pointer to the data for this subfield (from within a DDFRecord)
         * this method will return the data for this subfield.  The number of bytes
         * consumed as part of this field can also be fetched.  This number may
         * be one longer than the string length if there is a terminator character
         * used.<p>
         *
         * This function will return the raw binary data of a subfield for
         * types other than DDFString, including data past zero chars.  This is
         * the standard way of extracting DDFBinaryString subfields for instance.<p>
         *
         * @param sourceData The pointer to the raw data for this field.  This
         * may have come from DDFRecord::GetData(), taking into account skip factors
         * over previous subfields data.
         * @param dataSize The maximum number of bytes that are accessable after
         * sourceData.
         * @param consumedBytes Pointer to an integer into which the number of
         * bytes consumed by this field should be written.  May be nullptr to ignore.
         * This is used as a skip factor to increment pachSourceData to point to the
         * next subfields data.
         *
         * @return A pointer to a buffer containing the data for this field.
         * @see ExtractDataAsLong, ExtractDataAsDouble, ExtractDataAsString
         */
        const byte* ExtractDataAsBinary(const byte* sourceData, const size_t& dataSize, size_t* consumedBytes = nullptr) const noexcept;

        /**
         * Scan for the end of variable length data.  Given a pointer to the data
         * for this subfield (from within a DDFRecord) this method will return the
         * number of bytes which are data for this subfield.  The number of bytes
         * consumed as part of this field can also be fetched.  This number may
         * be one longer than the length if there is a terminator character
         * used.<p>
         *
         * This method is mainly for internal use, or for applications which
         * want the raw binary data to interpret themselves.  Otherwise use one
         * of ExtractStringData(), ExtractIntData() or ExtractFloatData().<p>
         *
         * @param sourceData The pointer to the raw data for this field.  This
         * may have come from DDFRecord::GetData(), taking into account skip factors
         * over previous subfields data.
         * @param dataSize The maximum number of bytes that are accessable after
         * sourceData.
         * @param pnConsumedBytes Pointer to an integer into which the number of
         * bytes consumed by this field should be written.  May be nullptr to ignore.
         *
         * @return The number of bytes at sourceData which are actual data for
         * this record (not including unit, or field terminator).
         * @see ExtractStringData(), ExtractIntData() or ExtractFloatData()
         */
        const size_t GetDataLenght(const byte* sourceData, const size_t& dataSize, size_t* consumedBytes = nullptr) const noexcept;

        /**
         * Gets a pointer to the subfield name (mnemonic name).
         */
        const string GetName() const noexcept { return label; }

        /**
         * Gets the subfield width (zero for variable).
         */
        const ulong GetWidth() const noexcept { return formatWidth; }

        /**
         * Check if subfields contained.
         * @return true if the field has subfields
         */
        const bool HasSubfields() const noexcept { return !subfieldDefinings.empty(); }

        /**
         * Get the general type of the subfield.  This can be used to
         * determine which of ExtractFloatData(), ExtractIntData() or
         * ExtractStringData() should be used.
         */
        const DDFdataType& GetDataType() const noexcept { return dataType; }

        /**
         * Get the binary format of the subfield.
         */
        const DDFbinaryFormat& GetBinaryFormat() const noexcept { return binaryFormat; }

        /**
         * Initialize the subfield definition from the information in the DDR record.
         * @param tagName
         * @param formatString
         * @param pachFieldArea
         * @return nullptr if the initialization fails or a pointer of the create DDFSubfieldDefining object.
         */
        static DDFSubfieldDefining* Initialize(char* tagName, const char* formatString, const char* pachFieldArea) noexcept;

        /**
         * The destructor.
         */
        ~DDFSubfieldDefining() noexcept;

        /**
         * Delete standart realization
         */
         DDFSubfieldDefining(const DDFSubfieldDefining&) = delete;
         DDFSubfieldDefining(const DDFSubfieldDefining&&) = delete;
         DDFSubfieldDefining& operator = (const DDFSubfieldDefining&) = delete;
         DDFSubfieldDefining& operator = (const DDFSubfieldDefining&&) = delete;

    private:
        bool ApplyFormat(const char*& stringFormat) noexcept;
        bool BuildSubfields(const char* pachFieldArea, const char*& stringFormat) noexcept;
    };

    /**
     * Contains instance data from one data record (DR).  The data is contained
     * as a list of DDFField instances partitioning the raw data into fields.
     */
    class DDFRecord {
        bool isClone = false;
        bool isReuseHeader = false;

        DDFModule& parentModule;

        ulong dataSize = 0ul;
        byte *binaryData = nullptr;

        vector<DDFField*> fields;
    public:
        /**
         * The constructor.
         * @param parentModule The reference of DDFModule object.
         */
        DDFRecord(DDFModule &parentModule) noexcept : parentModule(parentModule), fields() { }

        /**
         * Fetch field object based on index.
         * @param idx The index of the field to fetch.  Between 0 and GetFieldCount()-1.
         * @return A DDFField pointer, or nullptr if the index is out of range.
         */
        const DDFField* GetField(const ulong &idx) const;

        /**
         * Find the named field within this record.
         * @param fieldName The name of the field to fetch. The comparison is
         * case insensitive.
         * @param fieldIndex The instance of this field to fetch. Use zero (the
         * default) for the first instance.
         * @return Pointer to the requested DDFField.
         * @warning This pointer is to an internal object, and shouldn't be freed.
         * It remains valid until the next record read.
         */
        const DDFField *FindField(const string &fieldName, ulong fieldIndex = 0ul) const;

        /**
         * Get the number of DDFFields on this record.
         */
        const size_t GetFieldCount() const noexcept { return fields.size(); }

        /**
         * Make a copy of a record.
         *
         * This method is used to make a copy of a record that will become
         * the properly of application.
         *
         * @return A new copy of the DDFRecord.  This can be delete'd by the
         * application when no longer needed.
         */
        DDFRecord *Clone() const noexcept;

        /**
         * Fetch size of records raw data  in bytes.
         * @see GetBinaryData()
         */
        const ulong GetDataSize() const noexcept { return dataSize; }

        /**
         * Fetch the raw data for this record.  The returned pointer is effectively
         * to the data for the first field of the record, and is of size
         * @see GetDataSize().
         */
        const byte *GetBinaryData() const noexcept { return binaryData; }

        /**
         * Fetch the DDFModule with which this record is associated.
         */
        const DDFModule &GetModule() const noexcept { return parentModule; }

        /**
         * The destructor.
         */
        ~DDFRecord();

        /**
         * Delete standart realization
         */
         DDFRecord(const DDFRecord&) = delete;
         DDFRecord(const DDFRecord&&) = delete;
         DDFRecord& operator = (const DDFRecord&) = delete;
         DDFRecord& operator = (const DDFRecord&&) = delete;

        /**
         * Friend classes
         */
        friend class DDFModule;
    private:
        void Clear();
        bool Read();

        bool ReadHeader();
    };

    /**
     * This object represents one field in a DDFRecord.  This
     * models an instance of the fields data, rather than it's data definition
     * which is handled by the DDFFieldDefining class.  Note that a DDFField
     * doesn't have DDFSubfield children as you would expect.  To extract
     * subfield values use GetSubfieldData() to find the right data pointer and
     * then use ExtractIntData(), ExtractFloatData() or ExtractStringData().
     */
    class DDFField {
        size_t dataSize = 0ull;
        const byte *binaryData = nullptr;

        const DDFFieldDefining *fieldDefining = nullptr;
    public:
        /**
         * The constructor.
         * @param fieldDefining the pointer of DDFFieldDefining object.
         * @param fieldEntrySize the size of the field.
         * @param pachFieldArea associated binary data for this field
         */
        DDFField(const DDFFieldDefining *fieldDefining, const size_t fieldEntrySize, const byte *pachFieldArea) noexcept :
            dataSize(fieldEntrySize), binaryData(pachFieldArea), fieldDefining(fieldDefining) {}

        /**
         * Gets the number of bytes in the data block.
         * @see GetBinaryData()
         */
        const size_t GetDataSize() const noexcept { return dataSize; }

        /**
         * Gets the pointer to the entire data block for this field.
         * @warning This is an internal copy, and shouldn't be freed by the application.
         */
        const byte* GetBinaryData() const noexcept { return binaryData; }

        /**
         * Gets value of a subfield as a long. This is a convenience
         * function for fetching a subfield of a field within this record.
         * @param subfieldName The name of the subfield within the current field.
         * @return The value of the subfield, or zero if it failed for some reason.
         * @see GetSubfieldAsDouble, GetSubfieldAsString, GetSubfieldAsBinary
         */
        const long GetSubfieldAsLong(const string &sufieldName, const ulong fieldIdx = 0ul) const noexcept;

        /**
         * Gets value of a subfield as a double.  This is a convenience
         * function for fetching a subfield of a field within this record.
         * @param subfieldName The name of the subfield within the current field.
         * @return The value of the subfield, or zero if it failed for some reason.
         * @see GetSubfieldAsLong, GetSubfieldAsString, GetSubfieldAsBinary
         */
        const double GetSubfieldAsDouble(const string &sufieldName, const ulong fieldIdx = 0ul) const noexcept;

        /**
         * Gets value of a subfield as a string.  This is a convenience
         * function for fetching a subfield of a field within this record.
         * @param subfieldName The name of the subfield within the current field.
         * @return The value of the subfield, or nullptr if it failed for some reason.
         * @see GetSubfieldAsDouble, GetSubfieldAsLong, GetSubfieldAsBinary
         */
        const char* GetSubfieldAsString(const string &sufieldName, const ulong fieldIdx = 0ul) const noexcept;

        /**
         * Gets value of a subfield as a string.  This is a convenience
         * function for fetching a subfield of a field within this record.
         * @param subfieldName The name of the subfield within the current field.
         * @return The value of the subfield, or nullptr if it failed for some reason.
         * @warning The returned pointer is to internal data and should not be modified or
         * freed by the application.
         * @see GetSubfieldAsDouble, GetSubfieldAsLong, GetSubfieldAsString
         */
        const byte* GetSubfieldAsBinary(const string &sufieldName, const ulong fieldIdx = 0ul) const noexcept;

        /**
         * How many times do the subfields of this record repeat?  This
         * will always be one for non-repeating fields.
         *
         * @return The number of times that the subfields of this record occur
         * in this record.  This will be one for non-repeating fields.
         */
        const size_t GetRepeatCount() const noexcept;

        /**
         * Gets the corresponding DDFFieldDefining.
         */
        const DDFFieldDefining* GetFieldDefining() const noexcept { return fieldDefining; }

         /**
          * Destructor
          */
         ~DDFField() noexcept = default;

        /**
         * Delete standart realization
         */
        DDFField(const DDFField&) = delete;
        DDFField(const DDFField&&) = delete;
        DDFField& operator = (const DDFField&) = delete;
        DDFField& operator = (const DDFField&&) = delete;

    private:
        const byte* GetBinaryData(const DDFSubfieldDefining *&subfieldDef, const ulong &fieldIdx, size_t *byteLeft = nullptr) const noexcept;
    };
}
#endif /* defined(__Standards__iso8211__) */
