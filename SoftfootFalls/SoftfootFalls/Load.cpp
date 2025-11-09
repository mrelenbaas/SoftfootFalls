#include <string.h>
#include <stdio.h>

#include "Load.h"


Load::Load(const char* basePath)
{
	PATHS = new Node;
	PATHS->data = new PathNode;
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
		if ((PathNode*)(PATHS->data) != NULL
			&& ((PathNode*)(PATHS->data))->path != NULL)
		{
			delete[]((PathNode*)(PATHS->data))->path;
			((PathNode*)(PATHS->data))->path = NULL;
		}
		delete ((PathNode*)(PATHS->data));
		PATHS->data = NULL;
		temp = PATHS;
		PATHS = PATHS->next;
		delete temp;
		temp = NULL;
	} while (PATHS != NULL);
	delete PATHS;
	PATHS = NULL;
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
		PATHS
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
	PATHS = temp;
	
	return ((PathNode*)(PATHS->data))->path;
}
