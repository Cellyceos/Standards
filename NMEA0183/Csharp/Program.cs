//
//  Program.cs
//  NMEA 0183 - Standard For Interfacing Marine Electronic Devices
//
//  Created by Kirill Bravichev on 26/05/2015.
//  Copyright (c) 2015 Cellyceos. All rights reserved.
//

using Standards.Nmea.Sentences;

using System;
using System.Reflection;

namespace Standards {
    class Program {
        [STAThread]
        static void Main(string[] args) {
            string[] messages = { 
                "$GPZDA,154026,16,12,2011,00,00*4A",
                "$GPVTG,118.1,T,,M,00.00,N,00.00,K,A*04",
                "$GPGGA,154026,5957.5435,N,03020.9513,E,1,12,1.0,21.8,M,,,,*2A",
                "$GPRMC,154028,A,5957.5435,N,03020.9513,E,00.0,118.070,161211,,,A*47"
            };

            foreach (var message in messages) {
                Console.WriteLine("Message: {0}", message);
                var sentense = Sentence.Parse(message);

                if (sentense == null)
                    continue;

                var sentenseType = sentense.GetType();

                Console.WriteLine("Fullname: {0}", sentenseType.FullName);

                foreach (var property in sentenseType.GetProperties()) {
                    Console.WriteLine("Field {0} = {1}", property.Name, property.GetValue(sentense));
                }
            }

            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }
    }
}
