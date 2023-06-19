#include "nix.hpp"
void* PlatformAllocate(int size)
{
	void* result = malloc(size);

	return result;
}

void PlatformFree(void* chunk)
{
	free(chunk);
	chunk = NULL;
}

void ClearMemory(void* ptr, int size)
{
	memset(ptr, 0, size);
}

//PLATFORM_READ_FILE_DATA(platform_read_file_data)
struct file ReadFile(char* filename)
{
	struct file result = {};
	FILE *fileIO = NULL;
	int filesize = 0;

	fileIO = fopen(filename, "rb");

	if (fileIO)
	{
		fseek(fileIO,	0,SEEK_END);
		filesize = ftell(fileIO);

		result.size = filesize;
		
		rewind(fileIO);
		
		result.data = (char*) malloc(filesize+1);
		memset(result.data,0,filesize);
		fread(result.data,1, filesize-1,fileIO);

	}
	fclose(fileIO);

	return result;
}
bool WriteFile(char* data, int dataSize, char* output)
{
	bool result = false;
	FILE* file;

	file = fopen(output, "wb");
	if (file)
	{
		if (fwrite(data,1,dataSize,file ))
		{
			result = true;
		}
	}
	fclose(file);

	return result;
}
