#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <fstream>
#include <io.h>
#include <thread>
#include <windows.h>
#include <shellapi.h>
#include <direct.h>
#include "FunctionState.h"
#include "VarState.h"
using namespace std;

// 选项
void Choice() {
	printf("本系统功能：\n");
	printf("1：输入关键词，在文件中进行检索\n");
	printf("2：查看文本\n");
	printf("3：添加新文本\n");
	printf("4：删除文本\n");
	printf("5：主菜单\n");
	printf("9：管理停用词\n");
	printf("0：退出系统\n\n");
	printf("请选择：");
	string choice;
	bool flag;
	do {
		flag = true;
		cin >> choice;
		getchar();
		if (!check(choice)) {
			flag = false;
			printf("选项不合法，请重新输入：");
		}
	} while (!flag);
	switch (choice[0] - '0') {
	case 1: {
		CalculateTF_IDF();
		break;
	} case 2: {
		ShowFile();
		break;
	} case 3: {
		ImportFile();
		break;
	} case 4: {
		DeleteFile();
		break;
	} case 5: {
		system("cls");
		Choice();
		break;
	} case 9: {
		StopWords();
		break;
	} case 0: {
		printf("\n欢迎再次使用！\n");
		exit(0);
	}
	}
}

// 预处理
void Pretreat() {
	runFlag = false;
	readStopword();                            // 读取停用词
	ifstream InFile("PreTreatFile.txt");
	string FolderName;
	while (getline(InFile, FolderName)) {      // 遍历文本库中的每个文件夹
		FOLDERSET.insert(FolderName);
		struct _finddata_t FileInfo;
		string cur = FolderName + "\\\\*.txt";
		long long handle;
		if ((handle = _findfirst(cur.c_str(), &FileInfo)) == -1L) {
			printf(" %s 中没有文本文件，请检查文件夹路径是否正确。\n", FolderName.c_str());
		}
		else {
			FolderName += "\\\\";
			handle = _findfirst(cur.c_str(), &FileInfo);  // 遍历文件夹中的每个文本文件
			do {
				string FileName = FolderName + FileInfo.name;
				Index(FileName);                          // 建立索引
			} while (!(_findnext(handle, &FileInfo)));
			_findclose(handle);
		}
	}
	runFlag = true;                                       // 预处理完毕
}
bool checkStopWord(string str) {
	for (auto it : Stopword) {
		if (upper(it) == upper(str)) return true;
	}
	return false;
}
void readStopword() {
	ifstream InFile("StopWordList.txt");
	string str;
	while (getline(InFile, str)) {
		string word;
		for (int i = 0; i < str.size(); i++) {
			if (str[i] == ' ' || i == str.size() - 1) {
				if (str[i] != ' ') word += str[i];
				Stopword.insert(upper(word));
				StopwordForShow.insert(word);
				word = "";
			}
			else word += str[i];
		}
	}
}
// 建立索引
void BuildWordIndex(string str, int& allwordsNum, string FileName) {
	int strlen = str.length();
	string word;
	for (int i = 0; i < strlen; i++) {
		if (str[i] == ' ' || i == strlen - 1 || wordEnd(str[i])) {
			if (i == strlen - 1 && !wordEnd(str[i]) && str[i] != ' ') {
				if (str[i] >= 97) word += str[i] - 32;
				else word += str[i];
			}
			if (word != "") {
				allwordsNum++;
				if (Stopword.count(word)) {
					word = "";
					continue;
				}
				if (!IS_WORD_IN_THIS_FILE.count({ word, FileName })) { // 如果该单词的索引中没有该文件
					IS_WORD_IN_THIS_FILE.insert({ word, FileName });
					WordInTheseFiles[word].push_back(FileName); // 建立索引
				}
				ThisWordNumInThisFile[{FileName, word}]++;
			}
			word = "";
		}
		else {
			if (str[i] >= 97) word += str[i] - 32;
			else word += str[i];
		}
	}
}
void Index(string FileName) {
	ifstream inFile;
	inFile.open(FileName);
	string str;
	int allwordsNum = 0;
	while (getline(inFile, str)) {
		BuildWordIndex(str, allwordsNum, FileName);
	}
	NumOfWordsInThisFile[FileName] = allwordsNum;
	inFile.close();
	FileCount++;
	FILESET.insert(FileName);
}

