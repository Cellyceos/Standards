//
//  VDM.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for UAIS VHF Data-link Message
    ///</summary>
    public sealed class VDM : Sentence {
        /// <summary>
        /// Binary Data
        /// </summary>
        public byte[] BinaryPart { get; private set; }

        /// <summary>
        /// Radio channel code
        /// </summary>
        public string Chanel { get; private set; }

        /// <summary>
        /// Number of fill bits
        /// </summary>
        public int Fillbits { get; private set; }

        /// <summary>
        /// Count of fragments in the message
        /// </summary>
        public int Total { get; private set; }

        /// <summary>
        /// Fragment number of this sentence
        /// </summary>
        public int Current { get; private set; }

        /// <summary>
        /// Sequential message ID
        /// </summary>
        public int SequenceID { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            BinaryPart = null;
            Total = 0;
            Current = 0;
            Chanel = null;
            Fillbits = 0;
            SequenceID = 0;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            Total = ParseInteger(message[1]);
            Current = ParseInteger(message[2]);
            SequenceID = ParseInteger(message[3]);
            Chanel = message[4];
            BinaryPart = ParseBinary(message[5]);
            Fillbits = ParseInteger(message[6]);
        }
    }
}