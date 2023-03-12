#include <windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <vector>

///[TW] 230307 앞으로 남은 일:
/// 5. 최신 버전으로 다시 로드 합니다.
/// 6. 파일 목록이 수정되거나 변경 되면 변경 사항을 화면에 표시 합니다.
/// 또한, 리팩토링이 필요하다. 


//JSON을 바꾸는 프로그램, 
//Notes for Self.

//1. setting.ini 파일에 설정한 디렉토리 명을 가져 옵니다. 
// -> setting.ini 파일을 만들고, 이를 특정 경로에 저장하게 한다.

//2. 해당 디렉토리에 있는 json 파일명만 출력 합니다.
// -> JSON 파일을 미리 만들어놓거나, 써놓거나. .ini 파일이 위치한 JSON 파일 이름 -> 이름들을 출력하는 것이다.
//여러 개가 있으면, 여러 개를 보여줄 방법이 있어야 할 것.

//3. 특정 json 파일명을 선택하면 로드해서 화면에 출력합니다. (선택 방법은 알아서...)
// -> JSON 파일명을 선택해야 하는데, 일단 이는 cin으로 하자. (숫자를 계속해서 늘려가는 방식으로)

//4. 로드해서 출력한 파일이 수정된 경우, 자동으로 감지 해서 알려 줍니다.
// -> 로드해서 출력한 파일은 디렉토리를 저장. 
// 파일이 수정된 시간을 계속해서 비교, 만약 달라질 경우, 이를 감지해서 알려주는 방식. 

//예를 들어... "cat.json 파일이 수정되었습니다. 스페이스를 누르면 다시 로드 합니다."
// -> 이를 감지하는 건 한 순간. 그 때 발동시키면 된다.

//5. 최신 버전으로 다시 로드 합니다.
//

//6. 파일 목록이 수정되거나 변경 되면 변경 사항을 화면에 표시 합니다.
// 파일을 파싱해야 한다. 그러므로, 전에 미리 ini 파일의 내용을 파싱해서 저장할 필요가 있다.
//이를 필요할 때만 비교할 수 있기 때문에.

namespace fs = std::filesystem; //filesystem을 보기 위해.
using json = nlohmann::json; //json 묶기.

