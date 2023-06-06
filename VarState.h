#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
using namespace std;

long long FileCount;                                      // 文本库中文本数量
long long k = 5;
bool runFlag = false;

vector<string> Keywords, KeywordsForShow;                 // 关键词容器
map<pair<string, string>, int> ThisWordNumInThisFile;     // 统计该文件中每个关键词出现的次数
unordered_map<string, int> NumOfWordsInThisFile;          // 记录该文件的单词总数

set<pair<string, string>> IS_WORD_IN_THIS_FILE;           // 判断该文件是否在该关键词的索引当中
unordered_map<string, vector<string>> WordInTheseFiles;   // 统计含有该关键词的文件名，便于后续检索

map<pair<string, string>, double> Keyword_TF_InThisFile;  // 关键词在某一文件中TF值
unordered_map<string, double> Keyword_IDF;                // 关键词的IDF值
map<pair<string, string>, double> TheFileToKeywordTF_IDF; // 关键词在某一文件中的TF-IDF值
unordered_map<string, double> FileTF_IDF;                 // 每个文件的总TF_IDF值

unordered_set<string> FILESET;                            // 记录一共有哪些文件
unordered_set<string> FOLDERSET;                          // 记录文本库有哪些文件夹

unordered_set<string> Stopword;                           // 记录停用词用于比对
unordered_set<string> StopwordForShow;                    // 记录停用词用于显示