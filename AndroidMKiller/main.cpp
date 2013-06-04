//
//  main.cpp
//  AndroidMKiller
//
//  Created by Ghost on 13-5-11.
//  Copyright (c) 2013年 Ghost. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

#define FolderPath "./"

string head = "LOCAL_PATH := $(call my-dir)\n\
\n\
include $(CLEAR_VARS)\n\
\n\
LOCAL_MODULE := game_shared\n\
\n\
LOCAL_MODULE_FILENAME := libgame\n\
\n\
LOCAL_SRC_FILES := hellocpp/main.cpp \\\n";
vector<string> cppFile;
vector<string> hFile;
string prefix = "../../Classes/";
string middle = "\n\
LOCAL_C_INCLUDES := ";
string tail = "\n\
\n\
LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static\n\
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static\n\
\n\
LOCAL_CFLAGS := -fexceptions\n\
\n\
include $(BUILD_SHARED_LIBRARY)\n\
\n\
$(call import-module,CocosDenshion/android) \n\
$(call import-module,cocos2dx) \n\
$(call import-module,extensions) \n\
$(call import-module,external/Box2D)\n";

string final = "";

void scan_folder(const char* folder_path, int depth);

int main(int argc,char* argv[])
{
    cppFile.clear();
    hFile.clear();
    chdir("../../Classes/");
	scan_folder(FolderPath, 0);
    
    final += head;
    for (int i = 0 ; i < cppFile.size() - 1 ; ++i)
    {
        string temp = cppFile[i];
        final += "					" + temp + " \\\n";
    }
    
    final += "					" + cppFile[cppFile.size() - 1] + " \n";
    
    final += middle;
    
    for (int i = 0 ; i < hFile.size() - 1 ; ++i)
    {
        string temp = hFile[i];
        final += "					" + temp + " \\\n";
    }
    
    final += "					" + hFile[hFile.size() - 1] + " \n";
    
    final += tail;
    
//    printf("%s", final.c_str());
    
    chdir("proj.android/jni");
    
    FILE *fp = fopen("Android.mk", "w+");
    fputs(final.c_str(), fp);
    fclose(fp);
    
	return 0;
}

/**
 文件夹下文件扫描实现
 @param folder_path 文件夹的路径
 */

void scan_folder(const char* folder_path, int depth)
{
    printf("enter %s", folder_path);
    string tempString(folder_path);
    tempString.erase(tempString.length() - 1);
    string hFileTempString = "$(LOCAL_PATH)/" + prefix + tempString;
    hFile.push_back(hFileTempString);
    
    for (int i = 0 ; i < depth - 1 ; ++i)
    {
        chdir("../");   
    }
    
	/*
     打开一个目录并建立一个目录流,返回一个指向DIR结构的指针，
     该指针用于读取目录数据项.
     */
    string newPath = folder_path;
	DIR* dp = opendir(newPath.c_str());
	if(NULL == dp)
	{
		fprintf(stderr,"cannot open directory:%s    %s\n",folder_path, strerror(errno));
		return ;
	}
    
	struct dirent *entry;
	struct stat statbuf;
    
	chdir(folder_path);//cd目录切换
	/*
     readdir,返回一个指针，指针指向的结构里保存着目录流中下一个目录项的有关资源。
     后续的readdir调用将返回后续的目录项.
     */
	while(NULL != (entry = readdir(dp)))
	{
		lstat(entry->d_name,&statbuf);
        printf("%s\n", entry->d_name);
		if(S_ISDIR(statbuf.st_mode))
		{
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
			{
				continue;
			}
			char tempStr[256];
			strcpy(tempStr,folder_path);
			strcat(tempStr,entry->d_name);
			strcat(tempStr,"/");
			//fprintf(stdout,"before:%s\n",tempStr);
			scan_folder(tempStr, depth + 1);
		}
		else
		{
            int length = entry->d_namlen;
            if (entry->d_name[length-4] == '.' && entry->d_name[length-3] == 'c' && entry->d_name[length-2] == 'p' && entry->d_name[length-1] == 'p')
            {
                string cppFileTempString = prefix + folder_path + entry->d_name;
                cppFile.push_back(cppFileTempString);
            }
            //			fprintf(stdout,"%s%s\n",folder_path,entry->d_name);
		}
	}
    chdir("../");    
	closedir(dp);//关闭一个目录流并释放与之关联的资源
}
