#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef unsigned int        UINT32;
typedef int                 INT32;
typedef unsigned short      UINT16;
typedef unsigned short      CHAR16;
typedef short               INT16;
typedef unsigned char       BOOLEAN;
typedef unsigned char       UINT8;
typedef char                CHAR8;
typedef char                INT8;

//byte order: BGR, not RGB
#define RED   2
#define GREEN 1
#define BLUE  0

#define MAX_FILE_NAME_LEN 100

typedef struct tagBITMAPFILEHEADER
{
    UINT16 bfType;
    UINT32 bfSize;
    UINT16 bfReserved1;
    UINT16 bfReserved2;
    UINT32 bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFO
{
    UINT32 biSize;
    UINT32 biWidth;         //in pixel
    UINT32 biHeight;        //in pixel
    UINT16 biPlanes;        //=1
    UINT16 biBitCount;      //bits/pixel, =1,4,8,16,24,32
    UINT32 biCompression;   //0:RGB,1:RLE8,2:RLE4,3:BITFIELDS 24/32 4:JPEG  5:PNG
    UINT32 biSizeImage;     //image size in byte,
    UINT32 biXpelsPerMeter; //pixel/meter
    UINT32 biYpelsPerMeter; //pixel/meter
    UINT32 biClrUsed;
    UINT32 biClrImportant;
} BITMAPINFO;

int print_help(){
    printf("Usage: ./colorBlend testCase [-s SRC_PATH] [-d DST_PATH] [-o OUT_PATH] [-l LIGHT] [-t SATURATION] [-h HUE] [-rgb RED GREEN BLUE] [-bm BLEND_MODE] [-as ALPHA_SRC] [-ad ALPHA_DST]"
            "example: \n"
            "   ./colorBlend 1 -l 120 -t 240 -s src_24.bmp -d dst_24.bmp -o out_24.bmp \n"
            "   ./colorBlend 2 -rgb 233 0 233 \n"
            "   ./colorBlend 3 -l 142 -t 120 -h 74 \n"
            "   ./colorBlend 4 -bm 5 -as 100 -ad 100 \n"
            "0: print help! \n"
            "1: Get src's color with light and saturation adjustable: get_src_with_light_and_saturation() \n"
            "2: convert RGB to light saturation hue. rgb_2_color() \n"
            "3: convert light saturation hue to RGB. color_2_rgb() \n"
            "4: blend SRC and DST by BLEND_MODE blend_src_dst() \n");
}


int bmp_parse(UINT8* buf, BITMAPFILEHEADER *bmpHeader, BITMAPINFO *bmpInfo, UINT8 **pixel_ptr)
{
    UINT8* ptr = buf;
    UINT32 data;

    if((buf) && (bmpHeader) && (bmpInfo) && (*pixel_ptr == NULL))
    {
        //bmp header
        bmpHeader->bfType = (((UINT16)ptr[0])<<8) + (UINT16)ptr[1];
        bmpHeader->bfSize = (((UINT32)ptr[5])<<24) + (((UINT32)ptr[4])<<16) + (((UINT32)ptr[3])<<8) + (UINT32)ptr[2];
        bmpHeader->bfOffBits = (((UINT32)ptr[0xd])<<24) + (((UINT32)ptr[0xc])<<16) + (((UINT32)ptr[0xb])<<8) + (UINT32)ptr[0xa];
        //printf("COLOR_BLEND: %s:%d: data=%x %x %x %x %x %x %x %x \n",
        //     __FUNCTION__, __LINE__, ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7]);

        data = (((UINT32)ptr[5])<<24) + (((UINT32)ptr[4])<<16) + (((UINT32)ptr[3])<<8) + (UINT32)ptr[2];
        //printf("COLOR_BLEND: %s:%d: bfType=0x%x, bfSize=0x%x ,bfOffBits=0x%x, data=0x%x\n",
        // __FUNCTION__, __LINE__, bmpHeader->bfType, bmpHeader->bfSize, bmpHeader->bfOffBits, data);

        //bmp info
        bmpInfo->biSize = (((UINT32)ptr[0x11])<<24) + (((UINT32)ptr[0x10])<<16) + (((UINT32)ptr[0xf])<<8) + (UINT32)ptr[0xe];
        bmpInfo->biWidth = (((UINT32)ptr[0x15])<<24) + (((UINT32)ptr[0x14])<<16) + (((UINT32)ptr[0x13])<<8) + (UINT32)ptr[0x12];
        bmpInfo->biHeight = (((UINT32)ptr[0x19])<<24) + (((UINT32)ptr[0x18])<<16) + (((UINT32)ptr[0x17])<<8) + (UINT32)ptr[0x16];
        bmpInfo->biPlanes = (((UINT32)ptr[0x1b])<<8) + (UINT32)ptr[0x1a];
        bmpInfo->biBitCount = (((UINT32)ptr[0x1d])<<8) + (UINT32)ptr[0x1c];
        bmpInfo->biCompression = (((UINT32)ptr[0x21])<<24) + (((UINT32)ptr[0x20])<<16) + (((UINT32)ptr[0x1f])<<8) + (UINT32)ptr[0x1e];
        bmpInfo->biSizeImage = (((UINT32)ptr[0x25])<<24) + (((UINT32)ptr[0x24])<<16) + (((UINT32)ptr[0x23])<<8) + (UINT32)ptr[0x22];
        bmpInfo->biXpelsPerMeter = (((UINT32)ptr[0x29])<<24) + (((UINT32)ptr[0x28])<<16) + (((UINT32)ptr[0x27])<<8) + (UINT32)ptr[0x26];
        bmpInfo->biYpelsPerMeter = (((UINT32)ptr[0x2d])<<24) + (((UINT32)ptr[0x2c])<<16) + (((UINT32)ptr[0x2b])<<8) + (UINT32)ptr[0x2a];
        bmpInfo->biClrUsed = (((UINT32)ptr[0x31])<<24) + (((UINT32)ptr[0x30])<<16) + (((UINT32)ptr[0x2f])<<8) + (UINT32)ptr[0x2e];
        bmpInfo->biClrImportant = (((UINT32)ptr[0x35])<<24) + (((UINT32)ptr[0x34])<<16) + (((UINT32)ptr[0x33])<<8) + (UINT32)ptr[0x32];
        //printf("COLOR_BLEND: %s:%d: biSize=0x%x, biWidth=%d ,biHeight=%d, biPlanes=%d, biBitCount=%d, biCompression=%d, biSizeImage=0x%x,"
        //    "  biXpelsPerMeter=%d, biYpelsPerMeter=%d, biClrUsed=%d, biClrImportant=%d\n", __FUNCTION__, __LINE__,
        //    bmpInfo->biSize, bmpInfo->biWidth, bmpInfo->biHeight, bmpInfo->biPlanes,
        //    bmpInfo->biBitCount, bmpInfo->biCompression, bmpInfo->biSizeImage, bmpInfo->biXpelsPerMeter,
        //    bmpInfo->biYpelsPerMeter, bmpInfo->biClrUsed, bmpInfo->biClrImportant);

        *pixel_ptr = &ptr[bmpHeader->bfOffBits];
        //printf("COLOR_BLEND: %s:%d: ptr=%p, pixel_ptr=%p \n",  __FUNCTION__, __LINE__, ptr,*pixel_ptr);

        return 0;
    } else{
        printf("COLOR_BLEND: %s:%d: bad parameters: buf=%p, bmpHeader=%p, bmpInfo=%p, *pixel_ptr=%p \n",
             __FUNCTION__, __LINE__, buf, bmpHeader, bmpInfo, *pixel_ptr);
        return -1;
    }
}

int gen_out_info_base_on_src(UINT8* buf_src, BITMAPFILEHEADER *bmpHeader_src, BITMAPINFO *bmpInfo_src, UINT8 **pixel_ptr_src,
    UINT8** buf_out, BITMAPFILEHEADER *bmpHeader_out, BITMAPINFO *bmpInfo_out, UINT8 **pixel_ptr_out)
{
    UINT8 *ptr = NULL;

    if((buf_src) && (bmpHeader_src) && (bmpInfo_src) && (*pixel_ptr_src != NULL) &&
        (buf_out) && (bmpHeader_out) && (bmpInfo_out) && (*pixel_ptr_out == NULL))
    {
        *buf_out = calloc(bmpHeader_src->bfSize, 1);
        if(*buf_out){
            memcpy(bmpHeader_out, bmpHeader_src, sizeof(BITMAPFILEHEADER));
            memcpy(bmpInfo_out, bmpInfo_src, sizeof(BITMAPINFO));
            memcpy(*buf_out, buf_src, bmpHeader_out->bfOffBits);

            //printf("COLOR_BLEND: %s:%d: bfType=0x%x, bfSize=0x%x ,bfOffBits=0x%x, sizeof(bmpHeader_src)=%ld\n",
            //     __FUNCTION__, __LINE__,
            //     bmpHeader_out->bfType, bmpHeader_out->bfSize, bmpHeader_out->bfOffBits, sizeof(BITMAPFILEHEADER));


            ptr = *buf_out;
            *pixel_ptr_out = &ptr[bmpHeader_out->bfOffBits];

           // printf("COLOR_BLEND: %s:%d: buf_out=%p, pixel_ptr_out=%p, offset=%x, data=%x %x %x %x %x %x %x %x %x %x %x %x\n",
           //      __FUNCTION__, __LINE__,
           //     *buf_out,*pixel_ptr_out, bmpHeader_out->bfOffBits,
           //     ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],ptr[0xa],ptr[0xb]);
        }

        return 0;
    } else {
        printf("COLOR_BLEND: %s:%d: bad parameters: buf=%p %p, bmpHeader=%p %p, bmpInfo=%p %p, *pixel_ptr=%p %p \n",
             __FUNCTION__, __LINE__,
             buf_src, *buf_out, bmpHeader_src,bmpHeader_out, bmpInfo_src,bmpInfo_out, *pixel_ptr_src,*pixel_ptr_out);
        return -1;
    }
}

inline float convert_RGB_by_light(float fRGB, UINT32 iLight, float fLight){
    float fNew_rgb;

    if(iLight >= 120){
        fNew_rgb = fRGB + (1.0 - fRGB) * fLight;
    }else{
        fNew_rgb = fRGB * fLight;
    }
    return fNew_rgb;
}

int convert_RGB_by_saturation(float *finRed, float *finGreen, float *finBlue, float fSaturation)
{
    int rc = 0;
    float minVal, maxVal, tarVal;
    float fRed = *finRed, fGreen = *finGreen, fBlue = *finBlue;

    if(fRed > fGreen){
        minVal = fGreen;
        maxVal = fRed;
    }else{
        minVal = fRed;
        maxVal = fGreen;
    }

    if(minVal > fBlue){
        minVal = fBlue;
    }

    if(maxVal < fBlue){
        maxVal = fBlue;
    }

    tarVal = (minVal + maxVal)/2;

    //RGB go to target with Saturation.
    *finRed = tarVal - (tarVal - fRed) * fSaturation;
    *finGreen =  tarVal - (tarVal - fGreen) * fSaturation;
    *finBlue =  tarVal - (tarVal - fBlue) * fSaturation;

    return rc;
}

int get_src_with_light_and_saturation(UINT32 iLight, UINT32 iSaturation,
    UINT8* buf_src, BITMAPFILEHEADER *bmpHeader_src, BITMAPINFO *bmpInfo_src, UINT8 *pixel_ptr_src,
    UINT8* buf_out, BITMAPFILEHEADER *bmpHeader_out, BITMAPINFO *bmpInfo_out, UINT8 *pixel_ptr_out)
{
    UINT8 *ptr = NULL;
    UINT32 pixel_cnt = 0;
    UINT32 width = 0;
    UINT32 hight = 0;
    UINT32 byte_per_pixel = 0;
    UINT32 byte_per_row = 0;
    UINT32 i,j,offset;
    float fRed, fBlue, fGreen;
    float fAlpha_R = 1.0, fAlpha_G = 1.0, fAlpha_B = 1.0;
    float fLight, fSaturation;

    //calculate the light ration
    if(iLight >= 120){
        //fRGB_new = fRGB+(1.0-fRGB)*fLight
        fLight = ((float)iLight - 120)/120;
    }else{
        //fRGB_new = fRGB*fLight
        fLight = ((float)iLight)/120;
    }

    //calculate the saturation ratio
    fSaturation = ((float)iSaturation)/240;

    printf("COLOR_BLEND: %s:%d: iLight=%d, iSaturation=%d \n",  __FUNCTION__, __LINE__, iLight,iSaturation);

    //data copy with alpha ration
    if((buf_src) && (bmpHeader_src) && (bmpInfo_src) && (pixel_ptr_src != NULL) &&
        (buf_out) && (bmpHeader_out) && (bmpInfo_out) && (pixel_ptr_out != NULL) &&
        (bmpInfo_out->biBitCount == 24))
    {
            ptr = buf_out;
            width = bmpInfo_out->biWidth;
            hight = bmpInfo_out->biHeight;
            pixel_cnt = hight * width;
            byte_per_pixel = bmpInfo_out->biBitCount >> 3;   // 24b/8=3 bytes
            byte_per_row = byte_per_pixel * width;

            for(i=0; i<hight; i++){
                for(j=0; j<width; j++){
                    offset = i*byte_per_row + j*byte_per_pixel;

                    fRed = ((float)(pixel_ptr_src[offset + RED]))/255;
                    fBlue = ((float)(pixel_ptr_src[offset + BLUE]))/255;
                    fGreen = ((float)(pixel_ptr_src[offset + GREEN]))/255;

                    if((iLight == 120) && (iSaturation == 240)){  // no calc
                        pixel_ptr_out[offset + RED] = (UINT32)((fAlpha_R * fRed) * 255);
                        pixel_ptr_out[offset + GREEN] = (UINT32)((fAlpha_G * fGreen) * 255);
                        pixel_ptr_out[offset + BLUE] = (UINT32)((fAlpha_B * fBlue) * 255);
                    } else {
                        convert_RGB_by_saturation(&fRed, &fGreen, &fBlue, fSaturation);

                        fRed = convert_RGB_by_light(fRed, iLight, fLight);
                        fGreen = convert_RGB_by_light(fGreen, iLight, fLight);
                        fBlue = convert_RGB_by_light(fBlue, iLight, fLight);

                        pixel_ptr_out[offset + RED] = (UINT32)(255*fRed);
                        pixel_ptr_out[offset + GREEN] = (UINT32)(255*fGreen);
                        pixel_ptr_out[offset + BLUE] = (UINT32)(255*fBlue);
                    }

                    //if(fRed != 1.0)
                    //    printf("%d %d:fRed=%f 0x%x, fBlue=%f ,fGreen=%f \n",i,j,
                    //        fRed, pixel_ptr_out[offset + RED], fBlue, fGreen);
                }
            }

            printf("COLOR_BLEND: %s:%d: width=%d, hight=%d, pixel_cnt=%d, byte_per_pixel=%d, pixel_bytes=%d %d. \n"
                "data=%x %x %x %x %x %x %x %x %x %x %x %x\n", __FUNCTION__, __LINE__,
                width, hight, pixel_cnt, byte_per_pixel,bmpInfo_out->biSizeImage, pixel_cnt*byte_per_pixel,
                ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],ptr[0xa],ptr[0xb]);

        return 0;
    } else {
        printf("COLOR_BLEND: %s:%d: bad parameters: buf=%p %p, bmpHeader=%p %p, bmpInfo=%p %p, pixel_ptr=%p %p \n",
             __FUNCTION__, __LINE__,
             buf_src,buf_out, bmpHeader_src,bmpHeader_out, bmpInfo_src,bmpInfo_out, pixel_ptr_src, pixel_ptr_out);
        return -1;
    }
}

