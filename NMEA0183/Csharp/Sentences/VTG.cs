//
//  VTG.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for Track Made Good and Ground Speed Message
    ///</summary>
    public sealed class VTG : Sentence {
        public VTG() {
            IsWarning = false;
        }

        /// <summary>
        /// True Track Degrees
        /// </summary>
        public double TrueDegree { get; private set; }

        /// <summary>
        /// Magnetic Track Degrees
        /// </summary>
        public double MagneticDegree { get; private set; }

        /// <summary>
        /// Speed Knots
        /// </summary>
        public double SpeedInKnots { get; private set; }

        /// <summary>
        /// Speed Kilometers Per Hour
        /// </summary>
        public double SpeedInKmh { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            TrueDegree = double.NaN;
            MagneticDegree = double.NaN;
            SpeedInKnots = double.NaN;
            SpeedInKmh = double.NaN;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            TrueDegree = ParseDouble(message[1]);
            MagneticDegree = ParseDouble(message[3]);

            SpeedInKnots = ParseDouble(message[5]);
            SpeedInKmh = ParseDouble(message[7]);
        }
    }
}
