//
//  GOST28147.cs
//  GOST28147-89
//
//  Created by Kirill Bravichev on 20/03/2013.
//  Copyright (c) 2013 Cellyceos. All rights reserved.
//

using System;
using System.Runtime.InteropServices;
using System.Security.Cryptography;

namespace Standards.Security.Cryptography {
    /// <summary>Represents the abstract base class from which all implementations of the GOST 28147-89 must inherit. </summary>
    [ComVisible(true)]
    public abstract class GOST28147 : SymmetricAlgorithm {
        private static readonly KeySizes[] _legalBlockSizes = {new KeySizes(64, 64, 0)};
        private static readonly KeySizes[] _legalKeySizes = {new KeySizes(256, 256, 0)};
        private static readonly KeySizes[] _legalSBoxSizes = {new KeySizes(512, 512, 0)};

        /// <summary>Gets or Sets a value indicating whether the message authentication code can be using.</summary>
        /// <returns>true if message authentication code can use; otherwise, false.</returns>
        public bool CanUseMAC { get; set; }

        /// <summary>Specifies the SBlocks value</summary>
        protected byte[] SBlocksValue; 

        /// <summary>Specifies the key sizes, in bits.</summary>
        protected KeySizes[] LegalSBlocksSizesValue;

        /// <summary>Represents the replacement units size, in bits, of the cryptographic operation.</summary>
        protected int SBlocksSizeValue;

        /// <summary>Gets the key sizes, in bits, that are supported by the symmetric algorithm.</summary>
        /// <returns>An array that contains the key sizes supported by the algorithm.</returns>
        public virtual KeySizes[] LegalSBlocksSizes { get { return (KeySizes[])LegalSBlocksSizesValue.Clone(); } }

        /// <summary>Gets or sets the size, in bits, of the replacement units used by the symmetric algorithm.</summary>
        /// <returns>The size, in bits, of the secret key used by the symmetric algorithm.</returns>
        /// <exception cref="T:System.Security.Cryptography.CryptographicException">The replacement units size is not valid. </exception>
        public virtual int SBlocksSize {
            get { return SBlocksSizeValue; }
            set {
                if (ValidSBlocksSize(value))
                    throw new CryptographicException("value");

                SBlocksSizeValue = value;
                SBlocksValue = null;
            }
        }

        /// <summary>Gets or sets the replacement units for the symmetric algorithm.</summary>
        /// <returns>The replacement units to use for the symmetric algorithm.</returns>
        /// <exception cref="T:System.ArgumentNullException">An attempt was made to set the replacement units to null. </exception>
        /// <exception cref="T:System.Security.Cryptography.CryptographicException">The replacement units size is invalid.</exception>
        public virtual byte[] SBlocks {
            get {
                if (SBlocksValue == null)
                    GenerateSBlocks();

                return (byte[])SBlocksValue.Clone();
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");

                if (!ValidSBlocksSize(value.Length << 2))
                    throw new CryptographicException("The S block size is invalid.");

                SBlocksValue = (byte[]) value.Clone();
                SBlocksSizeValue = value.Length << 2;
            }
        }

        /// <summary>Initializes a new instance of the <see cref="T:Standards.Security.Cryptography.GOST28147" /> class. </summary>
        protected GOST28147() {
            CanUseMAC = false;
            KeySizeValue = 256;
            BlockSizeValue = 64;
            SBlocksSizeValue = 512;
            FeedbackSizeValue = 64;
            ModeValue = CipherMode.ECB;
            PaddingValue = PaddingMode.None;
            LegalKeySizesValue = _legalKeySizes;
            LegalBlockSizesValue = _legalBlockSizes;
            LegalSBlocksSizesValue = _legalSBoxSizes;
        }

        /// <summary>When overridden in a derived class, generates a S block (<see cref="P:Standards.Security.Cryptography.GOST28147.SBlocks"/>) to use for the algorithm. </summary>
        public abstract void GenerateSBlocks();

        /// <summary>When overridden in a derived class, creates a symmetric encryptor object with the specified <see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/> property, initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>) and replacement units (<see cref="P:Standards.Security.Cryptography.GOST28147.SBlocks"/>).</summary>
        /// <param name="rgbKey">The secret key to use for the symmetric algorithm.</param>
        /// <param name="rgbIV">The initialization vector to use for the symmetric algorithm.</param>
        /// <param name="rgbSBox">The replacement units to use for the symmetric algorithm.</param>
        /// <returns>A symmetric encryptor object. </returns>
        public abstract ICryptoTransform CreateEncryptor(byte[] rgbKey, byte[] rgbIV, byte[] rgbSBox);

        /// <summary>When overridden in a derived class, creates a symmetric decryptor object with the specified <see cref="P:System.Security.Cryptography.SymmetricAlgorithm.Key"/> property, initialization vector (<see cref="P:System.Security.Cryptography.SymmetricAlgorithm.IV"/>) and replacement units (<see cref="P:Standards.Security.Cryptography.GOST28147.SBlocks"/>).</summary>
        /// <param name="rgbKey">The secret key to use for the symmetric algorithm.</param>
        /// <param name="rgbIV">The initialization vector to use for the symmetric algorithm.</param>
        /// <param name="rgbSBox">The replacement units to use for the symmetric algorithm.</param>
        /// <returns>A symmetric encryptor object. </returns>
        public abstract ICryptoTransform CreateDecryptor(byte[] rgbKey, byte[] rgbIV, byte[] rgbSBox);

        /// <summary>Determines whether the specified S box size is valid for the current algorithm.</summary>
        /// <returns>true if the specified key size is valid for the current algorithm; otherwise, false.</returns>
        /// <param name="bitLength">The length, in bits, to check for a valid key size. </param>
        public bool ValidSBlocksSize(int bitLength) {
            if (LegalSBlocksSizes == null)
                return false;

            foreach (var blockSize in LegalSBlocksSizes) {
                if (blockSize.SkipSize == 0 && blockSize.MinSize == bitLength)
                    return true;

                for (var currentSize = blockSize.MinSize; currentSize <= blockSize.MaxSize; currentSize += blockSize.SkipSize)
                    if (currentSize == bitLength)
                        return true;
            }

            return false;
        }

        /// <summary>Creates a cryptographic object that is used to perform the symmetric algorithm.</summary>
        /// <returns>A cryptographic object that is used to perform the symmetric algorithm.</returns>
        public new static GOST28147 Create() {
            return Create("GOST28147");
        }

        /// <summary>Creates a cryptographic object that specifies the implementation of AES to use to perform the symmetric algorithm.</summary>
        /// <returns>A cryptographic object that is used to perform the symmetric algorithm.</returns>
        /// <param name="algorithmName">The name of the specific implementation of AES to use.</param>
        /// <exception cref="T:System.ArgumentNullException">The <paramref name="algorithmName" /> parameter is null.</exception>
        public new static GOST28147 Create(string algorithmName) {
            if (string.IsNullOrEmpty(algorithmName))
                throw new ArgumentNullException("algorithmName");

            CryptoConfig.AddAlgorithm(typeof(GOST28147Managed), "GOST28147");

            return (GOST28147)CryptoConfig.CreateFromName(algorithmName);
        }
    }
}
