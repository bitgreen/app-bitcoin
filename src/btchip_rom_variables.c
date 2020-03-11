/*******************************************************************************
*   Ledger App - Bitcoin Wallet
*   (c) 2016-2019 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "btchip_internal.h"
#include "btchip_apdu_constants.h"

unsigned char const HEXDIGITS[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

unsigned char const BASE58TABLE[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x9,  0xa,  0xb,  0xc,  0xd,  0xe,  0xf,
    0x10, 0xff, 0x11, 0x12, 0x13, 0x14, 0x15, 0xff, 0x16, 0x17, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0xff, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0xff, 0xff, 0xff, 0xff, 0xff};

unsigned char const BASE58ALPHABET[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

unsigned char const SIGNMAGIC[] = {' ', 'S', 'i', 'g', 'n', 'e', 'd', ' ', 'M',
                                   'e', 's', 's', 'a', 'g', 'e', ':', '\n'};

unsigned char const TWOPOWER[] = {0x01, 0x02, 0x04, 0x08,
                                  0x10, 0x20, 0x40, 0x80};

unsigned char const OVERWINTER_PARAM_PREVOUTS[16] = { 'Z', 'c', 'a', 's', 'h', 'P', 'r', 'e', 'v', 'o', 'u', 't', 'H', 'a', 's', 'h' };
unsigned char const OVERWINTER_PARAM_SEQUENCE[16] = { 'Z', 'c', 'a', 's', 'h', 'S', 'e', 'q', 'u', 'e', 'n', 'c', 'H', 'a', 's', 'h' };
unsigned char const OVERWINTER_PARAM_OUTPUTS[16] = { 'Z', 'c', 'a', 's', 'h', 'O', 'u', 't', 'p', 'u', 't', 's', 'H', 'a', 's', 'h' };
unsigned char const OVERWINTER_PARAM_SIGHASH[16] = { 'Z', 'c', 'a', 's', 'h', 'S', 'i', 'g', 'H', 'a', 's', 'h', 0, 0, 0, 0 };
unsigned char const OVERWINTER_NO_JOINSPLITS[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

#ifdef HAVE_LIQUID

unsigned char const SYMMETRIC_KEY_SEED[18] = { 'S', 'y', 'm', 'm', 'e', 't', 'r', 'i', 'c', ' ', 'k', 'e', 'y', ' ', 's', 'e', 'e', 'd' };
unsigned char const SLIP77_LABEL[10] = { 0x00, 'S', 'L', 'I', 'P', '-', '0', '0', '7', '7', 0, 0 };

#endif

unsigned char const DISPATCHER_CLA[] = {
    BTCHIP_CLA, // btchip_apdu_setup,
    BTCHIP_CLA, // btchip_apdu_verify_pin,
    BTCHIP_CLA, // btchip_apdu_get_operation_mode,
    BTCHIP_CLA, // btchip_apdu_set_operation_mode,
    BTCHIP_CLA, // btchip_apdu_get_wallet_public_key,
    BTCHIP_CLA, // btchip_apdu_get_trusted_input,
    BTCHIP_CLA, // btchip_apdu_hash_input_start,
    BTCHIP_CLA, // btchip_apdu_hash_sign,
    BTCHIP_CLA, // btchip_apdu_hash_input_finalize_full,
    BTCHIP_CLA, // btchip_apdu_sign_message,
    BTCHIP_CLA, // btchip_apdu_get_random,
    BTCHIP_CLA, // btchip_apdu_get_firmware_version,
    BTCHIP_CLA, // btchip_apdu_set_alternate_coin_version
    BTCHIP_CLA, // btchip_apdu_get_coin_version
    BTCHIP_CLA, // btchip_apdu_liquid_get_commitments
    BTCHIP_CLA, // btchip_apdu_liquid_get_public_blinding_key
    BTCHIP_CLA, // btchip_apdu_liquid_get_nonce
    BTCHIP_CLA, // btchip_apdu_liquid_provide_issuance_information
    BTCHIP_CLA, // btchip_apdu_liquid_get_blinding_factor
    BTCHIP_CLA, // btchip_apdu_liquid_get_green_wallet_address
    BTCHIP_ADM_CLA // btchip_apdu_liquid_headless_setup
};

unsigned char const DISPATCHER_INS[] = {
    BTCHIP_INS_SETUP,                    // btchip_apdu_setup,
    BTCHIP_INS_VERIFY_PIN,               // btchip_apdu_verify_pin,
    BTCHIP_INS_GET_OPERATION_MODE,       // btchip_apdu_get_operation_mode,
    BTCHIP_INS_SET_OPERATION_MODE,       // btchip_apdu_set_operation_mode,
    BTCHIP_INS_GET_WALLET_PUBLIC_KEY,    // btchip_apdu_get_wallet_public_key,
    BTCHIP_INS_GET_TRUSTED_INPUT,        // btchip_apdu_get_trusted_input,
    BTCHIP_INS_HASH_INPUT_START,         // btchip_apdu_hash_input_start,
    BTCHIP_INS_HASH_SIGN,                // btchip_apdu_hash_sign,
    BTCHIP_INS_HASH_INPUT_FINALIZE_FULL, // btchip_apdu_hash_input_finalize_full,
    BTCHIP_INS_SIGN_MESSAGE,             // btchip_apdu_sign_message,
    BTCHIP_INS_GET_RANDOM,               // btchip_apdu_get_random,
    BTCHIP_INS_GET_FIRMWARE_VERSION,     // btchip_apdu_get_firmware_version,
    BTCHIP_INS_SET_ALTERNATE_COIN_VER, // btchip_apdu_set_alternate_coin_version
    BTCHIP_INS_GET_COIN_VER,           // btchip_apdu_get_coin_version
    BTCHIP_INS_LIQUID_GET_COMMITMENTS, // btchip_apdu_liquid_get_commitments
    BTCHIP_INS_LIQUID_GET_PUBLIC_BLINDING_KEY, // btchip_apdu_liquid_get_public_blinding_key
    BTCHIP_INS_LIQUID_GET_NONCE, // btchip_apdu_liquid_get_nonce
    BTCHIP_INS_LIQUID_PROVIDE_ISSUANCE_INFORMATION, // btchip_apdu_liquid_provide_issuance_information
    BTCHIP_INS_LIQUID_GET_BLINDING_FACTOR, // btchip_apdu_liquid_get_blinding_factor
    BTCHIP_INS_LIQUID_GET_GREEN_WALLET_ADDRESS, // btchip_apdu_liquid_get_green_wallet_address
    BTCHIP_INS_LIQUID_HEADLESS_SETUP // btchip_apdu_liquid_headless_setup
};

unsigned char const DISPATCHER_DATA_IN[] = {
    1, // btchip_apdu_setup,
    1, // btchip_apdu_verify_pin,
    0, // btchip_apdu_get_operation_mode,
    1, // btchip_apdu_set_operation_mode,
    1, // btchip_apdu_get_wallet_public_key,
    1, // btchip_apdu_get_trusted_input,
    1, // btchip_apdu_hash_input_start,
    1, // btchip_apdu_hash_sign,
    1, // btchip_apdu_hash_input_finalize_full,
    1, // btchip_apdu_sign_message,
    0, // btchip_apdu_get_random,
    0, // btchip_apdu_get_firmware_version,
    1, // btchip_apdu_set_alternate_coin_version
    0, // btchip_apdu_get_coin_version,
    1, // btchip_apdu_liquid_get_commitments
    1, // btchip_apdu_liquid_get_public_blinding_key
    1, // btchip_apdu_liquid_get_nonce
    1, // btchip_apdu_liquid_provide_issuance_information
    1, // btchip_apdu_liquid_get_blinding_factor
    1, // btchip_apdu_liquid_get_green_wallet_address
    1  // btchip_apdu_liquid_headless_setup
};

apduProcessingFunction const DISPATCHER_FUNCTIONS[] = {
    btchip_apdu_setup,
    btchip_apdu_verify_pin,
    btchip_apdu_get_operation_mode,
    btchip_apdu_set_operation_mode,
    btchip_apdu_get_wallet_public_key,
    btchip_apdu_get_trusted_input,
    btchip_apdu_hash_input_start,
    btchip_apdu_hash_sign,
    btchip_apdu_hash_input_finalize_full,
    btchip_apdu_sign_message,
    btchip_apdu_get_random,
    btchip_apdu_get_firmware_version,
    btchip_apdu_set_alternate_coin_version,
    btchip_apdu_get_coin_version,
    
#ifdef HAVE_LIQUID

    btchip_apdu_liquid_get_commitments,
    btchip_apdu_liquid_get_public_blinding_key,
    btchip_apdu_liquid_get_nonce,
    btchip_apdu_liquid_provide_issuance_information,
    btchip_apdu_liquid_get_blinding_factor,
    btchip_apdu_liquid_get_green_wallet_address,

#ifdef HAVE_LIQUID_HEADLESS
    btchip_apdu_liquid_headless_setup
#else
    btchip_apdu_not_implemented    
#endif    

#else

    btchip_apdu_not_implemented,
    btchip_apdu_not_implemented,
    btchip_apdu_not_implemented,
    btchip_apdu_not_implemented,
    btchip_apdu_not_implemented,
    btchip_apdu_not_implemented,
    btchip_apdu_not_implemented

#endif    
};
