//
//  GOST28147Transform.cs
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
    /// <summary>Performs a cryptographic transformation of data using the GOST 28147-89 algorithm. This class cannot be inherited.</summary>
    [SecurityCritical, ComVisible(true)]
    public sealed class GOST28147Transform : ICryptoTransform {
        private const uint C1 = 0x1010101;
        private const uint C2 = 0x1010104;

        private byte[] _ivValue;
        private uint[] _keyValue;
        private byte[,] _sBlocksValue;
        private readonly bool _canUseMAC;
        private readonly int _blockSizeValue;
        private readonly int _feedBackSizeValue;
        private readonly CipherMode _modeValue;
        private readonly PaddingMode _paddingValue;
        private readonly TransformMode _encryptOrDecrypt;

        /// <summary>Gets the input block size.</summary>
        /// <returns>The input block size in bytes.</returns>
        public int InputBlockSize { get; private set; }

        /// <summary>Gets the output block size.</summary>
        /// <returns>The output block size in bytes.</returns>
        public int OutputBlockSize { get; private set; }

        /// <summary>Gets a value indicating whether multiple blocks can be transformed.</summary>
        /// <returns>true if multiple blocks can be transformed; otherwise, false.</returns>
        public bool CanTransformMultipleBlocks { get; private set; }

        /// <summary>Gets a value indicating whether the current transform can be reused.</summary>
        /// <returns>Always true.</returns>
        public bool CanReuseTransform { get; private set; }

        internal GOST28147Transform(byte[] rgbKey, CipherMode cipherChainingMode, byte[] rgbIV, int blockSize, byte[] rgbSBlocks, int feedbackSize, PaddingMode padding, bool useMAC, TransformMode encryption) {
            _modeValue = cipherChainingMode;
            _blockSizeValue = blockSize >> 3;
            _feedBackSizeValue = feedbackSize >> 3;
            _paddingValue = padding;
            _canUseMAC = useMAC;

            InputBlockSize = _blockSizeValue;
            OutputBlockSize = _blockSizeValue;

            _encryptOrDecrypt = encryption;

            CanReuseTransform = true;
            CanTransformMultipleBlocks = cipherChainingMode != CipherMode.CFB;

            _keyValue = new uint[8];

            for (int i = 0, j = 0; i < _keyValue.Length; i++, j += 4)
                _keyValue[i] = rgbKey[j] | (uint)rgbKey[j + 1] << 8 | (uint)rgbKey[j + 2] << 16 | (uint)rgbKey[j + 3] << 24;

            _sBlocksValue = new byte[4, 256];

            for (var i = 0; i < _sBlocksValue.GetLength(1); i++) {
                var leftOffset = i >> 4;
                var rightOffset = i & 15;

                _sBlocksValue[0, i] = (byte)(rgbSBlocks[16 + leftOffset] << 4 | rgbSBlocks[rightOffset]);
                _sBlocksValue[1, i] = (byte)(rgbSBlocks[48 + leftOffset] << 4 | rgbSBlocks[32 + rightOffset]);
                _sBlocksValue[2, i] = (byte)(rgbSBlocks[80 + leftOffset] << 4 | rgbSBlocks[64 + rightOffset]);
                _sBlocksValue[3, i] = (byte)(rgbSBlocks[112 + leftOffset] << 4 | rgbSBlocks[96 + rightOffset]);
            }

            _ivValue = new byte[_feedBackSizeValue];

            ECBEncrypt(ref rgbIV, ref _ivValue);
        }

        /// <summary>Releases all resources used by the current instance of the <see cref="T:Standards.Security.Cryptography.GOST28147Transform" /> class.</summary>
        public void Dispose() {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        [SecurityCritical]
        private void Dispose(bool disposing) {
            if (!disposing) return;

            if (_keyValue != null) {
                Array.Clear(_keyValue, 0, _keyValue.Length);
                _keyValue = null;
            }

            if (_ivValue != null) {
                Array.Clear(_ivValue, 0, _ivValue.Length);
                _ivValue = null;
            }

            if (_sBlocksValue != null) {
                Array.Clear(_sBlocksValue, 0, _sBlocksValue.Length);
                _sBlocksValue = null;
            }
        }

        /// <summary>Computes the transformation for the specified region of the input byte array and copies the resulting transformation to the specified region of the output byte array.</summary>
        /// <returns>The number of bytes written.</returns>
        /// <param name="inputBuffer">The input on which to perform the operation on. </param>
        /// <param name="inputOffset">The offset into the input byte array from which to begin using data from. </param>
        /// <param name="inputCount">The number of bytes in the input byte array to use as data. </param>
        /// <param name="outputBuffer">The output to which to write the data to. </param>
        /// <param name="outputOffset">The offset into the output byte array from which to begin writing data from. </param>
        /// <exception cref="T:System.ArgumentNullException">The <paramref name="inputBuffer" /> parameter is null.-or- The <paramref name="outputBuffer" /> parameter is null. </exception>
        /// <exception cref="T:System.ArgumentException">The length of the input buffer is less than the sum of the input offset and the input count. </exception>
        /// <exception cref="T:System.ArgumentOutOfRangeException">The <paramref name="inputOffset" /> is out of range. This parameter requires a non-negative number.</exception>
        [SecuritySafeCritical]
        public int TransformBlock(byte[] inputBuffer, int inputOffset, int inputCount, byte[] outputBuffer, int outputOffset) {
            if (inputBuffer == null)
                throw new ArgumentNullException("inputBuffer");

            if (outputBuffer == null)
                throw new ArgumentNullException("outputBuffer");
            if (inputOffset < 0)
                throw new ArgumentOutOfRangeException("inputOffset", "Need non negative number");

            if (inputCount <= 0 || inputCount % InputBlockSize != 0 || inputCount > inputBuffer.Length)
                throw new ArgumentException("Invalid value");

            if (inputBuffer.Length - inputCount < inputOffset)
                throw new ArgumentException("Invalid offlenght");

            return _encryptOrDecrypt == TransformMode.Encrypt
                       ? EncryptData(inputBuffer, inputOffset, inputCount, outputBuffer, outputOffset)
                       : DecryptData(inputBuffer, inputOffset, inputCount, outputBuffer, outputOffset);
        }

        /// <summary>Computes the transformation for the specified region of the specified byte array.</summary>
        /// <returns>The computed transformation.</returns>
        /// <param name="inputBuffer">The input on which to perform the operation on. </param>
        /// <param name="inputOffset">The offset into the byte array from which to begin using data from. </param>
        /// <param name="inputCount">The number of bytes in the byte array to use as data. </param>
        /// <exception cref="T:System.ArgumentNullException">The <paramref name="inputBuffer" /> parameter is null. </exception>
        /// <exception cref="T:System.ArgumentException">The <paramref name="inputOffset" /> parameter is less than zero.-or- The <paramref name="inputCount" /> parameter is less than zero.-or- The length of the input buffer is less than the sum of the input offset and the input count. </exception>
        /// <exception cref="T:System.Security.Cryptography.CryptographicException">The <see cref="F:System.Security.Cryptography.PaddingMode.PKCS7" /> padding is invalid. </exception>
        /// <exception cref="T:System.ArgumentOutOfRangeException">The <paramref name="inputOffset" /> parameter is out of range. This parameter requires a non-negative number.</exception>
        [SecuritySafeCritical]
        public byte[] TransformFinalBlock(byte[] inputBuffer, int inputOffset, int inputCount) {
            if (inputBuffer == null)
                throw new ArgumentNullException("inputBuffer");
            
            if (inputOffset < 0)
                throw new ArgumentOutOfRangeException("inputOffset", "Need non-negative number");
            
            if (inputCount < 0 || inputCount > inputBuffer.Length)
                throw new ArgumentException("Invalid value");

            if (inputBuffer.Length - inputCount < inputOffset)
                throw new ArgumentException("Invalid lenght");

            if (inputCount == 0 && !_canUseMAC)
                return new byte[0];

            return _encryptOrDecrypt == TransformMode.Encrypt
                       ? FinalEncryptData(inputBuffer, inputOffset, inputCount)
                       : FinalDecryptData(inputBuffer, inputOffset, inputCount);
        }

        [SecurityCritical]
        private int EncryptData(byte[] inputBuffer, int inputOffset, int inputCount, byte[] outputBuffer, int outputOffset) {
            var total = 0;
            var lenght = inputCount / _blockSizeValue;

            var inputBlock = new byte[InputBlockSize];
            var outputBlock = new byte[OutputBlockSize];

            for (var i = 0; i < lenght; i++) {
                Buffer.BlockCopy(inputBuffer, inputOffset, inputBlock, 0, InputBlockSize);

                switch (_modeValue)
                {
                    case CipherMode.ECB:
                        ECBEncrypt(ref inputBlock, ref outputBlock);
                        break;
                    case CipherMode.OFB:
                        OFBEncrypt(ref inputBlock, ref outputBlock);
                        break;
                    case CipherMode.CFB:
                        CFBEncrypt(ref inputBlock, ref outputBlock);
                        break;
                    default:
                        throw new CryptographicException("CipherMode isn't supported by the algorithm!");
                }

                Buffer.BlockCopy(outputBlock, 0, outputBuffer, outputOffset, OutputBlockSize);

                inputOffset += _blockSizeValue;
                outputOffset += _blockSizeValue;
                total += _blockSizeValue;
            }

            return total;
        }

        [SecurityCritical]
        private byte[] FinalEncryptData(byte[] inputBuffer, int inputOffset, int inputCount) {
            if (_modeValue == CipherMode.ECB && _paddingValue == PaddingMode.None)
                return new byte[0];

            var outputBuffer = new byte[inputCount];
            var outputBlock = new byte[_blockSizeValue];

            switch (_modeValue) {
                case CipherMode.ECB:
                    ECBEncrypt(ref inputBuffer, ref outputBlock);
                    break;
                case CipherMode.OFB:
                    OFBEncrypt(ref inputBuffer, ref outputBlock);
                    break;
                case CipherMode.CFB:
                    CFBEncrypt(ref inputBuffer, ref outputBlock);
                    break;
                default:
                    throw new CryptographicException("CipherMode isn't supported by the algorithm!");
            }

            Buffer.BlockCopy(outputBlock, inputOffset, outputBuffer, inputOffset, inputCount);

            return outputBuffer;
        }

        [SecurityCritical]
        private void ECBEncrypt(ref byte[] inputBuffer, ref byte[] outputBuffer) {
            var n1 = inputBuffer[0] | ((uint)inputBuffer[1] << 8) | ((uint)inputBuffer[2] << 16) | ((uint)inputBuffer[3] << 24);
            var n2 = inputBuffer[4] | ((uint)inputBuffer[5] << 8) | ((uint)inputBuffer[6] << 16) | ((uint)inputBuffer[7] << 24);

            unchecked {
                n2 ^= CipherFunction(n1 + _keyValue[0]);
                n1 ^= CipherFunction(n2 + _keyValue[1]);
                n2 ^= CipherFunction(n1 + _keyValue[2]);
                n1 ^= CipherFunction(n2 + _keyValue[3]);
                n2 ^= CipherFunction(n1 + _keyValue[4]);
                n1 ^= CipherFunction(n2 + _keyValue[5]);
                n2 ^= CipherFunction(n1 + _keyValue[6]);
                n1 ^= CipherFunction(n2 + _keyValue[7]);

                n2 ^= CipherFunction(n1 + _keyValue[0]);
                n1 ^= CipherFunction(n2 + _keyValue[1]);
                n2 ^= CipherFunction(n1 + _keyValue[2]);
                n1 ^= CipherFunction(n2 + _keyValue[3]);
                n2 ^= CipherFunction(n1 + _keyValue[4]);
                n1 ^= CipherFunction(n2 + _keyValue[5]);
                n2 ^= CipherFunction(n1 + _keyValue[6]);
                n1 ^= CipherFunction(n2 + _keyValue[7]);

                n2 ^= CipherFunction(n1 + _keyValue[0]);
                n1 ^= CipherFunction(n2 + _keyValue[1]);
                n2 ^= CipherFunction(n1 + _keyValue[2]);
                n1 ^= CipherFunction(n2 + _keyValue[3]);
                n2 ^= CipherFunction(n1 + _keyValue[4]);
                n1 ^= CipherFunction(n2 + _keyValue[5]);
                n2 ^= CipherFunction(n1 + _keyValue[6]);
                n1 ^= CipherFunction(n2 + _keyValue[7]);

                n2 ^= CipherFunction(n1 + _keyValue[7]);
                n1 ^= CipherFunction(n2 + _keyValue[6]);
                n2 ^= CipherFunction(n1 + _keyValue[5]);
                n1 ^= CipherFunction(n2 + _keyValue[4]);
                n2 ^= CipherFunction(n1 + _keyValue[3]);
                n1 ^= CipherFunction(n2 + _keyValue[2]);
                n2 ^= CipherFunction(n1 + _keyValue[1]);
                n1 ^= CipherFunction(n2 + _keyValue[0]);

                outputBuffer[0] = (byte)n2;
                outputBuffer[1] = (byte)(n2 >> 8);
                outputBuffer[2] = (byte)(n2 >> 16);
                outputBuffer[3] = (byte)(n2 >> 24);

                outputBuffer[4] = (byte)n1;
                outputBuffer[5] = (byte)(n1 >> 8);
                outputBuffer[6] = (byte)(n1 >> 16);
                outputBuffer[7] = (byte)(n1 >> 24);
            }
        }

        [SecurityCritical]
        private void OFBEncrypt(ref byte[] inputBuffer, ref byte[] outputBuffer) {
            var buff = new byte[_feedBackSizeValue];

            var s0 = _ivValue[0] | ((uint)_ivValue[1] << 8) | ((uint)_ivValue[2] << 16) | ((uint)_ivValue[3] << 24);
            var s1 = _ivValue[4] | ((uint)_ivValue[5] << 8) | ((uint)_ivValue[6] << 16) | ((uint)_ivValue[7] << 24);

            unchecked {
                s0 = (uint) ((s0 + C1) % 0x100000000);
                s1 = (s1 + C2 - 1) % 0xFFFFFFFF + 1;

                _ivValue[0] = (byte) s0;
                _ivValue[1] = (byte) (s0 >> 8);
                _ivValue[2] = (byte) (s0 >> 16);
                _ivValue[3] = (byte) (s0 >> 24);

                _ivValue[4] = (byte) s1;
                _ivValue[5] = (byte) (s1 >> 8);
                _ivValue[6] = (byte) (s1 >> 16);
                _ivValue[7] = (byte) (s1 >> 24);
            }

            ECBEncrypt(ref _ivValue, ref buff);

            for (var i = 0; i < _blockSizeValue; i++)
                outputBuffer[i] = (byte) (inputBuffer[i] ^ buff[i]);
        }

        [SecurityCritical]
        private void CFBEncrypt(ref byte[] inputBuffer, ref byte[] outputBuffer) {
            for (var i = 0; i < _blockSizeValue; i++)
                outputBuffer[i] = (byte)(inputBuffer[i] ^ _ivValue[i]);
            
            ECBEncrypt(ref outputBuffer, ref _ivValue);
        }

        [SecuritySafeCritical]
        private int DecryptData(byte[] inputBuffer, int inputOffset, int inputCount, byte[] outputBuffer, int outputOffset) {
            var total = 0;

            var lenght = inputCount / _blockSizeValue;

            var inputBlock = new byte[InputBlockSize];
            var outputBlock = new byte[OutputBlockSize];

            for (var i = 0; i < lenght; i++)
                {
                    Buffer.BlockCopy(inputBuffer, inputOffset, inputBlock, 0, InputBlockSize);

                    switch (_modeValue)
                    {
                        case CipherMode.ECB:
                            ECBDecrypt(ref inputBlock, ref outputBlock);
                            break;
                        case CipherMode.OFB:
                            OFBEncrypt(ref inputBlock, ref outputBlock);
                            break;
                        case CipherMode.CFB:
                            CFBDecrypt(ref inputBlock, ref outputBlock);
                            break;
                        default:
                            throw new CryptographicException("Cipher Mode isn't supported by the algorithm!");
                    }

                    Buffer.BlockCopy(outputBlock, 0, outputBuffer, outputOffset, OutputBlockSize);

                    inputOffset += _blockSizeValue;
                    outputOffset += _blockSizeValue;
                    total += _blockSizeValue;
                }

            return total;
        }

        [SecurityCritical]
        private byte[] FinalDecryptData(byte[] inputBuffer, int inputOffset, int inputCount) {
            if (_modeValue == CipherMode.ECB && _paddingValue == PaddingMode.None)
                return new byte[0];

            var outputBuffer = new byte[inputCount];
            var outputBlock = new byte[_blockSizeValue];

            switch (_modeValue)
            {
                case CipherMode.ECB:
                    ECBDecrypt(ref inputBuffer, ref outputBlock);
                    break;
                case CipherMode.OFB:
                    OFBEncrypt(ref inputBuffer, ref outputBlock);
                    break;
                case CipherMode.CFB:
                    CFBDecrypt(ref inputBuffer, ref outputBlock);
                    break;
                default:
                    throw new CryptographicException("CipherMode isn't supported by the algorithm!");
            }

            Buffer.BlockCopy(outputBlock, inputOffset, outputBuffer, inputOffset, inputCount);

            return outputBuffer;
        }

        [SecurityCritical]
        private void ECBDecrypt(ref byte[] inputBuffer, ref byte[] outputBuffer) {
            var n1 = inputBuffer[0] | ((uint)inputBuffer[1] << 8) | ((uint)inputBuffer[2] << 16) | ((uint)inputBuffer[3] << 24);
            var n2 = inputBuffer[4] | ((uint)inputBuffer[5] << 8) | ((uint)inputBuffer[6] << 16) | ((uint)inputBuffer[7] << 24);

            unchecked {
                n2 ^= CipherFunction(n1 + _keyValue[0]);
                n1 ^= CipherFunction(n2 + _keyValue[1]);
                n2 ^= CipherFunction(n1 + _keyValue[2]);
                n1 ^= CipherFunction(n2 + _keyValue[3]);
                n2 ^= CipherFunction(n1 + _keyValue[4]);
                n1 ^= CipherFunction(n2 + _keyValue[5]);
                n2 ^= CipherFunction(n1 + _keyValue[6]);
                n1 ^= CipherFunction(n2 + _keyValue[7]);

                n2 ^= CipherFunction(n1 + _keyValue[7]);
                n1 ^= CipherFunction(n2 + _keyValue[6]);
                n2 ^= CipherFunction(n1 + _keyValue[5]);
                n1 ^= CipherFunction(n2 + _keyValue[4]);
                n2 ^= CipherFunction(n1 + _keyValue[3]);
                n1 ^= CipherFunction(n2 + _keyValue[2]);
                n2 ^= CipherFunction(n1 + _keyValue[1]);
                n1 ^= CipherFunction(n2 + _keyValue[0]);

                n2 ^= CipherFunction(n1 + _keyValue[7]);
                n1 ^= CipherFunction(n2 + _keyValue[6]);
                n2 ^= CipherFunction(n1 + _keyValue[5]);
                n1 ^= CipherFunction(n2 + _keyValue[4]);
                n2 ^= CipherFunction(n1 + _keyValue[3]);
                n1 ^= CipherFunction(n2 + _keyValue[2]);
                n2 ^= CipherFunction(n1 + _keyValue[1]);
                n1 ^= CipherFunction(n2 + _keyValue[0]);

                n2 ^= CipherFunction(n1 + _keyValue[7]);
                n1 ^= CipherFunction(n2 + _keyValue[6]);
                n2 ^= CipherFunction(n1 + _keyValue[5]);
                n1 ^= CipherFunction(n2 + _keyValue[4]);
                n2 ^= CipherFunction(n1 + _keyValue[3]);
                n1 ^= CipherFunction(n2 + _keyValue[2]);
                n2 ^= CipherFunction(n1 + _keyValue[1]);
                n1 ^= CipherFunction(n2 + _keyValue[0]);

                outputBuffer[0] = (byte) n2;
                outputBuffer[1] = (byte) (n2 >> 8);
                outputBuffer[2] = (byte) (n2 >> 16);
                outputBuffer[3] = (byte) (n2 >> 24);

                outputBuffer[4] = (byte) n1;
                outputBuffer[5] = (byte) (n1 >> 8);
                outputBuffer[6] = (byte) (n1 >> 16);
                outputBuffer[7] = (byte) (n1 >> 24);
            }
        }

        [SecurityCritical]
        private void CFBDecrypt(ref byte[] inputBuffer, ref byte[] outputBuffer) {
            for (var i = 0; i < _blockSizeValue; i++)
                outputBuffer[i] = (byte)(inputBuffer[i] ^ _ivValue[i]);

            ECBEncrypt(ref inputBuffer, ref _ivValue);
        }

        private uint CipherFunction(uint x) {
            x = (uint)_sBlocksValue[3, x >> 24 & 255] << 24 | (uint)_sBlocksValue[2, x >> 16 & 255] << 16 |
                (uint)_sBlocksValue[1, x >> 8 & 255] << 8 | _sBlocksValue[0, x & 255];

            //Circular shift
            return x << 11 | x >> (32 - 11);
        }

        /// <summary>Destructor</summary>
        ~GOST28147Transform() {
            Dispose(false);
        }
    }
}
