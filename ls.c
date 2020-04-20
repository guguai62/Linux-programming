#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<time.h>

#define FILEPATH_MAX (80)
#define MAX_PATH_LEN (256)
void mode_to_letters(int mode, char str[]);
void printFileInfo(struct stat *stat_ptr, char *fileName);
void printInode(struct stat *stat_ptr);
int readFileList(char* dirPath, int command[]);
void iterativeReadFileList(char* file_name, int command[]);
char *getFullPath(char relative_path[], char *dirPath, char *file_name);

typedef struct dirNode {
	char *path;
	struct dirNode *next;
}dirNode;

/*拼接路径*/
char *getFullPath(char relative_path[], char *dirPath, char *file_name) {
	memset(relative_path,0,MAX_PATH_LEN);
	strcpy(relative_path, dirPath);
	strcat(relative_path, "/");
	strcat(relative_path, file_name);
	return relative_path;
}

/*在目录文件中执行-a -l -i 的组合命令*/
int readFileList(char* dirPath,int command[]) {
	DIR *dir_ptr;
	struct dirent *dirent_ptr;
	dir_ptr = opendir(dirPath);
	char path[MAX_PATH_LEN];
	if (dir_ptr == NULL)
	{
		printf("File opening failure");
		return 1;
	}
	if (command[0]==1)
	{//打印所有隐藏文件
		while ((dirent_ptr = readdir(dir_ptr)) != NULL)
		{
			struct stat statbuf;
			char *d_path = getFullPath(path,dirPath,dirent_ptr->d_name);
			int k = stat(d_path, &statbuf);
			if (k < 0)
			{
				printf("文件打开失败");
			}
			else
			{
				if (command[3] == 1)
				{//打印inode
					printInode(&statbuf);
				}
				if (command[1] == 1)
				{
					printFileInfo(&statbuf, dirent_ptr->d_name);
				}
				else
				{
					printf("%s\n", dirent_ptr->d_name);
				}
			}
		}
		closedir(dir_ptr);
	}
	else
	{
		while ((dirent_ptr = readdir(dir_ptr)) != NULL)
		{
			if (dirent_ptr->d_name[0] != '.') {
				struct stat statbuf;
				char *d_path = getFullPath(path, dirPath, dirent_ptr->d_name);
				int k = stat(d_path, &statbuf);
				if (k<0)
				{
					printf("文件打开失败");
				}else
				{
					if (command[3] == 1)
					{//打印inode
						printInode(&statbuf);
					}
					if (command[1]==1)
					{
						printFileInfo(&statbuf,dirent_ptr->d_name);
					}
					else
					{
						printf("%s\n", dirent_ptr->d_name);
					}
				}
			}
		}
		closedir(dir_ptr);
	}
	return 1;
}

