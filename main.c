#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
    printf("0: print help! \n"
        "1: ./colorBlend 1 [light] [saturation]. Get src's red color with light and saturation adjustable: get_src_with_light_and_saturation()! \n"
        "2: print help! \n");
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
        //printf("COLOR_BLEND: bmp_parse: data=%x %x %x %x %x %x %x %x \n",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7]);
        
        data = (((UINT32)ptr[5])<<24) + (((UINT32)ptr[4])<<16) + (((UINT32)ptr[3])<<8) + (UINT32)ptr[2];
        //printf("COLOR_BLEND: bmp_parse: bfType=0x%x, bfSize=0x%x ,bfOffBits=0x%x, data=0x%x\n",bmpHeader->bfType, bmpHeader->bfSize, bmpHeader->bfOffBits, data);
        
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
        //printf("COLOR_BLEND: bmp_parse: biSize=0x%x, biWidth=%d ,biHeight=%d, biPlanes=%d, biBitCount=%d, biCompression=%d, biSizeImage=0x%x,"
        //    "  biXpelsPerMeter=%d, biYpelsPerMeter=%d, biClrUsed=%d, biClrImportant=%d\n",
        //    bmpInfo->biSize, bmpInfo->biWidth, bmpInfo->biHeight, bmpInfo->biPlanes, 
        //    bmpInfo->biBitCount, bmpInfo->biCompression, bmpInfo->biSizeImage, bmpInfo->biXpelsPerMeter,
        //    bmpInfo->biYpelsPerMeter, bmpInfo->biClrUsed, bmpInfo->biClrImportant);
            
        *pixel_ptr = &ptr[bmpHeader->bfOffBits];
        //printf("COLOR_BLEND: bmp_parse: ptr=%p, pixel_ptr=%p \n", ptr,*pixel_ptr);

        return 0;
    } else{
        printf("COLOR_BLEND: bmp_parse: bad parameters: buf=%p, bmpHeader=%p, bmpInfo=%p, *pixel_ptr=%p \n",
            buf, bmpHeader, bmpInfo, *pixel_ptr);
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
            
            //printf("COLOR_BLEND: gen_out_info_base_on_src: bfType=0x%x, bfSize=0x%x ,bfOffBits=0x%x, sizeof(bmpHeader_src)=%ld\n",
            //    bmpHeader_out->bfType, bmpHeader_out->bfSize, bmpHeader_out->bfOffBits, sizeof(BITMAPFILEHEADER));
        
            
            ptr = *buf_out;
            *pixel_ptr_out = &ptr[bmpHeader_out->bfOffBits];
            
           // printf("COLOR_BLEND: gen_out_info_base_on_src: buf_out=%p, pixel_ptr_out=%p, offset=%x, data=%x %x %x %x %x %x %x %x %x %x %x %x\n", 
           //     *buf_out,*pixel_ptr_out, bmpHeader_out->bfOffBits, 
           //     ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],ptr[0xa],ptr[0xb]);
        }
        
        return 0;
    } else {
        printf("COLOR_BLEND: gen_out_info_base_on_src: bad parameters: buf=%p %p, bmpHeader=%p %p, bmpInfo=%p %p, *pixel_ptr=%p %p \n",
            buf_src, *buf_out, bmpHeader_src,bmpHeader_out, bmpInfo_src,bmpInfo_out, *pixel_ptr_src,*pixel_ptr_out);
        return -1;
    }
}

