#include <windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <vector>

///[TW] 230307 ������ ���� ��:
/// 5. �ֽ� �������� �ٽ� �ε� �մϴ�.
/// 6. ���� ����� �����ǰų� ���� �Ǹ� ���� ������ ȭ�鿡 ǥ�� �մϴ�.
/// ����, �����丵�� �ʿ��ϴ�. 


//JSON�� �ٲٴ� ���α׷�, 
//Notes for Self.

//1. setting.ini ���Ͽ� ������ ���丮 ���� ���� �ɴϴ�. 
// -> setting.ini ������ �����, �̸� Ư�� ��ο� �����ϰ� �Ѵ�.

//2. �ش� ���丮�� �ִ� json ���ϸ� ��� �մϴ�.
// -> JSON ������ �̸� �������ų�, ����ų�. .ini ������ ��ġ�� JSON ���� �̸� -> �̸����� ����ϴ� ���̴�.
//���� ���� ������, ���� ���� ������ ����� �־�� �� ��.

//3. Ư�� json ���ϸ��� �����ϸ� �ε��ؼ� ȭ�鿡 ����մϴ�. (���� ����� �˾Ƽ�...)
// -> JSON ���ϸ��� �����ؾ� �ϴµ�, �ϴ� �̴� cin���� ����. (���ڸ� ����ؼ� �÷����� �������)

//4. �ε��ؼ� ����� ������ ������ ���, �ڵ����� ���� �ؼ� �˷� �ݴϴ�.
// -> �ε��ؼ� ����� ������ ���丮�� ����. 
// ������ ������ �ð��� ����ؼ� ��, ���� �޶��� ���, �̸� �����ؼ� �˷��ִ� ���. 

//���� ���... "cat.json ������ �����Ǿ����ϴ�. �����̽��� ������ �ٽ� �ε� �մϴ�."
// -> �̸� �����ϴ� �� �� ����. �� �� �ߵ���Ű�� �ȴ�.

//5. �ֽ� �������� �ٽ� �ε� �մϴ�.
//

//6. ���� ����� �����ǰų� ���� �Ǹ� ���� ������ ȭ�鿡 ǥ�� �մϴ�.
// ������ �Ľ��ؾ� �Ѵ�. �׷��Ƿ�, ���� �̸� ini ������ ������ �Ľ��ؼ� ������ �ʿ䰡 �ִ�.
//�̸� �ʿ��� ���� ���� �� �ֱ� ������.

namespace fs = std::filesystem; //filesystem�� ���� ����.
using json = nlohmann::json; //json ����.

//�ٲ� �κ��� ���� ����.
enum eTextColor
{
	BLACK,
	DARKBLUE,
	DARKGREEN,
	DARKRED = 4,
	BLUE = 9,
	GREEN = 10,
	RED = 12,
	YELLOW = 14,
	GRAY_BACK = 112,
	BLUE_BACK = 144,
	GREEN_BACK = 160,
	SKYBLUE_BACK = 176,
	RED_BACK = 192,
	PURPLE_BACK = 208
};

std::wstring StrToWStr(std::string _str);
std::string WStrToStr(std::wstring _str);
HANDLE ReturnFirstFile(std::wstring _fileDir, WIN32_FIND_DATA* _outWin32Data);
BOOL CreateINIFile(std::string _fileDir);
std::vector<std::string> StringToStrVector(std::string _input);

void SetConsoleColor(HANDLE& _handle, eTextColor _textCol);
void ResetColor(HANDLE& _handle);



