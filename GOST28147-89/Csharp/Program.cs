//
//  Program.cs
//  GOST28147-89
//
//  Created by Kirill Bravichev on 20/03/2013.
//  Copyright (c) 2013 Cellyceos. All rights reserved.
//

using Standards.Security.Cryptography;

using System;
using System.Diagnostics;
using System.IO;
using System.Security.Cryptography;

/**
 * CipherMode.ECB (Electronic Codebook) – метод простой замены,
 * CipherMode.CFB (Cipher Feedback) – метод гаммирования с обратной связью,
 * CipherMode.OFB (Output Feedback) – метод гаммирования.
 */

namespace Standards {
    class Program {
        static int fileSize = 280;
        static string fileName = "test";

        [STAThread]
        static void Main() {
            var rnd = RandomNumberGenerator.Create();

            var data = new byte[fileSize];
            rnd.GetBytes(data);

            var gost = GOST28147.Create();
            gost.Mode = CipherMode.ECB;
            gost.IV = new byte[8];

            using (var br = new BinaryReader(File.Open(@"Resources\SAMPLE.KEY", FileMode.Open)))
                gost.Key = br.ReadBytes((int)br.BaseStream.Length);

            using (var br = new BinaryReader(File.Open(@"Resources\SAMPLE.CHT", FileMode.Open)))
                gost.SBlocks = br.ReadBytes((int)br.BaseStream.Length);

            Console.Write("Creating {0}.bin file", fileName);

            using (var bw = new BinaryWriter(File.Create(string.Format("{0}.bin", fileName))))
                bw.Write(data, 0, data.Length);

            Console.WriteLine(" -- created with size {0}", fileSize);

            Console.WriteLine("Performance testing");

            Console.WriteLine("Simple replacement method");
            gost.Mode = CipherMode.ECB;
            Test(ref gost, ref data);

            Console.WriteLine("XOR encryption method");
            gost.Mode = CipherMode.OFB;
            Test(ref gost, ref data);

            Console.WriteLine("XOR encryption method with feedback");
            gost.Mode = CipherMode.CFB;
            Test(ref gost, ref data);

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }

        static void Test(ref GOST28147 gost, ref byte[] data) {
            var time = Stopwatch.StartNew();

            using (var bw = new BinaryWriter(File.Create(string.Format("{0}enc_{1}.bin", fileName, gost.Mode))))
                using (var encryptor = new CryptoStream(bw.BaseStream, gost.CreateEncryptor(), CryptoStreamMode.Write))
                    encryptor.Write(data, 0, data.Length);

            time.Stop();
            
            Console.WriteLine("Mode: {0}; Encryption time elapsed: {1}",gost.Mode, time.Elapsed);

            using (var br = new BinaryReader(File.Open(string.Format("{0}enc_{1}.bin", fileName, gost.Mode), FileMode.Open)))
                data = br.ReadBytes((int) br.BaseStream.Length);

            time.Restart();

            using (var bw = new BinaryWriter(File.Create(string.Format("{0}dec_{1}.bin", fileName, gost.Mode))))
                using (var encryptor = new CryptoStream(bw.BaseStream, gost.CreateDecryptor(), CryptoStreamMode.Write))
                    encryptor.Write(data, 0, data.Length);

            time.Stop();

            Console.WriteLine("Mode: {0}; Decryption time elapsed: {1}", gost.Mode, time.Elapsed);
        }
    }
}