//바뀐 부분을 보기 위해.
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
	
	//JSON 파일들의 리스트를 담는 std::string.
	std::string jsonListString = "";

	while (true)
	{
		int ans = -1;
		std::cout << "JSON을 계속 만드시겠습니까? 0 : 아니오 // 1: 예. => ";
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
			std::cout << "JSON 파일을 만들 경로를 입력해주세요. => ";
			std::cin >> TmpDir;
			std::cout << "JSON 파일의 이름을 입력해주세요. => ";
			std::cin >> TmpName;
			std::cout << "파일 내 들어갈 Key를 입력해주세요. => ";
			std::cin >> TmpKeyOne;
			std::cout << "파일 내 들어갈 Value를 입력해주세요. => ";
			std::cin >> TmpValOne;

			json tmpJsonFile;
			tmpJsonFile[TmpKeyOne] = TmpValOne;

			std::string tmpFileComplete = TmpDir + '\\' + TmpName + ".json";
			std::ofstream out(tmpFileComplete);
			out << tmpJsonFile;
			std::cout << tmpFileComplete << "가 생성되었습니다. " << std::endl;
		}
		else
		{
			std::cout << "잘못된 응답입니다. " << std::endl;
		}
		std::cout << '\n';
	}

	BOOL res = true;
	std::string fileDir = "";
	std::cout << ".ini File 내부에 기록될 파일 디렉토리를 넣으세요. => ";
	std::cin >> fileDir;

	std::cin.clear(); //cin 버퍼 클리어.

	res = CreateINIFile(fileDir);


	if (res == true)
	{
		std::cout << ".ini File을 오픈하고, 내부의 Directory를 가져옵니다. " << std::endl;
		//1. INI File 내부 Directory를 읽어온다.
		//하나밖에 없을 예정, '='까지의 기호들을 싹 다 버리고, 잡기. 
		//모든 코드에 동일한 형식대로 내용이 들어있을 것이기에 성립하는 내용.

		std::fstream forIni;
		fs::path nowPath = fs::current_path(); //현재 소속된 경로.
		std::wstring nowPathStr = nowPath;
		std::wstring fileName = nowPathStr.append(L"\\setting.ini");
		forIni.open(fileName);
		//INI file을 연 상황.

		std::string fromIniDirectory = "";
		
		bool metEquals = false;

		if (forIni.is_open())
		{
			char ch;
			//White Space를 스킵하지 않는다.
			while (forIni >> std::noskipws >> ch) 
			{
				if (metEquals && ch != '\n') //여기서 값을 받는 것이다. 
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
		//이 시점에서는 fromIniDirectory에 값이 들어갔음.
		std::cout << "------------------------------------" << '\n';
		std::string ext(".json");

		std::cout << "프로그램이 찾은 json 파일(들)입니다. " << '\n';

		//3. 고를 수 있게 한다. 
		std::vector<fs::directory_entry> jsonVector;

		int jsonFileCnt = 0;
		jsonListString = "";
		for (auto& it : fs::recursive_directory_iterator(fromIniDirectory))
		{
			//만들어져 있던 파일들의 이름을 읽어오는 것이다. 
			if (it.path().extension() == ext)
			{
				jsonVector.push_back(it);
				std::cout << jsonFileCnt << " : " << (it.path().stem().string()) + ext << '\n';
				
				/// 나중에 다른 점을 검사하려고 string에 넣어놓는다.
				jsonListString += ((it.path().stem().string()) + ext);
				jsonListString += '\n';

				jsonFileCnt++;
			}
		}
		
		if (jsonFileCnt == 0)
		{
			std::cout << "Json 파일을 발견하지 못했습니다. " << std::endl;
			return 0;
		}

		std::cout << "------------------------------------" << std::endl;
		std::cout << "파일을 열려면 해당 숫자를 입력해주세요. => ";
		int fileOpenAns = 0;
		std::cin >> fileOpenAns;

		jsonFileCnt = 0; //다시 세야 한다.
		fs::directory_entry chosenIt; //고른 JSON 파일.

		for (auto& it : fs::recursive_directory_iterator(fromIniDirectory))
		{
			//만들어져 있던 파일들의 이름을 읽어오는 것이다. 
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

		//이때는 이미 골라졌을 때이다. 
		//이제 정한 곳에서 열 수 있어야 한다.	
		
		//4. 고른 뒤 열 수 있게 한다.
		std::string chosenString = chosenIt.path().string();
		std::ifstream jsonIStream(chosenString.c_str());
		json readJson = json::parse(jsonIStream);
		
		std::string jsonParsedString = readJson.dump();
		std::cout << "------------------------------------" << std::endl;
		std::cout << (chosenIt.path().stem().string()) + ext << " -> " << "\n\n";
		std::cout << jsonParsedString << std::endl;
		std::cout << "------------------------------------" << std::endl;

		//이제부터 json 파일 보면서 기록. (Edwith의 3번까지 완료된 상황)
		std::string recordParsedString = jsonParsedString; //미리 나중에 변경될 것을 대비하여 Json 내부의 내용을 기록.

		//지켜봐야 할 예시: 
		//위에 출력했던 JSON 파일의 내용이 수정되었나 -> 
		// 파일 디렉토리 내부의 파일 목록이 수정되었거나. (JSON 한정)
		//<filesystem>의 last_write_time. (하나는 선택된 JSON, 하나는 그 위 상위 경로)

		//fromIniDirectory는 .ini에서 읽어온 것이다. 쓸 수 있는 것. (상위 파일 경로)
		//chosenString는 선택된 String. (구체적인 json파일)

		fs::path watchUpperDirPath = fromIniDirectory;
		fs::path watchJsonDirPath = chosenString; //filesystem의 path 생성자를 호출하는 것이다.

		auto originUpperLastTime = fs::last_write_time(watchUpperDirPath); //파일 디렉토리의 마지막 수정 시간
		auto originJsonLastTime = fs::last_write_time(watchJsonDirPath); // 마지막 로드된 JSON의 마지막 수정 시간
		
		//이제부터는 계속 Game Loop처럼 보면서 변경점이 있는지 보는 일이 주가 된다.

		fs::file_time_type updateUpperLastTime; //새롭게 등장하는 Last Time. (업데이트 받음)
		fs::file_time_type updateJsonLastTime;


		//string 검사할 기본 기준 : 
		//JSON 파일 리스트: jsonListString.
		//JSON String 내부 -> recordParsedString.

		while (true)
		{
			updateUpperLastTime = fs::last_write_time(watchUpperDirPath);
			updateJsonLastTime = fs::last_write_time(watchJsonDirPath);

			//Directory 시간이 다르다. 
			if (originUpperLastTime != updateUpperLastTime)
			{
				std::cout << "디렉토리 내부가 수정되었습니다. \n" << std::endl;
				//작동한다! 이제 여기서 디렉토리 재출력. -> (예전에 파일 내부 목록을 출력했을 때를 보고, -> 이를 하나의 String으로 바꾸든 해야 한다.
				//단, 경우의 수를 나눠서, 어떻게 변경사항이 발생했는지 역시 저장해야.
				
				std::string updatedListString = "";

				for (auto& it : fs::recursive_directory_iterator(fromIniDirectory))
				{
					//만들어져 있던 파일들의 이름을 읽어오는 것이다. 
					if (it.path().extension() == ext)
					{
						jsonVector.push_back(it);
						//직접 나중에 색깔 바꾸어 출력하기 위해.
						//std::cout << (it.path().stem().string()) + ext << '\n';

						/// 나중에 다른 점을 검사하려고 string에 넣어놓는다.
						updatedListString += ((it.path().stem().string()) + ext);
						updatedListString += '\n';

						jsonFileCnt++;
					}
				}


				///여기서 jsonListString과 updatedListString을 비교해야 한다.
				///중간에 삭제되던, 바뀐 점을 알아야 하므로, 각 string을 std::vector로 넣은뒤,
				///각 요소 비교하는 이중 for문.
				///예전에는 없었는데 이제 생겼으면 초록색 배경 (현재)
				///예전에는 있었는데 이제 사라졌으면 빨간색 배경 (과거)
				
				auto pastVec = StringToStrVector(jsonListString);
				auto nowVec = StringToStrVector(updatedListString);

				//원래 꺼 출력.
				std::cout << "원래 디렉토리 구성 ==>" << '\n';
				for (auto pastIt : pastVec) //과거 바뀐 리스트 보기.
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

				//바뀐거 출력.
				std::cout << "\n변경된 디렉토리 구성 ==> " << '\n';
				for (auto nowIt : nowVec) //현재 바뀐 리스트 보기.
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
					std::cout << "Json 파일을 발견하지 못했습니다. " << std::endl;
					return 0;
				}
				std::cout << "\n\n";

				//jsonListUpdateString은  이미 자동으로 업데이트된다. 
				originUpperLastTime = updateUpperLastTime;
			}

			//Json 시간이 다르다. 
			if (originJsonLastTime != updateJsonLastTime)
			{
				std::cout << "출력되었던 JSON이 수정되었습니다. " << std::endl;

				originJsonLastTime = updateJsonLastTime;
			}
		}
	}
	else
	{
		std::cout << ".ini File을 만드는 과정에서 오류가 발생했습니다. " << std::endl;
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
	hFindFile = FindFirstFile(_fileDir.c_str(), _outWin32Data); //이렇게 받는다. 파일 디렉토리 내에 FindFirstFile.
	return hFindFile;
}

//ini에 디렉토리를 적어놓는다. INI file은 proj 디렉토리에.
BOOL CreateINIFile(std::string _fileDir)
{
	BOOL res = true;
	std::wstring appName = L"DIRECTORY";
	std::wstring keyName = L"dir";
	std::wstring wFileDir = StrToWStr(_fileDir);

	fs::path nowPath = fs::current_path(); //현재 소속된 경로.
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
	//각 요소 비교하는데 쓰이는 toReturn의 내부 String에는 \n이 안들어감. 
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





//D:\CPPFiles\JSON_INI_TEST (내가 테스트 중인 폴더)
//D:\CPPFiles\JSON_INI_TEST