int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	//JSON ���ϵ��� ����Ʈ�� ��� std::string.
	std::string jsonListString = "";

	while (true)
	{
		int ans = -1;
		std::cout << "JSON�� ��� ����ðڽ��ϱ�? 0 : �ƴϿ� // 1: ��. => ";
		std::cin >> ans;

		if (ans == 0)
		{
			break;
		}
		else if (ans == 1)
		{
			std::string TmpDir = "";
			std::string TmpName = "";
			std::string TmpKeyOne = "";
			std::string TmpValOne = "";
			std::cout << "JSON ������ ���� ��θ� �Է����ּ���. => ";
			std::cin >> TmpDir;
			std::cout << "JSON ������ �̸��� �Է����ּ���. => ";
			std::cin >> TmpName;
			std::cout << "���� �� �� Key�� �Է����ּ���. => ";
			std::cin >> TmpKeyOne;
			std::cout << "���� �� �� Value�� �Է����ּ���. => ";
			std::cin >> TmpValOne;

			json tmpJsonFile;
			tmpJsonFile[TmpKeyOne] = TmpValOne;

			std::string tmpFileComplete = TmpDir + '\\' + TmpName + ".json";
			std::ofstream out(tmpFileComplete);
			out << tmpJsonFile;
			std::cout << tmpFileComplete << "�� �����Ǿ����ϴ�. " << std::endl;
		}
		else
		{
			std::cout << "�߸��� �����Դϴ�. " << std::endl;
		}
		std::cout << '\n';
	}

	BOOL res = true;
	std::string fileDir = "";
	std::cout << ".ini File ���ο� ��ϵ� ���� ���丮�� ��������. => ";
	std::cin >> fileDir;

	std::cin.clear(); //cin ���� Ŭ����.

	res = CreateINIFile(fileDir);


	if (res == true)
	{
		std::cout << ".ini File�� �����ϰ�, ������ Directory�� �����ɴϴ�. " << std::endl;
		//1. INI File ���� Directory�� �о�´�.
		//�ϳ��ۿ� ���� ����, '='������ ��ȣ���� �� �� ������, ���. 
		//��� �ڵ忡 ������ ���Ĵ�� ������ ������� ���̱⿡ �����ϴ� ����.

		std::fstream forIni;
		fs::path nowPath = fs::current_path(); //���� �Ҽӵ� ���.
		std::wstring nowPathStr = nowPath;
		std::wstring fileName = nowPathStr.append(L"\\setting.ini");
		forIni.open(fileName);
		//INI file�� �� ��Ȳ.

		std::string fromIniDirectory = "";
		
		bool metEquals = false;

		if (forIni.is_open())
		{
			char ch;
			//White Space�� ��ŵ���� �ʴ´�.
			while (forIni >> std::noskipws >> ch) 
			{
				if (metEquals && ch != '\n') //���⼭ ���� �޴� ���̴�. 
				{
					fromIniDirectory += ch;
				}

				if (ch == '=')
				{
					metEquals = true;
				}
			}
		}
		forIni.close();
		//�� ���������� fromIniDirectory�� ���� ����.
		std::cout << "------------------------------------" << '\n';
		std::string ext(".json");

		std::cout << "���α׷��� ã�� json ����(��)�Դϴ�. " << '\n';

		//3. �� �� �ְ� �Ѵ�. 
		std::vector<fs::directory_entry> jsonVector;

		int jsonFileCnt = 0;
		jsonListString = "";
		for (auto& it : fs::recursive_directory_iterator(fromIniDirectory))
		{
			//������� �ִ� ���ϵ��� �̸��� �о���� ���̴�. 
			if (it.path().extension() == ext)
			{
				jsonVector.push_back(it);
				std::cout << jsonFileCnt << " : " << (it.path().stem().string()) + ext << '\n';
				
				/// ���߿� �ٸ� ���� �˻��Ϸ��� string�� �־���´�.
				jsonListString += ((it.path().stem().string()) + ext);
				jsonListString += '\n';

				jsonFileCnt++;
			}
		}
		
		if (jsonFileCnt == 0)
		{
			std::cout << "Json ������ �߰����� ���߽��ϴ�. " << std::endl;
			return 0;
		}

		std::cout << "------------------------------------" << std::endl;
		std::cout << "������ ������ �ش� ���ڸ� �Է����ּ���. => ";
		int fileOpenAns = 0;
		std::cin >> fileOpenAns;

		jsonFileCnt = 0; //�ٽ� ���� �Ѵ�.
		fs::directory_entry chosenIt; //�� JSON ����.

		for (auto& it : fs::recursive_directory_iterator(fromIniDirectory))
		{
			//������� �ִ� ���ϵ��� �̸��� �о���� ���̴�. 
			if (it.path().extension() == ext)
			{
				//std::cout << jsonFileCnt << " : " << (it.path().stem().string()) + ext << '\n';
				if (fileOpenAns == jsonFileCnt)
				{
					chosenIt = it;
					break;
				}
				jsonFileCnt++;
			}
		}

		//�̶��� �̹� ������� ���̴�. 
		//���� ���� ������ �� �� �־�� �Ѵ�.	
		
		//4. �� �� �� �� �ְ� �Ѵ�.
		std::string chosenString = chosenIt.path().string();
		std::ifstream jsonIStream(chosenString.c_str());
		json readJson = json::parse(jsonIStream);
		
		std::string jsonParsedString = readJson.dump();
		std::cout << "------------------------------------" << std::endl;
		std::cout << (chosenIt.path().stem().string()) + ext << " -> " << "\n\n";
		std::cout << jsonParsedString << std::endl;
		std::cout << "------------------------------------" << std::endl;

		//�������� json ���� ���鼭 ���. (Edwith�� 3������ �Ϸ�� ��Ȳ)
		std::string recordParsedString = jsonParsedString; //�̸� ���߿� ����� ���� ����Ͽ� Json ������ ������ ���.

		//���Ѻ��� �� ����: 
		//���� ����ߴ� JSON ������ ������ �����Ǿ��� -> 
		// ���� ���丮 ������ ���� ����� �����Ǿ��ų�. (JSON ����)
		//<filesystem>�� last_write_time. (�ϳ��� ���õ� JSON, �ϳ��� �� �� ���� ���)

		//fromIniDirectory�� .ini���� �о�� ���̴�. �� �� �ִ� ��. (���� ���� ���)
		//chosenString�� ���õ� String. (��ü���� json����)

		fs::path watchUpperDirPath = fromIniDirectory;
		fs::path watchJsonDirPath = chosenString; //filesystem�� path �����ڸ� ȣ���ϴ� ���̴�.

		auto originUpperLastTime = fs::last_write_time(watchUpperDirPath); //���� ���丮�� ������ ���� �ð�
		auto originJsonLastTime = fs::last_write_time(watchJsonDirPath); // ������ �ε�� JSON�� ������ ���� �ð�
		
		//�������ʹ� ��� Game Loopó�� ���鼭 �������� �ִ��� ���� ���� �ְ� �ȴ�.

		fs::file_time_type updateUpperLastTime; //���Ӱ� �����ϴ� Last Time. (������Ʈ ����)
		fs::file_time_type updateJsonLastTime;


		//string �˻��� �⺻ ���� : 
		//JSON ���� ����Ʈ: jsonListString.
		//JSON String ���� -> recordParsedString.

		while (true)
		{
			updateUpperLastTime = fs::last_write_time(watchUpperDirPath);
			updateJsonLastTime = fs::last_write_time(watchJsonDirPath);

			//Directory �ð��� �ٸ���. 
			if (originUpperLastTime != updateUpperLastTime)
			{
				std::cout << "���丮 ���ΰ� �����Ǿ����ϴ�. \n" << std::endl;
				//�۵��Ѵ�! ���� ���⼭ ���丮 �����. -> (������ ���� ���� ����� ������� ���� ����, -> �̸� �ϳ��� String���� �ٲٵ� �ؾ� �Ѵ�.
				//��, ����� ���� ������, ��� ��������� �߻��ߴ��� ���� �����ؾ�.
				
				std::string updatedListString = "";

				for (auto& it : fs::recursive_directory_iterator(fromIniDirectory))
				{
					//������� �ִ� ���ϵ��� �̸��� �о���� ���̴�. 
					if (it.path().extension() == ext)
					{
						jsonVector.push_back(it);
						//���� ���߿� ���� �ٲپ� ����ϱ� ����.
						//std::cout << (it.path().stem().string()) + ext << '\n';

						/// ���߿� �ٸ� ���� �˻��Ϸ��� string�� �־���´�.
						updatedListString += ((it.path().stem().string()) + ext);
						updatedListString += '\n';

						jsonFileCnt++;
					}
				}


				///���⼭ jsonListString�� updatedListString�� ���ؾ� �Ѵ�.
				///�߰��� �����Ǵ�, �ٲ� ���� �˾ƾ� �ϹǷ�, �� string�� std::vector�� ������,
				///�� ��� ���ϴ� ���� for��.
				///�������� �����µ� ���� �������� �ʷϻ� ��� (����)
				///�������� �־��µ� ���� ��������� ������ ��� (����)
				
				auto pastVec = StringToStrVector(jsonListString);
				auto nowVec = StringToStrVector(updatedListString);

				//���� �� ���.
				std::cout << "���� ���丮 ���� ==>" << '\n';
				for (auto pastIt : pastVec) //���� �ٲ� ����Ʈ ����.
				{
					bool isExisting = false;
					for (auto nowIt : nowVec)
					{
						if (pastIt == nowIt)
						{
							isExisting = true;
						}
					}
					
					if (isExisting)
					{
						std::cout << pastIt << std::endl;
					}
					else
					{
						SetConsoleColor(hConsole, RED_BACK);
						std::cout << pastIt << std::endl;
						ResetColor(hConsole);
					}
				}

				//�ٲ�� ���.
				std::cout << "\n����� ���丮 ���� ==> " << '\n';
				for (auto nowIt : nowVec) //���� �ٲ� ����Ʈ ����.
				{
					bool didExist = false;
					for (auto pastIt : pastVec)
					{
						if (pastIt == nowIt)
						{
							didExist = true;
						}
					}

					if (didExist)
					{
						std::cout << nowIt << std::endl;
					}
					else
					{
						SetConsoleColor(hConsole, GREEN_BACK);
						std::cout << nowIt << std::endl;
						ResetColor(hConsole);
					}
				}

				if (jsonFileCnt == 0)
				{
					std::cout << "Json ������ �߰����� ���߽��ϴ�. " << std::endl;
					return 0;
				}
				std::cout << "\n\n";

				//jsonListUpdateString��  �̹� �ڵ����� ������Ʈ�ȴ�. 
				originUpperLastTime = updateUpperLastTime;
			}

			//Json �ð��� �ٸ���. 
			if (originJsonLastTime != updateJsonLastTime)
			{
				std::cout << "��µǾ��� JSON�� �����Ǿ����ϴ�. " << std::endl;

				originJsonLastTime = updateJsonLastTime;
			}
		}
	}
	else
	{
		std::cout << ".ini File�� ����� �������� ������ �߻��߽��ϴ�. " << std::endl;
		return 0;
	}
	
	
	
	std::cout << "END" << std::endl;

	return 0;
}

