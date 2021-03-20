///////////////////////////////////
// Some useful Helper functions////
///////////////////////////////////
 

#ifndef _HELPER_H_
#define _HELPER_H_

#include <pspkernel.h>
#include <psppower.h>
#include <pspiofilemgr.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h> 
#include <malloc.h>
#include <zlib.h>
#include <png.h>
#include <pspgu.h>
#include <pspgum.h>
//#include <gif_lib.h>

//#include <gif_lib_private.h> 
#include <ft2build.h>  
#include FT_FREETYPE_H
#define USEFREETYPE

#define printf	pspDebugScreenPrintf 

#define PNG_BYTES_TO_CHECK (4)

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif
 
int TEST_COMPONENTS;
//more useful defines
#define O_RDONLY	0x0001
#define O_WRONLY	0x0002
#define O_RDWR		0x0003
#define O_NBLOCK	0x0010
#define O_APPEND	0x0100
#define O_CREAT		0x0200
#define O_TRUNC		0x0400
#define O_NOWAIT	0x8000

struct ColourVertex
{
	unsigned int colour;
	float x,y,z;
};

#define deg2rad(x)      ((x) * (3.141526 / 180.0))

 
//function definitions
//PNG
unsigned char *loadPNGfromfile(const char *filename,int widp, int heip, int swizzleit);//only swizle with RAM (VRAM swizzling is ineffective)
//unsigned char *loadPNGfromfilereturn(const char *filename,int *widp, int *heip, int swizzleit, int *error);
//unsigned char *loadPNGvramfromfile(const char *filename,int widp, int heip);
//unsigned char *loadPNGvramfromfileoverwrite(const char *filename,int widp, int heip, unsigned int ptr);
//int check_if_png(char *file_name, FILE **fp);
int check_if_png(char *filename);
//FreeType Font
unsigned char *loadTextfromfile(const char *filename, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit);
//unsigned char *loadTextfromfilereturn(const char *filename, int widp, int heip, int *widR, int *heiR, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit);
//void		   loadTextfromfilegetdimension(const char *filename, int *widR, int *heiR, char *text, int textLength, int ptSize);
unsigned char *loadTextfrompreopened(FT_Face face, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit);
//unsigned char *loadTextfrompreopenedhighlight(FT_Face face, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int highCharNum, int colR2, int colG2, int colB2, int swizzleit);//this function changes the colour of one char


unsigned long RoundUpPow2(unsigned long value);
unsigned long RoundUpPow2(unsigned long value)
{
    --value;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    ++value;

    return value;
}
//util
//unsigned char* swizzle(const unsigned char* intex, unsigned int width, unsigned int height);
//void swizzle_fast(u8* out, const u8* in, unsigned int width, unsigned int height);
void swizzle(u8* out, const u8* in, unsigned int width, unsigned int height);
unsigned int ARGBtoHexABGR(int A, int R, int G, int B);

unsigned int ARGBtoHexABGR(int A, int R, int G, int B){

unsigned int finishHex;
finishHex += 0x01000000*(A/2);
finishHex += 0x00010000*B;
finishHex += 0x00000100*G;
finishHex += 0x00000001*R;
int returnVal;
returnVal = finishHex;
return returnVal;
}

unsigned char *fillTextARGB(int bufWid,int bufHei, int A,int R, int G, int B){
int i=0;
unsigned char *tex,*texptr;
texptr = tex;
for(i=0;i<bufWid*bufHei*4;i+=4){
texptr[i] = A;
texptr[i+1] = R;
texptr[i+2] = G;
texptr[i+3] = B;
}
return tex;
}

//Hexadecimal
int intToHex(int val){
int finishHex;
	finishHex = 0x00;//black

	finishHex+=0x01*val;
	int returnVal;
	returnVal = finishHex;
	return returnVal;
}
int matchString(char* data, char* searchTerm, int len){
int x=0;
for(x=0;x<len;x++){
	if(data[x] != searchTerm[x]){
		return 0;//return fail
	}
}
return 1;//return match
}

void reduceString(char* data,int number){
int x=0;
for(x=0;x<4000;x++){
data[x] = data[x+number];
}
}

void ReduceStringOffset(char* data,int number, int offset, int totallen){
int x=0;
for(x=offset;x<totallen;x++){
data[x] = data[x+number];
}
}

void insertStringNull(char* data, int offset, int totallen){
int x=0;
for(x=totallen+1;x>offset;x--){
data[x] = data[x-1];
}
}

