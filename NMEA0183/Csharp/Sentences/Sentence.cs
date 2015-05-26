//
//  Sentence.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 15/12/2011.
//  Copyright (c) 2011 Cellyceos. All rights reserved.
//

using System;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;

namespace Standards.Nmea.Sentences {
    /// <summary>
    /// Base abstract class for Sentence
    /// </summary>
    public abstract class Sentence {
        /// <summary>
        /// Warning
        /// </summary>
        public bool IsWarning { get; protected set; }
        
        /// <summary>
        /// Initializes a new instance of the <see cref="Sentence"/>  structure to a specified Sentences and Talkers
        /// </summary>
        protected Sentence() {
            IsWarning = true;
        }

        /// <summary>
        /// Check sum
        /// </summary>
        private static bool CheckHashSumm(string str) {
            byte hashSumm = 0;
            byte computeSumm = 0;

            Parallel.Invoke(() => byte.TryParse(str.Substring(str.Length - 2), NumberStyles.HexNumber, null,out hashSumm),
                            () => computeSumm = GetHashSumm(str.Remove(str.Length - 3)));

            return computeSumm == hashSumm;
        }

        /// <summary>
        /// Get Check sum
        /// </summary>
        /// <returns>Check sum for this message</returns>
        public static byte GetHashSumm(string str) {
            return str.Aggregate<char, byte>(0, (current, hashSumm) => Convert.ToByte(current ^ hashSumm));
        }

        /// <summary>
        /// abstract function need override
        /// </summary>
        public abstract void Clear();

        /// <summary>
        /// abstract function need override
        /// </summary>
        protected abstract void FillFromMessage(ref string[] message);

        /// <summary>
        /// Converts the string representation of the name to an equivalent Sentence object.
        /// </summary>
        /// <param name="msg">NMEA message</param>
        public static Sentence Parse(string msg) {
            msg = msg.Trim(new[] {'\r', '\0', '$'});

            if (!CheckHashSumm(msg))
                return null;

            msg = msg.Remove(msg.Length - 3);

            var splitmsg = msg.Split(new[] { ',' });

            var type = Type.GetType("Standards.Nmea.Sentences." + splitmsg[0].Substring(splitmsg[0].Length > 4 ? 2 : 1, 3));

            if (type == null)
                return null;

            var sente = (Sentence)Activator.CreateInstance(type);

            sente.FillFromMessage(ref splitmsg);

            return sente;
        }

        /// <summary>
        /// Converts the string representation of a time interval to its TimeSpan equivalent. 
        /// </summary>
        /// <param name="str">A string that contains a time to convert.</param>
        internal TimeSpan ParseTime(string str) {
            try {
                return TimeSpan.ParseExact(str, str.Length > 6 ? @"hhmmss\.fff" : "hhmmss", null);
            } catch (Exception) {
                return TimeSpan.Zero;
            }            
        }

        /// <summary>
        /// Converts the specified string representation of a date and time to its DateTime equivalent.
        /// </summary>
        /// <param name="str">A string that contains a date to convert.</param>
        internal DateTime ParseDate(string str) {
            try {
                return DateTime.ParseExact(str, "ddMMyy", null);
            } catch (Exception) {
                return DateTime.MinValue;
            }
        }

        /// <summary>
        /// Converts the string representation of a number to its double-precision floating-point number equivalent.
        /// </summary>
        /// <param name="str">A string that contains a number to convert.</param>
        internal double ParseDouble(string str) {
            return String.IsNullOrEmpty(str) ? 0.0 : double.Parse(str, NumberFormatInfo.InvariantInfo);
        }

        /// <summary>
        /// Converts the string representation of a number to its 32-bit signed integer equivalent.
        /// </summary>
        /// <param name="str">A string that contains a number to convert.</param>
        internal int ParseInteger(string str) {
            return String.IsNullOrEmpty(str) ? 0 : int.Parse(str, NumberFormatInfo.InvariantInfo);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="str"></param>
        internal byte[] ParseBinary(string str) {
            int nBytesCount = str.Length * 6 / 8;

            if (((str.Length * 6) % 8) != 0) ++nBytesCount;

            var vecBinary = new byte[nBytesCount];

            int currentByte = -1;
            byte bitsLeft = 0;

            foreach (var t in str.TakeWhile(c => c >= '0' && c <= 'W' || c >= '`' && c <= 'w').Select(ConvertToSixBit)) {
                if (bitsLeft != 0)
                    vecBinary[currentByte] |= Convert.ToByte(t >> (6 - bitsLeft));

                if (bitsLeft < 6) {
                    bitsLeft += 2;

                    vecBinary[++currentByte] |= Convert.ToByte((t << bitsLeft) >> bitsLeft);
                    vecBinary[currentByte] <<= bitsLeft;
                } else { 
                    bitsLeft -= 6;
                }
            }

            return vecBinary;
        }

        private static byte ConvertToSixBit(char ch) {
            var code = (byte)ch;

            code += 40;
            code += Convert.ToByte(code > 128 ? 32 : 40);

            return Convert.ToByte(code & 63);            
        }

        /// <summary>
        /// Converts the string representation of a number to its double-precision floating-point number equivalent.
        /// </summary>
        /// <param name="coord">A string that contains a number to convert.</param>
        /// <param name="hemi">A string that contains a single character.</param>
        internal double ParseCoord(string coord, string hemi) {
            var a = ParseDouble(coord);

            var b = (int)(a / 100);
            a = b + (a / 100 - b) / 60 * 100;

            return (hemi == "S") || (hemi == "W") ? -a : a;
        }
    }
}