// 处理关键词的输入和检索
void showInputMesg() {
	printf("请输入检索关键词：（空格间隔，换行即结束输入，未输入内容则返回主菜单）\n");
	bool flag;
	do {
		string str, word;
		flag = false;
		getline(cin, str);
		for (int i = 0; i < str.size(); i++) {
			if (str[i] < 0 || str[i] > 127 || wordEnd(str[i])) {
				flag = true;
				printf("输入内容可能含有中文或标点，请重新输入。\n");
				break;
			}
			if (str[i] != ' ') word += str[i];
			if (str[i] == ' ' || i == str.size() - 1) {
				if (Stopword.count(upper(word))) {
					printf("输入内容含有停用词，请重新输入。\n");
					flag = true;
					break;
				}
				if (word != "") {
					KeywordsForShow.push_back(word);
					Keywords.push_back(upper(word));
					word = "";
				}
			}
		}
	} while (flag);
	if (!Keywords.size()) {
		system("cls");
		printf("未输入关键词，正在返回主菜单...\n\n");
		Choice();
	}
}
void CalculateTF_IDF() {
	clear();
	system("cls");
	showInputMesg();         // 输入关键词

	if (!runFlag) printf("（首次运行，速度可能较慢，请稍等...）\n");
	if (!runFlag) {
		printf("正在检索中...\n");
		while (!runFlag);
	}
	else {
		printf("正在检索中...\n");
	}
	for (int i = 0; i < Keywords.size(); i++) {  // 计算关键词IDF
		Keyword_IDF[Keywords[i]] = log10(1.0 * FileCount / (WordInTheseFiles[Keywords[i]].size()));
	}
	for (int i = 0; i < Keywords.size(); i++) { // 遍历每个关键词的倒排列表
		for (auto it : WordInTheseFiles[Keywords[i]]) {
			Keyword_TF_InThisFile[{it, Keywords[i]}] = 1.0 * ThisWordNumInThisFile[{it, Keywords[i]}] / NumOfWordsInThisFile[it];
			TheFileToKeywordTF_IDF[{it, Keywords[i]}] = Keyword_TF_InThisFile[{it, Keywords[i]}] * Keyword_IDF[Keywords[i]];
			FileTF_IDF[it] += TheFileToKeywordTF_IDF[{it, Keywords[i]}];
		}
	}
	Output();                                   // 输出检索结果
}
// 处理输出信息
void Output() {
	vector<pair<string, double>> FileTF_IDFVector(FileTF_IDF.begin(), FileTF_IDF.end());
	if (!FileTF_IDFVector.size()) {
		printf("没有与以上关键词匹配的文本文件。\n");
		Choice();
		return;
	}
	printf("\n检索结果如下：\n");
	sort(FileTF_IDFVector.begin(), FileTF_IDFVector.end(), TF_IDFCMP);
	printf("Top-------------Name--------------Frequency---------TF*IDF-------\n");
	for (int i = 0; i < FileTF_IDFVector.size() && i < k; i++) {
		printf(" %d\t%s\t\t\t   %-8f\n", i + 1, FileTF_IDFVector[i].first.c_str(), FileTF_IDFVector[i].second);
		for (int j = 0; j < Keywords.size(); j++) {
			printf("\t\t%s\t\t    %d\t\t   %-8f\n",
				KeywordsForShow[j].c_str(), ThisWordNumInThisFile[{FileTF_IDFVector[i].first.c_str(), Keywords[j]}],
				TheFileToKeywordTF_IDF[{FileTF_IDFVector[i].first, Keywords[j]}]);
		}
		printf("-----------------------------------------------------------------\n");
	}
	printf("\n");
	Choice();
}
// 重置容器
void clear() {
	Keywords.clear();
	KeywordsForShow.clear();
	TheFileToKeywordTF_IDF.clear();
	FileTF_IDF.clear();
}

