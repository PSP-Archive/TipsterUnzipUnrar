/*
AnonymousTipster's Unzip Plugin/App
Based on MiniZip for zlib
*/

#include <pspdebug.h>
#include <psppower.h>
#include <pspiofilemgr.h>
#include <pspgu.h>
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

#include "Fonts.h"

//#include "unrarlib/rarmainH.h"

#define RENDER_OPTIONS_02 GU_TEXTURE_32BITF|GU_NORMAL_8BIT|GU_VERTEX_32BITF|GU_TRANSFORM_2D

struct Vertex
{
	float u, v;
	char nx,ny,nz;
	float x,y,z;
};

#include "helper.h"

#include "unzip.h"

//#define IRSHELL_PLUGIN
int irshell = 0;//is launched as plugin?

#define DANZEFF_SCEGU
#include "danzeff.h"

PSP_MODULE_INFO("AnTi Unzip", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define printf	pspDebugScreenPrintf

static unsigned int __attribute__((aligned(16))) list[262144];


#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

static unsigned short __attribute__((aligned(16))) mainMenuPixels[512*256];
int mainMenuSelected = 0;
char* extractDir;
char* password;
int changeOptionCount = 0;
int keyboardOpen = 0;
static char keyboardString[170];
static int currKeyboard = 0;//current char of the keyboard string
void (*keyboardReturnFunc)();

//unsigned char __attribute__((aligned(16)))*titleText_temp;
//struct Vertex __attribute__((aligned(32))) titleTextVerts[2];
//unsigned char __attribute__((aligned(16)))*titleText2_temp;
//struct Vertex __attribute__((aligned(32))) titleText2Verts[2];
unsigned char __attribute__((aligned(16)))*zipBackTex_temp;
unsigned char __attribute__((aligned(16)))*rarBackTex_temp;
struct Vertex __attribute__((aligned(32))) titleBackVerts[2];

unsigned char __attribute__((aligned(16)))*zipText_temp;
struct Vertex __attribute__((aligned(32))) zipTextVerts[2];
unsigned char __attribute__((aligned(16)))*extractDirText_temp;
struct Vertex __attribute__((aligned(32))) extractDirTextVerts[2];
unsigned char __attribute__((aligned(16)))*extractText_temp;
struct Vertex __attribute__((aligned(32))) extractTextVerts[2];
unsigned char __attribute__((aligned(16)))*exitText_temp;
struct Vertex __attribute__((aligned(32))) exitTextVerts[2];
unsigned char __attribute__((aligned(16)))*extraText_temp;
struct Vertex __attribute__((aligned(32))) extraTextVerts[2];
unsigned char __attribute__((aligned(16)))*typingText_temp;
struct Vertex __attribute__((aligned(32))) typingTextVerts[2];

int redrawMenuText = 1;

#define video_directory "ms0:/PSP/VIDEO/"

char *zip_argv1;

int appState = 1;//0=zip 1=rar 2=extras
int returnAppState = 1;
int extrasState = 0;
int extrasMenuSelected = 0;

struct ZipEntry
{
	char *name;
	//char ratio[16];
};

int contentsY = 0;
int showingContents = 0;

int zipEntriesTotal = 0;
int contentsSelected = 0;

int browseType = 0;


struct GameEntry
{
	char *name;
	//char ratio[16];
};


int gameEntriesTotal = 0;
int gameDirSelected = 0;

struct DirEntry
{
	char *name;
	int type;
	//char ratio[16];
};

int browseEntriesTotal = 0;
int browseDirSelected = 0;
char* browseDirPath;

#define MAX_ENTRIES 1000


struct ZipEntry zipEntry[MAX_ENTRIES];
struct GameEntry gameEntry[MAX_ENTRIES];
struct DirEntry dirEntry[MAX_ENTRIES];
struct DirEntry tempDirEntry[MAX_ENTRIES];
//struct ZipEntry zipEntry2[MAX_ENTRIES];

void drawTexture2D(int x, int y, float wid, float height, int bx, int by, int bw, const void* tex, int texMode);

void screen_init()
	{
	pspDebugScreenSetTextColor(0xffffff);
	pspDebugScreenSetBackColor(0x000000);
	pspDebugScreenInit();
	}


void print_error(char *s)
{
	screen_init();
	pspDebugScreenSetXY(0, 0);
	pspDebugScreenPrintf("Error:\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("\"%s\"\n", s);
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("Press X");
	}


char *static_init()
	{

	return(0);
	}


int init()
	{
	char *result = static_init();
	if (result != 0)
		{
        print_error(result);

		return(0);
		}


	return(1);
	}

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

unsigned int ARGBToHexABGR(int A, int R, int G, int B);
unsigned int ARGBToHexABGR(int A, int R, int G, int B){

unsigned int finishHex;
finishHex += 0x01000000*(A/2);
finishHex += 0x00010000*B;
finishHex += 0x00000100*G;
finishHex += 0x00000001*R;
int returnVal;
returnVal = finishHex;
return returnVal;
}


void keyboardCallbackDirectory(){
	int k=0;
	for(k=0;k<512;k++){
	extractDir[k] = '\0';
	}
	for(k=0;k<currKeyboard;k++){
	extractDir[k] = keyboardString[k];
	}
	extractDir[currKeyboard] = '\0';
	if(extractDir[strlen(extractDir)-1] != '/'){extractDir[strlen(extractDir)] = '/';}
}
void keyboardCallbackFile(){
	int k=0;
	for(k=0;k<currKeyboard;k++){
	zip_argv1[k] = keyboardString[k];
	}
	zip_argv1[currKeyboard] = '\0';
}
void keyboardCallbackPassword(){
	int k=0;
	password[0] = '\0';
	for(k=0;k<currKeyboard;k++){
	password[k] = keyboardString[k];
	}
	password[currKeyboard] = '\0';
	if(password[0] == '\0'){SetUsepassword(0);}else{SetUsepassword(1);}

}

int do_list_to_struct(uf)
    unzFile uf;
{
    uLong i;
    unz_global_info gi;
    int err;

    err = unzGetGlobalInfo (uf,&gi);
    if (err!=UNZ_OK)
        printf("error %d with zipfile in unzGetGlobalInfo \n",err);
    //printf(" Length  Method   Size  Ratio   Date    Time   CRC-32     Name\n");
    //printf(" ------  ------   ----  -----   ----    ----   ------     ----\n");
	zipEntriesTotal = (int)gi.number_entry;
    for (i=0;i<gi.number_entry;i++)
    {
        char filename_inzip[256];
        unz_file_info file_info;
        uLong ratio=0;
        const char *string_method;
        char charCrypt=' ';
        err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
        if (err!=UNZ_OK)
        {
            printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
            break;
        }
        if (file_info.uncompressed_size>0)
            ratio = (file_info.compressed_size*100)/file_info.uncompressed_size;

		//sprintf(zipEntry[i].ratio,"%7lu",ratio);

        /* display a '*' if the file is crypted */
        if ((file_info.flag & 1) != 0)
            charCrypt='*';

        if (file_info.compression_method==0)
            string_method="Stored";
        else
        if (file_info.compression_method==Z_DEFLATED)
        {
            uInt iLevel=(uInt)((file_info.flag & 0x6)/2);
            if (iLevel==0)
              string_method="Defl:N";
            else if (iLevel==1)
              string_method="Defl:X";
            else if ((iLevel==2) || (iLevel==3))
              string_method="Defl:F"; /* 2:fast , 3 : extra fast*/
        }
        else
            string_method="Unkn. ";

		sprintf(zipEntry[i].name,"%s",filename_inzip);
		/*
        printf("%7lu  %6s%c%7lu %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
                file_info.uncompressed_size,string_method,
                charCrypt,
                file_info.compressed_size,
                ratio,
                (uLong)file_info.tmu_date.tm_mon + 1,
                (uLong)file_info.tmu_date.tm_mday,
                (uLong)file_info.tmu_date.tm_year % 100,
                (uLong)file_info.tmu_date.tm_hour,(uLong)file_info.tmu_date.tm_min,
                (uLong)file_info.crc,filename_inzip);*/
        if ((i+1)<gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err!=UNZ_OK)
            {
                printf("error %d with zipfile in unzGoToNextFile\n",err);
                break;
            }
        }
    }

    return 0;
}

