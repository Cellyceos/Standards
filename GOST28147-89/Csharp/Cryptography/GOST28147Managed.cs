//
//  GOST28147Managed.cs
//  GOST28147-89
//
//  Created by Kirill Bravichev on 20/03/2013.
//  Copyright (c) 2013 Cellyceos. All rights reserved.
//

using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Cryptography;

namespace Standards.Security.Cryptography {
    /// <summary>Provides a managed implementation of the GOST 28147-89 symmetric algorithm. </summary>
    [ComVisible(true)]
    public sealed class GOST28147Managed : GOST28147 {
        static private readonly RandomNumberGenerator _rng = RandomNumberGenerator.Create();
        private static readonly Random _rnd = new Random();

        /// <summary>Creates a symmetric encryptor object with the specified <see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/> property and initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>).</summary>
        /// <param name="rgbKey">The secret key to use for the symmetric algorithm. </param>
        /// <param name="rgbIV">The initialization vector to use for the symmetric algorithm. </param>
        /// <returns>A symmetric encryptor object. </returns>
        public override ICryptoTransform CreateEncryptor(byte[] rgbKey, byte[] rgbIV) {
            return CreateEncryptor(rgbKey, rgbIV, SBlocks);
        }

        /// <summary>Creates a symmetric encryptor object with the specified <see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/> property, initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>) and replacement units (<see cref="P:Standards.Security.Cryptography.GOST28147.SBlocks"/>).</summary>
        /// <param name="rgbKey">The secret key to use for the symmetric algorithm.</param>
        /// <param name="rgbIV">The initialization vector to use for the symmetric algorithm.</param>
        /// <param name="rgbSBlocks">The replacement units to use for the symmetric algorithm.</param>
        /// <returns>A symmetric encryptor object. </returns>
        public override ICryptoTransform CreateEncryptor(byte[] rgbKey, byte[] rgbIV, byte[] rgbSBlocks) {
            return new GOST28147Transform(rgbKey, ModeValue, rgbIV, BlockSizeValue, rgbSBlocks, FeedbackSizeValue, PaddingValue, CanUseMAC, TransformMode.Encrypt);
        }

        /// <summary>Creates a symmetric decryptor object with the specified <see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/> property and initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>).</summary>
        /// <param name="rgbKey">The secret key to use for the symmetric algorithm. </param>
        /// <param name="rgbIV">The initialization vector to use for the symmetric algorithm. </param>
        /// <returns>A symmetric decryptor object.</returns>
        public override ICryptoTransform CreateDecryptor(byte[] rgbKey, byte[] rgbIV) {
            return CreateDecryptor(rgbKey, rgbIV, SBlocks);
        }

        /// <summary>Creates a symmetric decryptor object with the specified <see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/> property, initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>) and replacement units (<see cref="P:Standards.Security.Cryptography.GOST28147.SBlocks"/>).</summary>
        /// <param name="rgbKey">The secret key to use for the symmetric algorithm.</param>
        /// <param name="rgbIV">The initialization vector to use for the symmetric algorithm.</param>
        /// <param name="rgbSBlocks">The replacement units to use for the symmetric algorithm.</param>
        /// <returns>A symmetric encryptor object. </returns>
        public override ICryptoTransform CreateDecryptor(byte[] rgbKey, byte[] rgbIV, byte[] rgbSBlocks) {
            return new GOST28147Transform(rgbKey, ModeValue, rgbIV, BlockSizeValue, rgbSBlocks, FeedbackSizeValue, PaddingValue, CanUseMAC, TransformMode.Decrypt);
        }

        /// <summary>Generates a random key (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/>) to use for the algorithm.</summary>
        [SecurityCritical]
        public override void GenerateKey() {
            KeyValue = new byte[KeySizeValue >> 3];
            _rng.GetBytes(KeyValue);
        }

        /// <summary>Generates a random initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>) to use for the algorithm.</summary>
        [SecurityCritical]
        public override void GenerateIV() {
            IVValue = new byte[BlockSizeValue >> 3];
            _rng.GetBytes(IVValue);
        }

        /// <summary>Generates a random S block (<see cref="P:Standards.Security.Cryptography.GOST28147.SBlocks"/>) to use for the algorithm. </summary>
        [SecurityCritical]
        public override void GenerateSBlocks() {
            SBlocksValue = new byte[] {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
            };

            for (var i = 0; i < 8; i++) {
                var offset = i << 4;

                for (var j = 0; j < 16; j++) {
                    var idx = _rnd.Next(0, 15);
                    var tmp = SBlocksValue[offset + j];

                    SBlocksValue[offset + j] = SBlocksValue[offset + idx];
                    SBlocksValue[offset + idx] = tmp;
                }
            }
        }

    }
}
