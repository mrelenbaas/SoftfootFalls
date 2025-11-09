#include "SoftfootFalls.h"

#include "Time.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <cassert>



char** splitString(const std::string& inputString, int& tokenCount) {
#ifdef __linux__
	// Create a mutable copy of the input string as strtok modifies the string.
	char* cstr = new char[inputString.length() + 1];
	strcpy(cstr, inputString.c_str());

	// First pass to count tokens
	char* tempCstr = new char[inputString.length() + 1];
	strcpy(tempCstr, inputString.c_str());
	char* token = strtok(tempCstr, " ");
	tokenCount = 0;
	while (token != nullptr) {
		tokenCount++;
		token = strtok(nullptr, " ");
	}
	delete[] tempCstr; // Clean up temporary buffer

	// Allocate memory for the char** array
	char** tokensArray = new char* [tokenCount];

	// Second pass to store tokens
	int i = 0;
	token = strtok(cstr, " "); // Use the original mutable copy
	while (token != nullptr) {
		tokensArray[i] = new char[strlen(token) + 1]; // Allocate for each token
		strcpy(tokensArray[i], token);
		token = strtok(nullptr, " ");
		i++;
	}

	delete[] cstr; // Clean up the mutable copy of the input string
	return tokensArray;
#endif
	return NULL;
}

void LinuxProcess(char* text)
{
#ifdef __linux__

	/*
	int spaceCount = 0;
	int textLimit = strlen(command);
	for (int i = 0; i < textLimit; ++i)
	{
		if (command[i] == ' ')
		{
			++spaceCount;
		}
	}
	++spaceCount;
	printf("space count: %i\n", spaceCount);
	char modifiedText[textLimit];
	//modifiedText[textLimit] = nullptr;
	for (int i = 0; i < textLimit; ++i)
	{
		if (command[i] == ' ')
		{
			modifiedText[i] = '\0';
		}
		else
		{
			modifiedText[i] = command[i];
		}
	}
	printf("command: %s\n", command);
	printf("mod : %s\n", modifiedText);
	char* gridTexts[spaceCount + 1];
	gridTexts[spaceCount] = NULL;
	gridTexts[0] = modifiedText;
	if (spaceCount > 1)
	{
		int j = 1;
		for (int i = 0; i < textLimit; ++i)
		{
			if (modifiedText[i] == '\0')
			{
				printf(">> %s\n", &modifiedText[i + 1]);
				gridTexts[j] = &modifiedText[i + 1];
				++j;
			}
		}
	}
	for (int i = 0; i < spaceCount + 1; ++i)
	{
		printf("gridTexts[%i]: %s\n", i, gridTexts[i]);
	}
	*/

	//std::string sentence = "This is a sample string to split";
	int count = 0;
	char** words = splitString(command, count);


	std::cout << "Tokens:" << std::endl;
	for (int i = 0; i < count; ++i) {
		std::cout << words[i] << std::endl;
	}

	/*char* token;
	token = strtok(command, " ");
	int tokensLimit = strlen(token);//WRONG
	if (tokensLimit == strlen(command))
	printf("\nTOKEN LENGTH: %i\n", tokensLimit);
	char* tokens[tokensLimit + 1];
	int tokenCounter = 0;
	while (token != NULL) {
		tokens[tokenCounter] = token;
		token = strtok(NULL, " \t\n");
		++tokenCounter;
	}
	tokens[tokenCounter] = nullptr;
	for (int i = 0; i < tokensLimit; ++i)
	{
		printf("tokens[%i]: %s\n", i, tokens[i]);
	}*/
	//execl("/bin/ls", command, NULL);
	pid_t pid = fork();
	if (pid == -1) perror("fork failed");
	else if (pid == 0) // Child.
	{
		std::cout << "Child process (PID: " << getpid() << ") is about to execute a new program." << std::endl;
		execv("/bin/ls", words);
		//perror("execv failed");
	}
	else // Parent.
	{
		std::cout << "Parent process (PID: " << getpid() << ") is waiting for child (PID: " << pid << ")." << std::endl;
		int status;
		waitpid(pid, &status, 0); // Wait for the child process to complete
		std::cout << "Parent process: Child finished with status " << WEXITSTATUS(status) << std::endl;
	}
#endif
}