void ListContents(const char* zipfile){
unzFile uf=NULL;
char filename_try[256+16] = "";
pspDebugScreenSetXY(0, 0);

strncpy(filename_try, zipfile,256-1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
		filename_try[ 256 ] = '\0';

		uf = unzOpen(filename_try);
		if (uf==NULL)
        {
            strcat(filename_try,".zip");
            uf = unzOpen(filename_try);
        }
	sceDisplayWaitVblankStart();
	int err;
	err = do_list_to_struct(uf);

	//if(err!=UNZ_OK){printf("Err:%i",err);}

unzCloseCurrentFile(uf);

showingContents = 1;

}

void reorderDirList();

void recacheGameDir(){
struct SceIoDirent dir;
	memset(&dir, 0, sizeof(SceIoDirent)); 
	pspDebugScreenSetXY(0,6);
	//printf("Getting Directory...");
	static int dfd;
	dfd = sceIoDopen("ms0:/PSP/GAME/");
	if(dfd > 0){
	//pspDebugScreenSetXY(0,0);
	//printf("Found Directory...\n");
	int f=0;for(f=0;f<MAX_ENTRIES;f++){if(gameEntry[f].name){free(gameEntry[f].name);}gameEntry[f].name = NULL;}
	int count = 0;count = 0;
	
	while(sceIoDread(dfd, &dir) > 0){
	static int success;
	//success = sceIoDread(dfd, dir);
	//if(dir){
	static char* name;
	name = (char*)memalign(16,300);
	sprintf(name,"%s",dir.d_name);
	static int s=0;
	s=strlen(name);
	
	//if((name[s-1] == 'x' && name[s-2] == 'r' && name[s-3] == 't' && name[s-4] == '.')||(name[s] == 'x' && name[s-1] == 'r' && name[s-2] == 't' && name[s-3] == '.')){
	//printf("Found: %s\n",name);
	gameEntry[count].name = name;
	
		count++;if(count>MAX_ENTRIES-1){count = MAX_ENTRIES-1;}
	//}
	//printf("COUNT: %i",count);
	gameEntriesTotal = count;if(gameEntriesTotal < 0){gameEntriesTotal = 0;}
	}
	
	//}
	}
	sceIoDclose(dfd);
	//sceKernelThreadSleep(10000);
	//free(&dir);

}

void recacheDir(const char *dirToScan){
struct SceIoDirent dir;
	memset(&dir, 0, sizeof(SceIoDirent)); 
	pspDebugScreenSetXY(0,6);
	//printf("Getting Directory...");
	static int dfd;
	dfd = sceIoDopen(dirToScan);
	if(dfd > 0){
	//pspDebugScreenSetXY(0,0);
	//printf("Found Directory...\n");
	int f=0;for(f=0;f<MAX_ENTRIES;f++){if(tempDirEntry[f].name){free(tempDirEntry[f].name);}tempDirEntry[f].name = NULL;}
	int count = 0;count = 0;
	
	while(sceIoDread(dfd, &dir) > 0){
	static int success;
	//success = sceIoDread(dfd, dir);
	//if(dir){
	static char* name;
	name = (char*)memalign(16,300);
	sprintf(name,"%s",dir.d_name);
	static int s=0;
	s=strlen(name);
	
	//if((name[s-1] == 'x' && name[s-2] == 'r' && name[s-3] == 't' && name[s-4] == '.')||(name[s] == 'x' && name[s-1] == 'r' && name[s-2] == 't' && name[s-3] == '.')){
	//printf("Found: %s\n",name);
	tempDirEntry[count].name = name;
	tempDirEntry[count].type = 0;
	//dirEntry[count].name[strlen(dirEntry[count].name)] = '\0';
	int strl;strl = strlen(tempDirEntry[count].name);
	if(tempDirEntry[count].name[strl-3] == '.' || tempDirEntry[count].name[strl-4] == '.' || tempDirEntry[count].name[strl-2] == '.' || tempDirEntry[count].name[strl-1] == '.'){/*file*/tempDirEntry[count].type = 1;}
	if((tempDirEntry[count].name[0] == '.' && tempDirEntry[count].name[1] == '\0') || (tempDirEntry[count].name[0] == '.' && tempDirEntry[count].name[1] == '.' && tempDirEntry[count].name[2] == '\0')){tempDirEntry[count].type = 2;}

		count++;if(count>MAX_ENTRIES-1){count = MAX_ENTRIES-1;}
	//}
	//printf("COUNT: %i",count);
	browseEntriesTotal = count;if(browseEntriesTotal < 0){browseEntriesTotal = 0;}
	}
	
	//}
	}
	sceIoDclose(dfd);

	//reorder list
	reorderDirList();
	//sceKernelThreadSleep(10000);
	//free(&dir);

}

void reorderDirList(){
int i=0;
int cur=0;
for(i=0;i<browseEntriesTotal;i++){
	if(tempDirEntry[i].type == 2){/*up folder*/
	dirEntry[cur].name = (char*)memalign(16,300);
	sprintf(dirEntry[cur].name,"%s",tempDirEntry[i].name);
	dirEntry[cur].type = tempDirEntry[i].type;
	cur++;
	}
}
for(i=0;i<browseEntriesTotal;i++){
	if(tempDirEntry[i].type == 0){/*directory*/
	dirEntry[cur].name = (char*)memalign(16,300);
	sprintf(dirEntry[cur].name,"%s",tempDirEntry[i].name);
	dirEntry[cur].type = tempDirEntry[i].type;
	cur++;
	}
}
for(i=0;i<browseEntriesTotal;i++){
	if(tempDirEntry[i].type == 1){/*file*/
	dirEntry[cur].name = (char*)memalign(16,300);
	sprintf(dirEntry[cur].name,"%s",tempDirEntry[i].name);
	dirEntry[cur].type = tempDirEntry[i].type;
	cur++;
	}
}

}

char* stripToSlash(char* string){
int i=0;
int len=0;
len = strlen(string);
for(i=len;i>0;i--){
	if(string[i] == '/'){break;}else{string[i] = '\0';}
}
return string;
}

void extractIndividual(const char* filename, int folders){
	//create and move to extraction directory
	char* dirC;dirC = (char*)malloc(512);
	if(folders == 1){sprintf(dirC,"%s%s",extractDir,filename);dirC = stripToSlash(dirC);}else{sprintf(dirC,"%s",extractDir);}
	makedir(dirC);
	chdir(dirC);

	unzFile uf=NULL;
char filename_try[256+16] = "";
pspDebugScreenSetXY(0, 0);

strncpy(filename_try, zip_argv1,256-1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
		filename_try[ 256 ] = '\0';

		uf = unzOpen(filename_try);
		if (uf==NULL)
        {
            strcat(filename_try,".zip");
            uf = unzOpen(filename_try);
        }
	//sceDisplayWaitVblankStart();
	//do_list_to_struct(uf);
	//printf("Trying:%s\n",filename);
	do_extract_onefile(uf,filename,1,1,password);

unzCloseCurrentFile(uf);

free(dirC);
}

void DoExtractRAR(const char *rarfile,const char *extDir,const char *pass){
	
makedir(extDir);
int argc;int i=0;
const char **argv = calloc ( 8, sizeof(char *) );
//argv = (char**)memalign(16,512);
int count;
const char ** currentString = argv;
if(pass == NULL){
argc = 5;
count = argc+1;

for ( i = 0; i < count; i++ )
  {
	if(i==0)asprintf ( currentString, "unrar");
	if(i==1)asprintf ( currentString, "x");
	if(i==2)asprintf ( currentString, "%s",rarfile);
	if(i==3)asprintf ( currentString, "%s",extDir);
	if(i==4)asprintf ( currentString, "-y");
	if(i> 4)asprintf ( currentString, " ");
    currentString++;
    //printf("currentString address: %p\n", currentString);  
  }
}else{

argc = 6;
count = argc+1;

for ( i = 0; i < count; i++ )
  {
	if(i==0)asprintf ( currentString, "unrar");
	if(i==1)asprintf ( currentString, "x");
	if(i==2)asprintf ( currentString, "%s",rarfile);
	if(i==3)asprintf ( currentString, "%s",extDir);
	if(i==4)asprintf ( currentString, "-y");
	if(i==5)asprintf ( currentString, "-p%s",pass);
	if(i> 5)asprintf ( currentString, " ");
    currentString++;
    //printf("currentString address: %p\n", currentString);  
  }

}
  // reset memory block to the original address.  
  // In other words, go the beginning of the "array"
  currentString = argv;
  //printf("currentString address after reset: %p\n", currentString);
  

  // display the string at this particular slot.
  // we have to use the star to de-reference
  //for ( i = 0; i < count; i++ )
  //{
  //  printf( "%s\n", *currentString );
  //  currentString++;
  //}

	//argv[0] = "unrar";
	//argv[1] = "x";
	//argv[2] = rarfile;
	//argv[3] = extDir;
	//argv[4] = "-y";
	mainRAR(argc,argv);

  // reset
  currentString = argv;


for ( i = 0; i < count; i++ )
  {
    free ( *currentString );
    currentString++;
  }
free(argv);


}

int isRar(const char* fl){
int last;last = strlen(fl)-1;
if(fl[last] == 'r' && fl[last-1] == 'a' && fl[last-2] == 'r' && fl[last-3] == '.'){return(1);}
if(fl[last] == 'R' && fl[last-1] == 'A' && fl[last-2] == 'R' && fl[last-3] == '.'){return(1);}
if(fl[last] == 'a' && fl[last-1] == 'a' && fl[last-2] == 'R' && fl[last-3] == '.'){return(1);}
return(0);
}

// AHMAN
// Arguments added for passing from iR Shell
int main(int argc, char *argv[])
	{
	if(argv[1] != NULL){irshell = 1;}//set to irshell plugin if parameter passed
	char* tempStr;
	extractDir = memalign(16,512);
	tempStr = memalign(16,512);
	password = memalign(16,512);
	int x=0;for(x=0;x<512;x++){extractDir[x] = NULL;}
	x=0;for(x=0;x<512;x++){password[x] = '\0';}
	sprintf(extractDir,"ms0:/");

	x=0;
	for(x=0;x<MAX_ENTRIES;x++){
		zipEntry[x].name = (char*)memalign(16,256);
	}

	if(irshell == 1){
	if (argc > 1)
		zip_argv1 = argv[1];
	else
		zip_argv1 = NULL;

	if(isRar(zip_argv1)==1){appState = 1;}else{appState = 0;}
	sprintf(tempStr,"%s",zip_argv1);
	sprintf(extractDir,"%s",stripToSlash(tempStr));
	}else{
	zip_argv1 = memalign(16,512);
	sprintf(zip_argv1,"ms0:/");
	}

	if (init())
		{
		main_loop();
		}


	if(irshell == 0){
	free(zip_argv1);
	}
	sceKernelExitGame();
	return(0);
} 
/*
void reduceString(char* data,int number){
int x=0;
for(x=0;x<4000;x++){
data[x] = data[x+number];
}
}

int matchString(char* data, char* searchTerm, int len){
int x=0;
for(x=0;x<len;x++){
	if(data[x] != searchTerm[x]){
		return 0;//return fail
	}
}
return 1;//return match
}*/

char* reduceString2(char* data,int number){
int x=0;
for(x=0;x<512;x++){
data[x] = data[x+number];
}
return data;
}

void folderSCEtoKXploit(const char *foldname){
char* origStr;
origStr = memalign(16,512);
sprintf(origStr,"%s",foldname);
char* newStr;
newStr = memalign(16,512);sprintf(newStr,"%s",origStr);
char* finalStr;
finalStr = memalign(16,512);
//char search[40]; sprintf(search,"%__SCE__");
//search type1
//search = "%__SCE__";
if(matchString(newStr,"%__SCE__",8) == 1){
newStr = reduceString2(newStr,8);
sprintf(newStr,"%s%",newStr);

sprintf(origStr,"%s",foldname);
sprintf(finalStr,"%s",newStr);
printf("Orig:%sNew:%s",origStr,finalStr);
//sceKernelDelayThread(1000000);
sceIoChdir("ms0:/PSP/GAME/");
sceIoRename(origStr,finalStr);
}else{
if(matchString(newStr,"__SCE__",7) == 1){
newStr = reduceString2(newStr,7);
sprintf(newStr,"%s",newStr);

sprintf(origStr,"%s",foldname);
sprintf(finalStr,"%s",newStr);
printf("Orig:%sNew:%s",origStr,finalStr);
//sceKernelDelayThread(1000000);
sceIoChdir("ms0:/PSP/GAME/");
sceIoRename(origStr,finalStr);
}}
//recacheGameDir();
free(origStr);
free(newStr);
free(finalStr);
changeOptionCount = 0;
printf("DONE!!!!");

	//sceIoChdir("ms0:/PSP/GAME/");
	//sceIoChdir("ms0:/PSP/GAME/");
	//sceIoRename("%__SCE__Links2","Links2%");
recacheGameDir();
}

char* stripUpOneDir(char* dirp){
	dirp[strlen(dirp)-1] = '\0';
	dirp = stripToSlash((const char*)dirp);
	
return(dirp);
}

void drawTexture2D(int x, int y, float wid, float height, int bx, int by, int bw, const void* tex, int texMode){
	if(tex){
		sceGuTexFlush();
		sceKernelDcacheWritebackAll();

		sceGuDisable(GU_DEPTH_TEST);
		sceGuEnable(GU_BLEND);
		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0xffffffff, 0xffffffff);
		//render the 2D buffer to the screen
		// setup the source buffer as a 512x512 texture, but only copy 480x272
		sceGuTexMode(texMode,0,0,0);
		sceGuTexImage(0,bx,by,bw,tex);
		//sceGuTexImage(0,512,512,512,pixels);

		sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
		sceGuTexFilter(GU_NEAREST,GU_NEAREST);
		sceGuTexScale(1.0f/512.0f,1.0f/512.0f); // scale UVs to 0..1
		sceGuTexOffset(0.0f,0.0f);
		
		//sceGuAmbientColor(0xffffffff);
		
		struct Vertex* __attribute__((aligned(16))) BufferVert;
		BufferVert = (struct Vertex*)sceGuGetMemory(2 * sizeof(struct Vertex));
		BufferVert[0].x = x;
		BufferVert[0].y = y;
		BufferVert[0].z = 0;
		BufferVert[0].u = 0;
		BufferVert[0].v = 0;
		BufferVert[1].x = x+wid;
		BufferVert[1].y = y+height;
		BufferVert[1].z = 0;
		BufferVert[1].u = wid;
		BufferVert[1].v = height;
		sceGuDrawArray(GU_SPRITES,RENDER_OPTIONS_02,2,0,BufferVert);
		
		
		sceGuTexFlush();
		sceKernelDcacheWritebackAll();

		sceGuEnable(GU_DEPTH_TEST);
		sceGuDisable(GU_BLEND);

	}

}

void main_loop()
	{
	SetupCallbacks();
	// AHMAN
	// Add handling for iR Shell argument playback
	// The rest is unmodified to maintain original compatibility when launching standalone
	if (zip_argv1) {
		screen_init();
		pspDebugScreenSetXY(0, 0);
		pspDebugScreenPrintf("AnonymousTipster's .zip plugin\n\nFile: %s...", zip_argv1);
		//sceKernelDelayThread(100000);
		printf("Init GU.../n");
		sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	//sceGuAlphaFunc(GU_GREATER,0,0xff);
	//sceGuEnable(GU_ALPHA_TEST);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_LIGHTING);
	sceGuEnable(GU_LIGHT0);
	sceGuEnable(GU_LIGHT1);
	sceGuEnable(GU_LIGHT2);
	sceGuEnable(GU_LIGHT3);
	sceGuDisable(GU_ALPHA_TEST);
	//sceGuEnable(GU_UNKNOWN_17);//activate color-keying
	//sceGuEnable(GU_BLEND);
	sceGuFinish();
	sceGuSync(0,0);	
	sceKernelDcacheWritebackAll();
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	int menuOptionsX = -300;

	//sceGuStart(GU_DIRECT,list);
	//do little intro-fade
	int delay;delay = 300;
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff111111);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff222222);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff333333);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff444444);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff555555);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff666666);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff777777);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff888888);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff999999);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffaaaaaa);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffbbbbbb);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffcccccc);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffdddddd);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffeeeeee);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffffffff);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	danzeff_load();//load OSK
	//pspDebugScreenPrintf("/nD:%i/n",danzeff_isinitialized());

	//malloc textures
	//titleText_temp = memalign(16,4*512*32);
	//titleText2_temp = memalign(16,4*512*32);
	zipBackTex_temp = memalign(16,4*512*256);
	rarBackTex_temp = memalign(16,4*512*256);

	zipText_temp = memalign(16,4*512*32);
	extractDirText_temp = memalign(16,4*512*32);
	extractText_temp = memalign(16,4*512*32);
	exitText_temp = memalign(16,4*512*32);
	exitText_temp = memalign(16,4*512*32);
	typingText_temp = memalign(16,4*512*32);

	browseDirPath = memalign(16,512);sprintf(browseDirPath,"ms0:/");

	//render text to textures
	//titleText_temp = loadTextfromfile("./OSKgraphics/CoolText.ttf", 512, 64, "AnonymousTipster's",18, 20, 0,0,0,    1);
	//titleText2_temp = loadTextfromfile("./OSKgraphics/CoolText.ttf", 512, 64, "Unzip Plugin",12, 20, 0,0,0,    1);
	rarBackTex_temp = loadPNGfromfile("./OSKgraphics/rarBack.png",512, 256, 1);
	zipBackTex_temp = loadPNGfromfile("./OSKgraphics/zipBack.png",512, 256, 1);

