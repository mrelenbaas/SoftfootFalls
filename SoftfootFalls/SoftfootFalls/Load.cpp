#include <string.h>
#include <stdio.h>

#include "Load.h"


Load::Load(const char* basePath)
{
	paths = new Node;
	paths->data = new PathNode;
	int size = strlen(basePath);
	(*this).basePath = new char[size + 1];
	for (int i = 0; i < size; ++i)
	{
		(*this).basePath[i] = basePath[i];
	}
	(*this).basePath[size] = '\0';
}

Load::~Load()
{
	delete[] basePath;
	Node* temp;
	do
	{
		if ((PathNode*)(paths->data) != NULL
			&& ((PathNode*)(paths->data))->path != NULL)
		{
			delete[]((PathNode*)(paths->data))->path;
			((PathNode*)(paths->data))->path = NULL;
		}
		delete ((PathNode*)(paths->data));
		paths->data = NULL;
		temp = paths;
		paths = paths->next;
		delete temp;
		temp = NULL;
	} while (paths != NULL);
	delete paths;
	paths = NULL;
}

bool Load::Print(SDL_Surface* surface, const char* filename)
{
	bool result = false;

	if (surface == NULL)
		SDL_Log("Error: Load failed: %s, %s\n", filename, SDL_GetError());
	else
	{
		SDL_Log("Loaded: %s\n", filename);
		result = true;
	}

	return result;
}

const char* Load::Path(const char* filename)
{
	int size = strlen(basePath) + strlen(relativePath) + strlen(filename) + 1;
	Node* temp = new Node
	{
		new PathNode
		{
			new char[size + 1]
		},
		paths
	};
	for (int i = 0; i < size; ++i)
	{
		if (i < (int)strlen(basePath))
		{
			((PathNode*)(temp->data))->path[i] = basePath[i];
		}
		else if (i < (int)strlen(basePath) + (int)strlen(relativePath))
		{
			((PathNode*)(temp->data))->path[i] = relativePath[i - strlen(basePath)];
		}
		else if (i < (int)strlen(basePath) + (int)strlen(relativePath) + (int)strlen(filename))
		{
			((PathNode*)(temp->data))->path[i] = filename[i - strlen(basePath) - strlen(relativePath)];
		}
	}
	((PathNode*)(temp->data))->path[size - 1] = '\0';
	paths = temp;
	
	return ((PathNode*)(paths->data))->path;
}
