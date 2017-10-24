// wDESDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>

#define MAX_LENGTH		255
#define BYTE_TO_BIT		8

#define BLOCK_LENGTH	8
#define KEY_LENGTH		8
#define ROUND_NUM		16


int gIPTransform[64] =
{
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7 
};

char ** getRoundKeys(char *key, int roundNum)
{

}

void IPTransform(char *data)
{

}

int *getBits(char *data)
{
	if (data == NULL)
	{
		return NULL;
	}

	int dataLen = strnlen_s(data, MAX_LENGTH);
	int bitsLen = dataLen * BYTE_TO_BIT;
	int *bits = new int[bitsLen];

	for (int i = 0; i < dataLen; i++)
	{
		for (int j = 0; j < BYTE_TO_BIT; j++)
		{
			int temp = data[i];
			bits[i*BYTE_TO_BIT + j] = (temp >> (BYTE_TO_BIT - j + 1)) & 1;
		}
	}

	return bits;
}

char * encryptDes(char *src, char *key)
{
	int srcLen = strnlen_s(src, MAX_LENGTH);
	int keyLen = strnlen_s(key, MAX_LENGTH);
	int encryptedDataLen;
	int encryptBlockNum;

	// The length of the key should be 64 bits, 8 bytes.
	if (keyLen != KEY_LENGTH)
	{
		return NULL;
	}
	
	// 
	if (srcLen % BLOCK_LENGTH != 0)
	{
		encryptedDataLen = (srcLen / BLOCK_LENGTH + 1)*BLOCK_LENGTH;
	}
	else
	{
		encryptedDataLen = srcLen;
	}
	char * encryptedData = new char[encryptedDataLen];

	char **roundKeys = getRoundKeys(key,ROUND_NUM);
	if (roundKeys == NULL)
	{
		return NULL;
	}

	// Compute the number of block
	encryptBlockNum = srcLen / BLOCK_LENGTH;

	for (int i = 0; i < encryptBlockNum; i++)
	{
		char *blockData = new char[BLOCK_LENGTH];
		IPTransform(blockData);
		char *leftPart;
		char *rightPart;

		for (int j = 0; j < ROUND_NUM; j++)
		{
			char *roundkey = roundKeys[j];

			int *blockBits = getBits(blockData);



		}

		delete[] blockData;
	}

	for (int i = 0; i < ROUND_NUM; i++)
	{
		if (roundKeys != NULL && roundKeys[i] != NULL)
		{
			delete[] roundKeys[i];
		}
	}

	if (roundKeys != NULL)
	{
		delete[] roundKeys;
	}

	return encryptedData;
}


int _tmain(int argc, _TCHAR* argv[])
{
	char *data = "123456789101112";
	char *key = "87654321";

	char *code = encryptDes(data, key);

	return 0;
}

