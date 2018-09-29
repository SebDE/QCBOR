/*==============================================================================
 Copyright 2018 Laurence Lundblade
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 (This is the MIT license)
 ==============================================================================*/
//
//  half_precision_test.c
//  QCBOR
//
//  Created by Laurence Lundblade on 9/19/18.
//  Copyright © 2018 Laurence Lundblade. All rights reserved.
//

#include "half_precision_test.h"
#include "qcbor.h"
#include "half_to_double_from_rfc7049.h"
#include <math.h> // For INFINITY and NAN and isnan()

static const uint8_t ExpectedHalf[] = {
    0xAD,
        0x64,
            0x7A, 0x65, 0x72, 0x6F,
        0xF9, 0x00, 0x00,   // 0.000
        0x6A,
            0x69, 0x6E, 0x66, 0x69, 0x6E, 0x69, 0x74, 0x69, 0x74, 0x79,
        0xF9, 0x7C, 0x00,   // Infinity
        0x73,
            0x6E, 0x65, 0x67, 0x61, 0x74, 0x69, 0x76, 0x65, 0x20, 0x69, 0x6E, 0x66, 0x69, 0x6E, 0x69, 0x74, 0x69, 0x74, 0x79,
        0xF9, 0xFC, 0x00,   // -Inifinity
        0x63,
            0x4E, 0x61, 0x4E,
        0xF9, 0x7E, 0x00,   // NaN
        0x63,
            0x6F, 0x6E, 0x65,
        0xF9, 0x3C, 0x00,   // 1.0
        0x69,
            0x6F, 0x6E, 0x65, 0x20, 0x74, 0x68, 0x69, 0x72, 0x64,
        0xF9, 0x35, 0x55,   // 0.333251953125
        0x76,
            0x6C, 0x61, 0x72, 0x67, 0x65, 0x73, 0x74, 0x20, 0x68, 0x61, 0x6C, 0x66, 0x2D, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6F, 0x6E,
        0xF9, 0x7B, 0xFF,   // 65504.0
        0x78, 0x18, 0x74, 0x6F, 0x6F, 0x2D, 0x6C, 0x61, 0x72, 0x67, 0x65, 0x20, 0x68, 0x61, 0x6C, 0x66, 0x2D, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6F, 0x6E,
        0xF9, 0x7C, 0x00,   // Infinity
        0x72,
            0x73, 0x6D, 0x61, 0x6C, 0x6C, 0x65, 0x73, 0x74, 0x20, 0x73, 0x75, 0x62, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C,
        0xF9, 0x00, 0x01,   // 0.000000059604
        0x6F,
            0x73, 0x6D, 0x61, 0x6C, 0x6C, 0x65, 0x73, 0x74, 0x20, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C,
        0xF9, 0x03, 0xFF,   // 0.0000609755516
        0x71,
            0x62, 0x69, 0x67, 0x67, 0x65, 0x73, 0x74, 0x20, 0x73, 0x75, 0x62, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C,
        0xF9, 0x04, 0x00,   // 0.000061988
        0x70,
            0x73, 0x75, 0x62, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x20, 0x73, 0x69, 0x6E, 0x67, 0x6C, 0x65,
        0xF9, 0x00, 0x00,
        0x03,
        0xF9, 0xC0, 0x00    // -2.0
};



