//
//  RMB.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

namespace Standards.Nmea.Sentences {
    ///<summary>
    /// Base class for Recommended Minimum Navigation Information Message
    ///</summary>
    public sealed class RMB : Sentence {
        /// <summary>
        /// Status, V = Navigation receiver warning
        /// </summary>
        public string Status { get; private set; }

        /// <summary>
        /// Cross Track error - nautical miles
        /// </summary>
        public double CrossTrack { get; private set; }

        /// <summary>
        /// Direction to Steer, Left or Right
        /// </summary>
        public string Direction { get; private set; }

        /// <summary>
        ///  TO Waypoint ID
        /// </summary>
        public string ToWaypoint { get; private set; }

        /// <summary>
        /// FROM Waypoint ID
        /// </summary>
        public string FromWaypoint { get; private set; }

        /// <summary>
        /// Destination Waypoint Latitude
        /// </summary>
        public double Latitude { get; private set; }

        /// <summary>
        /// Destination Waypoint Longitude
        /// </summary>
        public double Longitude { get; private set; }

        /// <summary>
        /// Range to destination in nautical miles
        /// </summary>
        public double Range { get; private set; }

        /// <summary>
        /// Bearing to destination in degrees True
        /// </summary>
        public double Bearing { get; private set; }

        /// <summary>
        /// Destination closing velocity in knots
        /// </summary>
        public double Velocity { get; private set; }

        /// <summary>
        /// Arrival Status, A = Arrival Circle Entered
        /// </summary>
        public string ArrivalStatus { get; private set; }

        /// <summary>
        /// Clear all Properties
        /// </summary>
        public override void Clear() {
            Status = null;
            CrossTrack = double.NaN;
            Direction = null;
            ToWaypoint = null;
            FromWaypoint = null;
            Latitude = double.NaN;
            Longitude = double.NaN;
            Range = double.NaN;
            Bearing = double.NaN;
            Velocity = double.NaN;
            ArrivalStatus = null;
        }

        /// <summary>
        /// Fill Class From Splited Message
        /// </summary>
        protected override void FillFromMessage(ref string[] message) {
            Clear();

            Status = message[1];
            IsWarning = Status != "A";

            CrossTrack = ParseDouble(message[2]);
            Direction = message[3];
            ToWaypoint = message[4];
            FromWaypoint = message[5];
            Latitude = ParseCoord(message[6], message[7]);
            Longitude = ParseCoord(message[8], message[9]);
            Range = ParseDouble(message[10]);
            Bearing = ParseDouble(message[11]);
            Velocity = ParseDouble(message[12]);
            ArrivalStatus = message[13];
        }
    }
}
