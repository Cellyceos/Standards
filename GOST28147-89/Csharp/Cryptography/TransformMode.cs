//
//  TransformMode.cs
//  GOST28147-89
//
//  Created by Kirill Bravichev on 20/03/2013.
//  Copyright (c) 2013 Cellyceos. All rights reserved.
//

using System;

namespace Standards.Security.Cryptography {
    [Serializable]
    internal enum TransformMode {
        Encrypt,
        Decrypt
    }
}