int half_precision_encode_basic()
{
    UsefulBuf_MakeStackUB(EncodedHalfsMem, 220);

    QCBOREncodeContext EC;
    QCBOREncode_Init(&EC, EncodedHalfsMem);
    // These are mostly from https://en.wikipedia.org/wiki/Half-precision_floating-point_format
    QCBOREncode_OpenMap(&EC);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "zero", 0.00F);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "infinitity", INFINITY);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "negative infinitity", -INFINITY);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "NaN", NAN);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "one", 1.0F);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "one third", 0.333251953125F);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "largest half-precision",65504.0F);
    // Float 65536.0F is 0x47800000 in hex. It has an exponent of 16, which is larger than 15, the largest half-precision exponent
    QCBOREncode_AddFloatAsHalfToMap(&EC, "too-large half-precision", 65536.0F);
    // Should convert to smallest possible half precision which is encodded as 0x00 0x01 or 5.960464477539063e-8
    QCBOREncode_AddFloatAsHalfToMap(&EC, "smallest subnormal", 0.0000000596046448F);
    QCBOREncode_AddFloatAsHalfToMap(&EC, "smallest normal",    0.0000610351526F); // in hex single is 0x387fffff, exponent -15, significand 7fffff
    QCBOREncode_AddFloatAsHalfToMap(&EC, "biggest subnormal",  0.0000610351563F); // in hex single is 0x38800000, exponent -14, significand 0
    QCBOREncode_AddFloatAsHalfToMap(&EC, "subnormal single", 4e-40F); 
    QCBOREncode_AddFloatAsHalfToMapN(&EC, 3, -2.0F);
    QCBOREncode_CloseMap(&EC);
    
    EncodedCBOR EncodedHalfs;
    int nReturn = QCBOREncode_Finish2(&EC, &EncodedHalfs);
    if(nReturn) {
        return -1;
    }
    
    if(UsefulBuf_Compare(EncodedHalfs.Bytes, UsefulBuf_FromByteArrayLiteral(ExpectedHalf))) {
        return -3;
    }
    
    return 0;
}


int half_precision_decode_basic()
{
    UsefulBufC HalfPrecision = UsefulBuf_FromByteArrayLiteral(ExpectedHalf);
    
    QCBORDecodeContext DC;
    QCBORDecode_Init(&DC, HalfPrecision, 0);
    
    QCBORItem Item;

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_MAP) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 0.0F) {
        return -1;
    }
    
    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != INFINITY) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != -INFINITY) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item); // TODO, is this really converting right? It is carrying payload, but this confuses things.
    if(Item.uDataType != QCBOR_TYPE_FLOAT || !isnan(Item.val.fnum)) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 1.0F) {
        return -1;
    }
    
    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 0.333251953125F) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 65504.0F) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != INFINITY) {
        return -1;
    }
    
    QCBORDecode_GetNext(&DC, &Item); // TODO: check this
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 0.0000000596046448F) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item); // TODO: check this
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 0.0000609755516F) {
        return -1;
    }

    QCBORDecode_GetNext(&DC, &Item); // TODO check this
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 0.0000610351563F) {
        return -1;
    }
    
    QCBORDecode_GetNext(&DC, &Item); 
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != 0) {
        return -1;
    }
    
    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_FLOAT || Item.val.fnum != -2.0F) {
        return -1;
    }
    
    if(QCBORDecode_Finish(&DC)) {
        return -1;
    }
    
    return 0;
}


int half_precision_to_float_transitive_test()
{
    for(uint32_t uHalfP = 0; uHalfP < 0xffff; uHalfP += 1) {
        // Contruct the CBOR for the half-precision float by hand
        UsefulBuf_MakeStackUB(EncodedCBORMem, 3);
        UsefulOutBuf UOB;
        UsefulOutBuf_Init(&UOB, EncodedCBORMem);
        
        const uint8_t uHalfPrecInitialByte = HALF_PREC_FLOAT + (CBOR_MAJOR_TYPE_SIMPLE << 5); // 0xf9
        UsefulOutBuf_AppendByte(&UOB, uHalfPrecInitialByte); // The initial byte for a half-precision float
        UsefulOutBuf_AppendUint16(&UOB, (uint16_t)uHalfP);
        
        
        // Now parse the hand-constructed CBOR. This will invoke the conversion to a float
        QCBORDecodeContext DC;
        QCBORDecode_Init(&DC, UsefulOutBuf_OutUBuf(&UOB), 0);
        
        QCBORItem Item;
        QCBORDecode_GetNext(&DC, &Item);
        if(Item.uDataType != QCBOR_TYPE_FLOAT) {
            return -1;
        }
        
        //printf("%04x  QCBOR:%15.15f  \n", uHalfP,Item.val.fnum);

        
        // Now generate CBOR with the half-precision value. This will invoke the conversion from float to half
        UsefulBuf_MakeStackUB(OtherEncodedCBORMem, 5);
        QCBOREncodeContext EC;
        QCBOREncode_Init(&EC, OtherEncodedCBORMem);
        QCBOREncode_AddFloatAsHalf(&EC, Item.val.fnum);
        EncodedCBOR EnCBOR;
        QCBOREncode_Finish2(&EC, &EnCBOR); // todo check return code
        
        
        // Finally parse the CBOR by hand to get at half-precision that was actually encoded.
        UsefulInputBuf UIB;
        UsefulInputBuf_Init(&UIB, EnCBOR.Bytes);
        if(UsefulInputBuf_GetByte(&UIB) != uHalfPrecInitialByte) {
            return -2;
        }
        if(UsefulInputBuf_GetUint16(&UIB) != uHalfP) { // the moment of truth did we get back what we started with?
            return -3;
        }
    }
    
    return 0;
}


