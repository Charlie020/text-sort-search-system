#pragma once
#include <iostream>
using namespace std;

void Choice();                           // 选项
void Pretreat();                         // 预处理
bool checkStopWord(string str);
void readStopword();                     // 读取停用词表
void Index(string FileName);         // 建立索引
void BuildWordIndex(string str, int& allwordsNum, string FileInfoName);

// 处理关键词的输入和检索
void showInputMesg();
void CalculateTF_IDF();
void Output();                           // 处理输出信息
void clear();                            // 重置容器

void ShowFile();                         // 程序内查看文本信息
void ImportFile();                          // 新建文件
void DeleteFile();                       // 删除文件
void DeleteData(string FileInfoName);
void DeleteData_ByLine(string str, string FileInfoName);
void removeDir(const char* dirPath);

void StopWords();                        //管理停用词

bool check(string str) {
	if (str.length() != 1) return false;
	if (str[0] < 48 || (str[0] > 53 && str[0] != '9')) return false;
	return true;
}

string upper(string str) {
	for (int i = 0; i < str.size(); i++) {
		if (str[i] >= 97) str[i] -= 32;
	}
	return str;
}
// 判断是否到达了单词的末尾
bool wordEnd(char c) {
	if (c == '.' || c == ',' || c == '!' || c == '?' || c == '|'
		|| c == '\\' || c == '-' || c == '(' || c == ')' || c == '\'')
		return true;
	return false;
}
bool TF_IDFCMP(pair<string, double>& a, pair<string, double>& b) {
	return a.second > b.second;
}