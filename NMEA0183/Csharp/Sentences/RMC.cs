//
//  RMC.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

using System;

namespace Standards.Nmea.Sentences {
    /// <summary>
    /// Base class for Recommended Minimum Navigation Information Message
    /// </summary>
    public sealed class RMC : Sentence {
        /// <summary>
        /// Time (UTC)
        /// </summary>
        public TimeSpan Time { get; private set; }

        /// <summary>
        /// Status, V = Navigation receiver warning
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
        /// Speed over ground, knots
        /// </summary>
        public double Speed { get; private set; }

        /// <summary>
        /// Track made good, degrees true
        /// </summary>
        public double Course { get; private set; }

        /// <summary>
        /// Date
        /// </summary>
        public DateTime Date { get; private set; }     

        /// <summary>
        /// Magnetic Variation, degrees
        /// </summary>
        public double MagneticVariation { get; private set; }

        /// <summary>
        /// The mode indicator
        /// </summary>
        public string Mode { get; private set; }

        /// <summary>
        /// Clear all properties
        /// </summary>
        public override void Clear() {
            Time = TimeSpan.Zero;
            Status = null;
            Latitude = double.NaN;
            Longitude = double.NaN;
            Speed = double.NaN;
            Course = double.NaN;
            Date = DateTime.MinValue;
            MagneticVariation = double.NaN;
            Mode = null;
        }

        /// <summary>
        /// Fill Class from NMEA Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            //clear properties
            Clear();

            //Parse Date, Time and Status
            Time = ParseTime(message[1]);
            Date = ParseDate(message[9]);
            Status = message[2];

            IsWarning = Status != "A";

            //return if Navigation receiver warning
            if (IsWarning) return;

            //Parse Coordinates
            Latitude = ParseCoord(message[3], message[4]);
            Longitude = ParseCoord(message[5], message[6]);

            //Parse Speed and Curse
            Speed = ParseDouble(message[7]);
            Course = ParseDouble(message[8]);

            //Parse Magnetic Variation
            MagneticVariation = ParseDouble(message[10]);

            if (message.Length > 11)
                //Parse Mode indicator
                Mode = message[11];
        }
    }
}