// 程序内查看文本信息
void ShowFile() {
	printf("请输入要查看的文件路径（如：Data\\\\123.txt，-1表示返回）：\n");
	string filename;
	cin >> filename;
	if (filename == "-1") {
		system("cls");
		Choice();
	}
	if (!runFlag) printf("（首次运行，速度可能较慢，请稍等...）\n");
	if (!runFlag) {
		printf("正在查找...\n");
		while (!runFlag);
	}
	bool flag;
	do {
		if (filename == "-1") break;
		flag = true;
		if (!FILESET.count(filename)) {
			flag = false;
			printf("没有找到该文件，请重新输入：");
			cin >> filename;
		}
	} while (!flag);
	if (filename == "-1") {
		system("cls");
		Choice();
	}
	DeleteData(filename);
	string path = "notepad.exe " + filename;
	WinExec(path.c_str(), SW_SHOW);              // 调用记事本打开文件
	Index(filename);
	printf("\n");
	Choice();
}
// 添加文件
void ImportFile() {
	system("cls");
	printf("请输入要添加的文件（夹）路径（-1表示返回）：\n");

	string filename;
	cin >> filename;
	if (filename == "-1") {
		system("cls");
		Choice();
	}
	if (!runFlag) printf("（首次运行，速度可能较慢，请稍等...）\n");
	if (!runFlag) {
		printf("正在处理...\n");
		while (!runFlag);
	}
	bool flag;
	do {
		if (filename == "-1") break;
		flag = true;
		if (FOLDERSET.count(filename) || FILESET.count(filename)) {
			flag = false;
			printf("文件已存在，请重新输入：");
			cin >> filename;
		}
	} while (!flag);
	if (filename == "-1") {
		system("cls");
		Choice();
	}

	if (filename.substr(filename.length() - 4, 4) == ".txt") { // 判断是否是文本文件
		ifstream in(filename.c_str());
		if (in.good()) {
			Index(filename);
			printf("导入成功！\n\n");
		}
		else {
			printf("文件不存在，请检查路径。\n\n");
		}
	}
	else {                                                      // 若是文件夹
		FOLDERSET.insert(filename);
		struct _finddata_t FileInfo;
		string cur = filename + "\\\\*.txt";
		long long handle;
		if ((handle = _findfirst(cur.c_str(), &FileInfo)) == -1L) {
			printf(" %s 中没有文本文件，请检查文件夹路径是否正确。\n\n", filename.c_str());
		}
		else {
			filename += "\\\\";
			handle = _findfirst(cur.c_str(), &FileInfo);        // 遍历文件夹中每个文本文件
			do {
				string FileName = filename + FileInfo.name;
				Index(FileName);
			} while (!(_findnext(handle, &FileInfo)));
			_findclose(handle);
			ofstream out("PreTreatFile.txt");
			for (auto it : FOLDERSET) {                         // 更新文本库中的文件夹
				out << it << endl;
			}
			printf("\n文件导入成功！\n\n");
		}
	}
	Choice();
}
// 删除文件
void removeDir(const char* dirPath)
{
	struct _finddata_t fb;
	char  path[250];
	long long  handle = 0;
	strcpy_s(path, dirPath);
	strcat_s(path, "/*");

	handle = _findfirst(path, &fb);
	if (handle != 0) {
		while (0 == _findnext(handle, &fb)) {
			memset(path, 0, sizeof(path));
			strcpy_s(path, dirPath);
			strcat_s(path, "/");
			strcat_s(path, fb.name);
			remove(path);
		}
		_findclose(handle);
	}
}
void DeleteData_ByLine(string str, string FileName) {
	int strlen = str.length();
	string word;
	for (int i = 0; i < strlen; i++) {
		if (str[i] == ' ' || i == strlen - 1 || wordEnd(str[i])) {
			if (i == strlen - 1 && !wordEnd(str[i]) && str[i] != ' ') {
				if (str[i] >= 97) word += str[i] - 32;
				else word += str[i];
			}
			if (word != "") {
				if (IS_WORD_IN_THIS_FILE.count({ word, FileName })) { // 如果该单词的索引中有该文件
					IS_WORD_IN_THIS_FILE.erase({ word, FileName });   // 则删除该单词对该文件的索引
					vector<string>::iterator it = find(WordInTheseFiles[word].begin(), WordInTheseFiles[word].end(), FileName);
					if (it != WordInTheseFiles[word].end()) WordInTheseFiles[word].erase(it);
				}
				if (ThisWordNumInThisFile[{FileName, word}]) {
					ThisWordNumInThisFile.erase({ FileName, word });
				}
				if (Keyword_TF_InThisFile[{FileName, word}]) {
					Keyword_TF_InThisFile.erase({ FileName, word });
				}
				if (Keyword_IDF[word]) {
					Keyword_IDF.erase(word);
				}
			}
			word = "";
		}
		else {
			if (str[i] >= 97) word += str[i] - 32;
			else word += str[i];
		}
	}
}
void DeleteData(string FileName) {
	ifstream inFile(FileName);
	string str;
	while (getline(inFile, str)) {
		DeleteData_ByLine(str, FileName);
	}
	NumOfWordsInThisFile.erase(FileName);
	inFile.close();
	FileCount--;
	FILESET.erase(FileName);                // 文本库中移除该文件
}
void DeleteFile() {
	system("cls");
	printf("请输入要删除的文件（夹）路径（-1表示返回）：\n");

	string filename;
	cin >> filename;
	if (filename == "-1") {
		system("cls");
		Choice();
	}
	if (!runFlag) printf("（首次运行，速度可能较慢，请稍等...）\n");
	if (!runFlag) {
		printf("正在查找要删除的文件...\n");
		while (!runFlag);
	}
	bool flag;
	do {
		if (filename == "-1") break;
		flag = true;
		if (!FILESET.count(filename) && !FOLDERSET.count(filename)) {
			flag = false;
			printf("没有找到该文件，请重新输入：");
			cin >> filename;
		}
	} while (!flag);
	if (filename == "-1") {
		system("cls");
		Choice();
	}
	printf("请输入删除方式：1.仅删除索引  2.删除索引和文件\n");
	int c;
	scanf_s("%d", &c);
	if (filename.substr(filename.length() - 4, 4) == ".txt") {        // 判断是否是文本文件
		DeleteData(filename);
		FILESET.erase(filename);
		FileCount--;
		if (c == 2) {
			remove((filename).c_str());
			printf("\n删除索引和文件成功！\n");
		}
		else printf("\n删除索引成功！\n");
	}
	else {
		struct _finddata_t FileInfo;
		string cur = filename + "\\\\*.txt";
		long long handle;
		if ((handle = _findfirst(cur.c_str(), &FileInfo)) == -1L) {
			printf(" %s 中没有文本文件，请检查文件夹路径是否正确。\n\n", filename.c_str());
		}
		else {
			handle = _findfirst(cur.c_str(), &FileInfo);               // 遍历文件夹中每个文件
			do {
				string FileName = filename + "\\\\" + FileInfo.name;
				DeleteData(FileName);                                  // 删除每个文件的索引
			} while (!(_findnext(handle, &FileInfo)));
			_findclose(handle);
			FOLDERSET.erase(filename);
			ofstream out("PreTreatFile.txt");
			for (auto it : FOLDERSET) {                                // 更新文本库
				out << it << endl;
			}
			if (c == 2) {                                              // 判断是否是硬删除
				removeDir(filename.c_str());
				_rmdir(filename.c_str());
				printf("\n删除索引和文件夹成功！\n");
			}
			else printf("\n删除索引成功！\n");
		}
	}
	printf("\n");
	Choice();
}