int getIntLength(int integer){
	if(integer >= 0){
	if(integer < 10){return 1;}
	if(integer < 100){return 2;}
	if(integer < 1000){return 3;}
	if(integer < 10000){return 4;}
	if(integer < 100000){return 5;}
	if(integer < 1000000){return 6;}
	if(integer < 10000000){return 7;}
	if(integer < 100000000){return 8;}
	}else{
	if(integer > -10){return 2;}
	if(integer > -100){return 3;}
	if(integer > -1000){return 4;}
	if(integer > -10000){return 5;}
	if(integer > -100000){return 6;}
	if(integer > -1000000){return 7;}
	if(integer > -10000000){return 8;}
	if(integer > -100000000){return 9;}
	}

	return 1;
}

/* Build a path, append a directory slash if requested */
void build_path(char *output,const char *root, const char *path, int append)
{
	while(*root != 0)
	{
		*output++ = *root++;
	}

	if(*(root-1) != '/')
	{
		*output++ = '/';
	}

	while(*path != 0)
	{
		*output++ = *path++;
	}
	if(append)
		*output++ = '/';

	*output++ = 0;
}

//misc
unsigned long RoundUpPow2(unsigned long value);
int getMaxInt(int x, int y);
int getMaxFloat(float x, float y);
//void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg);


void DrawScreenQuad(struct Vertex Vert[2],unsigned char *texture, int xpos, int ypos, int width, int height, int texWid, int texHei, int texBufWid, int swizzle);
void DrawScreenQuad(struct Vertex Vert[2],unsigned char *texture, int xpos, int ypos, int width, int height, int texWid, int texHei, int texBufWid, int swizzle){
	if(texture && Vert){
sceGuEnable(GU_BLEND);
sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0xffffffff, 0xffffffff);
Vert[0].x = xpos;
Vert[0].y = ypos;
Vert[0].z = 0;
Vert[0].u = 0;
Vert[0].v = 0;
Vert[1].x = xpos+width;
Vert[1].y = ypos+height;
Vert[1].z = 0;
Vert[1].u = texWid;
Vert[1].v = texHei;

sceGuTexMode(GU_PSM_8888,0,0,swizzle);
sceGuTexImage(0,texWid,texHei,texBufWid,texture);
sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR,GU_LINEAR_MIPMAP_LINEAR);
 
sceGumDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_NORMAL_8BIT|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,Vert);
sceGuDisable(GU_BLEND);
	}

}


unsigned int ramaddr = 0;
unsigned int ramaddr2 = 0;

  
//helper functions
#define VRAM_OFFSET ((512*280*4)*3)
// 0x198000
static unsigned int vramaddr = 0;
/*
unsigned char *loadPNGfromfilereturn(const char *filename,int *widp, int *heip, int swizzleit, int *error)
{
  //this function loads into RAM, not VRAM
	//u32* vram32;
	//u16* vram16;
	//int bufferwidth;
	//int pixelformat;
	//int unknown;

	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;
	FILE *fp;
	int wid, hei;
	//wid = RoundUpPow2(widp);
	//hei = RoundUpPow2(heip);
	int maxWid, maxHei;

	//first, check if it is a png
	pspDebugScreenSetXY(0,9); 
    printf("Opening...");
	int retpng;
	if ((fp = fopen(filename, "rb")) == NULL){error = 1; return NULL;}
	printf("opened");
	retpng = check_if_png(filename);
	if(retpng == 0){fclose(fp);*error = 8;return NULL;}

	//load the PNG
	if ((fp = fopen(filename, "rb")) == NULL){error = 1; return NULL;}
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		*error = 1;
		return NULL;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		*error = 2;
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	wid = RoundUpPow2(width);
	hei = RoundUpPow2(height);
	*widp = width; 
	*heip = height;
	maxWid = getMaxInt(wid,(int)width);
	maxHei = getMaxInt(hei,(int)height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	line = (u32*)memalign(16,RoundUpPow2(width) * 4);
	if (!line) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		*error = 3;
		return NULL;
	}

  int c;
  x=0;
  y=0;	
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output,__attribute__((aligned(16)))*outptr;
  //unsigned char *output2, *outptr2;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  if(!png_ptr){
	  *error = 4;
	  return NULL;
  }
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;
  for (y=0;y<maxHei;y++) {
	  if(y < height){
		  png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		  if(!line){*error = 5;return NULL;}
	  }
		  //line = NULL;}
  for (x=0;x<maxWid;x++){
	  int r = 0x00; 
	  int g = 0x00; 
	  int b = 0x00;
	  int a = 0x00;
	  //if(x < maxWid-width || y < maxHei-height){a = 0x00;}
	  
	  if(x <= width  && y <= height){
	  
			u32 color32 = line[x];
			r = color32 & 0xff; 
			g = (color32 >> 8) & 0xff;
			b = (color32 >> 16) & 0xff;
			a = (color32 >> 24) & 0xff;
			}
		outptr[(int)((x*4)+(y*maxWid*4))] = r;
		outptr[(int)((x*4)+(y*maxWid*4)+1)] = g;
		outptr[(int)((x*4)+(y*maxWid*4)+2)] = b;
		outptr[(int)((x*4)+(y*maxWid*4)+3)] = a;
  }
  }

  fclose(fp);

  *error = 0;

  if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16)))*swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }
}*/

