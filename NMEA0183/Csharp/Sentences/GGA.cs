//
//  GGA.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

using System;

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for Geographic Position, Loran-C Message
    ///</summary>
    public sealed class GGA : Sentence {
        public GGA() {
            IsWarning = false;
        }

        /// <summary>
        /// Time (UTC)
        /// </summary>
        public TimeSpan Time { get; private set; }

        /// <summary>
        /// Latitude
        /// </summary>
        public double Latitude { get; private set; }

        /// <summary>
        /// Longitude
        /// </summary>
        public double Longitude { get; private set; }

        /// <summary>
        /// GPS Quality Indicator
        /// </summary>
        public int QualityIndicator { get; private set; }

        /// <summary>
        /// Number of satellites in view
        /// </summary>
        public int Satellites { get; private set; }

        /// <summary>
        /// Horizontal Dilution of precision
        /// </summary>
        public double Dilution { get; private set; }

        /// <summary>
        /// Antenna Altitude above/below mean-sea-level (geoid)
        /// </summary>
        public double Altitude { get; private set; }

        /// <summary>
        /// Geoidal separation, the difference between the WGS-84 earth ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
        /// </summary>
        public double GeoidalSeparation { get; private set; }

        /// <summary>
        /// Age of differential GPS data, time in seconds since last SC104 type 1 or 9 update, null field when DGPS is not used
        /// </summary>
        public int Age { get; private set; }

        /// <summary>
        /// Differential reference station ID, 0000-1023
        /// </summary>
        public string StationID { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            Time = TimeSpan.Zero;
            Latitude = double.NaN;
            Longitude = double.NaN;
            QualityIndicator = 0;
            Satellites = 0;
            Dilution = double.NaN;
            Altitude = double.NaN;
            GeoidalSeparation = double.NaN;
            Age = 0;
            StationID = null;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            Time = ParseTime(message[1]);

            Latitude = ParseCoord(message[2], message[3]);
            Longitude = ParseCoord(message[4], message[5]);

            QualityIndicator = ParseInteger(message[6]);

            Satellites = ParseInteger(message[7]);

            Dilution = ParseDouble(message[8]);

            Altitude = ParseDouble(message[9]);

            GeoidalSeparation = ParseDouble(message[11]);

            Age = ParseInteger(message[13]);

            StationID = message[14];
        }
    }
}
