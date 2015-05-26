//
//  ZDA.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

using System;

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for Time and Distance to Variable Point Message
    ///</summary>
    public sealed class ZDA : Sentence {
        public ZDA() {
            IsWarning = false;
        }

        /// <summary>
        /// Time (UTC)
        /// </summary>
        public TimeSpan Time { get; private set; }

        /// <summary>
        /// Day, 01 to 31
        /// </summary>
        public int Day { get; private set; }

        /// <summary>
        /// Month, 01 to 12
        /// </summary>
        public int Month { get; private set; }

        /// <summary>
        /// Year (4 digit format)
        /// </summary>
        public int Year { get; private set; }

        /// <summary>
        /// Local zone minutes description, same sign as local hours
        /// </summary>
        public int MinuteOffset { get; private set; }

        /// <summary>
        /// Local zone description, 00 to +/- 13 hours
        /// </summary>
        public int HourOffset { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            Time = TimeSpan.Zero;
            Day = 0;
            Month = 0;
            Year = 0;
            MinuteOffset = 0;
            HourOffset = 0;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            Time = ParseTime(message[1]);

            Day = ParseInteger(message[2]);
            Month = ParseInteger(message[3]);
            Year = ParseInteger(message[4]);

            HourOffset = ParseInteger(message[5]);
            MinuteOffset = ParseInteger(message[6]);
        }
    }
}