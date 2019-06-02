#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "PeMutator.h"

#define SIZE_FILE 50*1024*1024

static uint8_t *buffer = NULL;
static uint8_t tmp = NULL;
static uint32_t size_file = 0;

void usage()
{
	std::cout << "Usage: simple_mutate_pe.exe <path_to_image>\r\n"
		<< "       path_to_image - path to the target PE image file\r\n";
}

bool read_bin_file_to_mem (char *path) {
	
	//Открытие файла
	FILE *hFileOpen;
	hFileOpen = fopen(path, "rb");
	if (hFileOpen == NULL)
	{
		printf("Error open file %s \n", path);
		return false;
	}

	//Считывание файла
	buffer = (uint8_t*) malloc(SIZE_FILE);
	if (buffer == NULL)
	{
		printf("No free memory for open file %s \n", path);
		fclose(hFileOpen);
		return false;
	}

	while (!feof(hFileOpen))
	{
		tmp = getc(hFileOpen);
		if (size_file < SIZE_FILE) buffer[size_file] = tmp;
		else
		{
			printf("Error file size = %d Size file must be < %d \n", size_file, SIZE_FILE);
			fclose(hFileOpen);
			return false;
		}
		size_file++;
	}

	fclose(hFileOpen);

	return true;
}

bool write_bin_file_from_mem(char *path) {

	if (buffer == NULL)
	{
		printf("Dont read file to mutate %s \n", path);
		return false;
	}

	//Открытие файла
	FILE *hFileHex;
	hFileHex = fopen(path, "wb+");

	if (hFileHex == NULL)
	{
		printf("Error open file %s \n", path);
		return false;
	}

	//Запись файла
	for (int j = 0; j < (size_file - 1); j++)
	{
		fwrite((uint8_t*)(buffer+j), sizeof(uint8_t), 1, hFileHex);
	};

	fclose(hFileHex);
	return true;
}

int main(int argc, char* argv[])
{
	char result_str[] = "Mutated_";

	if (argc < 2) {
		usage();
		return (-1);
	}

	//Считывание файла в память
	bool result = read_bin_file_to_mem(argv[1]);

	if (result == true) {
		//Мутирование
		result = Mutate(buffer, size_file, true);

		//Запись файла на диск
		if (result == true) {
			strcat(result_str, argv[1]);
			result = write_bin_file_from_mem(result_str);
		}
	}

	if (result == true) {
		printf("\n\n! File is %s success genering ! \n", result_str);
	}
	
	Sleep(3000);
	return 0;
}