unsigned char *loadPNGfromfile(const char *filename,int widp, int heip, int swizzleit)
{
  //this function loads into RAM, not VRAM
	//u32* vram32;
	//u16* vram16;
	//int bufferwidth;
	//int pixelformat;
	//int unknown;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;
	FILE *fp;
	int wid, hei;
	//wid = RoundUpPow2(widp);
	//hei = RoundUpPow2(heip);
	int maxWid, maxHei;

	//load the PNG
	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, NULL);//user_warning_fn
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	wid = RoundUpPow2(width);
	hei = RoundUpPow2(height);
	//widp = 10; 
	//heip = 10; 
	maxWid = getMaxInt(wid,(int)width);
	maxHei = getMaxInt(hei,(int)height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	line = (u32*)memalign(16,RoundUpPow2(width) * 4);
	if (!line) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

  int c;
  x=0;
  y=0;	
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16))) *output;
  unsigned char __attribute__((aligned(16))) *outptr;
  //unsigned char *output2, *outptr2;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;
  for (y=0;y<maxHei;y++) {
	  if(y < height){
		  png_read_row(png_ptr, (u8*) line, png_bytep_NULL);//}else{
		  //line = NULL;}
  for (x=0;x<maxWid;x++){
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0x00;
	  //if(x < maxWid-width || y < maxHei-height){a = 0x00;}
	  
	  if(x <= width  && y <= height){
	  
			u32 color32 = line[x];
			r = color32 & 0xff; 
			g = (color32 >> 8) & 0xff;
			b = (color32 >> 16) & 0xff;
			a = (color32 >> 24) & 0xff;
			}
		outptr[(int)((x*4)+(y*maxWid*4))] = r;
		outptr[(int)((x*4)+(y*maxWid*4)+1)] = g;
		outptr[(int)((x*4)+(y*maxWid*4)+2)] = b;
		outptr[(int)((x*4)+(y*maxWid*4)+3)] = a;
  }
  }else{for (x=0;x<maxWid;x++){outptr[(int)((x*4)+(y*maxWid*4)+3)] = 0;}}

  }

  fclose(fp);

  if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16)))*swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }
}