/*在目录文件中执行-a -l -i -R的组合命令*/
void iterativeReadFileList(char* dirPath, int command[]){
	char path[MAX_PATH_LEN];

	dirNode *dir_node;
	dirNode *current_node;
	dir_node = (dirNode*)malloc(sizeof(dirNode));
	dir_node->path = (char*)malloc(MAX_PATH_LEN);
	strcpy(dir_node->path, dirPath);
	dir_node->next = NULL;
	current_node = dir_node;

	if (command[0] == 1)
	{//打印所有隐藏文件
		while (current_node != NULL)
		{
			DIR *dir_ptr;
			struct dirent *dirent_ptr;
			dir_ptr = opendir(current_node->path);
			if (dir_ptr == NULL)
			{
				printf("file opening failure");
				break;
			}
			printf("%s:\n", current_node->path);
			while ((dirent_ptr = readdir(dir_ptr)) != NULL)
			{
				//去除当前和上一层目录
				if (strcmp(dirent_ptr->d_name, ".") == 0 || strcmp(dirent_ptr->d_name, "..") == 0)
				{
					struct stat statbuf;
					char *d_path = getFullPath(path, current_node->path, dirent_ptr->d_name);
					int k = stat(d_path, &statbuf);
					if (k < 0)
					{
						printf("文件打开失败");
						return;
					}
					if (command[3] == 1)
					{//打印inode
						printInode(&statbuf);
					}
					if (command[1] == 1)
					{
						printFileInfo(&statbuf, dirent_ptr->d_name);
					}
					else
					{
						printf("%s\n", dirent_ptr->d_name);
					}
				}
				else
				{
					struct stat statbuf;
					char *d_path = getFullPath(path, current_node->path, dirent_ptr->d_name);
					int k = stat(d_path, &statbuf);
					if (k < 0)
					{
						printf("文件打开失败");
						break;
					}
					if (S_ISDIR(statbuf.st_mode))
					{
						dirNode *dir_node_new;
						dir_node_new = (dirNode*)malloc(sizeof(dirNode));
						dir_node_new->path = (char*)malloc(MAX_PATH_LEN);
						strcpy(dir_node_new->path, d_path);
						dir_node_new->next = NULL;
						dir_node->next = dir_node_new;
						dir_node = dir_node->next;
					}
					if (command[3] == 1)
					{//打印inode
						printInode(&statbuf);
					}
					if (command[1] == 1)
					{
						printFileInfo(&statbuf, dirent_ptr->d_name);
					}
					else
					{
						printf("%s\n", dirent_ptr->d_name);
					}
				}
			}
			printf("\n");
			current_node = current_node->next;
			closedir(dir_ptr);
		}
		return;
	}
	else
	{
		while (current_node !=NULL)
		{
			DIR *dir_ptr;
			struct dirent *dirent_ptr;
			dir_ptr = opendir(current_node->path);
			if (dir_ptr == NULL)
			{
				printf("file opening failure");
				break;
			}
			printf("%s:\n",current_node->path);
			while ((dirent_ptr = readdir(dir_ptr)) != NULL)
			{
				//去除当前和上一层目录
				if (strcmp(dirent_ptr->d_name, ".") != 0 || strcmp(dirent_ptr->d_name, "..") != 0)
				{
					if (dirent_ptr->d_name[0] != '.') {
						struct stat statbuf;
						char *d_path = getFullPath(path, current_node->path, dirent_ptr->d_name);
						int k = stat(d_path, &statbuf);
						if (k < 0)
						{
							printf("文件打开失败");
							break;
						}
						if (S_ISDIR(statbuf.st_mode))
						{
							dirNode *dir_node_new;
							dir_node_new = (dirNode*)malloc(sizeof(dirNode));
							dir_node_new->path = (char*)malloc(MAX_PATH_LEN);
							strcpy(dir_node_new->path, d_path);
							dir_node_new->next = NULL;
							dir_node->next = dir_node_new;
							dir_node = dir_node->next;
						}
						if (command[3] == 1)
						{//打印inode
							printInode(&statbuf);
						}
						if (command[1] == 1)
						{
							printFileInfo(&statbuf, dirent_ptr->d_name);
						}
						else
						{
							printf("%s\n", dirent_ptr->d_name);
						}
					}
				}
			}
			printf("\n");
			current_node = current_node->next;
			closedir(dir_ptr);
		}
		return;
	}
}


void readDirInfo(char *file_name, int command[]) {
	struct stat statBuf;
	int k = stat(file_name, &statBuf);
	if (k < 0)
	{
		printf("文件打开失败\n");
		return;
	}

	//文件成功打开
	if (command[4]==1)
	{//有d命令
		if (command[3]==1)
		{
			printInode(&statBuf);
		}
		if (command[1] == 1)
		{
			printFileInfo(&statBuf, file_name);
		}
		else
		{
			printf("%s\n", file_name);
		}
	}
	else
	{//没有d命令
		//判断是否是目录
		if (S_ISDIR(statBuf.st_mode))
		{
			if (command[2] == 1)
			{//含有-R命令
				iterativeReadFileList(file_name, command);
			}
			else
			{  //没有-R命令   现在可能的组合有 -a -i -l
				readFileList(file_name, command);
			}
		}
		else
		{  //非目录则不需要管-R命令
			if (command[3] == 1)
			{
				printInode(&statBuf);
			}
			if (command[1] == 1)
			{
				printFileInfo(&statBuf, file_name);
			}
			else
			{
				printf("%s\n", file_name);
			}
		}
	}

}