int rgb_2_color(UINT32 iRed, UINT32 iGreen, UINT32 iBlue)
{
    UINT8 *ptr = NULL;
    UINT32 pixel_cnt = 0;
    UINT32 width = 0;
    UINT32 hight = 0;
    UINT32 byte_per_pixel = 0;
    UINT32 byte_per_row = 0;
    UINT32 i,j,offset;
    float fRed, fBlue, fGreen;
    float fLight, fSaturation, fHue;
    float fMin, fMax, fC, fMm, fTmp;
    INT32 iLight, iSaturation, iHue;
    int rc = 0;

    fRed = ((float)iRed)/255;
    fBlue = ((float)iBlue)/255;
    fGreen = ((float)iGreen)/255;

    if(fRed > 1)
        fRed = 1;
    if(fBlue > 1)
        fBlue = 1;
    if(fGreen > 1)
        fGreen = 1;

    if(fRed > fBlue){
        fMin = fBlue;
        fMax = fRed;
    }else{
        fMax = fBlue;
        fMin = fRed;
    }

    if(fGreen > fMax){
        fMax = fGreen;
    }
    if(fGreen < fMin){
        fMin = fGreen;
    }

    fC = fMax - fMin;
    fMm = fMax + fMin;

    //calc light
    fLight = fMm / 2;
    iLight = (INT32)(fLight * 240);

    //calc saturation.
    if(fLight == 1){
        fSaturation =  -1;
    }else{
        if(fMm > 1){
            fTmp = 2 - fMm;
        }else{
            fTmp = fMm;
        }
        if(fTmp == 0)
            fSaturation = -1;
        else
            fSaturation = fC / fTmp;
    }
    iSaturation = (INT32)(fSaturation * 240);

    //calc hue
    if(fC != 0){
        if(fMax == fBlue){
            fTmp = (fRed - fGreen) / fC + 4;
        }else if(fMax == fGreen){
            fTmp = (fBlue - fRed) / fC + 2;
        }else{ //fMax == fRed
            if(fGreen > fBlue)
                fTmp = (fGreen - fBlue) / fC;
            else
                fTmp = (fGreen - fBlue) / fC + 6;
        }
    }else{
        fTmp = -1;
    }
    fHue = (INT32)(fTmp * 60);
    iHue = (INT32)(fHue *240 / 360);

    printf("COLOR_BLEND: %s:%d: RGB %d %d %d: iHue=%d %.0f', iLight=%d %.2f, iSaturation=%d %.2f \n",
        __FUNCTION__, __LINE__, iRed, iGreen, iBlue, iHue,fHue, iLight,fLight, iSaturation, fSaturation);

    return rc;
}