//管理停用词
void StopWords() {
	system("cls");
	printf("停用词：\n");
	for (auto it : StopwordForShow) cout << it << ' ';
	printf("\n1：添加停用词。\n");
	printf("2：删除停用词。\n");
	printf("0：返回并更新索引。\n");
	printf("请选择：");
	bool change = false;
	while (true) {
		string choice;
		bool flag;
		do {
			flag = true;
			cin >> choice;
			if (!check(choice) || choice[0] == '3' || choice[0] == '4' || choice[0] == '5' || choice[0] == '9') {
				flag = false;
				printf("选项不合法，请重新输入：");
			}
		} while (!flag);
		switch (choice[0] - '0') {
		case 1: {
			printf("请输入要添加的停用词：");
			string str;
			cin >> str;
			while (Stopword.count(upper(str))) {                // 比对已存在的停用词
				printf("该停用词已存在，请重新输入：");
				cin >> str;
			}
			Stopword.insert(upper(str));
			StopwordForShow.insert(str);
			printf("\n添加成功！\n");
			change = true;
			break;
		} case 2: {
			if (!Stopword.size()) {
				printf("停用词表为空，请更改选项。\n");
				break;
			}
			printf("请输入要删除的停用词：");
			string str;
			cin >> str;
			while (!Stopword.count(upper(str))) {
				printf("未找到该停用词，请重新输入：");
				cin >> str;
			}
			Stopword.erase(upper(str));
			StopwordForShow.erase(str);
			printf("\n删除成功！\n");
			change = true;
			break;
		} case 0: {
			if (change) {
				if (!runFlag) {
					printf("正在更新，请稍等...\n");
					while (!runFlag);
				}
				ofstream InFile("StopWordList.txt");
				for (auto it : StopwordForShow) {             // 更新停用词表
					InFile << it << ' ';
				}
				printf("\n请重新启动系统以更新索引！\n");
				exit(0);
			}
			else {
				system("cls");
				Choice();
			}
			break;
		}
		}
		printf("停用词：\n");
		for (auto it : StopwordForShow) cout << it << ' ';
		printf("\n\n");
		printf("请继续选择：");
	}
}

int main() {
	printf("欢迎使用基于关键词的文本排序检索系统！\n");

	thread treatThread(Pretreat);
	treatThread.detach();

	Choice();

	return 0;
}