std::wstring StrToWStr(std::string _str)
{
	std::wstring later = L"";
	later.assign(_str.begin(), _str.end());
	return later;
}

std::string WStrToStr(std::wstring _str)
{
	std::string later = "";
	later.assign(_str.begin(), _str.end());
	return later;
}

HANDLE ReturnFirstFile(std::wstring _fileDir, WIN32_FIND_DATA* _outWin32Data)
{
	HANDLE hFindFile;
	hFindFile = FindFirstFile(_fileDir.c_str(), _outWin32Data); //�̷��� �޴´�. ���� ���丮 ���� FindFirstFile.
	return hFindFile;
}

//ini�� ���丮�� ������´�. INI file�� proj ���丮��.
BOOL CreateINIFile(std::string _fileDir)
{
	BOOL res = true;
	std::wstring appName = L"DIRECTORY";
	std::wstring keyName = L"dir";
	std::wstring wFileDir = StrToWStr(_fileDir);

	fs::path nowPath = fs::current_path(); //���� �Ҽӵ� ���.
	std::wstring nowPathStr = nowPath;
	std::wstring fileName = nowPathStr.append(L"\\setting.ini");
	res = WritePrivateProfileString(appName.c_str(), keyName.c_str(), wFileDir.c_str(), fileName.c_str());
	return res;
}

void SetConsoleColor(HANDLE& _handle, eTextColor _textCol)
{
	SetConsoleTextAttribute(_handle, _textCol);
}

void ResetColor(HANDLE& _handle)
{
	SetConsoleTextAttribute(_handle, 15);
}

std::vector<std::string> StringToStrVector(std::string _input)
{
	//�� ��� ���ϴµ� ���̴� toReturn�� ���� String���� \n�� �ȵ�. 
	std::vector<std::string> toReturn;
	std::string conveyor = "";
	for (auto it : _input)
	{
		if (it == '\n')
		{
			toReturn.push_back(conveyor);
			conveyor = "";
		}
		else
		{
			conveyor += it;
		}
	}
	return toReturn;
}





//D:\CPPFiles\JSON_INI_TEST (���� �׽�Ʈ ���� ����)
//D:\CPPFiles\JSON_INI_TEST