int color_2_rgb(UINT32 iLight, UINT32 iSaturation, UINT32 iHue)
{
    UINT8 *ptr = NULL;
    UINT32 pixel_cnt = 0;
    UINT32 width = 0;
    UINT32 hight = 0;
    UINT32 byte_per_pixel = 0;
    UINT32 byte_per_row = 0;
    UINT32 i,j,offset;
    float fRed, fBlue, fGreen;
    float fLight, fSaturation, fHue;
    float fMin, fMax, fC, fMm, fTmp;
    INT32 iRed, iGreen, iBlue;
    int rc = 0;

    //calculate the light ration
    fLight = ((float)iLight)/240;
    fSaturation = ((float)iSaturation)/240;
    if(iHue > 239)   //now iHue in 0~239
        iHue = 239;
    //fHue = ((float)iHue)*358.5/239;
    fHue = ((float)iHue)*360/240;
    if(fLight > 1)
        fLight = 1;
    if(fSaturation > 1)
        fSaturation = 1;


    fMm = fLight * 2;
    if(fMm > 1){
        fTmp = 2 - fMm;
    }else{
        fTmp = fMm;
    }
    fC = fSaturation * fTmp;

    fMax = (fMm + fC)/2;
    fMin = (fMm - fC)/2;

    //if(fMax == fBlue){
    //    fTmp = (fRed - fGreen) / fC + 4;
    //}else if(fMax == fGreen){
    //    fTmp = (fBlue - fRed) / fC + 2;
    //}else{ //fMax == fRed
    //    if(fGreen > fBlue)
    //        fTmp = (fGreen - fBlue) / fC;
    //    else
    //        fTmp = (fGreen - fBlue) / fC + 6;
    //}
    fTmp = (fHue / 60);
    switch ((int)fTmp)
    {
        case 0:
            fRed = fMax;
            fGreen = fMin + fTmp*fC;
            fBlue = fMin;
            break;
        case 1:
            fRed = fMin - (fTmp-2)*fC;;
            fGreen = fMax;
            fBlue = fMin;
            break;
        case 2:
            fRed = fMin;
            fGreen = fMax;
            fBlue = fMin + (fTmp-2)*fC;
            break;
        case 3: //./colorBlend 3 -l 162 -t 84 -h 144
            fRed = fMin;
            fGreen = fMin - (fTmp-4)*fC;
            fBlue = fMax;
            break;
        case 4:
            fRed = fMin + (fTmp-4)*fC;
            fGreen = fMin;
            fBlue = fMax;
            break;
        case 5:  // ./colorBlend 3 -l 179 -t 240 -h 232
            fRed = fMax;
            fGreen = fMin;
            fBlue = fMin - (fTmp-6)*fC;
            break;
        default:
            fRed = fGreen = fBlue = 0;
            break;
    }

    iRed = (UINT32)(fRed * 255);
    iGreen = (UINT32)(fGreen * 255);
    iBlue = (UINT32)(fBlue * 255);
    printf("COLOR_BLEND: %s:%d: RGB %d %d %d. fTmp=%f, fHue=%f, iHue=%d\n",
        __FUNCTION__, __LINE__, iRed, iGreen, iBlue, fTmp, fHue, iHue);

    return rc;
}

