#include <stdio.h>
#include <unistd.h>
#include <dirent.h>


 

#define BUF_SIZE	256
#define  byte  unsigned char

int getFileSize(FILE* inFile);
void traverseDir(char* pPath, int nDeepth);
int adustHeader(char* pPath, char*  newPath);


void showUsage()
{
	printf("\n");
	printf("*****************************************\n");
	printf("*\n");
	printf("*\tError !!! no input file path! \n");
	printf("*\tUsage: \t adjst  20220421  [20220422 ...]   \n");
	printf("*\tPlease input a directory \n");
	printf("*\t\n");
	printf("*****************************************\n");

}


void traverseDir(char* pPath, int nDeepth)
{
    DIR* pDir = NULL;
    struct dirent* pSTDirEntry;
    char* pChild = NULL;


    if ((pDir = opendir(pPath)) == NULL)
    {
        return;
    }
    else
    {
        while ((pSTDirEntry = readdir(pDir)) != NULL)
        {
            if ((strcmp(pSTDirEntry->d_name, ".") == 0) || (strcmp(pSTDirEntry->d_name, "..") == 0))
            {
                continue;
            }
            else
            {
                int i; 
                for (i = 0; i < nDeepth; i++)
                {
                    //cout << "\t";
		    printf("\t");
                }

                
                int wav_name_len = strlen(pSTDirEntry->d_name);
                if(strcmp(pSTDirEntry->d_name + wav_name_len -4,".wav") == 0)
                {

                    //cout << pSTDirEntry->d_name << endl;
                    printf("%s\t\t%s\n",pPath, pSTDirEntry->d_name );
		            adustHeader(pPath,pSTDirEntry->d_name);
                }

                if (pSTDirEntry->d_type & DT_DIR)
                {
                    pChild = (char*)malloc(sizeof(char) * (NAME_MAX + 1));
                    if (pChild == NULL)
                    {
                        perror("memory not enough.");
                        return;
                    }
                    memset(pChild, 0, NAME_MAX + 1);
                    strcpy(pChild, pPath);
                    strcat(pChild, pSTDirEntry->d_name);
                    traverseDir(pChild, nDeepth + 1);
                    free(pChild);
                    pChild = NULL;
                }
            }
        }
        closedir(pDir);
    }
}



int adustHeader(char* pPath, char*  newPath)
{
	FILE* inFile, * outFile;
	char StrLine[BUF_SIZE];

	byte* header = (byte*)malloc(56);

    unsigned char   nano = 1;
    size_t buf_size = 56;

	//if (fopen_s(&inFile, newPath.c_str(), "rb") != 0)
	char infile[strlen(newPath) + strlen(pPath) + 2];
	strcpy(infile,pPath);
	strcat(infile,"/");
	strcat(infile,newPath);

	inFile = fopen(infile,"rb");
	if (!inFile )
	{
		printf("cannot oepn file :%s\n",infile);
		exit(1);
	}

	//newPath += ".1.wav";
	//if (fopen_s(&outFile, newPath.c_str(), "wb") != 0)
	char outfile[strlen(infile) + 6];
	strcpy(outfile,infile);
	strcat(outfile,".1.wav");

	outFile = fopen(outfile,"wb");
	if (!outFile)
	{
		perror("write file failed");
		exit(2);
	}

	int fSize = getFileSize(inFile);
	int totalDataLen = fSize - 8;
	int dataLen = fSize - 56;
	int factChunkSize2 = dataLen / 4;	
	size_t n56 = fread(header, 1, 56, inFile);
	if (n56 != 56)
	{
		perror("Read file error!");
		exit(-1);
	}
	/*
	header[0] = 'R';
	header[1] = 'I';
	header[2] = 'F';
	header[3] = 'F';
	*/
	header[4] = (byte)(totalDataLen & 0xff);
	header[5] = (byte)(totalDataLen >> 8 & 0xff);
	header[6] = (byte)(totalDataLen >> 16 & 0xff);
	header[7] = (byte)(totalDataLen >> 24 & 0xff);
	/*
	header[8] = 'W';
	header[9] = 'A';
	header[10] = 'V';
	header[11] = 'E';
	header[12] = 'f';
	header[13] = 'm';
	header[14] = 't';
	header[15] = ' ';
	header[16] = 18;
	header[17] = 0;
	header[18] = 0;
	header[19] = 0;
	header[20] = 6;
	header[21] = 0;
	header[22] = 1;
    */

    nano = header[22] == 1 ;

   if(nano)
   {
    header[44] = (byte)(dataLen & 0xff);
	header[45] = (byte)(dataLen >> 8 & 0xff);
	header[46] = (byte)(dataLen >> 16 & 0xff);
	header[47] = (byte)(dataLen >> 24 & 0xff);
    }
    else
    {
	header[44] = (byte)(factChunkSize2 & 0xff);
	header[45] = (byte)(factChunkSize2 >> 8 & 0xff);
	header[46] = (byte)(factChunkSize2 >> 16 & 0xff);
	header[47] = (byte)(factChunkSize2 >> 24 & 0xff);

    }

	header[52] = (byte)(dataLen & 0xff);
	header[53] = (byte)(dataLen >> 8 & 0xff);
	header[54] = (byte)(dataLen >> 16 & 0xff);
	header[55] = (byte)(dataLen >> 24 & 0xff);

	size_t  nr = fwrite(header, 1, buf_size, outFile);
	if (nr < buf_size)
	{
		printf("write file %d \n",nr);
		exit(-2);
	}
	while (!feof(inFile))
	{
		size_t n = fread(StrLine, 1, BUF_SIZE - 1, inFile);
		fwrite(StrLine, 1, n, outFile);
	}

	fclose(outFile);
	fclose(inFile);
}


int getFileSize(FILE* inFile)
{
	int fileSize = 0;
	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
	return fileSize;
}

int main(int argc, char** argv)
{
   if(argc < 2)
   {
	showUsage();
	return 1;
   }
   int i;
    for (i = 1; i < argc; ++i)
    {
        traverseDir(argv[i], 0);
    }

    return 0;
}