static std::string FakeExec(const char* text)
{
	std::array<char, 128> buffer{};
	std::string result;
#ifdef _WIN32
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(text, "r"), _pclose);
#else
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
#endif
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
// https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa
void WindowsProcess(char* narrowText)
{
#ifdef _WIN32
	auto NarrowTextToWideText = [&]()
	{
		size_t limit;                                           // Declare a variable for wide command's size.
		mbstowcs_s(&limit, nullptr, 0, narrowText, _TRUNCATE);  // Define the wide command's size.
		wchar_t* command = new wchar_t[limit];                     // Declare wide command.
		mbstowcs_s(&limit, command, limit, narrowText, _TRUNCATE); // Define wide command.
		return command;
	};
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	wchar_t* command = NarrowTextToWideText();
	if (!CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
	{
		printf("ERROR: Failed to create process: %ul\n", GetLastError());
	}
	delete[] command;
	printf("SUCCESS: Process ID, %i\n----\n", processInfo.dwProcessId);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	//try {
	//	// Example: execute the "dir" command in Command Prompt and print its output
	//	// For other commands, replace "dir" with the desired command.
	//	std::string output = FakeExec("dir");
	//	std::cout << "Command Output:\n" << output << std::endl;
	//}
	//catch (const std::runtime_error& e) {
	//	std::cerr << "Error: " << e.what() << std::endl;
	//	//return 1;
	//}
#endif
}

void RunProcess(char* text)
{
#ifdef _WIN32
	WindowsProcess(text);
#elif __linux__
	LinuxProcess(command);
#endif
}

const char* ERROR_NULL_TERMINATOR = "ERROR: The null terminator on program name's copy was not found.";
const char* ERROR_FILE_OPENED = "ERROR: Failed to open file.";
const char* SUCCESS_FILE_OPENED = "SUCCESS: File opened.\n";
const char* ERROR_FILE_CLOSED = "ERROR: Failed to close file.";
const char* SUCCESS_FILE_CLOSED = "SUCCESS: File closed.\n";

static void Write(const char* filename, const char* text)
{
	std::ofstream writer(filename, std::ios::app);
	assert(writer.is_open() == true && ERROR_FILE_OPENED);
	printf(SUCCESS_FILE_OPENED);
	writer << text << std::endl;
	writer.close();
	assert(writer.is_open() == false && ERROR_FILE_CLOSED);
	printf(SUCCESS_FILE_CLOSED);
}

// https://en.cppreference.com/w/cpp/string/basic_string/getline.html
static void Read(const char* filename)
{
	std::ifstream reader(filename);
	assert(reader.is_open() == true && ERROR_FILE_OPENED);
	printf(SUCCESS_FILE_OPENED);
	std::string line;
	int i = 0;
	while (std::getline(reader, line))
	{
		printf("line[%i]: ", i);
		for (int i = 0; i < line.size(); ++i) printf("%c", line[i]);
		printf("\n");
		++i;
	}
	reader.close();
	assert(reader.is_open() == false && ERROR_FILE_CLOSED);
	printf(SUCCESS_FILE_CLOSED);
}

// Base case for the recursion: handles no arguments
void processStrings() {
	// End of recursion
	std::cout << "--- End of processing ---" << std::endl;
}

// Variadic template function to process C-style strings
template <typename T, typename... Args>
void processStrings(T firstArg, Args... args) {
	// Ensure the first argument is a C-style string
	static_assert(std::is_convertible<T, const char*>::value, "Argument must be convertible to const char*");

	const char* c_str = static_cast<const char*>(firstArg);
	std::cout << "Processing string: \"" << c_str << "\" (Length: " << std::strlen(c_str) << ")" << std::endl;
	Write("data.csv", c_str);

	// Recursively call with the remaining arguments
	processStrings(args...);
}

class StringNode
{
public:
	StringNode(const char* text)
	{
		printf("StringNode Constructor\n");
		(*this).text = new char[strlen(text) + 1];
		int i;
		for (i = 0; text[i] != '\0'; ++i)
		{
			(*this).text[i] = text[i];
		}
		printf("StringNode: %s\n", text);
		(*this).text[--i] = '\0';
	};
	~StringNode()
	{
		delete[] text;
	};
	void Print()
	{
		printf("HERER: %s\n", text);
	};
	void SetNext(StringNode* next)
	{
		(*this).next = next;
	};
	StringNode* GetNext()
	{
		return next;
	};
private:
	char* text = NULL;
	StringNode* next = NULL;
};
class StringList
{
public:
	StringList(){};
	~StringList()
	{
		while (head != NULL)
		{
			StringNode* temp = head;
			head = temp->GetNext();
			delete temp;
		}
	};
	void Print()
	{
		printf("START PRINT\n");
		StringNode* temp = head;
		while (temp != NULL)
		{
			temp->Print();
			temp = head->GetNext();
		}
	};
	void Add(const char* text)
	{
		printf("StringList.Add\n");
		StringNode* stringNode = new StringNode(text);
		stringNode->SetNext(head);
	};
private:
	StringNode* head = NULL;
};

int main(int argc, char* argv[])
{
	StringList stringList;
	stringList.Add("test\0");
	stringList.Add("test2\0");
	stringList.Print();

	// Consume argv.
	// ----------------------------------------------------------------
	// https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf (Pg. 13, Thumbnail 31)
	// Where argv[0] is the program name.
	// Where argv[1] through argv[argc - 1] are the program parameters.
	assert(argc > 0 && "ERROR: argc should have been greater than zero.\n");
	printf("SUCCESS: argc is greater than zero.\n");
	assert(argv[0] != NULL && "ERROR: The program name is not available from the host environment.\n");
	printf("SUCCESS: The program name is available from the host environment.\n");
	printf("argv\n----\n");
	for (int i = 0; i < argc; ++i) printf("| argv[%i]: %s\n", i, argv[i]);
	printf("----\n");
	char* programName = argv[0];

	// Seperate directory and filename.
	// ----------------------------------------------------------------
	char* programDirectory = new char[strlen(programName) + 1] {};
	for (int i = 0; i < strlen(programName) + 1; ++i) programDirectory[i] = programName[i];
	assert(programDirectory[strlen(programName)] == '\0' && ERROR_NULL_TERMINATOR);
	printf("SUCCESS: Program Name copied: %s\n", programDirectory);
	char* programFilename = NULL;
	for (int i = strlen(programDirectory); i > 0; --i) if (programDirectory[i] == SEPARATOR)
	{
		programDirectory[i] = '\0';
		programFilename = &programDirectory[i] + 1;
		break;
	}
	assert(programDirectory[strlen(programDirectory)] == '\0' && ERROR_NULL_TERMINATOR);
	printf("SUCCESS: Program Directory created: %s\n", programDirectory);
	assert(programFilename != NULL && ERROR_NULL_TERMINATOR);
	assert(programFilename[strlen(programFilename)] == '\0' && ERROR_NULL_TERMINATOR);
	printf("SUCCESS: Programed Filename created: %s\n", programFilename);

	// Setup prompt and grid-sized history.
	// ----------------------------------------------------------------
	const char* startingPrompt = "Type then enter.\0";
	char* command = new char[ROW_SIZE];
	int copyI = 0;
	int copyJ = 0;
	for (copyI = 0; startingPrompt[copyI] != '\0'; ++copyI) command[copyI] = startingPrompt[copyI];
	command[copyI] = '\0';
	char* gridTexts[ROW_SIZE]{};
	for (int copyI = 0; copyI < ROW_SIZE; ++copyI)
	{
		gridTexts[copyI] = new char[ROW_SIZE];
		for (copyJ = 0; copyJ < ROW_SIZE - 1; ++copyJ) gridTexts[copyI][copyJ] = '\0';
		gridTexts[copyI][ROW_SIZE - 1] = '\0';
	}

	// Test writer.
	//Write("data.csv", "test\0");
	//processStrings("one", "two", "three");

	// Test reader.
	Read("data.csv");

	delete[] gridTexts[ROW_SIZE - 1];
	for (int i = ROW_SIZE - 2; i >= 0; --i)
	{
		gridTexts[i + 1] = gridTexts[i];
		for (copyI = 0; gridTexts[i + 1][copyI] != '\0'; ++copyI)
		{
			gridTexts[i + 1][copyI] = gridTexts[i][copyI];
		}
	}
	gridTexts[0] = new char[ROW_SIZE];
	for (copyI = 0; command[copyI] != '\0'; ++copyI)
	{
		gridTexts[0][copyI] = command[copyI];
		command[copyI] = '\0';
	}
	gridTexts[0][copyI] = '\0';
	//Write("data.csv", gridTexts[0]);
	//Read("data.csv");
	RunProcess(gridTexts[0]);


	// Setup alphabet animation.
	int alphabetIndex = 0;


	

	// Init SDL and art.
	// ----------------------------------------------------------------
	LoadSDL();
	LoadArt(programName);

	// Deallocate memory.
	delete[] programDirectory;

	while (window.IsRunning())
	{
		UpdateTimer();
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (IsWindowQuit(event)) window.Quit();
			if (event.type == KEY_PRESSED)
			{
				//printf("event: %i\n", (int)Key(event));
				SDL_Keycode key_code = Key(event);
#ifdef _WIN32
				if (key_code >= SDLK_SPACE && key_code <= SDLK_Z)
#elif __linux__
				if (key_code >= SDLK_SPACE && key_code <= SDLK_z)
#endif
				{
					char ascii_char = static_cast<char>(key_code);
					printf("%c", ascii_char);
					copyI = 0;
					while (copyI < ROW_SIZE - 2)
					{
						if (command[copyI] == '\0')
						{
							command[copyI + 1] = '\0';
							command[copyI] = ascii_char;
							break;
						}
						++copyI;
					}
				}
				else
				{
					switch (Key(event))
					{
					case SDLK_BACKSPACE:
						copyI = 0;
						while (copyI < ROW_SIZE - 1)
						{
							if (command[copyI] == '\0')
							{
								command[copyI - 1] = '\0';
								break;
							}
							++copyI;
						}
						break;
					case SDLK_RETURN:
						printf("\n----\n");
						delete[] gridTexts[ROW_SIZE - 1];
						for (int i = ROW_SIZE - 2; i >= 0; --i)
						{
							gridTexts[i + 1] = gridTexts[i];
							for (copyI = 0; gridTexts[i + 1][copyI] != '\0'; ++copyI)
							{
								gridTexts[i + 1][copyI] = gridTexts[i][copyI];
							}
						}
						gridTexts[0] = new char[ROW_SIZE];
						for (copyI = 0; command[copyI] != '\0'; ++copyI)
						{
							gridTexts[0][copyI] = command[copyI];
							command[copyI] = '\0';
						}
						gridTexts[0][copyI] = '\0';
						Write("data.csv", gridTexts[0]);
						Read("data.csv");
						RunProcess(gridTexts[0]);
						break;
					}
				}
			}
			window.HandleEvent(event);
		}
		SDL_RenderClear(window.GetRenderer());
		DrawBackground(viewports);
		DrawBackgroundForeground(viewports);
		float centerX = 0.0f;
		float centerY = 0.0f;
		SDL_GetMouseState(&centerX, &centerY);
		centerY = window.GetHeight() - centerY;
		for (int i = ROW_SIZE - 1; i >= 0 ; --i)
		{
			float normal = (float)i / (float)ROW_SIZE;
			viewports[v_tile].w = centerX / ROW_SIZE;
			viewports[v_tile].h = centerY / ROW_SIZE;
			viewports[v_tile].x = ((0 + centerX) * normal);
			viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - (centerY * normal));
			SetRow(viewports);
			viewports[v_tile].w = centerX / ROW_SIZE;
			viewports[v_tile].h = centerY / ROW_SIZE;
			viewports[v_tile].x = (window.GetWidth() - viewports[v_tile].w - (centerX * normal));
			viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - (centerY * normal));
			SetColumn(viewports);
			for (int j = 0; j < ROW_SIZE; ++j)
			{
				float xStep = (float)(viewports[v_column].x + viewports[v_column].w - viewports[v_row].x) / ROW_SIZE;
				viewports[v_tile].w = (viewports[v_column].x + viewports[v_column].w - viewports[v_row].x) / ROW_SIZE;
				viewports[v_tile].h = viewports[v_tile].w;
				viewports[v_tile].x = (viewports[v_row].x + (j * xStep));
				viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - (centerY * normal));
				if (viewports[v_tile].x < 0) viewports[v_tile].x = 0;
				if (viewports[v_tile].y < 0) viewports[v_tile].y = 0;
				if (viewports[v_tile].w < 0) viewports[v_tile].w = 0;
				if (viewports[v_tile].h < 0) viewports[v_tile].h = 0;
				if (j == alphabetIndex)
				{
					if (gridTexts[i][alphabetIndex] == '\0')
					{
						alphabetIndex = 0;
					}
					else
					{
						alphabet[TextToIndex(gridTexts[i][alphabetIndex])].Draw(&viewports[v_tile]);
						++alphabetIndex;
					}
				}
			}
		}
		DrawShellPrompt(viewports, command);
		SDL_RenderPresent(window.GetRenderer());
	}
	UnloadArt();
	UnloadSDL();
	for (int i = 0; i < ROW_SIZE; ++i) delete[] gridTexts[i];
	delete[] command;
	return 0;
}