/*
    Function: blend src and dst pixels by blend_mode

    base on: Cr = Cs*Fs + Cd*Fd ,   Ar = As*Fs + Ad*Fd
    1. CLEAR    Fs = Fd = 0
    2. SRC      Fs = 1, Fd = 0
    3. DST      Fs = 0, Fd = 1
    4. SRC OVER Fs = 1, Fd = (1-As)
    5. DST OVER Fs = (1-Ad), Fd = 1
    6. SRC IN   Fs = Ad, Fd = 0
    7. DST IN   Fs = 0, Fd = As
    8. SRC OUT  Fs = (1-Ad), Fd = 0
    9. DST OUT  Fs = 0, Fd = (1-As)
    10.SRC ATOP Fs = Ad, Fd = (1-As)
    11.DST ATOP Fs = (1-Ad), Fd = As
    12.XOR      Fs = (1-Ad), Fd = (1-As)
*/
int blend_src_dst(UINT32 blend_mode, UINT32 iAlha_s, UINT32 iAlha_d,
    UINT8* buf_src, BITMAPFILEHEADER *bmpHeader_src, BITMAPINFO *bmpInfo_src, UINT8 *pixel_ptr_src,
    UINT8* buf_dst, BITMAPFILEHEADER *bmpHeader_dst, BITMAPINFO *bmpInfo_dst, UINT8 *pixel_ptr_dst,
    UINT8* buf_out, BITMAPFILEHEADER *bmpHeader_out, BITMAPINFO *bmpInfo_out, UINT8 *pixel_ptr_out)
{
    UINT8 *ptr = NULL;
    UINT32 pixel_cnt = 0;
    UINT32 width = 0;
    UINT32 hight = 0;
    UINT32 byte_per_pixel = 0;
    UINT32 byte_per_row = 0;
    UINT32 i,j,offset;
    float fRed_s, fBlue_s, fGreen_s, fRed_d, fBlue_d, fGreen_d, fRed_o, fBlue_o, fGreen_o;
    float fFs, fFd;    //fraction.
    float fAs, fAd, fAo;
    UINT32 iAlha_o;
    char mode_str[10];

    //calc alpha coe in float
    if(iAlha_s > 100){
        iAlha_s = 100;
    }
    fAs = ((float)iAlha_s)/100;

    if(iAlha_d > 100){
        iAlha_d = 100;
    }
    fAd = ((float)iAlha_d)/100;

    //calculate Fs Fd
    switch(blend_mode){
        case 1:  //1. CLEAR    Fs = Fd = 0\
            fFs = 0;
            fFd = 0;
            strcpy(mode_str,"CLEAR");
            break;
        case 2:  //2. SRC      Fs = 1, Fd = 0
            fFs = 1;
            fFd = 0;
            strcpy(mode_str,"SRC");
            break;
        case 3:  //3. DST      Fs = 0, Fd = 1
            fFs = 0;
            fFd = 1;
            strcpy(mode_str,"DST");
            break;
        case 4:  //4. SRC OVER Fs = 1, Fd = (1-As)
            fFs = 1;
            fFd = 1 - fAs;
            strcpy(mode_str,"SRC OVER");
            break;
        case 5:  //5. DST OVER Fs = (1-Ad), Fd = 1
            fFs = 1 - fAd;
            fFd = 1;
            strcpy(mode_str,"DST OVER");
            break;
        case 6:  //6. SRC IN   Fs = Ad, Fd = 0
            fFs = fAd;
            fFd = 0;
            strcpy(mode_str,"SRC IN");
            break;
        case 7:  //7. DST IN   Fs = 0, Fd = As
            fFs = 0;
            fFd = fAs;
            strcpy(mode_str,"DST IN");
            break;
        case 8:  //8. SRC OUT  Fs = (1-Ad), Fd = 0
            fFs = 1 - fAd;
            fFd = 0;
            strcpy(mode_str,"SRC OUT");
            break;
        case 9:  //9. DST OUT  Fs = 0, Fd = (1-As)
            fFs = 0;
            fFd = 1 - fAs;
            strcpy(mode_str,"DST OUT");
            break;
        case 10:  //10.SRC ATOP Fs = Ad, Fd = (1-As)
            fFs = fAd;
            fFd = 1 - fAs;
            strcpy(mode_str,"SRC ATOP");
            break;
        case 11:  //11.DST ATOP Fs = (1-Ad), Fd = As
            fFs = 1 - fAd;
            fFd = fAs;
            strcpy(mode_str,"DST ATOP");
            break;
        case 12:  //12.XOR      Fs = (1-Ad), Fd = (1-As)
            fFs = 1 - fAd;
            fFd = 1 - fAs;
            strcpy(mode_str,"XOR");
            break;
        default:
            printf("%s:%d: Bad parameter blend_mode=%d \n",
                __FUNCTION__, __LINE__, blend_mode);
            return -1;
    }

    //calc output alpha: Ar = As*Fs + Ad*Fd
    fAo = fAs*fFs + fAd*fFd;
    iAlha_o = (UINT32)(100 * fAo);

    printf("COLOR_BLEND: %s:%d: blend_mode=%d %s:, iAlha_s=%d, iAlha_d=%d, iAlha_o=%d, fFs=%.2f, fFd=%.2f \n",
        __FUNCTION__, __LINE__, blend_mode, mode_str, iAlha_s, iAlha_d, iAlha_o, fFs, fFd);

    //data copy with alpha ration
    if((buf_src) && (bmpHeader_src) && (bmpInfo_src) && (pixel_ptr_src != NULL) &&
        (buf_dst) && (bmpHeader_dst) && (bmpInfo_dst) && (pixel_ptr_dst != NULL) &&
        (buf_out) && (bmpHeader_out) && (bmpInfo_out) && (pixel_ptr_out != NULL) &&
        (bmpInfo_out->biBitCount == 24))
    {
        ptr = buf_out;
        width = bmpInfo_out->biWidth;
        hight = bmpInfo_out->biHeight;
        pixel_cnt = hight * width;
        byte_per_pixel = bmpInfo_out->biBitCount >> 3;   // 24b/8=3 bytes
        byte_per_row = byte_per_pixel * width;

        for(i=0; i<hight; i++){
            for(j=0; j<width; j++){
                offset = i*byte_per_row + j*byte_per_pixel;

                fRed_s = ((float)(pixel_ptr_src[offset + RED]))/255;
                fBlue_s = ((float)(pixel_ptr_src[offset + BLUE]))/255;
                fGreen_s = ((float)(pixel_ptr_src[offset + GREEN]))/255;

                fRed_d = ((float)(pixel_ptr_dst[offset + RED]))/255;
                fBlue_d = ((float)(pixel_ptr_dst[offset + BLUE]))/255;
                fGreen_d = ((float)(pixel_ptr_dst[offset + GREEN]))/255;

                //Cr = Cs*Fs + Cd*Fd
                fRed_o = (fRed_s*fFs + fRed_d*fFd);
                fGreen_o = (fGreen_s*fFs + fGreen_d*fFd);
                fBlue_o = (fBlue_s*fFs + fBlue_d*fFd);

                pixel_ptr_out[offset + RED] = (UINT32)(fRed_o * 255);
                pixel_ptr_out[offset + GREEN] = (UINT32)(fGreen_o * 255);
                pixel_ptr_out[offset + BLUE] = (UINT32)(fBlue_o * 255);

                //if(fRed != 1.0)
                //    printf("%s %d: %d %d:fRed=%f 0x%x, fBlue=%f ,fGreen=%f \n",i,j,
                //        __FUNCTION__, __LINE__,
                //        fRed, pixel_ptr_out[offset + RED], fBlue, fGreen);
            }
        }

        printf("COLOR_BLEND: %s:%d: width=%d, hight=%d, pixel_cnt=%d, byte_per_pixel=%d, pixel_bytes=%d %d. \n"
            "data=%x %x %x %x %x %x %x %x %x %x %x %x\n", __FUNCTION__, __LINE__,
            width, hight, pixel_cnt, byte_per_pixel,bmpInfo_out->biSizeImage, pixel_cnt*byte_per_pixel,
            ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],ptr[0xa],ptr[0xb]);

        return 0;
    } else {
        printf("COLOR_BLEND: %s:%d: bad parameters: buf=%p %p %p, bmpHeader=%p %p %p, "
            "bmpInfo=%p %p %p, pixel_ptr=%p %p %p \n",
             __FUNCTION__, __LINE__, buf_src, buf_dst, buf_out,
             bmpHeader_src, bmpHeader_dst, bmpHeader_out,
             bmpInfo_src, bmpInfo_dst, bmpInfo_out,
             pixel_ptr_src, pixel_ptr_dst, pixel_ptr_out);
        return -1;
    }
}