int half_precision_to_float_vs_rfc_test()
{
    for(uint32_t uHalfP = 0; uHalfP < 0xffff; uHalfP += 1) {
        unsigned char x[2];
        x[1] = uHalfP & 0xff;
        x[0] = uHalfP >> 8;
        double d = decode_half(x);
        
        // Contruct the CBOR for the half-precision float by hand
        UsefulBuf_MakeStackUB(__xx, 3);
        UsefulOutBuf UOB;
        UsefulOutBuf_Init(&UOB, __xx);
        
        const uint8_t uHalfPrecInitialByte = HALF_PREC_FLOAT + (CBOR_MAJOR_TYPE_SIMPLE << 5); // 0xf9
        UsefulOutBuf_AppendByte(&UOB, uHalfPrecInitialByte); // The initial byte for a half-precision float
        UsefulOutBuf_AppendUint16(&UOB, (uint16_t)uHalfP);

        // Now parse the hand-constructed CBOR. This will invoke the conversion to a float
        QCBORDecodeContext DC;
        QCBORDecode_Init(&DC, UsefulOutBuf_OutUBuf(&UOB), 0);
        
        QCBORItem Item;
        
        QCBORDecode_GetNext(&DC, &Item);
        if(Item.uDataType != QCBOR_TYPE_FLOAT) {
            return -1;
        }
        
        //printf("%04x  QCBOR:%15.15f  RFC: %15.15f\n", uHalfP,Item.val.fnum, d );
        
        if(Item.val.fnum != d) {
            return -2;
        }
    }
    return 0;
}


/*
 {"zero": 0.0, "negative zero": -0.0, "infinitity": Infinity, "negative infinitity": -Infinity, "NaN": NaN, "one": 1.0, "one third": 0.333251953125, "largest half-precision": 65504.0, "largest half-precision point one": 65504.1, "too-large half-precision": 65536.0, "smallest subnormal": 5.96046448e-8, "smallest normal": 0.00006103515261202119, "biggest subnormal": 0.00006103515625, "subnormal single": 4.00000646641519e-40, 3: -2.0, "large single exp": 2.5521177519070385e+38, "too-large single exp": 5.104235503814077e+38, "biggest single with prec": 16777216.0, "first single with prec loss": 16777217.0, 1: "fin"}
 
 */
