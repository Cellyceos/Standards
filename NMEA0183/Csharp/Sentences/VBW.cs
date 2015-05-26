//
//  VBW.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for Set and Drift Message
    ///</summary>
    public sealed class VBW : Sentence {
        /// <summary>
        /// Longitudinal water speed, "-" means astern
        /// </summary>
        public double LonWaterSpeed { get; private set; }

        /// <summary>
        /// Transverse water speed, "-" means port
        /// </summary>
        public double TranWaterSpeed { get; private set; }

        /// <summary>
        /// Status, A = data valid
        /// </summary>
        public string WaterStatus { get; private set; }

        /// <summary>
        /// Longitudinal ground speed, "-" means astern
        /// </summary>
        public double LonGroundSpeed { get; private set; }

        /// <summary>
        /// Transverse ground speed, "-" means port
        /// </summary>
        public double TranGroundSpeed { get; private set; }

        /// <summary>
        /// Status, A = data valid
        /// </summary>
        public string GroundStatus { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            LonWaterSpeed = double.NaN;
            TranWaterSpeed = double.NaN;
            WaterStatus = null;

            LonGroundSpeed = double.NaN;
            TranGroundSpeed = double.NaN;
            GroundStatus = null;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            LonWaterSpeed = ParseDouble(message[1]);
            TranWaterSpeed = ParseDouble(message[2]);
            WaterStatus = message[3];

            LonGroundSpeed = ParseDouble(message[4]); 
            TranGroundSpeed = ParseDouble(message[5]);
            GroundStatus = message[6];
        }
    }
}
