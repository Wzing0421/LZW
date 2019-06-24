#include <iostream>
#include <string.h>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <fstream>
using namespace std;

map <string, int> dict;
map <int, string> revdict; //���������洢�����ʱ����ֵ��
struct encodeinfo
{
	string P;
	int index;
};
vector<encodeinfo> LZW_encode(string s, int encodenum){
	string P = "";
	char C;
	vector<encodeinfo> EncodeResult; //�洢����֮��Ľ��

	for (int i = 0; i < s.length(); i++){
		C = s[i];
		string tempStr = P + C;
		//���ֵ�����Ѱ������ַ���
		map<string, int>::iterator iter = dict.find(tempStr);
		if (iter != dict.end()){//�ҵ���
			P = tempStr;
		}
		else{//û�ҵ�
			encodeinfo a = { P, dict[P] };
			EncodeResult.push_back(a);//��P�Ķ�Ӧ�ı���������
			//������һ���µ�����
			encodenum++;
			dict[tempStr] = encodenum;
			P = C;
		}
	}
	encodeinfo a = { P, dict[P] };
	EncodeResult.push_back(a);//���ս�β��
	
	
	/*
	//���������ֵ�����
	map<string, int>::iterator iter = dict.begin();
	for (; iter != dict.end(); iter++){
	cout << iter->first << " " << iter->second << endl;
	}*/

	/*
	//���Ǳ�����̵����
	cout << "LZW�����������Ϣ���£�" << endl;
	for (int i = 0; i < EncodeResult.size(); i++){
		cout << EncodeResult[i].P << " " << EncodeResult[i].index << endl;
	}
	*/
	return EncodeResult;
	
}
string LZW_decode(vector<encodeinfo> code, int beginnum){
	string ret = "";//������������
	string P = "";
	char C;
	int pW, cW;
	//��һ������ʼ���������һ���ķ���cW���������
	cW = code[0].index;
	ret += revdict[cW];//�������
	
	for (int i = 1; i < code.size(); i++){
		pW = cW;
		cW = code[i].index;
		map<int, string>::iterator iter = revdict.find(cW);

		if (iter != revdict.end()){ //�ҵ���
			//�������
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
		cout << "������Ҫ������ַ�����" << endl;
		string s;
		cin >> s;
		cout << endl;
		if (s == "quit"){
			break;
		}
		else{
			cout << "�������ַ����ǣ�" <<s<< endl;
			dict.clear();
			revdict.clear();
			
			//Ӧ�����Ƚ���һ���������е����ַ�ASCII�����ַ�����
			
			for (int i = 0; i < 128; i++){
				string s = "t";
				s[0] = char(i);
				dict[s] = i;

				revdict[i] = char(i);
			}
			vector<encodeinfo> res = LZW_encode(s, 128);

			cout << "��������ַ�����" << LZW_decode(res, 128) << endl;
			cout << endl;
		}
	}*/

	/*
	ͨ����˵��LZW��׼��������ַ���ӳ��ɶ�����ͨ��Ϊ12λ�������֣���12λ��4096�ֿ��ܵĴ����У�256�����������ַ���ʣ�µ�3840�����ֵ��ַ���
	���Ǵ�ʵ�ʵĽǶȣ�12λ4096������̫���ˣ������������ڱ��볤���ı����������������õ���32λ�����ģ�Ҳ����4B,1 int�������б�������ֵĴ洢
	*/

	
	//��������Ϊ�ļ��İ汾
	cout << "�����Ҫ�����ļ�����" << endl;
	string filename;
	cin >> filename;
	
	cout << "����������ļ��ĵ�ַ�� " << endl;
	string outfname;
	cin >> outfname;

	FILE *pFile;
	char *file = (char*)filename.c_str();
	fopen_s(&pFile, file, "rb");  //��ȡ�Ѵ��ļ���ָ��
	fseek(pFile, 0, SEEK_END);  //����fseek���ļ�ָ���Ƶ��ļ�ĩβ
	int nLen = ftell(pFile);    //����ftell��ȡ�ļ���ָ�뵱ǰ���ļ�λ�á�
	//���λ�þ����ļ���С��
	cout <<"�ļ���С�ǣ� "<< nLen <<" Byte"<< endl;
	fclose(pFile);
	

	dict.clear();
	revdict.clear();

	//Ӧ�����Ƚ���һ���������е����ַ�ASCII�����ַ�����

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
		allTxtStr += '\t';//����������ָ��
	}
	fin.close();
	//cout << allTxtStr << endl;

	
	cout << "���ڱ���..." << endl;
	vector<encodeinfo> res = LZW_encode(allTxtStr, 128);
	cout << "��������ֳ���: " << res.size()  << endl;
	
	double ratio = (double)nLen / (4 *res.size());
	printf("ѹ����Ϊ��%10.3f\n", ratio);
	
	cout << "���ڽ���..." << endl;
	string result = LZW_decode(res, 128);//result�洢���Ǳ����Ľ��
	//cout << result << endl;
	

	
	//string1�Ǵ��ָ���ַ�
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