/*
unsigned char *loadPNGvramfromfile(const char *filename,int widp, int heip)
{
  //this function loads into RAM, not VRAM
	//u32* vram32;
	//u16* vram16;
	//int bufferwidth;
	//int pixelformat;
	//int unknown;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;
	FILE *fp;
	int wid, hei;
	wid = RoundUpPow2(widp);
	hei = RoundUpPow2(heip);
	int maxWid, maxHei;

	//load the PNG
	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	maxWid = getMaxInt(wid,(int)width);
	maxHei = getMaxInt(hei,(int)height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	line = (u32*) memalign(16,RoundUpPow2(width) * 4);
	if (!line) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

  int c;
  x=0;
  y=0;	
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output,__attribute__((aligned(16)))*outptr;
  unsigned char __attribute__((aligned(16)))*output2, __attribute__((aligned(16)))*outptr2;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  if (vramaddr == 0)
    vramaddr = (0x40000000 | 0x04000000) + VRAM_OFFSET;
  outptr = output = (unsigned char *)vramaddr;

  outptr = output;
  for (y=0;y<height;y++) {
	  //if(y < height){
		  png_read_row(png_ptr, (u8*) line, png_bytep_NULL);//}else{
		  //line = NULL;}
  for (x=0;x<maxWid;x++){
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0x00;
	  //if(x < maxWid-width || y < maxHei-height){a = 0x00;}
	  
	  if(x <= width  && y <= height){
	  
			u32 color32 = line[x];
			r = color32 & 0xff; 
			g = (color32 >> 8) & 0xff;
			b = (color32 >> 16) & 0xff;
			a = (color32 >> 24) & 0xff;
			}
		outptr[(int)((x*4)+(y*maxWid*4))] = r;
		outptr[(int)((x*4)+(y*maxWid*4)+1)] = g;
		outptr[(int)((x*4)+(y*maxWid*4)+2)] = b;
		outptr[(int)((x*4)+(y*maxWid*4)+3)] = a;
  }
  }

  fclose(fp);
  vramaddr += sizediff;
  if ((vramaddr & 0xff) != 0)
    vramaddr = (vramaddr & 0xffffff00) + 0x100;

  return output; 
}


unsigned char *loadPNGvramfromfileoverwrite(const char *filename,int widp, int heip, unsigned int ptr)
{
  //this function loads into RAM, not VRAM
	//u32* vram32;
	//u16* vram16;
	//int bufferwidth;
	//int pixelformat;
	//int unknown;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, x, y;
	u32* line;
	FILE *fp;
	int wid, hei;
	wid = RoundUpPow2(widp);
	hei = RoundUpPow2(heip);
	int maxWid, maxHei;

	//load the PNG
	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	maxWid = getMaxInt(wid,(int)width);
	maxHei = getMaxInt(hei,(int)height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	line = (u32*) memalign(16,RoundUpPow2(width) * 4);
	if (!line) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

  int c;
  x=0;
  y=0;	
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output,__attribute__((aligned(16)))*outptr;
  unsigned char __attribute__((aligned(16)))*output2, __attribute__((aligned(16)))*outptr2;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  //if (vramaddr == 0)vramaddr = (0x40000000 | 0x04000000) + VRAM_OFFSET;
  outptr = output = (unsigned char *)ptr;

  outptr = output;
  for (y=0;y<height;y++) {
	  //if(y < height){
		  png_read_row(png_ptr, (u8*) line, png_bytep_NULL);//}else{
		  //line = NULL;}
  for (x=0;x<maxWid;x++){
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0x00;
	  //if(x < maxWid-width || y < maxHei-height){a = 0x00;}
	  
	  if(x <= width  && y <= height){
	  
			u32 color32 = line[x];
			r = color32 & 0xff; 
			g = (color32 >> 8) & 0xff;
			b = (color32 >> 16) & 0xff;
			a = (color32 >> 24) & 0xff;
			}
		outptr[(int)((x*4)+(y*maxWid*4))] = r;
		outptr[(int)((x*4)+(y*maxWid*4)+1)] = g;
		outptr[(int)((x*4)+(y*maxWid*4)+2)] = b;
		outptr[(int)((x*4)+(y*maxWid*4)+3)] = a;
  }
  }

  fclose(fp);
  //vramaddr += sizediff;
  //if ((vramaddr & 0xff) != 0)
    // vramaddr = (vramaddr & 0xffffff00) + 0x100;

  return output; 
}
*/
//#ifdef USEFREETYPE
unsigned char *loadTextfromfile(const char *filename, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit){
FT_Library fntLibrary;
int error;
//load library
error = FT_Init_FreeType( &fntLibrary );
if(error){printf("cannot open font library"); return NULL;}

FT_Face face;
error = FT_New_Face( fntLibrary, filename, 0, &face ); 
if ( error == FT_Err_Unknown_File_Format ) { printf("Font error"); return NULL;} else if ( error ) { printf("Font error"); return NULL;}

error = FT_Set_Pixel_Sizes( face,  0,  ptSize );

FT_UInt glyph_index;
int i;
int c;
  int x=0;
  int y=0;	
  int maxWid, maxHei;
  maxWid = RoundUpPow2(widp);
  maxHei = RoundUpPow2(heip);
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16))) *output;
  unsigned char __attribute__((aligned(16))) *outptr;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;

  //first, blank off the whole image to avoid artifacts
  int b=0;
  for(b=0;b<sizediff;b++){
	outptr[b] = 0x00;
  }

int xpos, ypos;
xpos = 0;
ypos = 0;
int max_descent = 0;
int max_lateral = 0;

for(i=0;i<textLength;i++){
//turn ASCII code into a glyph index
glyph_index = FT_Get_Char_Index( face, text[i] );
//load the needed glyph
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
//turn the glyph into an anti-aliased bitmap
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );

if(face->glyph->bitmap.rows > max_descent){max_descent = face->glyph->bitmap.rows;}
if(face->glyph->bitmap.width > max_lateral){max_lateral = face->glyph->bitmap.width;}
if(text[i] == 10 || text[i] == 13){
//enter character, so newline
xpos = -face->glyph->bitmap.width;
ypos += max_descent+2;
}else{
  for (y=0;y<face->glyph->bitmap.rows;y++) {
		  //png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
	for (x=0;x<face->glyph->bitmap.width;x++){ 
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0xff;
	  
			//u32 color32 = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.pitch)];
			//r = color32 & 0xff; 
			//g = (color32 >> 8) & 0xff;
			//b = (color32 >> 16) & 0xff;
			//a = (color32 >> 24) & 0xff;

		int hr = 0x00;
		int hg = 0x00;
		int hb = 0x00;
		hr = intToHex(colR);
		hg = intToHex(colG);
		hb = intToHex(colB);

		if((int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3) < sizediff){

		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4))] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hr;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+1)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hg;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+2)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hb;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)];
		}
  } 
  }
}//end else 'return'
  //From FA++
			
  int advance;
  advance = (face->glyph->metrics.horiAdvance >> 6)+3;//face->glyph->bitmap.width+1;
  xpos += advance;
	if (xpos+advance+max_lateral > widp-2){
			xpos = -face->glyph->bitmap.width;
			advance = 0;
			ypos += max_descent+2;//face->glyph->bitmap.rows;  
		}

}

FT_Done_FreeType(fntLibrary);

if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16)))*swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }
}


unsigned char *loadTextfrompreopened(FT_Face face, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit){
	if(face){
		int error;
error = FT_Set_Pixel_Sizes( face,  0,  ptSize );

FT_UInt glyph_index;
int i;
int c;
  int x=0;
  int y=0;	
  int maxWid, maxHei;
  maxWid = RoundUpPow2(widp);
  maxHei = RoundUpPow2(heip);
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output;
  unsigned char __attribute__((aligned(16)))*outptr;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;

  //first, blank off the whole image to avoid artifacts
  int b=0;
  for(b=0;b<sizediff;b++){
	outptr[b] = 0x00;
  }

int xpos, ypos;
xpos = 0;
ypos = 0;
int max_descent = 0;
int max_lateral = 0;

for(i=0;i<textLength;i++){
//turn ASCII code into a glyph index
glyph_index = FT_Get_Char_Index( face, text[i] );
//load the needed glyph
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
//turn the glyph into an anti-aliased bitmap
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );

if(face->glyph->bitmap.rows > max_descent){max_descent = face->glyph->bitmap.rows;}
if(face->glyph->bitmap.width > max_lateral){max_lateral = face->glyph->bitmap.width;}
if(text[i] == 10 || text[i] == 13){
//enter character, so newline
xpos = -face->glyph->bitmap.width;
ypos += max_descent+2;
}else{
  for (y=0;y<face->glyph->bitmap.rows;y++) {
		  //png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
	for (x=0;x<face->glyph->bitmap.width;x++){ 
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0xff;
	  
			//u32 color32 = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.pitch)];
			//r = color32 & 0xff; 
			//g = (color32 >> 8) & 0xff;
			//b = (color32 >> 16) & 0xff;
			//a = (color32 >> 24) & 0xff;

		int hr = 0x00;
		int hg = 0x00;
		int hb = 0x00;
		hr = intToHex(colR);
		hg = intToHex(colG);
		hb = intToHex(colB);

		if((int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3) < sizediff){

		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4))] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hr;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+1)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hg;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+2)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hb;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)];
		}
  } 
  }
}//end else 'return'
  //From FA++
			
  int advance;
  advance = (face->glyph->metrics.horiAdvance >> 6)+3;//face->glyph->bitmap.width+1;
  xpos += advance;
	if (xpos+advance+max_lateral > widp-2){
			xpos = -face->glyph->bitmap.width;
			advance = 0;
			ypos += max_descent+2;//face->glyph->bitmap.rows;  
		}

}

