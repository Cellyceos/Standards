//
//  RMA.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for Recommended Minimum Navigation Information Message
    ///</summary>
    public sealed class RMA : Sentence
    {
        /// <summary>
        /// Blink Warning
        /// </summary>
        public string Status { get; private set; }

        /// <summary>
        /// Latitude
        /// </summary>
        public double Latitude { get; private set; }

        /// <summary>
        /// Longitude
        /// </summary>
        public double Longitude { get; private set; }

        /// <summary>
        /// Time Difference A, μS (not used)
        /// </summary>
        public string DifferenceA { get; private set; }

        /// <summary>
        /// Time Difference B, μS (not used)
        /// </summary>
        public string DifferenceB { get; private set; }

        /// <summary>
        /// Speed over ground, knots
        /// </summary>
        public double Speed { get; private set; }

        /// <summary>
        /// Track made good, degrees true
        /// </summary>
        public double Course { get; private set; }

        /// <summary>
        /// Magnetic Variation, degrees
        /// </summary>
        public double MagneticVariation { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            Status = null;
            Latitude = double.NaN;
            Longitude = double.NaN;
            DifferenceA = null;
            DifferenceB = null;
            Speed = double.NaN;
            Course = double.NaN;
            MagneticVariation = double.NaN;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            Status = message[1];
            IsWarning = Status != "A";

            Latitude = ParseCoord(message[2], message[3]);
            Longitude = ParseCoord(message[4], message[5]);

            DifferenceA = message[6];
            DifferenceB = message[7];

            Speed = ParseDouble(message[8]);
            Course = ParseDouble(message[9]);

            MagneticVariation = ParseDouble(message[10]);
        }
    }
}
