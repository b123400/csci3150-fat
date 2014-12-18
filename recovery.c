#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

//Copy from Sample Code
#pragma pack(push,1)
struct BootEntry
{
	unsigned char BS_jmpBoot[3];	/* Assembly instruction to jump to boot code */
	unsigned char BS_OEMName[8];	/* OEM Name in ASCII */
	unsigned short BPB_BytsPerSec; /* Bytes per sector. Allowed values include 512, 1024, 2048, and 4096 */
	unsigned char BPB_SecPerClus; /* Sectors per cluster (data unit). Allowed values are powers of 2, but the cluster size must be 32KB or smaller */
	unsigned short BPB_RsvdSecCnt;	/* Size in sectors of the reserved area */
	unsigned char BPB_NumFATs;	/* Number of FATs */
	unsigned short BPB_RootEntCnt; /* Maximum number of files in the root directory for FAT12 and FAT16. This is 0 for FAT32 */
	unsigned short BPB_TotSec16;	/* 16-bit value of number of sectors in file system */
	unsigned char BPB_Media;	/* Media type */
	unsigned short BPB_FATSz16; /* 16-bit size in sectors of each FAT for FAT12 and FAT16.  For FAT32, this field is 0 */
	unsigned short BPB_SecPerTrk;	/* Sectors per track of storage device */
	unsigned short BPB_NumHeads;	/* Number of heads in storage device */
	unsigned long BPB_HiddSec;	/* Number of sectors before the start of partition */
	unsigned long BPB_TotSec32; /* 32-bit value of number of sectors in file system.  Either this value or the 16-bit value above must be 0 */
	unsigned long BPB_FATSz32;	/* 32-bit size in sectors of one FAT */
	unsigned short BPB_ExtFlags;	/* A flag for FAT */
	unsigned short BPB_FSVer;	/* The major and minor version number */
	unsigned long BPB_RootClus;	/* Cluster where the root directory can be found */
	unsigned short BPB_FSInfo;	/* Sector where FSINFO structure can be found */
	unsigned short BPB_BkBootSec;	/* Sector where backup copy of boot sector is located */
	unsigned char BPB_Reserved[12];	/* Reserved */
	unsigned char BS_DrvNum;	/* BIOS INT13h drive number */
	unsigned char BS_Reserved1;	/* Not used */
	unsigned char BS_BootSig; /* Extended boot signature to identify if the next three values are valid */
	unsigned long BS_VolID;	/* Volume serial number */
	unsigned char BS_VolLab[11]; /* Volume label in ASCII. User defines when creating the file system */
	unsigned char BS_FilSysType[8];	/* File system type label in ASCII */
};

struct DirEntry
{
	unsigned char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate; 
	unsigned short DIR_FstClusHI; 
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO; 
	unsigned long DIR_FileSize;

};
#pragma pack(pop)

void printUsage() {
    printf("Usage: ./recover -d [device filename] [other arguments]\n");
    printf("-i                   Print boot sector information\n");
    printf("-l                   List all the directory entries\n");
    printf("-r target -o dest    File recovery with 8.3 filename\n");
    printf("-R target -o dest    File recovert with long filename\n");
}

void showBootSectorInfo(char* diskPath) {
    FILE *fp = fopen(diskPath, "r");
	char buf[512];

	struct BootEntry *be;
	if (!fp) {
		fprintf(stderr, "Cannot find the file.\n");
		exit(0);
	}
	
	fread(buf, sizeof(buf), 1, fp);
	be = (struct BootEntry *)buf;

	unsigned int sector_total = (be->BPB_TotSec32 - (be->BPB_NumFATs * be->BPB_FATSz32) - be->BPB_RsvdSecCnt)/ be->BPB_SecPerClus;

	unsigned int *fat = malloc(be->BPB_FATSz32 * be->BPB_BytsPerSec);
	pread(fileno(fp), fat, be->BPB_FATSz32 * be->BPB_BytsPerSec, be->BPB_RsvdSecCnt * be->BPB_BytsPerSec);

	printf("Number of FATs = %u.\n", be->BPB_NumFATs);
	printf("Numeber of bytes per sector = %u.\n", be->BPB_BytsPerSec);
	printf("Numeber of sectors per cluster = %u.\n", be->BPB_SecPerClus);
	printf("Numeber of reserved sectors = %u.\n", be->BPB_RsvdSecCnt);
    
    int i = 0;
	int allocated = 0;

	for(i = 2;i < sector_total+2; i++){
        if(fat[i]){
		allocated++;
		}
    }
    
    printf("Number of allocated clusters = %u.\n", allocated);
    printf("Number of free clusters = %u.\n", sector_total-allocated);   

	fclose(fp);
}

void listDirectory(char* diskPath) {
	return;
}

int main(int argc, char *argv[]) {
    int option = 0;
    int dFlag = 0;
    int iFlag = 0;
    int lFlag = 0;
    int rFlag = 0;
    int RFlag = 0;
    int oFlag = 0;
    char diskPath[100];
    while ((option = getopt(argc,argv,"d:ilr:R:o:"))!=-1) {
        switch (option) {
            case 'd':
                dFlag = 1;
                strcpy(diskPath, optarg);
                break;
            case 'i':
                iFlag = 1;
                break;
            case 'l':
                lFlag = 1;
                listDirectory(diskPath);
                break;
            case 'r':
                rFlag = 1;
                printf("small r = recover with 8.3 filename, target: %s\n",optarg);
                break;
            case 'R':
                RFlag = 1;
                printf("large R = recover with long filename, target: %s\n",optarg);
                break;
            case 'o':
                oFlag = 1;
                printf("output to: %s\n",optarg);
                break;
            default:
                printf("unrecognized argument: %c",option);
        }
    }
    if (dFlag + iFlag + lFlag + rFlag + RFlag + oFlag == 0) {
        printUsage();
    } else if (iFlag && lFlag) {
        // wrong usage = print usage
        printUsage();
    } else if (!iFlag && !lFlag && (!dFlag || !oFlag || (rFlag + RFlag != 1))) {
        // anyway wrong usage, fuck you
        printUsage();
    }
    
    if (iFlag) {
        showBootSectorInfo(diskPath);
    }
    
    return 0;
}