if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16))) *swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }

	}else{return NULL;}
}
//#endif
/*
unsigned char *loadTextfromfilereturn(const char *filename, int widp, int heip, int *widR, int *heiR, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit){
FT_Library fntLibrary;
int error;
//load library
error = FT_Init_FreeType( &fntLibrary );
if(error){printf("cannot open font library"); return NULL;}

FT_Face face;
error = FT_New_Face( fntLibrary, filename, 0, &face ); 
if ( error == FT_Err_Unknown_File_Format ) { printf("Font error"); return NULL;} else if ( error ) { printf("Font error"); return NULL;}

error = FT_Set_Pixel_Sizes( face,  0,  ptSize );

FT_UInt glyph_index;
int i;
int c;
  int x=0;
  int y=0;	
  int maxWid, maxHei;
  maxWid = RoundUpPow2(widp);
  maxHei = RoundUpPow2(heip);

  int countWid = 0;
  int countHei = 0;
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output,__attribute__((aligned(16)))*outptr;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;

  //first, blank off the whole image to avoid artifacts
  int b=0;
  for(b=0;b<sizediff;b++){
	outptr[b] = 0x00;
  }

int xpos, ypos;
xpos = 0;
ypos = 0;
int max_descent = 0;
int max_lateral = 0;

for(i=0;i<textLength;i++){
//turn ASCII code into a glyph index
glyph_index = FT_Get_Char_Index( face, text[i] );
//load the needed glyph
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
//turn the glyph into an anti-aliased bitmap
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );

if(face->glyph->bitmap.rows > max_descent){max_descent = face->glyph->bitmap.rows;}
if(face->glyph->bitmap.width > max_lateral){max_lateral = face->glyph->bitmap.width;}
if(text[i] == 10 || text[i] == 13){
//enter character, so newline
xpos = -face->glyph->bitmap.width;
ypos += max_descent+2;
countHei += face->glyph->bitmap.rows;
}else{
	if(countHei <= 0){countHei = face->glyph->bitmap.rows;}
  for (y=0;y<face->glyph->bitmap.rows;y++) {
		  //png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
	for (x=0;x<face->glyph->bitmap.width;x++){ 
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0xff;
	  
			//u32 color32 = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.pitch)];
			//r = color32 & 0xff; 
			//g = (color32 >> 8) & 0xff;
			//b = (color32 >> 16) & 0xff;
			//a = (color32 >> 24) & 0xff;

		int hr = 0x00;
		int hg = 0x00;
		int hb = 0x00;
		hr = intToHex(colR);
		hg = intToHex(colG);
		hb = intToHex(colB);

		if((int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3) < sizediff){

		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4))] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hr;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+1)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hg;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+2)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hb;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)];
		}
  } 
  }
}//end else 'return'
  //From FA++
			
  int advance;
  advance = (face->glyph->metrics.horiAdvance >> 6)+3;//face->glyph->bitmap.width+1;
  //if(i==textLength-1){if(text[i] != ' '){countWid += advance;}}
  xpos += advance;
  countWid += advance;
	if (xpos+advance+max_lateral > widp-2){
			xpos = -face->glyph->bitmap.width;
			advance = 0;
			ypos += max_descent+2;//face->glyph->bitmap.rows;  
		}

}

*widR = countWid;
*heiR = max_descent;

FT_Done_FreeType(fntLibrary);

if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16)))*swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }
}

void loadTextfromfilegetdimension(const char *filename, int *widR, int *heiR, char *text, int textLength, int ptSize){
	FT_Library fntLibrary;
int error;
//load library
error = FT_Init_FreeType( &fntLibrary );
if(error){printf("cannot open font library"); return NULL;}

FT_Face face;
error = FT_New_Face( fntLibrary, filename, 0, &face ); 
if ( error == FT_Err_Unknown_File_Format ) { printf("Font error"); return NULL;} else if ( error ) { printf("Font error"); return NULL;}

error = FT_Set_Pixel_Sizes( face,  0,  ptSize );

FT_UInt glyph_index;
int i;
int c;
  int x=0;
  int y=0;	
  //int maxWid, maxHei;
  //maxWid = RoundUpPow2(widp);
  //maxHei = RoundUpPow2(heip);

  int countWid = 0;
  int countHei = 0;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;


int xpos, ypos;
xpos = 0;
ypos = 0;
int max_descent = 0;
int max_lateral = 0;
pspDebugScreenSetXY(0,9); 
int spaceAdvance;
glyph_index = FT_Get_Char_Index( face, '_' ); 
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );
spaceAdvance = ((face->glyph->metrics.horiAdvance >> 6)+3)-2;
//spaceAdvance = 12;

for(i=0;i<textLength;i++){
//turn ASCII code into a glyph index
glyph_index = FT_Get_Char_Index( face, text[i] );
//load the needed glyph
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
//turn the glyph into an anti-aliased bitmap
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );

if(face->glyph->bitmap.rows > max_descent){max_descent = face->glyph->bitmap.rows;}
if(face->glyph->bitmap.width > max_lateral){max_lateral = face->glyph->bitmap.width;}
if(text[i] == 10 || text[i] == 13){
//enter character, so newline 
xpos = -face->glyph->bitmap.width;
ypos += max_descent+2;
countHei += face->glyph->bitmap.rows;
}else{
	if(countHei <= 0){countHei = face->glyph->bitmap.rows;}
  
  } 
//}//end else 'return'
  //From FA++
			
  int advance;
  advance = (face->glyph->metrics.horiAdvance >> 6)+3;//face->glyph->bitmap.width+1;
  //advance = face->glyph->bitmap.width+3;
  if(text[i] == ' '){advance = spaceAdvance;}
  //if(i==textLength-1){if(text[i] != ' '){countWid += advance;}}
  printf("Advancing: %i/n", advance);
  xpos += advance;
  countWid += advance;
  
	//if (xpos+advance+max_lateral > 512){
	//		xpos = -face->glyph->bitmap.width;
	//		advance = 0;
	//		ypos += max_descent+2;//face->glyph->bitmap.rows;  
	//	}

}

*widR = countWid;
*heiR = max_descent;

FT_Done_FreeType(fntLibrary);
}

unsigned char *loadTextfrompreopened(FT_Face face, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int swizzleit){
	if(face){
		int error;
error = FT_Set_Pixel_Sizes( face,  0,  ptSize );

FT_UInt glyph_index;
int i;
int c;
  int x=0;
  int y=0;	
  int maxWid, maxHei;
  maxWid = RoundUpPow2(widp);
  maxHei = RoundUpPow2(heip);
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output,__attribute__((aligned(16)))*outptr;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;

  //first, blank off the whole image to avoid artifacts
  int b=0;
  for(b=0;b<sizediff;b++){
	outptr[b] = 0x00;
  }

int xpos, ypos;
xpos = 0;
ypos = 0;
int max_descent = 0;
int max_lateral = 0;

for(i=0;i<textLength;i++){
//turn ASCII code into a glyph index
glyph_index = FT_Get_Char_Index( face, text[i] );
//load the needed glyph
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
//turn the glyph into an anti-aliased bitmap
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );

if(face->glyph->bitmap.rows > max_descent){max_descent = face->glyph->bitmap.rows;}
if(face->glyph->bitmap.width > max_lateral){max_lateral = face->glyph->bitmap.width;}
if(text[i] == 10 || text[i] == 13){
//enter character, so newline
xpos = -face->glyph->bitmap.width;
ypos += max_descent+2;
}else{
  for (y=0;y<face->glyph->bitmap.rows;y++) {
		  //png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
	for (x=0;x<face->glyph->bitmap.width;x++){ 
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0xff;
	  
			//u32 color32 = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.pitch)];
			//r = color32 & 0xff; 
			//g = (color32 >> 8) & 0xff;
			//b = (color32 >> 16) & 0xff;
			//a = (color32 >> 24) & 0xff;

		int hr = 0x00;
		int hg = 0x00;
		int hb = 0x00;
		hr = intToHex(colR);
		hg = intToHex(colG);
		hb = intToHex(colB);

		if((int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3) < sizediff){

		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4))] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hr;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+1)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hg;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+2)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hb;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)];
		}
  } 
  }
}//end else 'return'
  //From FA++
			
  int advance;
  advance = (face->glyph->metrics.horiAdvance >> 6)+3;//face->glyph->bitmap.width+1;
  xpos += advance;
	if (xpos+advance+max_lateral > widp-2){
			xpos = -face->glyph->bitmap.width;
			advance = 0;
			ypos += max_descent+2;//face->glyph->bitmap.rows;  
		}

}

if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16))) *swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }

	}else{return NULL;}
}

unsigned char *loadTextfrompreopenedhighlight(FT_Face face, int widp, int heip, char *text, int textLength, int ptSize, int colR, int colG, int colB, int highCharNum, int colR2, int colG2, int colB2, int swizzleit){
	if(face){
		int error;
error = FT_Set_Pixel_Sizes( face,  0,  ptSize );

FT_UInt glyph_index;
int i;
int c;
  int x=0;
  int y=0;	
  int maxWid, maxHei;
  maxWid = RoundUpPow2(widp);
  maxHei = RoundUpPow2(heip);
  //unsigned char *input = inptr;
  unsigned char __attribute__((aligned(16)))*output,__attribute__((aligned(16)))*outptr;
  size_t sizediff;
  sizediff = maxWid*maxHei*4;
  //if (ramaddr == 0)
    //ramaddr = (unsigned int)malloc(sizediff);//(0x40000000 | 0x04000000) + VRAM_OFFSET;
  c=0;
  ramaddr = (unsigned int)memalign(16,sizediff);
  outptr = output = (unsigned char *)ramaddr;
  outptr = output;

  //first, blank off the whole image to avoid artifacts
  int b=0;
  for(b=0;b<sizediff;b++){
	outptr[b] = 0x00;
  }

int xpos, ypos;
xpos = 0;
ypos = 0;
int max_descent = 0;
int max_lateral = 0;

for(i=0;i<textLength;i++){
//turn ASCII code into a glyph index
glyph_index = FT_Get_Char_Index( face, text[i] );
//load the needed glyph
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
//turn the glyph into an anti-aliased bitmap
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal );

if(face->glyph->bitmap.rows > max_descent){max_descent = face->glyph->bitmap.rows;}
if(face->glyph->bitmap.width > max_lateral){max_lateral = face->glyph->bitmap.width;}
if(text[i] == 10 || text[i] == 13){
//enter character, so newline
xpos = -face->glyph->bitmap.width;
ypos += max_descent+2;
}else{
  for (y=0;y<face->glyph->bitmap.rows;y++) {
		  //png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
	for (x=0;x<face->glyph->bitmap.width;x++){ 
	  int r = 0xff; 
	  int g = 0xff; 
	  int b = 0xff;
	  int a = 0xff;
	  
			//u32 color32 = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.pitch)];
			//r = color32 & 0xff; 
			//g = (color32 >> 8) & 0xff;
			//b = (color32 >> 16) & 0xff;
			//a = (color32 >> 24) & 0xff;

		int hr = 0x00;
		int hg = 0x00;
		int hb = 0x00;
		if(i == highCharNum){
		hr = intToHex(colR2);
		hg = intToHex(colG2);
		hb = intToHex(colB2);
		}else{
		hr = intToHex(colR);
		hg = intToHex(colG);
		hb = intToHex(colB);
		}

		if((int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3) < sizediff){

		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4))] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hr;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+1)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hg;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+2)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)] & hb;
		outptr[(int)(((x+xpos)*4)+((y+ptSize+ypos-(face->glyph->metrics.horiBearingY >> 6))*maxWid*4)+3)] = face->glyph->bitmap.buffer[x+(y*face->glyph->bitmap.width)];
		}
  } 
  }
}//end else 'return'
  //From FA++
			
  int advance;
  advance = (face->glyph->metrics.horiAdvance >> 6)+3;//face->glyph->bitmap.width+1;
  xpos += advance;
	if (xpos+advance+max_lateral > widp-2){
			xpos = -face->glyph->bitmap.width;
			advance = 0;
			ypos += max_descent+2;//face->glyph->bitmap.rows;  
		}

}

if(swizzleit == 0){
  return output; 
  }else{
  //return output;
  unsigned char __attribute__((aligned(16)))*swizzled;
  swizzled = (unsigned char*)memalign(16,maxWid*maxHei*4);
	swizzle(swizzled,output,maxWid,maxHei);
	//free old texture
	free(output);
	return swizzled;
  }

	}else{return NULL;}
}*/

