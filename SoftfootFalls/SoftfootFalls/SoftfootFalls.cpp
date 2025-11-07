#include "SoftfootFalls.h"

#include "Time.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

#include "Point.h"


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
	int textLimit = strlen(text);
	for (int i = 0; i < textLimit; ++i)
	{
		if (text[i] == ' ')
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
		if (text[i] == ' ')
		{
			modifiedText[i] = '\0';
		}
		else
		{
			modifiedText[i] = text[i];
		}
	}
	printf("text: %s\n", text);
	printf("mod : %s\n", modifiedText);
	char* texts[spaceCount + 1];
	texts[spaceCount] = NULL;
	texts[0] = modifiedText;
	if (spaceCount > 1)
	{
		int j = 1;
		for (int i = 0; i < textLimit; ++i)
		{
			if (modifiedText[i] == '\0')
			{
				printf(">> %s\n", &modifiedText[i + 1]);
				texts[j] = &modifiedText[i + 1];
				++j;
			}
		}
	}
	for (int i = 0; i < spaceCount + 1; ++i)
	{
		printf("texts[%i]: %s\n", i, texts[i]);
	}
	*/

	//std::string sentence = "This is a sample string to split";
	int count = 0;
	char** words = splitString(text, count);


	std::cout << "Tokens:" << std::endl;
	for (int i = 0; i < count; ++i) {
		std::cout << words[i] << std::endl;
	}

	/*char* token;
	token = strtok(text, " ");
	int tokensLimit = strlen(token);//WRONG
	if (tokensLimit == strlen(text))
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
	//execl("/bin/ls", text, NULL);
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

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
// https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa
void WindowsProcess(char* narrowText)
{
#ifdef _WIN32
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	size_t wideTextSize;                                                      // Declare a variable for wide text's size.
	mbstowcs_s(&wideTextSize, nullptr, 0, narrowText, _TRUNCATE);             // Define the wide text's size.
	wchar_t* wideText = new wchar_t[wideTextSize];                            // Declare wide text.
	mbstowcs_s(&wideTextSize, wideText, wideTextSize, narrowText, _TRUNCATE); // Define wide text.
	if (!CreateProcess(NULL, wideText, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
	{
		printf("ERROR: Failed to create process: %ul\n", GetLastError());
	}
	delete[] wideText;
	printf("SUCCESS: Created Process\n");
	printf("SUCCESS: Process ID, %i\n", processInfo.dwProcessId);
	printf("SUCCESS: Thread ID, %i\n", processInfo.dwThreadId);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
#endif
}

int main(int argc, char* argv[])
{
	if (argc < 1) return 1;
	const char* BASE_PATH = BasePath(argv[0]);

//#ifdef _WIN32
//	//const wchar_t* wideString = L"";
//	//const char* narrowText = argv[0];
//	const char* narrowText = "DIR";
//	STARTUPINFO startupInfo;
//	ZeroMemory(&startupInfo, sizeof(startupInfo));
//	startupInfo.cb = sizeof(startupInfo);
//	PROCESS_INFORMATION processInfo;
//	ZeroMemory(&processInfo, sizeof(processInfo));
//	size_t wideTextSize;                                                      // Declare a variable for wide text's size.
//	mbstowcs_s(&wideTextSize, nullptr, 0, narrowText, _TRUNCATE);             // Define the wide text's size.
//	wchar_t* wideText = new wchar_t[wideTextSize];                            // Declare wide text.
//	mbstowcs_s(&wideTextSize, wideText, wideTextSize, narrowText, _TRUNCATE); // Define wide text.
//	if (!CreateProcess(NULL, wideText, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
//	{
//		printf("ERROR: Failed to create process: %ul\n", GetLastError());
//	}
//	else
//	{
//		printf("HERHEHREHRHERERHEHR");
//		std::cin.get();
//	}
//	printf("THREREERERER");
//	std::cin.get();
//	//delete[] wideText;
//	printf("SUCCESS: Created Process\n");
//	printf("SUCCESS: Process ID, %i\n", processInfo.dwProcessId);
//	printf("SUCCESS: Thread ID, %i\n", processInfo.dwThreadId);
//	WaitForSingleObject(processInfo.hProcess, INFINITE);
//	CloseHandle(processInfo.hProcess);
//	CloseHandle(processInfo.hThread);
//#endif

	
	Load* load = new Load(BASE_PATH);
	if (!window.Init()) return 1;
	SDL_Renderer* linuxRenderer = GetLinuxRenderer(window.GetWindow());
	if (linuxRenderer) window.SetRenderer(linuxRenderer);
	LoadArt(load);

	int alphabetIndex = 0;
	const char* temp = "Prompt\0";
	char* text = new char[ROW_SIZE];
	int copyI = 0;
	int copyJ = 0;
	for (copyI = 0; temp[copyI] != '\0'; ++copyI)
	{
		text[copyI] = temp[copyI];
	}
	text[copyI] = '\0';
	char* texts[ROW_SIZE]{};
	for (int copyI = 0; copyI < ROW_SIZE; ++copyI)
	{
		texts[copyI] = new char[ROW_SIZE];
		for (copyJ = 0; copyJ < ROW_SIZE - 1; ++copyJ)
		{
			texts[copyI][copyJ] = '\0';
		}
		texts[copyI][ROW_SIZE - 1] = '\0';
	}
	while (window.IsRunning())
	{
		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] += currentTime - previousTime;
		if (deltas[animation] > limits[animation])
		{
			deltas[animation] -= limits[animation];
			if (animationIndex++ >= ANIMATION_LIMIT) animationIndex = 0;
		}
		if (deltas[minute_1] > limits[minute_1]) minuteToggle = !minuteToggle;
		for (int i = 0; i < TimerEnum_Size; ++i)
		{
			if (deltas[i] > limits[i]) deltas[i] -= limits[i];
			normals[i] = (double)deltas[i] / limits[i];
		}
		previousTime = currentTime;

		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (IsWindowQuit(event)) window.Quit();
			else if (event.type == KEY_RELEASED)
			{
			}
			else if (event.type == KEY_PRESSED)
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
						if (text[copyI] == '\0')
						{
							text[copyI + 1] = '\0';
							text[copyI] = ascii_char;
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
							if (text[copyI] == '\0')
							{
								text[copyI - 1] = '\0';
								break;
							}
							++copyI;
						}
						break;
					case SDLK_RETURN:
						delete[] texts[ROW_SIZE - 1];
						for (int i = ROW_SIZE - 2; i >= 0; --i)
						{
							texts[i + 1] = texts[i];
							for (copyI = 0; texts[i + 1][copyI] != '\0'; ++copyI)
							{
								texts[i + 1][copyI] = texts[i][copyI];
							}
						}
						texts[0] = new char[ROW_SIZE];
						for (copyI = 0; text[copyI] != '\0'; ++copyI)
						{
							texts[0][copyI] = text[copyI];
							text[copyI] = '\0';
						}
						texts[0][copyI] = '\0';
//#ifdef _WIN32
//							WindowsProcess(texts[0]);
//#elif __linux__
//							LinuxProcess(texts[0]);
//#endif
						break;
					}
				}
			}
			window.HandleEvent(event);
		}
		if (window.IsMinimized()) continue;
		SDL_RenderClear(window.GetRenderer());
		DrawBackground(viewports);
		DrawForeground(viewports);
		float normal = normals[second_1];
		float mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		float centerX = mouseX;
		float centerY = window.GetHeight() - mouseY;
		for (int i = ROW_SIZE - 1; i >= 0 ; --i)
		{
			normal = (float)i / (float)ROW_SIZE;
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
				viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
				if (viewports[v_tile].x < 0) viewports[v_tile].x = 0;
				if (viewports[v_tile].y < 0) viewports[v_tile].y = 0;
				if (viewports[v_tile].w < 0) viewports[v_tile].w = 0;
				if (viewports[v_tile].h < 0) viewports[v_tile].h = 0;
				if (j == alphabetIndex)
				{
					if (texts[i][alphabetIndex] == '\0')
					{
						alphabetIndex = 0;
					}
					else
					{
						alphabet[TextToIndex(texts[i][alphabetIndex])].Draw(&viewports[v_tile]);
						++alphabetIndex;
					}
				}
			}
		}
		DrawShellPrompt(viewports, text);
		SDL_RenderPresent(window.GetRenderer());
	}
	UnloadArt();
	window.Free();
	SDL_Quit();
	for (int i = 0; i < ROW_SIZE; ++i) delete[] texts[i];
	delete[] text;
	delete load;
	delete[] BASE_PATH;
	return 0;
}