inline float convert_RGB_by_light(float fRGB, UINT32 iLight, float fLight){
    float fNew_rgb;

    if(iLight >= 120){       
        //fRGB_new = fRGB+(1.0-fRGB)*fLight
        fNew_rgb = fRGB + (1.0 - fRGB) * fLight;
    }else{        
        //fRGB_new = fRGB*fLight
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

int get_src_with_light_and_saturation(int argc, char *argv[], 
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
    //UINT32 iRed, iBlue, iGreen;
    UINT32 iLight = 120, iSaturation = 240;
    
    //get input para
    if(argc > 2){
        iLight = (UINT32)atoi(argv[2]);
        if(iLight > 240){
            iLight = 240;
        }        
    }
        
    if(argc > 3){
        iSaturation = (UINT32)atoi(argv[3]);
        if(iSaturation > 240){
            iSaturation = 240;
        }
    }
    
    //calculate the light ration
    if(iLight >= 120){       
        //fRGB_new = fRGB+(1.0-fRGB)*fLight
        fLight = ((float)iLight - 120)/120;
    }else{        
        //fRGB_new = fRGB*fLight
        fLight = ((float)iLight)/120;
    }
    //calculate the saturation ratio
    if(1){       
        fSaturation = ((float)iSaturation)/240;
    }
    
    printf("COLOR_BLEND: get_src_with_light_and_saturation: iLight=%d, iSaturation=%d \n", iLight,iSaturation);
    
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
            
            printf("COLOR_BLEND: get_src_with_light_and_saturation: width=%d, hight=%d, pixel_cnt=%d, byte_per_pixel=%d, pixel_bytes=%d %d. \n"
                "data=%x %x %x %x %x %x %x %x %x %x %x %x\n", 
                width, hight, pixel_cnt, byte_per_pixel,bmpInfo_out->biSizeImage, pixel_cnt*byte_per_pixel,
                ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],ptr[0xa],ptr[0xb]);
        
        return 0;
    } else {
        printf("COLOR_BLEND: get_src_with_light_and_saturation: bad parameters: buf=%p %p, bmpHeader=%p %p, bmpInfo=%p %p, pixel_ptr=%p %p \n",
            buf_src,buf_out, bmpHeader_src,bmpHeader_out, bmpInfo_src,bmpInfo_out, pixel_ptr_src, pixel_ptr_out);
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
        printf("COLOR_BLEND: read_file: can't open file %s\n",file_name);
        return -1;
    }
    
    //获取文件长度
	rc = fseek(fp, 0L, SEEK_END); //定位到文件末 
	if(rc != 0){
	    printf("COLOR_BLEND: read_file: can't seek file %s\n",file_name);
	    return -1;
	}
	len = ftell(fp); //文件长度
	fseek(fp, 0L, SEEK_SET); //恢复到文件头
    
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
    char src_name[]={"src_24.bmp"};
    char dst_name[]={"dst_24.bmp"};
    char out_name[]={"out_24.bmp"};
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
    
    INT32 test_case = 0;
    int rc = 0;
    int count =0;
    UINT8 byte=0;
    
    //if(argc>1){
    //    printf("The command line has %d arguments :\n",argc-1);
    //    for (count = 1; count < argc; ++count) {
    //        printf("%d: %s\n",count,argv[count]);
    //    }
    //}
    if(argc > 1)
        test_case = (UINT32)atoi(argv[1]);
    else{
        print_help();
        return 0;
    }
    
BEGIN:
    rc = read_file(src_name, &pBuf_src, &src_size);
    if (rc <= 0){
        printf("COLOR_BLEND: MAIN: can't open bitmap file %s\n",src_name);
        return -1;
    }

    rc = read_file(dst_name, &pBuf_dst, &dst_size);
    if (rc <= 0){
        printf("COLOR_BLEND: MAIN: can't open bitmap file %s\n",dst_name);
        return -1;
    }
    
    byte = pBuf_dst[0];
    printf("COLOR_BLEND: MAIN: name=%s  %s, size src dst=%d %d, pBuf_dst[0]=%x \n",src_name, dst_name, src_size, dst_size,byte);

    bmp_parse(pBuf_src, &bmpHeader_src, &bmpInfo_src, &srcPixels);    
    bmp_parse(pBuf_dst, &bmpHeader_dst, &bmpInfo_dst, &dstPixels);  
    gen_out_info_base_on_src(pBuf_src, &bmpHeader_src, &bmpInfo_src, &srcPixels, 
        &pBuf_out, &bmpHeader_out, &bmpInfo_out, &outPixels);
    
PROCESS_IMAGE:
    
    switch(test_case){
        case 0:
            print_help();
            break;
        case 1:
            get_src_with_light_and_saturation(argc, argv, pBuf_src, &bmpHeader_src, &bmpInfo_src, srcPixels, 
                pBuf_out, &bmpHeader_out, &bmpInfo_out, outPixels);
            out_size = bmpHeader_out.bfSize;
            break;
        case 2:
            print_help();
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
        printf("COLOR_BLEND: MAIN: write out bitmap data to %s, size=%d. \n", out_name, out_size);
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