static const uint8_t sExpectedSmallest[] = {
    0xB4, 0x64, 0x7A, 0x65, 0x72, 0x6F, 0xF9, 0x00, 0x00, 0x6D, 0x6E, 0x65, 0x67, 0x61, 0x74, 0x69, 0x76, 0x65, 0x20, 0x7A, 0x65, 0x72, 0x6F, 0xF9, 0x80, 0x00, 0x6A, 0x69, 0x6E, 0x66, 0x69, 0x6E, 0x69, 0x74, 0x69, 0x74, 0x79, 0xF9, 0x7C, 0x00, 0x73, 0x6E, 0x65, 0x67, 0x61, 0x74, 0x69, 0x76, 0x65, 0x20, 0x69, 0x6E, 0x66, 0x69, 0x6E, 0x69, 0x74, 0x69, 0x74, 0x79, 0xF9, 0xFC, 0x00, 0x63, 0x4E, 0x61, 0x4E, 0xF9, 0x7E, 0x00, 0x63, 0x6F, 0x6E, 0x65, 0xF9, 0x3C, 0x00, 0x69, 0x6F, 0x6E, 0x65, 0x20, 0x74, 0x68, 0x69, 0x72, 0x64, 0xF9, 0x35, 0x55, 0x76, 0x6C, 0x61, 0x72, 0x67, 0x65, 0x73, 0x74, 0x20, 0x68, 0x61, 0x6C, 0x66, 0x2D, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6F, 0x6E, 0xF9, 0x7B, 0xFF, 0x78, 0x20, 0x6C, 0x61, 0x72, 0x67, 0x65, 0x73, 0x74, 0x20, 0x68, 0x61, 0x6C, 0x66, 0x2D, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x20, 0x6F, 0x6E, 0x65, 0xFB, 0x40, 0xEF, 0xFC, 0x03, 0x33, 0x33, 0x33, 0x33, 0x78, 0x18, 0x74, 0x6F, 0x6F, 0x2D, 0x6C, 0x61, 0x72, 0x67, 0x65, 0x20, 0x68, 0x61, 0x6C, 0x66, 0x2D, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6F, 0x6E, 0xFA, 0x47, 0x80, 0x00, 0x00, 0x72, 0x73, 0x6D, 0x61, 0x6C, 0x6C, 0x65, 0x73, 0x74, 0x20, 0x73, 0x75, 0x62, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0xFB, 0x3E, 0x70, 0x00, 0x00, 0x00, 0x1C, 0x5F, 0x68, 0x6F, 0x73, 0x6D, 0x61, 0x6C, 0x6C, 0x65, 0x73, 0x74, 0x20, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0xFA, 0x38, 0x7F, 0xFF, 0xFF, 0x71, 0x62, 0x69, 0x67, 0x67, 0x65, 0x73, 0x74, 0x20, 0x73, 0x75, 0x62, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0xF9, 0x04, 0x00, 0x70, 0x73, 0x75, 0x62, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x20, 0x73, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0xFB, 0x37, 0xC1, 0x6C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF9, 0xC0, 0x00, 0x70, 0x6C, 0x61, 0x72, 0x67, 0x65, 0x20, 0x73, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x65, 0x78, 0x70, 0xFA, 0x7F, 0x40, 0x00, 0x00, 0x74, 0x74, 0x6F, 0x6F, 0x2D, 0x6C, 0x61, 0x72, 0x67, 0x65, 0x20, 0x73, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x65, 0x78, 0x70, 0xFB, 0x47, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x18, 0x62, 0x69, 0x67, 0x67, 0x65, 0x73, 0x74, 0x20, 0x73, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x70, 0x72, 0x65, 0x63, 0xFA, 0x4B, 0x80, 0x00, 0x00, 0x78, 0x1B, 0x66, 0x69, 0x72, 0x73, 0x74, 0x20, 0x73, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x70, 0x72, 0x65, 0x63, 0x20, 0x6C, 0x6F, 0x73, 0x73, 0xFB, 0x41, 0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x63, 0x66, 0x69, 0x6E
};


