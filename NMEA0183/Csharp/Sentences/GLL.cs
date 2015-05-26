//
//  GLL.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

using System;

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for GPS Range Residuals Message
    ///</summary>
    public sealed class GLL : Sentence {
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
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            Time = TimeSpan.Zero;
            Status = null;

            Latitude = double.NaN;
            Longitude = double.NaN;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            Latitude = ParseCoord(message[1], message[2]);
            Longitude = ParseCoord(message[3], message[4]);

            Time = ParseTime(message[5]);

            Status = message[6];
            IsWarning = Status == "V";
        }
    }
}
