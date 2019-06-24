#include <iostream>
#include <string.h>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <fstream>
using namespace std;

map <string, int> dict;
map <int, string> revdict; //这是用来存储译码的时候的字典的
struct encodeinfo
{
	string P;
	int index;
};
vector<encodeinfo> LZW_encode(string s, int encodenum){
	string P = "";
	char C;
	vector<encodeinfo> EncodeResult; //存储编码之后的结果

	for (int i = 0; i < s.length(); i++){
		C = s[i];
		string tempStr = P + C;
		//在字典里面寻找这个字符串
		map<string, int>::iterator iter = dict.find(tempStr);
		if (iter != dict.end()){//找到了
			P = tempStr;
		}
		else{//没找到
			encodeinfo a = { P, dict[P] };
			EncodeResult.push_back(a);//将P的对应的编码存放起来
			//建立起一个新的索引
			encodenum++;
			dict[tempStr] = encodenum;
			P = C;
		}
	}
	encodeinfo a = { P, dict[P] };
	EncodeResult.push_back(a);//最终结尾处
	
	
	/*
	//这是整个字典的输出
	map<string, int>::iterator iter = dict.begin();
	for (; iter != dict.end(); iter++){
	cout << iter->first << " " << iter->second << endl;
	}*/

	/*
	//这是编码过程的输出
	cout << "LZW编码输出的信息如下：" << endl;
	for (int i = 0; i < EncodeResult.size(); i++){
		cout << EncodeResult[i].P << " " << EncodeResult[i].index << endl;
	}
	*/
	return EncodeResult;
	
}
string LZW_decode(vector<encodeinfo> code, int beginnum){
	string ret = "";//最终译码的输出
	string P = "";
	char C;
	int pW, cW;
	//第一步，初始化，读入第一个的符号cW，解码输出
	cW = code[0].index;
	ret += revdict[cW];//解码输出
	
	for (int i = 1; i < code.size(); i++){
		pW = cW;
		cW = code[i].index;
		map<int, string>::iterator iter = revdict.find(cW);

		if (iter != revdict.end()){ //找到了
			//解码输出
			ret += iter->second;
			P = revdict[pW];
			C = revdict[cW][0];
			string tempStr = P + C;
			beginnum++;
			revdict[beginnum] = tempStr;
		}
		else
		{
			P = revdict[pW];
			C = revdict[pW][0];
			beginnum++;
			string tempStr = P + C;
			revdict[beginnum] = tempStr;
			ret += tempStr;
		}
	}
	return ret;
}
int main(){

	/*
	while (1){
		cout << endl;
		cout << "请输入要编码的字符串：" << endl;
		string s;
		cin >> s;
		cout << endl;
		if (s == "quit"){
			break;
		}
		else{
			cout << "待编码字符串是：" <<s<< endl;
			dict.clear();
			revdict.clear();
			
			//应该首先建立一个包含所有单个字符ASCII码表的字符串表
			
			for (int i = 0; i < 128; i++){
				string s = "t";
				s[0] = char(i);
				dict[s] = i;

				revdict[i] = char(i);
			}
			vector<encodeinfo> res = LZW_encode(s, 128);

			cout << "解码输出字符串：" << LZW_decode(res, 128) << endl;
			cout << endl;
		}
	}*/

	/*
	通常来说，LZW标准下输入的字符串映射成定长（通常为12位）的码字，在12位，4096种可能的代码中，256个代表单个的字符，剩下的3840给出现的字符串
	但是从实际的角度，12位4096种码字太少了，根本不够用于编码长的文本，所以我这里面用的是32位定长的（也就是4B,1 int）来进行编码后码字的存储
	*/

	
	//这是输入为文件的版本
	cout << "请输出要编码文件名：" << endl;
	string filename;
	cin >> filename;
	
	cout << "请输入解码文件的地址： " << endl;
	string outfname;
	cin >> outfname;

	FILE *pFile;
	char *file = (char*)filename.c_str();
	fopen_s(&pFile, file, "rb");  //获取已打开文件的指针
	fseek(pFile, 0, SEEK_END);  //先用fseek将文件指针移到文件末尾
	int nLen = ftell(pFile);    //再用ftell获取文件内指针当前的文件位置。
	//这个位置就是文件大小。
	cout <<"文件大小是： "<< nLen <<" Byte"<< endl;
	fclose(pFile);
	

	dict.clear();
	revdict.clear();

	//应该首先建立一个包含所有单个字符ASCII码表的字符串表

	for (int i = 0; i < 128; i++){
		string s = "t";
		s[0] = char(i);
		dict[s] = i;

		revdict[i] = char(i);
	}

	ifstream fin(filename.c_str());

	string allTxtStr;
	string  s;
	while (getline(fin, s))
	{
		allTxtStr += s;
		allTxtStr += '\t';//这个是用来分割的
	}
	fin.close();
	//cout << allTxtStr << endl;

	
	cout << "正在编码..." << endl;
	vector<encodeinfo> res = LZW_encode(allTxtStr, 128);
	cout << "编码后码字长度: " << res.size()  << endl;
	
	double ratio = (double)nLen / (4 *res.size());
	printf("压缩比为：%10.3f\n", ratio);
	
	cout << "正在解码..." << endl;
	string result = LZW_decode(res, 128);//result存储的是编码后的结果
	//cout << result << endl;
	

	
	//string1是待分割的字符
	char string1[1000000];
	int i;
	//cout << result.length() << endl;
	for (i = 0; i < result.length(); i++){
		string1[i] = result[i];
	}
	string1[i] = '\0';

	char seps[] = "\t";
	char *token = NULL;
	char *next_token = NULL;


	ofstream fout(outfname,ios::out);

	// Establish string and get the first token: 
	token = strtok_s(string1, seps, &next_token);

	// While there are tokens in "string1" or "string2" 
	while (token != NULL)
	{
		// Get next token: 
		if (token != NULL)
		{
			fout << token << endl;
			token = strtok_s(NULL, seps, &next_token);
		}
	}
	fout.close();
	
	system("pause");
	return 0;
}