int double_as_smallest_encode_basic()
{
    UsefulBuf_MakeStackUB(EncodedHalfsMem, 420);
    
    QCBOREncodeContext EC;
    QCBOREncode_Init(&EC, EncodedHalfsMem);
    // These are mostly from https://en.wikipedia.org/wiki/Half-precision_floating-point_format
    QCBOREncode_OpenMap(&EC);
    // 64                                   # text(4)
    //    7A65726F                          # "zero"
    // F9 0000                              # primitive(0)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "zero", 0.00);

    // 64                                   # text(4)
    //    7A65726F                          # "negative zero"
    // F9 8000                              # primitive(0)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "negative zero", -0.00);
    
    // 6A                                   # text(10)
    //    696E66696E6974697479              # "infinitity"
    // F9 7C00                              # primitive(31744)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "infinitity", INFINITY);
    
    // 73                                   # text(19)
    //    6E6567617469766520696E66696E6974697479 # "negative infinitity"
    // F9 FC00                              # primitive(64512)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "negative infinitity", -INFINITY);
    
    // 63                                   # text(3)
    //    4E614E                            # "NaN"
    // F9 7E00                              # primitive(32256)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "NaN", NAN);
    
    // TODO: test a few NaN variants
    
    // 63                                   # text(3)
    //    6F6E65                            # "one"
    // F9 3C00                              # primitive(15360)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "one", 1.0);
    
    // 69                                   # text(9)
    //    6F6E65207468697264                # "one third"
    // F9 3555                              # primitive(13653)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "one third", 0.333251953125);
    
    // 76                                   # text(22)
    //    6C6172676573742068616C662D707265636973696F6E # "largest half-precision"
    // F9 7BFF                              # primitive(31743)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "largest half-precision",65504.0);

    // 76                                   # text(22)
    //    6C6172676573742068616C662D707265636973696F6E # "largest half-precision"
    // F9 7BFF                              # primitive(31743)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "largest half-precision point one",65504.1);
    
    // Float 65536.0F is 0x47800000 in hex. It has an exponent of 16, which is larger than 15, the largest half-precision exponent
    // 78 18                                # text(24)
    //    746F6F2D6C617267652068616C662D707265636973696F6E # "too-large half-precision"
    // FA 47800000                          # primitive(31743)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "too-large half-precision", 65536.0);
    
    // The smallest possible half-precision subnormal, but digitis are lost converting
    // to half, so this turns into a double
    // 72                                   # text(18)
    //    736D616C6C657374207375626E6F726D616C # "smallest subnormal"
    // FB 3E700000001C5F68                  # primitive(4499096027744984936)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "smallest subnormal", 0.0000000596046448);
    
    // The smallest possible half-precision snormal, but digitis are lost converting
    // to half, so this turns into a single TODO: confirm this is right
    // 6F                                   # text(15)
    //    736D616C6C657374206E6F726D616C    # "smallest normal"
    // FA 387FFFFF                          # primitive(947912703)
    // in hex single is 0x387fffff, exponent -15, significand 7fffff
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "smallest normal",    0.0000610351526F);
    
    // 71                                   # text(17)
    //    62696767657374207375626E6F726D616C # "biggest subnormal"
    // F9 0400                              # primitive(1024)
    // in hex single is 0x38800000, exponent -14, significand 0
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "biggest subnormal",  0.0000610351563F);
    
    // 70                                   # text(16)
    //    7375626E6F726D616C2073696E676C65  # "subnormal single"
    // FB 37C16C2800000000                  # primitive(4017611261645684736)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "subnormal single", 4e-40F);
    
    // 03                                   # unsigned(3)
    // F9 C000                              # primitive(49152)
    QCBOREncode_AddDoubleAsSmallestToMapN(&EC, 3, -2.0);
    
    // 70                                   # text(16)
    //    6C617267652073696E676C6520657870  # "large single exp"
    // FA 7F400000                          # primitive(2134900736)
    // (0x01LL << (DOUBLE_NUM_SIGNIFICAND_BITS-1)) | ((127LL + DOUBLE_EXPONENT_BIAS) << DOUBLE_EXPONENT_SHIFT);
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "large single exp", 2.5521177519070385E+38); // Exponent fits  single

    // 74                                   # text(20)
    //    746F6F2D6C617267652073696E676C6520657870 # "too-large single exp"
    // FB 47F8000000000000                  # primitive(5185894970917126144)
    // (0x01LL << (DOUBLE_NUM_SIGNIFICAND_BITS-1)) | ((128LL + DOUBLE_EXPONENT_BIAS) << DOUBLE_EXPONENT_SHIFT);
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "too-large single exp", 5.104235503814077E+38); // Exponent too large for single

    // 66                                   # text(6)
    //    646664666465                      # "dfdfde"
    // FA 4B800000                          # primitive(1266679808)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "biggest single with prec",16777216); // Single with no precision loss
    
    // 78 18                                # text(24)
    //    626967676573742073696E676C6520776974682070726563 # "biggest single with prec"
    // FA 4B800000                          # primitive(1266679808)
    QCBOREncode_AddDoubleAsSmallestToMap(&EC, "first single with prec loss",16777217); // Double becuase of precision loss
    
    // Just a convenient marker when cutting and pasting encoded CBOR
    QCBOREncode_AddSZStringToMapN(&EC, 1, "fin");

    QCBOREncode_CloseMap(&EC);
    
    EncodedCBOR EncodedHalfs;
    int nReturn = QCBOREncode_Finish2(&EC, &EncodedHalfs);
    if(nReturn) {
        return -1;
    }
    
    if(UsefulBuf_Compare(EncodedHalfs.Bytes, UsefulBuf_FromByteArrayLiteral(sExpectedSmallest))) {
        return -3;
    }
    
    return 0;
}




