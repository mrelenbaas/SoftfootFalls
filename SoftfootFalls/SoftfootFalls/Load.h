#ifdef _WIN32
#include <SDL3/SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif


struct Node
{
	void* data = NULL;
	Node* next = NULL;
};

struct PathNode
{
	char* path = NULL;
};

class Load
{
public:
	Load(const char*);
	~Load();
	bool Print(SDL_Surface*, const char*);
	const char* Path(const char*);
private:
	char* basePath;
#ifdef _WIN32
	const char* relativePath = "art\\";
#elif __linux__
	const char* relativePath = "SoftfootFalls/x64/debug/art/";
#endif
	Node* paths;
};
