#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void printUsage() {
    printf("Usage: ./recover -d [device filename] [other arguments]\n");
    printf("-i                   Print boot sector information\n");
    printf("-l                   List all the directory entries\n");
    printf("-r target -d dest    File recovery with 8.3 filename\n");
    printf("-R target -p dest    File recovert with long filename\n");
}

int main(int argc, char *argv[]) {
    int option = 0;
    int dFlag = 0;
    int iFlag = 0;
    int lFlag = 0;
    int rFlag = 0;
    int RFlag = 0;
    int oFlag = 0;
    while ((option = getopt(argc,argv,"d:ilr:R:o:"))!=-1) {
        switch (option) {
            case 'd':
                dFlag = 1;
                printf("disk image: %s\n",optarg);
                break;
            case 'i':
                iFlag = 1;
                printf("i flag on = print boot sector info\n");
                break;
            case 'l':
                lFlag = 1;
                printf("l flag = list directory\n");
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


    return 0;
}
