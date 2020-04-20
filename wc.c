#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

void printFileInfo(char *file_name) {
	struct stat statBuf;
	FILE *fp;
	int char_count = 0;
	int lines_count = 0;
	int word_count = 0;
	int flag = 0;
	int line_flag = 0;
	int k = stat(file_name,&statBuf);
	if (k<0) {
		printf("file open failure\n");
		return;
	}
	if (S_ISDIR(statBuf.st_mode)) {
		printf("%s: Is a directory\n0\t0\t0\t%s\n", file_name,file_name);
		return;
	}
	// char_count = statBuf.st_size;
	fp = fopen(file_name,"r");
	char ch;
	int count=0;
	while ((ch = fgetc(fp))!= -1) {
		char_count++;
		if (ch == '\n') {
			if (line_flag == 1) {
				lines_count++;
				line_flag = 0;
			}
		}else{
			line_flag = 1;
		}
		if (ch == '\t' || ch == ' ' || ch == '\n') {
			flag = 1;
			continue;
		}else{
			if (flag == 1) {
				word_count++;
				flag = 0;
			}
		}
	}
	fclose(fp);
	printf("  %d  %d %d %s\n",lines_count,word_count,char_count,file_name );
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("error input\n" );
	}else{
		printFileInfo(argv[1]);
	}
	return 0;
}