int read_file( char* file_name, UINT8** buf, INT32* size)
{
    int rc=0;
    int len = 0;
    FILE *fp = fopen(file_name, "rb");

    if (fp == NULL)
    {
        printf("COLOR_BLEND: %s:%d: can't open file %s\n", __FUNCTION__, __LINE__, file_name);
        return -1;
    }

    //get file len
    rc = fseek(fp, 0L, SEEK_END); //to end of file
    if(rc != 0){
        printf("COLOR_BLEND: %s:%d: can't seek file %s\n", __FUNCTION__, __LINE__, file_name);
        return -1;
    }
    len = ftell(fp); //file len
    fseek(fp, 0L, SEEK_SET); //move to file head

    *buf = malloc(len);

    rc = fread(*buf, 1, len, fp);
    if(rc != len){
        *size = rc;
    }else{
        *size = len;
    }

    if(fp)
        fclose(fp);

    return rc;
}

int main(int argc,char *argv[]){
    FILE *fp_out = NULL;
    char src_name[MAX_FILE_NAME_LEN]={"src_24.bmp"};
    char dst_name[MAX_FILE_NAME_LEN]={"dst_24.bmp"};
    char out_name[MAX_FILE_NAME_LEN]={"out_24.bmp"};
    UINT8* pBuf_src = NULL;
    UINT8* pBuf_dst = NULL;
    UINT8* pBuf_out = NULL;
    INT32 src_size = 0;
    INT32 dst_size = 0;
    INT32 out_size = 0;
    BITMAPFILEHEADER bmpHeader_src;
    BITMAPFILEHEADER bmpHeader_dst;
    BITMAPFILEHEADER bmpHeader_out;
    BITMAPINFO bmpInfo_src;
    BITMAPINFO bmpInfo_dst;
    BITMAPINFO bmpInfo_out;
    UINT8* srcPixels = NULL;
    UINT8* dstPixels = NULL;
    UINT8* outPixels = NULL;

    UINT32 iAlha_s, iAlha_d;    //RGB's alpha
    UINT32 iRed, iBlue, iGreen;
    UINT32 iLight = 120, iSaturation = 240, iHue = 0;
    INT32 test_case = 0, blend_mode = 0;

    int rc = 0;
    int count =0;
    UINT8 byte=0;
    UINT32 i;

    //if(argc>1){
    //    printf("The command line has %d arguments :\n",argc-1);
    //    for (count = 1; count < argc; ++count) {
    //        printf("%d: %s\n",count,argv[count]);
    //    }
    //}
    if(argc > 1){
        test_case = (UINT32)atoi(argv[1]);

        i = 2;
        while (argv[i]) {
            if (strcmp(argv[i], "-s") == 0) {
                i++;
                if (argv[i])
                    strcpy(src_name, argv[i]);
            }
            if (strcmp(argv[i], "-d") == 0) {
                i++;
                if (argv[i])
                    strcpy(dst_name, argv[i]);
            }
            if (strcmp(argv[i], "-o") == 0) {
                i++;
                if (argv[i])
                    strcpy(out_name, argv[i]);
            }
            if (strcmp(argv[i], "-l") == 0) {
                i++;
                if (argv[i]){
                    iLight = (UINT32)atoi(argv[i]);
                    if(iLight > 240){
                        iLight = 240;
                    }
                }
            }
            if (strcmp(argv[i], "-t") == 0) {
                i++;
                if (argv[i]){
                    iSaturation = (UINT32)atoi(argv[i]);
                    if(iSaturation > 240){
                        iSaturation = 240;
                    }
                }
            }
            if (strcmp(argv[i], "-h") == 0) {
                i++;
                if (argv[i]){
                    iHue = (UINT32)atoi(argv[i]);
                    if(iHue >= 240){
                        iHue = 239;
                    }
                }
            }
            if (strcmp(argv[i], "-rgb") == 0) {
                i++;
                if (argv[i]){
                    iRed = (UINT32)atoi(argv[i]);
                    if(iRed > 255){
                        iRed = 255;
                    }
                }
                i++;
                if (argv[i]){
                    iGreen = (UINT32)atoi(argv[i]);
                    if(iGreen > 255){
                        iGreen = 255;
                    }
                }
                i++;
                if (argv[i]){
                    iBlue = (UINT32)atoi(argv[i]);
                    if(iBlue > 255){
                        iBlue = 255;
                    }
                }
            }
            if (strcmp(argv[i], "-as") == 0) {
                i++;
                if (argv[i]){
                    iAlha_s = (UINT32)atoi(argv[i]);
                    if(iAlha_s > 100){
                        iAlha_s = 100;
                    }
                }
            }
            if (strcmp(argv[i], "-ad") == 0) {
                i++;
                if (argv[i]){
                    iAlha_d = (UINT32)atoi(argv[i]);
                    if(iAlha_d > 100){
                        iAlha_d = 100;
                    }
                }
            }
            if (strcmp(argv[i], "-bm") == 0) {
                i++;
                if (argv[i]){
                    blend_mode = (UINT32)atoi(argv[i]);
                    if(blend_mode > 20){
                        printf("error blend_mode=%d \n", blend_mode);
                        return -1;
                    }
                }
            }
            if (argv[i])
                i++;
        }
    }else{
        print_help();
        return 0;
    }

BEGIN:
    rc = read_file(src_name, &pBuf_src, &src_size);
    if (rc <= 0){
        printf("COLOR_BLEND: %s:%d: can't open bitmap file %s\n", __FUNCTION__, __LINE__, src_name);
        return -1;
    }

    rc = read_file(dst_name, &pBuf_dst, &dst_size);
    if (rc <= 0){
        printf("COLOR_BLEND: %s:%d: can't open bitmap file %s\n", __FUNCTION__, __LINE__, dst_name);
        return -1;
    }

    byte = pBuf_dst[0];
    printf("COLOR_BLEND: %s:%d: name=%s  %s, size src dst=%d %d, pBuf_dst[0]=%x \n",
         __FUNCTION__, __LINE__, src_name, dst_name, src_size, dst_size,byte);

    bmp_parse(pBuf_src, &bmpHeader_src, &bmpInfo_src, &srcPixels);
    bmp_parse(pBuf_dst, &bmpHeader_dst, &bmpInfo_dst, &dstPixels);
    gen_out_info_base_on_src(pBuf_src, &bmpHeader_src, &bmpInfo_src, &srcPixels,
        &pBuf_out, &bmpHeader_out, &bmpInfo_out, &outPixels);

PROCESS_IMAGE:

    switch(test_case){
        case 1:
            rc = get_src_with_light_and_saturation(iLight, iSaturation,
                pBuf_src, &bmpHeader_src, &bmpInfo_src, srcPixels,
                pBuf_out, &bmpHeader_out, &bmpInfo_out, outPixels);
            if(rc == 0){
                out_size = bmpHeader_out.bfSize;
            }
            break;
        case 2:
            rgb_2_color(iRed, iGreen, iBlue);
            break;
        case 3:
            color_2_rgb(iLight, iSaturation, iHue);
            break;
        case 4:
            rc = blend_src_dst(blend_mode, iAlha_s, iAlha_d,
                pBuf_src, &bmpHeader_src, &bmpInfo_src, srcPixels,
                pBuf_dst, &bmpHeader_dst, &bmpInfo_dst, dstPixels,
                pBuf_out, &bmpHeader_out, &bmpInfo_out, outPixels);
            if(rc == 0){
                out_size = bmpHeader_out.bfSize;
            }
            break;
        default:
            print_help();
            break;
    }

END:
    if(out_size)
        fp_out = fopen(out_name, "wb");

    if(fp_out){
        fwrite(pBuf_out, out_size, 1, fp_out);
        printf("COLOR_BLEND: %s:%d: write out bitmap data to %s, size=%d. \n",
             __FUNCTION__, __LINE__, out_name, out_size);
        fclose(fp_out);
    }

RELEASE:
    if(pBuf_dst)
        free(pBuf_dst);

    if(pBuf_src)
        free(pBuf_src);

    if(pBuf_out)
        free(pBuf_out);

    printf("exit \n \n \n");
    return 0;
}