void mode_to_letters(int mode, char str[])
{
	strcpy(str, "----------");

	if (S_ISDIR(mode))
	{
		str[0] = 'd';
	}

	if (S_ISCHR(mode))
	{
		str[0] = 'c';
	}

	if (S_ISBLK(mode))
	{
		str[0] = 'b';
	}
	if (S_ISLNK(mode))
	{
		str[0] = 'l';
	}
	if (S_ISSOCK(mode)) {
		str[0] = 's';
	}
	if (S_ISFIFO(mode)) {
		str[0] = 'p';
	}

	if ((mode & S_IRUSR))
	{
		str[1] = 'r';
	}

	if ((mode & S_IWUSR))
	{
		str[2] = 'w';
	}

	if ((mode & S_IXUSR))
	{
		str[3] = 'x';
	}

	if ((mode & S_IRGRP))
	{
		str[4] = 'r';
	}

	if ((mode & S_IWGRP))
	{
		str[5] = 'w';
	}

	if ((mode & S_IXGRP))
	{
		str[6] = 'x';
	}

	if ((mode & S_IROTH))
	{
		str[7] = 'r';
	}

	if ((mode & S_IWOTH))
	{
		str[8] = 'w';
	}

	if ((mode & S_IXOTH))
	{
		str[9] = 'x';
	}
}

void printInode(struct stat *stat_ptr) {
	printf("%ld ", stat_ptr->st_ino);
}

void printFileInfo(struct stat *stat_ptr,char *fileName) {
	//根据st_uid获取用户名
	struct passwd *getpwuid(), *pw_ptr;
	pw_ptr = getpwuid(stat_ptr->st_uid);

	//根据st_gid获取所在用户组名
	struct group *getgrgid(), *grp_ptr;
	grp_ptr = getgrgid(stat_ptr->st_gid);

	char time_str[80];
	struct tm * current_time = localtime(&(stat_ptr->st_mtime));
	strftime(time_str, 100, "%b %d %H:%M", current_time);

	char modestr[11];
	mode_to_letters(stat_ptr->st_mode, modestr);
	printf("%s %ld ", modestr, stat_ptr->st_nlink);
	printf("%s ", pw_ptr->pw_name);
	printf("%s ",  grp_ptr->gr_name);
	printf("%ld ", stat_ptr->st_size);
	printf("%s ", time_str);
	printf("%s\n", fileName);
}

void main(int argc, char *argv[])
{
	struct stat statBuf;
	char *file_name;
	int opt=0;
	int command[5];  //a l R i d
	memset(command, 0, sizeof(command));
	while ((opt = getopt(argc,argv,"alRdi"))!=-1)
	{
		switch (opt)
		{
			case 'a':
				command[0] = 1;
				break;
			case 'l':
				command[1] = 1;
				break;
			case 'R':
				command[2] = 1;
				break;
			case 'i':
				command[3] = 1;
				break;
			case 'd':
				command[4] = 1;
				break;
			case '?':
				break;
				exit(0);
		}
	}
	if (optind != argc)
	{
		//输入命令包含文件名
		file_name = argv[optind];
	}
	else
	{
		//输入命令不包含文件名  设置默认文件名为当前目录
		file_name = ".";
		//file_name = (char *)malloc(FILEPATH_MAX);
		//getcwd(file_name, FILEPATH_MAX);
	}
	readDirInfo(file_name, command);
}

