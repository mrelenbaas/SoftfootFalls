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
	do
	{
		delete[] ((PathNode*)(paths->data))->path;
		delete paths->data;
		paths = paths->next;
	} while (paths != NULL);
}

const char* Load::Path(const char* filename)
{
	int size = strlen(basePath) + strlen(relativePath) + strlen(filename) + 1;
	paths = new Node
	{
		new PathNode
		{
			new char[size + 1]
		},
		paths->next
	};
	for (int i = 0; i < size; ++i)
	{
		if (i < (int)strlen(basePath))
		{
			((PathNode*)(paths->data))->path[i] = basePath[i];
		}
		else if (i < (int)strlen(basePath) + (int)strlen(relativePath))
		{
			((PathNode*)(paths->data))->path[i] = relativePath[i - strlen(basePath)];
		}
		else if (i < (int)strlen(basePath) + (int)strlen(relativePath) + (int)strlen(filename))
		{
			((PathNode*)(paths->data))->path[i] = filename[i - strlen(basePath) - strlen(relativePath)];
		}
	}
	((PathNode*)(paths->data))->path[size - 1] = '\0';
	
	return ((PathNode*)(paths->data))->path;
}