/*
unsigned long RoundUpPow2(unsigned long value)
{
    --value;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    ++value;

    return value;
}*/


//I would use a macro for these,but they don't seem to work v_v
int getMaxInt(int x, int y){
	if(x > y){return x;}else{return y;}
}
int getMaxFloat(float x, float y){
	if(x > y){return x;}else{return y;}
}
/*
void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
	// ignore PNG warnings
	int x;
}*/

int check_if_png(char *filename)
{
	/*
   png_byte buf[PNG_BYTES_TO_CHECK];

   pspDebugScreenSetXY(0,10);
   printf("Checking PNG Validity");

   // Read in some of the signature bytes 
   //if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK){printf("fread failed");
   //   return 0;}
   SceUID fileh;
   fileh = sceIoOpen((const char*)filename,   O_RDWR | O_CREAT, 0777);

   (void) sceIoRead(fileh,buf,PNG_BYTES_TO_CHECK);

   // Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
   //   Return nonzero (true) if they match 

   //printf("Returning Val: %i", (int)(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK)));

   sceIoClose(fileh);

   return(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));*/
	return 0;
}

//swizzles the texture (only use with static textures, else too slow)
void swizzle(u8* out, const u8* in, unsigned int width, unsigned int height)
{
	//note this function is modified to take into account that 1 pixel (32bit) is
	//actually 4 u8, so if you don't want this, change width*4 to width
   unsigned int i,j;
   unsigned int rowblocks = ((width*4) / 16);
 
   for (j = 0; j < height; ++j)
   {
      for (i = 0; i < (width*4); ++i)
      {
         unsigned int blockx = i / 16;
         unsigned int blocky = j / 8;
 
         unsigned int x = (i - blockx*16);
         unsigned int y = (j - blocky*8);
         unsigned int block_index = blockx + ((blocky) * rowblocks);
         unsigned int block_address = block_index * 16 * 8;
 
         out[block_address + x + y * 16] = in[i+j*(width*4)];
      }
   }
}

#endif

