#include <cstddef>


struct Node
{
	void* data = NULL;
	Node* next = NULL;
};

struct PathNode
{
	char* path = NULL;
};