/*
	FT_Library fntLibrary;
int error;
//load library
error = FT_Init_FreeType( &fntLibrary );
if(error){printf("cannot open font library"); return NULL;}

FT_Face face;
error = FT_New_Face( fntLibrary, "./OSKgraphics/UIText.ttf", 0, &face ); 
if ( error == FT_Err_Unknown_File_Format ) { printf("Font error"); return NULL;} else if ( error ) { printf("Font error"); return NULL;}
*/

//position button
	int finished;
	finished = 0;

	while(finished == 0){
		sceGuStart(GU_DIRECT,list);
		// clear screen
		sceGuClearColor(0xffffffff);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
		sceGuDisable(GU_DEPTH_TEST);

		//DrawScreenQuad(titleTextVerts,titleText_temp, 5, 40, 512, 32, 512, 32, 512, 1);
		//DrawScreenQuad(titleText2Verts,titleText2_temp, 5, 60, 512, 32, 512, 32, 512, 1);


		if(menuOptionsX < 0){menuOptionsX+=6;}if(menuOptionsX > 0){menuOptionsX = 0;}

		if(appState == 0){
			returnAppState = appState;
		DrawScreenQuad(titleBackVerts,zipBackTex_temp, 0, 272-256, 512, 256, 512, 256, 512, 1);
		int p=0;
	unsigned char rgba[4];
	rgba[0] = 0;
	rgba[1] = 0;
	rgba[2] = 0;
	rgba[3] = 0;
	unsigned short blank;
	blank = color_to_abgr4444(rgba);
	for(p=0;p<512*256;p++){
		mainMenuPixels[p] = blank;
	}

	if(showingContents == 0){
		//render main menu
		char* stringRend;
		int i=0;
		stringRend = memalign(16,512);
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Zip File:%s",zip_argv1);
		if(mainMenuSelected == -1){printStringSmallBG(5,5,stringRend,strlen(stringRend),255,0,0,255,255,255, mainMenuPixels, 512);}else{printStringSmallBG(5,5,stringRend,strlen(stringRend),0,0,0,255,255,255, mainMenuPixels, 512);}

		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Extract To:%s",extractDir);
		if(mainMenuSelected == 0){printStringSmallBG(5,20,stringRend,strlen(stringRend),255,0,0,255,255,255, mainMenuPixels, 512);}else{printStringSmallBG(5,20,stringRend,strlen(stringRend),0,0,0,255,255,255, mainMenuPixels, 512);}
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Extract...");
		if(mainMenuSelected == 1){printStringSmall(5,35,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,35,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		if(GetUsePassword()==0){sprintf(stringRend,"NoPassword:%s",password);}
		if(GetUsePassword()==1){sprintf(stringRend,"Password:%s",password);}
		if(mainMenuSelected == 2){printStringSmall(5,50,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,50,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"List Contents");
		if(mainMenuSelected == 3){printStringSmall(5,65,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,65,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		

		for(i=0;i<512;i++){stringRend[i] = NULL;}
		if(irshell == 0){
		sprintf(stringRend,"Exit");
		}else{
		sprintf(stringRend,"Exit To iRShell");
		}
		if(mainMenuSelected == 4){printStringSmall(5,80,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,80,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}

		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Extras...");
		if(mainMenuSelected == 5){printStringSmall(5,95,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,95,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		

		//draw input string
		//change later
		char *stringRender;stringRender = (char*)memalign(16,170);
		int y=0;for(y=0;y<170;y++){stringRender[y] = '\0';}
		keyboardString[currKeyboard] = '\0';
		sprintf(stringRender,"%s",(char*)keyboardString);
			if(strlen(stringRender) < 48){printStringSmall(0,115,stringRender,strlen(stringRender),0,0,0, mainMenuPixels, 512);}else{
								  printStringSmall(0,115,stringRender,strlen(stringRender),0,0,0, mainMenuPixels, 512);}

				

		//render to screen
		drawTexture2D(menuOptionsX, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);
		

		changeOptionCount++;
		SceCtrlData pad;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    sceCtrlReadBufferPositive(&pad, 1); 
	if(keyboardOpen == 1){
	if(danzeff_isinitialized()){
		danzeff_moveTo(480-192,0);
		danzeff_render();
		unsigned int returned;returned = 0;
		returned = danzeff_readInput(pad);
		pspDebugScreenSetXY(0,0);
		//printf("Returned:%u",returned);
		
		if(returned != 8 && returned > 4){//(returned >= 48 && returned <= 90) || (returned  >= 97 && returned <= 122) || (returned == 32)){
		keyboardString[currKeyboard] = returned;currKeyboard++;
		sceKernelDelayThread(1000*150);
		}
		if(returned == 8){
			keyboardString[currKeyboard] = '\0';currKeyboard--;if(currKeyboard < 0){currKeyboard = 0;}
			sceKernelDelayThread(1000*150);
		}
		if(returned == DANZEFF_SELECT){keyboardOpen = 0;}
		if(returned == DANZEFF_START){keyboardOpen = 0;keyboardReturnFunc();}
		
	}
	}

	if(keyboardOpen == 0){
	//if(pad.Buttons & PSP_CTRL_TRIANGLE){finished = 1;}
	if(changeOptionCount > 6){
	if(pad.Buttons & PSP_CTRL_UP){mainMenuSelected--;changeOptionCount = 0;redrawMenuText = 1;}
	if(pad.Buttons & PSP_CTRL_DOWN){mainMenuSelected++;changeOptionCount = 0;redrawMenuText = 1;}
	if(mainMenuSelected < -1){mainMenuSelected = -1;}
	if(mainMenuSelected > 5){mainMenuSelected = 5;}//MAX_MENU_ITEMS

	if(pad.Buttons & PSP_CTRL_LTRIGGER || pad.Buttons & PSP_CTRL_RTRIGGER){appState = 1;changeOptionCount = 0;}

	if(pad.Buttons & PSP_CTRL_CROSS){
		changeOptionCount = 0;
		redrawMenuText = 1;
		if(mainMenuSelected ==-1){keyboardOpen = 1;keyboardString[0] = '\0';sprintf(keyboardString,"%s",zip_argv1);currKeyboard = strlen(keyboardString);zip_argv1[currKeyboard] = '\0'; keyboardReturnFunc = keyboardCallbackFile;}
		if(mainMenuSelected == 0){keyboardOpen = 1;keyboardString[0] = '\0';sprintf(keyboardString,"%s",extractDir);currKeyboard = strlen(keyboardString);extractDir[currKeyboard] = '\0'; keyboardReturnFunc = keyboardCallbackDirectory;}
		if(mainMenuSelected == 1){pspDebugScreenSetXY(0,0);sceDisplayWaitVblankStart();unzipToDir(zip_argv1,extractDir,password);}
		if(mainMenuSelected == 2){keyboardOpen = 1;keyboardString[0] = '\0';sprintf(keyboardString,"%s",password);currKeyboard = strlen(keyboardString);password[currKeyboard] = '\0'; keyboardReturnFunc = keyboardCallbackPassword;}
		if(mainMenuSelected == 3){ListContents(zip_argv1);}
		if(mainMenuSelected == 4){finished = 1;}
		if(mainMenuSelected == 5){appState = 2;}
		sceKernelDelayThread(1000*200);
	}
	if(pad.Buttons & PSP_CTRL_SQUARE){//alternate
		changeOptionCount = 0;
		redrawMenuText = 1;
		if(mainMenuSelected ==-1){appState = 3;recacheDir(browseDirPath);browseType = 1;}
		if(mainMenuSelected == 0){appState = 3;recacheDir(browseDirPath);browseType = 0;}
		sceKernelDelayThread(1000*200);
	}
	}//end changeOption
	}
	if(pad.Buttons & PSP_CTRL_SELECT){if(keyboardOpen == 1){keyboardOpen = 0;}}
	}//end if contents not shown
	if(showingContents == 1){

		//render contents
		/*
		char* stringRend;
		int i=0;
		stringRend = memalign(16,512);
		int e=0;
		for(e=0;e<zipEntriesTotal;e++){
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Name:%s  Zipped Ratio:%s",zipEntry[e].name,zipEntry[e].ratio);
		int y;
		y = contentsY+(8*e);
		if(y>=0){
			if(contentsSelected != e){printStringSmall(5,y,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
			if(contentsSelected == e){printStringSmall(5,y,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}
		}
		}
		*/
		
		char *stringToRender;stringToRender = (char*)memalign(16,512);
		int y=0;
		p=0;
		int offset = 0;
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		sprintf(stringToRender,"Cross To Extract With Dir, Square To Extract File Only, Circle To Cancel",zipEntry[p].name);
		printStringSmallBG(5,5,stringToRender,strlen(stringToRender),0,0,0,255,255,255, mainMenuPixels, 512);
		for(p=0;p<zipEntriesTotal;p++){
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		if(p-contentsSelected >= 0 && p-contentsSelected <= 15){
		sprintf(stringToRender,"%s",zipEntry[p].name);
		if(contentsSelected == p){printStringSmall(5,15,stringToRender,strlen(stringToRender),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,15+((p-contentsSelected)*8)+(offset*8),stringToRender,strlen(stringToRender),0,0,0, mainMenuPixels, 512);}
		if(strlen(stringToRender) > 101){/*over line for 512*/offset++;}
		}
		}
		free(stringToRender);
		//render to screen
		drawTexture2D(menuOptionsX, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);

		//controls
		SceCtrlData pad;
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceCtrlReadBufferPositive(&pad, 1); 

		if(pad.Buttons & PSP_CTRL_DOWN && changeOptionCount > 5){contentsSelected++;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_UP && changeOptionCount > 5){contentsSelected--;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_LEFT){contentsSelected--;}
		if(pad.Buttons & PSP_CTRL_RIGHT){contentsSelected++;}

		if(contentsSelected < 0){contentsSelected = 0;}
		if(contentsSelected > zipEntriesTotal-1){contentsSelected = zipEntriesTotal-1;}

		changeOptionCount++;
		if((pad.Buttons & PSP_CTRL_CIRCLE) && changeOptionCount > 10){changeOptionCount = 0;showingContents = 0;}
		if((pad.Buttons & PSP_CTRL_CROSS) && changeOptionCount > 10){extractIndividual((const char*)zipEntry[contentsSelected].name,1);}
		if((pad.Buttons & PSP_CTRL_SQUARE) && changeOptionCount > 10){extractIndividual((const char*)zipEntry[contentsSelected].name,0);}
		



	}
	}
	if(appState == 1){
		returnAppState = appState;
		DrawScreenQuad(titleBackVerts,rarBackTex_temp, 0, 272-256, 512, 256, 512, 256, 512, 1);
		int p=0;
	unsigned char rgba[4];
	rgba[0] = 0;
	rgba[1] = 0;
	rgba[2] = 0;
	rgba[3] = 0;
	unsigned short blank;
	blank = color_to_abgr4444(rgba);
	for(p=0;p<512*256;p++){
		mainMenuPixels[p] = blank;
	}

	if(showingContents == 0){
		//render main menu
		char* stringRend;
		int i=0;
		stringRend = memalign(16,512);
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"RAR File:%s",zip_argv1);
		if(mainMenuSelected == -1){printStringSmallBG(5,5,stringRend,strlen(stringRend),255,0,0,255,255,255, mainMenuPixels, 512);}else{printStringSmallBG(5,5,stringRend,strlen(stringRend),0,0,0,255,255,255, mainMenuPixels, 512);}

		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Extract To:%s",extractDir);
		if(mainMenuSelected == 0){printStringSmallBG(5,20,stringRend,strlen(stringRend),255,0,0,255,255,255, mainMenuPixels, 512);}else{printStringSmallBG(5,20,stringRend,strlen(stringRend),0,0,0,255,255,255, mainMenuPixels, 512);}
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Extract...");
		if(mainMenuSelected == 1){printStringSmall(5,35,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,35,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		if(GetUsePassword()==0){sprintf(stringRend,"NoPassword:%s",password);}
		if(GetUsePassword()==1){sprintf(stringRend,"Password:%s",password);}
		if(mainMenuSelected == 2){printStringSmall(5,50,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,50,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		//for(i=0;i<512;i++){stringRend[i] = NULL;}
		//sprintf(stringRend,"List Contents");
		//if(mainMenuSelected == 3){printStringSmall(5,65,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,65,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		

		for(i=0;i<512;i++){stringRend[i] = NULL;}
		if(irshell == 0){
		sprintf(stringRend,"Exit");
		}else{
		sprintf(stringRend,"Exit To iRShell");
		}
		if(mainMenuSelected == 3){printStringSmall(5,65,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,65,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}

		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Extras...");
		if(mainMenuSelected == 4){printStringSmall(5,80,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,80,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
		

		//draw input string
		//change later
		char *stringRender;stringRender = (char*)memalign(16,170);
		int y=0;for(y=0;y<170;y++){stringRender[y] = '\0';}
		keyboardString[currKeyboard] = '\0';
		sprintf(stringRender,"%s",(char*)keyboardString);
			if(strlen(stringRender) < 48){printStringSmall(0,115,stringRender,strlen(stringRender),0,0,0, mainMenuPixels, 512);}else{
								  printStringSmall(0,115,stringRender,strlen(stringRender),0,0,0, mainMenuPixels, 512);}

				

		//render to screen
		drawTexture2D(menuOptionsX, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);
		

		changeOptionCount++;
		SceCtrlData pad;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    sceCtrlReadBufferPositive(&pad, 1); 
	if(keyboardOpen == 1){
	if(danzeff_isinitialized()){
		danzeff_moveTo(480-192,0);
		danzeff_render();
		unsigned int returned;returned = 0;
		returned = danzeff_readInput(pad);
		pspDebugScreenSetXY(0,0);
		//printf("Returned:%u",returned);
		
		if(returned != 8 && returned > 4){//(returned >= 48 && returned <= 90) || (returned  >= 97 && returned <= 122) || (returned == 32)){
		keyboardString[currKeyboard] = returned;currKeyboard++;
		sceKernelDelayThread(1000*150);
		}
		if(returned == 8){
			keyboardString[currKeyboard] = '\0';currKeyboard--;if(currKeyboard < 0){currKeyboard = 0;}
			sceKernelDelayThread(1000*150);
		}
		if(returned == DANZEFF_SELECT){keyboardOpen = 0;}
		if(returned == DANZEFF_START){keyboardOpen = 0;keyboardReturnFunc();}
		
	}
	}

	if(keyboardOpen == 0){
	//if(pad.Buttons & PSP_CTRL_TRIANGLE){finished = 1;}
	if(changeOptionCount > 6){
	if(pad.Buttons & PSP_CTRL_UP){mainMenuSelected--;changeOptionCount = 0;redrawMenuText = 1;}
	if(pad.Buttons & PSP_CTRL_DOWN){mainMenuSelected++;changeOptionCount = 0;redrawMenuText = 1;}
	if(mainMenuSelected < -1){mainMenuSelected = -1;}
	if(mainMenuSelected > 4){mainMenuSelected = 4;}//MAX_MENU_ITEMS

	if(pad.Buttons & PSP_CTRL_LTRIGGER || pad.Buttons & PSP_CTRL_RTRIGGER){appState = 0;changeOptionCount = 0;}

	if(pad.Buttons & PSP_CTRL_CROSS){
		changeOptionCount = 0;
		redrawMenuText = 1;
		if(mainMenuSelected ==-1){keyboardOpen = 1;keyboardString[0] = '\0';sprintf(keyboardString,"%s",zip_argv1);currKeyboard = strlen(keyboardString);zip_argv1[currKeyboard] = '\0'; keyboardReturnFunc = keyboardCallbackFile;}
		if(mainMenuSelected == 0){keyboardOpen = 1;keyboardString[0] = '\0';sprintf(keyboardString,"%s",extractDir);currKeyboard = strlen(keyboardString);extractDir[currKeyboard] = '\0'; keyboardReturnFunc = keyboardCallbackDirectory;}
		if(mainMenuSelected == 1){pspDebugScreenSetXY(0,0);sceDisplayWaitVblankStart();DoExtractRAR(zip_argv1,extractDir,password);/*unzipToDir(zip_argv1,extractDir,password);*/}
		if(mainMenuSelected == 2){keyboardOpen = 1;keyboardString[0] = '\0';sprintf(keyboardString,"%s",password);currKeyboard = strlen(keyboardString);password[currKeyboard] = '\0'; keyboardReturnFunc = keyboardCallbackPassword;}
		//if(mainMenuSelected == 3){ListContents(zip_argv1);}
		if(mainMenuSelected == 3){finished = 1;}
		if(mainMenuSelected == 4){appState = 2;}
		sceKernelDelayThread(1000*200);
		pspDebugScreenPrintf("thrDelayed");
	}
	if(pad.Buttons & PSP_CTRL_SQUARE){//alternate
		changeOptionCount = 0;
		redrawMenuText = 1;
		if(mainMenuSelected ==-1){appState = 3;recacheDir(browseDirPath);browseType = 1;}
		if(mainMenuSelected == 0){appState = 3;recacheDir(browseDirPath);browseType = 0;}
		sceKernelDelayThread(1000*200);
	}
	}//end changeOption
	}
	if(pad.Buttons & PSP_CTRL_SELECT){if(keyboardOpen == 1){keyboardOpen = 0;}}
	}//end if contents not shown
	if(showingContents == 1){

		//render contents
		/*
		char* stringRend;
		int i=0;
		stringRend = memalign(16,512);
		int e=0;
		for(e=0;e<zipEntriesTotal;e++){
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Name:%s  Zipped Ratio:%s",zipEntry[e].name,zipEntry[e].ratio);
		int y;
		y = contentsY+(8*e);
		if(y>=0){
			if(contentsSelected != e){printStringSmall(5,y,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}
			if(contentsSelected == e){printStringSmall(5,y,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}
		}
		}
		*/
		
		char *stringToRender;stringToRender = (char*)memalign(16,512);
		int y=0;
		p=0;
		int offset = 0;
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		sprintf(stringToRender,"Cross To Extract With Dir, Square To Extract File Only, Circle To Cancel",zipEntry[p].name);
		printStringSmallBG(5,5,stringToRender,strlen(stringToRender),0,0,0,255,255,255, mainMenuPixels, 512);
		for(p=0;p<zipEntriesTotal;p++){
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		if(p-contentsSelected >= 0 && p-contentsSelected <= 15){
		sprintf(stringToRender,"%s",zipEntry[p].name);
		if(contentsSelected == p){printStringSmall(5,15,stringToRender,strlen(stringToRender),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,15+((p-contentsSelected)*8)+(offset*8),stringToRender,strlen(stringToRender),0,0,0, mainMenuPixels, 512);}
		if(strlen(stringToRender) > 101){/*over line for 512*/offset++;}
		}
		}
		free(stringToRender);
		//render to screen
		drawTexture2D(menuOptionsX, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);

		//controls
		SceCtrlData pad;
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceCtrlReadBufferPositive(&pad, 1); 

		if(pad.Buttons & PSP_CTRL_DOWN && changeOptionCount > 5){contentsSelected++;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_UP && changeOptionCount > 5){contentsSelected--;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_LEFT){contentsSelected--;}
		if(pad.Buttons & PSP_CTRL_RIGHT){contentsSelected++;}

		if(contentsSelected < 0){contentsSelected = 0;}
		if(contentsSelected > zipEntriesTotal-1){contentsSelected = zipEntriesTotal-1;}

		changeOptionCount++;
		if((pad.Buttons & PSP_CTRL_CIRCLE) && changeOptionCount > 10){changeOptionCount = 0;showingContents = 0;}
		if((pad.Buttons & PSP_CTRL_CROSS) && changeOptionCount > 10){extractIndividual((const char*)zipEntry[contentsSelected].name,1);}
		if((pad.Buttons & PSP_CTRL_SQUARE) && changeOptionCount > 10){extractIndividual((const char*)zipEntry[contentsSelected].name,0);}
		



	}
	}
	if(appState == 2){//extras
		if(extrasState == 0){//menu
		int p=0;
	unsigned char rgba[4];
	rgba[0] = 0;
	rgba[1] = 0;
	rgba[2] = 0;
	rgba[3] = 0;
	unsigned short blank;
	blank = color_to_abgr4444(rgba);
	for(p=0;p<512*256;p++){
		mainMenuPixels[p] = blank;
	}
		//render main menu
		char* stringRend;
		int i=0;
		stringRend = memalign(16,512);
		for(i=0;i<512;i++){stringRend[i] = NULL;}
		sprintf(stringRend,"Folder: _SCE_ to kxploit",zip_argv1);
		if(extrasMenuSelected == 0){printStringSmall(5,5,stringRend,strlen(stringRend),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,5,stringRend,strlen(stringRend),0,0,0, mainMenuPixels, 512);}

		//render to screen
		drawTexture2D(menuOptionsX, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);

		//controls
		SceCtrlData pad;
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceCtrlReadBufferPositive(&pad, 1); 
		changeOptionCount++;
		if((pad.Buttons & PSP_CTRL_CIRCLE) && changeOptionCount > 10){appState = returnAppState;changeOptionCount = 0;showingContents = 0;}
		if(pad.Buttons & PSP_CTRL_CROSS && changeOptionCount > 5){extrasState = 1;recacheGameDir();changeOptionCount = 0;}
		free(stringRend);
		}
		if(extrasState == 1){//sce to kxploit
		int p=0;
	unsigned char rgba[4];
	rgba[0] = 0;
	rgba[1] = 0;
	rgba[2] = 0;
	rgba[3] = 0;
	unsigned short blank;
	blank = color_to_abgr4444(rgba);
	for(p=0;p<512*256;p++){
		mainMenuPixels[p] = blank;
	}
		//render directory
		char *stringToRender;stringToRender = (char*)memalign(16,512);
		int y=0;
		p=0;
		int offset = 0;
		for(p=0;p<gameEntriesTotal;p++){
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		if(p-gameDirSelected >= 0 && p-gameDirSelected <= 15){
		sprintf(stringToRender,"%s",gameEntry[p].name);
		if(gameDirSelected == p){printStringSmall(5,5,stringToRender,strlen(stringToRender),255,0,0, mainMenuPixels, 512);}else{printStringSmall(5,5+((p-gameDirSelected)*8)+(offset*8),stringToRender,strlen(stringToRender),0,0,0, mainMenuPixels, 512);}
		if(strlen(stringToRender) > 101){/*over line for 512*/offset++;}
		}
		}
		free(stringToRender);

		//render to screen
		drawTexture2D(menuOptionsX, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);

		//controls
		SceCtrlData pad;
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceCtrlReadBufferPositive(&pad, 1); 
		changeOptionCount++;
		if(pad.Buttons & PSP_CTRL_DOWN && changeOptionCount > 5){gameDirSelected++;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_UP && changeOptionCount > 5){gameDirSelected--;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_LEFT){gameDirSelected--;}
		if(pad.Buttons & PSP_CTRL_RIGHT){gameDirSelected++;}

		if(gameDirSelected < 0){gameDirSelected = 0;}
		if(gameDirSelected > gameEntriesTotal-1){gameDirSelected = gameEntriesTotal-1;}
		if((pad.Buttons & PSP_CTRL_CIRCLE) && changeOptionCount > 10){extrasState = 0;changeOptionCount = 0;}
		if((pad.Buttons & PSP_CTRL_CROSS) && changeOptionCount > 10){folderSCEtoKXploit(gameEntry[gameDirSelected].name);changeOptionCount = 0;}
		}


	}
	if(appState == 3){//directory browse
		int p=0;
	unsigned char rgba[4];
	rgba[0] = 0;
	rgba[1] = 0;
	rgba[2] = 0;
	rgba[3] = 0;
	unsigned short blank;
	blank = color_to_abgr4444(rgba);
	for(p=0;p<512*256;p++){
		mainMenuPixels[p] = blank;
	}
		//render directory
		char *stringToRender;stringToRender = (char*)memalign(16,512);
		int y=0;
		p=0;
		int offset = 0;
		for(p=0;p<browseEntriesTotal;p++){
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		if(browseType == 0){sprintf(stringToRender,"%s","Cross To Navigate, Circle To Cancel, Square To Confirm Current Directory");}else{sprintf(stringToRender,"%s","Cross To Confirm File, Circle To Cancel");}
		printStringSmall(5,5,stringToRender,strlen(stringToRender),0,0,0, mainMenuPixels, 512);
		for(y=0;y<512;y++){stringToRender[y] = '\0';}
		printStringSmall(5,15,browseDirPath,strlen(browseDirPath),0,0,0, mainMenuPixels, 512);
		if(p-browseDirSelected >= 0 && p-browseDirSelected <= 15){
		sprintf(stringToRender,"%s",dirEntry[p].name);
		if(browseDirSelected == p){printStringSmall(5,25,stringToRender,strlen(stringToRender),255,0,0, mainMenuPixels, 512);}else{
			if(dirEntry[p].type == 1){printStringSmall(5,25+((p-browseDirSelected)*8)+(offset*8),stringToRender,strlen(stringToRender),0,0,150, mainMenuPixels, 512);}else{
				printStringSmall(5,25+((p-browseDirSelected)*8)+(offset*8),stringToRender,strlen(stringToRender),50,50,50, mainMenuPixels, 512);}
		}
		if(strlen(stringToRender) > 101){/*over line for 512*/offset++;}
		}
		}
		free(stringToRender);

		//render to screen
		drawTexture2D(0, 136,512,256,512,256,512,mainMenuPixels, GU_PSM_4444);

		//controls
		SceCtrlData pad;
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
		sceCtrlReadBufferPositive(&pad, 1); 
		changeOptionCount++;
		if(pad.Buttons & PSP_CTRL_DOWN && changeOptionCount > 5){browseDirSelected++;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_UP && changeOptionCount > 5){browseDirSelected--;changeOptionCount = 0;}
		if(pad.Buttons & PSP_CTRL_LEFT){browseDirSelected--;}
		if(pad.Buttons & PSP_CTRL_RIGHT){browseDirSelected++;}

		if(browseDirSelected < 0){browseDirSelected = 0;}
		if(browseDirSelected > browseEntriesTotal-1){browseDirSelected = browseEntriesTotal-1;}
		if((pad.Buttons & PSP_CTRL_CIRCLE) && changeOptionCount > 8){appState = returnAppState;}
		if((pad.Buttons & PSP_CTRL_CROSS) && changeOptionCount > 8){
			if(dirEntry[browseDirSelected].type == 0){sprintf(browseDirPath,"%s%s/",browseDirPath,dirEntry[browseDirSelected].name);recacheDir((const char*)browseDirPath);browseDirSelected = 0;}else{
				if(dirEntry[browseDirSelected].type == 1 && browseType == 1){/*browsing for file*/sprintf(zip_argv1,"%s%s",browseDirPath,dirEntry[browseDirSelected].name);appState = returnAppState;}else{
			if(dirEntry[browseDirSelected].type == 2){sprintf(browseDirPath,"%s",stripUpOneDir(browseDirPath));recacheDir((const char*)browseDirPath);browseDirSelected = 0;}
			}}
			changeOptionCount = 0;
			}
		if((pad.Buttons & PSP_CTRL_SQUARE) && changeOptionCount > 8  && browseType == 0){
		sprintf(extractDir,"%s",browseDirPath);
		appState = returnAppState;
		changeOptionCount = 0;
		}
		//if((pad.Buttons & PSP_CTRL_TRIANGLE) && changeOptionCount > 10){sprintf(browseDirPath,"%s",stripUpOneDir(browseDirPath));recacheDir((const char*)browseDirPath);browseDirSelected = 0;changeOptionCount = 0;}
		


	}
		
		sceGuFinish();sceGuSync(0,0);
		sceDisplayWaitVblankStart();sceGuSwapBuffers();
		
		//sceKernelDelayThread(10);

	}
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffffffff);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffeeeeee);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffdddddd);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffcccccc);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffbbbbbb);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xffaaaaaa);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff999999);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff888888);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff777777);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff666666);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff555555);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff444444);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff333333);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff222222);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);
	sceGuStart(GU_DIRECT,list);sceGuClearColor(0xff111111);sceGuClearDepth(0);sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);sceGuFinish();sceGuSync(0,0);sceDisplayWaitVblankStart();sceGuSwapBuffers();sceKernelDelayThread(delay);


		return;
	}
	
	}
	

