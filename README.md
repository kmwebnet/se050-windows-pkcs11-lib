# NXP SE050 Secure Element DLL for Windows 11

This repository creates a dynamic link library (DLL) to enable the utilization of the NXP SE050 Secure Element on Windows 11. The SE050 is interfaced using the FTDI FT260 to convert it to a USB accessible device. The communication with the SE050 follows the PKCS#11 standard.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [License](#license)
- [Disclaimer](#disclaimer)

## Features

- Seamless integration with Windows 11.
- USB conversion using FTDI FT260.
- PKCS#11 standard compliant communication.

## Installation

1. **Prerequisites**:
    - Windows 11 operating system.
    - Visual Studio 2022.
    - place Plug ant Trust Middleware 04.03.01 from official NXP website as unzipped simw-top folder.
    - NXP SE050C1 with FTDI FT260 USB-HID converter.

2. **Compiling**:
    - Download the latest release from the [releases page](#).
    - Unzip and place the Plug and Trust MIddleware in this directory as simw-top folder.
    - Build on Visual Studio 2022. upon success, compiled DLL named se050key.dll will be generated on build\x64.
    
## Usage

- Make sure se050key.dll and LibFT260.dll are in the same directory as the executable.
- If you can't make se050key.dll loading, adjust LibFT260.dll location to be able to load. 
- It can load if you place LibFT260.dll the location as same as the applications directory in most of cases.

- Tested applications: 
  [PKCS11Admin](https://www.pkcs11admin.net)
  [PuTTY CAC](https://github.com/NoMoreFood/putty-cac/releases)

# OpenSC and OpenSSL sample
- Install [OpenSC for windows](github.com/OpenSC/OpenSC/releases/tag/0.23.0) 
- Install [OpenSSL 3](https://slproweb.com/products/Win32OpenSSL.html) not Light package

- Refer about pre setting key and cert: [SE050 Configurations](https://www.nxp.jp/docs/en/application-note/AN12436.pdf)

```powershell

Add 'C:\Program Files\OpenSC Project\OpenSC\tools\' PATH to system environment variable.

PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --list-slots
Available slots:
Slot 0 (0x1):
  token label        : SSS_PKCS11
  token manufacturer : NXP
  token model        :
  token flags        : rng, token initialized
  hardware version   : 3.1
  firmware version   : 4.3
  serial num         :
  pin min/max        : 0/10

# read cert from Default Connectivity Key
PS C:\Users\km> cmd /c "pkcs11-tool.exe --module <your path>se050key.dll --id 010000F0 --read-object --type cert --slot 1 | openssl x509 -inform DER"
-----BEGIN CERTIFICATE-----
MIIB0DCCAXegAwIBAgIUBABQAWhzzrtzuLQEfo8KlGaAAAAwCgYIKoZIzj0EAwIw
VjEXMBUGA1UECwwOUGx1ZyBhbmQgVHJ1c3QxDDAKBgNVBAoMA05YUDEtMCsGA1UE
...
CgYIKoZIzj0EAwIDRwAwRAIgX9lOj5B6/UP+PhNnYAGy9WxLMjFxhnFEBdj0Uhpc
cDcCIDvYe29cY3LiFoq3AfUmxjMviE/vam+oCWxE0UuFUGsm
-----END CERTIFICATE-----

# EC keypair generation and signing/verifying
PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --keypairgen --id 05000000 --key-type EC:secp256r1
Using slot 0 with a present token (0x1)
Key pair generated:
Private Key Object; EC
  label:      sss:05000000
  ID:         05000000
  Usage:      sign, derive
  Access:     sensitive, always sensitive
  Allowed mechanisms: ECDSA,ECDSA-SHA1
warning: PKCS11 function C_GetAttributeValue(UNIQUE_ID) failed: rv = CKR_ATTRIBUTE_SENSITIVE (0x11)

Public Key Object; EC  EC_POINT 256 bits
  EC_POINT:   044104d1f163da5dc0e7a9aeb7e7dfc86e8b2ffdee8ca1f6a9b8ea6b611ab85b19cf79ee1e42f907fb8d3036daa68bc8377cd2d8c5b4583c28d4535a37407ace02958e
  EC_PARAMS:  06082a8648ce3d030107
  label:      sss:05000000
  ID:         05000000
  Usage:      derive
  Access:     none
warning: PKCS11 function C_GetAttributeValue(UNIQUE_ID) failed: rv = CKR_ATTRIBUTE_SENSITIVE (0x11)

PS C:\Users\km> echo 123456789abcdefghijklmnopqrstu > .\testhash.dat # 32byte test data

PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --id 05000000 --sign --mechanism ECDSA -i .\testhash.dat -o test.sig

PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --id 05000000 --verify --mechanism ECDSA -i .\testhash.dat --signature-file test.sig
Using slot 0 with a present token (0x1)
Using signature algorithm ECDSA
Signature is valid

# Genarate AES key and encrypt/decrypt test

PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --keygen --key-type AES:256 --label "sss:05000002"
Using slot 0 with a present token (0x1)
Key generated:
Secret Key Object; AESwarning: PKCS11 function C_GetAttributeValue(VALUE_LEN) failed: rv = CKR_BUFFER_TOO_SMALL (0x150)

PKCS11:WARN :Not allowed to readout Symmetric key value
warning: PKCS11 function C_GetAttributeValue(VALUE) failed: rv = CKR_ATTRIBUTE_SENSITIVE (0x11)

  label:      sss:05000002
  ID:         05000002
  Usage:      encrypt, decrypt
  Access:     sensitive, always sensitive
PKCS11:WARN :Attribute required : 0x00000004

warning: PKCS11 function C_GetAttributeValue(UNIQUE_ID) failed: rv = CKR_ATTRIBUTE_SENSITIVE (0x11)

PS C:\Users\km> echo test > test.txt

PS C:\Users\km> cat .\test.txt
test

PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --id 05000002 --encrypt --mechanism AES-CBC --input-file test.txt --output-file test.sec
Using slot 0 with a present token (0x1)
Using encrypt algorithm AES-CBC

PS C:\Users\km> cat .\test.sec
�B[���.�����Yu

PS C:\Users\km> pkcs11-tool.exe --module <your path>se050key.dll --id 05000002 --decrypt --mechanism AES-CBC --input-file test.sec --output-file test.res
Using slot 0 with a present token (0x1)
Using decrypt algorithm AES-CBC

PS C:\Users\km> cat .\test.res
test

```

## License
This project is licensed under the Apache 2.0 License unless stated otherwise.

## Disclaimer
This project is not affiliated with NXP in any way.
This software is for verification purposes only. 
The authors or maintainers are not responsible for any damages or losses incurred from its use in commercial or critical systems.
