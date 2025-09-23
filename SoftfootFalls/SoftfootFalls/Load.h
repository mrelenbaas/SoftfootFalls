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
	const char* Path(const char*);
private:
	char* basePath;
#ifdef _WIN32
	const char* relativePath = "art\\";
#elif __linux__
	const char* relativePath = "/art/";
#endif
	Node* paths;
};