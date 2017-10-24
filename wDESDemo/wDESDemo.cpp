// wDESDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>

#define MAX_LENGTH		255
#define BYTE_TO_BIT		8

#define BLOCK_LENGTH	8
#define KEY_LENGTH		8
#define XOR_KEY_LENGTH	6
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

int gKeyTransorm[56] =
{
	57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 
	10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 
	63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};

int gCompressTransform[48] = 
{
	14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32,
};

int gKeyCircleShift[ROUND_NUM] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };


// get bits array(0,1) from a char array
int *getBits(char *data, int dataLen)
{
	if (data == NULL)
	{
		return NULL;
	}

	//int dataLen = strnlen_s(data, MAX_LENGTH);
	int bitsLen = dataLen * BYTE_TO_BIT;
	int *bits = new int[bitsLen];

	for (int i = 0; i < dataLen; i++)
	{
		for (int j = 0; j < BYTE_TO_BIT; j++)
		{
			int temp = data[i];
			bits[i*BYTE_TO_BIT + j] = (temp >> (BYTE_TO_BIT - j - 1)) & 1;
		}
	}

	return bits;
}

// get char array from bits
// len --> the length of bits
char *fromBits(int * bits, int len)
{
	int cLen = len / BYTE_TO_BIT; // should estimate whether the length of bits can be divisible by BYTE_TO_BIT or not
	char *data = new char[cLen];

	for (int i = 0; i < cLen; i++)
	{
		data[i] = 0;

		for (int j = 0; j < BYTE_TO_BIT; j++)
		{
			data[i] |= bits[i*BYTE_TO_BIT + j] << (BYTE_TO_BIT - j - 1);
		}
	}

	return data;
}

char ** getRoundKeys(char *key, int roundNum)
{
	char **roundKeys = new char*[roundNum];
	//for (int i = 0; i < roundNum; i++)
	//{
	//	roundKeys[i] = new char[XOR_KEY_LENGTH];
	//}

	int *blockBits = getBits(key, KEY_LENGTH);
	int *transformedBits = new int[56];
	for (int i = 0; i < 56; i++)
	{
		//int temp = gKeyTransorm[i] - 1;
		//transformedBits[temp - temp/8] = blockBits[i];
		transformedBits[i] = blockBits[gKeyTransorm[i] - 1];
	}
	delete[] blockBits;

	for (int i = 0; i < roundNum; i++)
	{
		int *shiftedBits = new int[56];
		int shift = gKeyCircleShift[i];
		int *temp = new int[shift];
		int *keyBits = new int[48];
		
		// left
		for (int j = 0; j < shift; j++)
		{
			temp[j] = transformedBits[j];
		}

		for (int j = 0; j < 28 - shift; j++)
		{
			shiftedBits[j] = transformedBits[j + shift];
		}

		for (int j = 0; j < shift; j++)
		{
			shiftedBits[j + 28 - shift] = temp[j];
		}

		// right
		for (int j = 0; j < shift; j++)
		{
			temp[j] = transformedBits[j + 28];
		}

		for (int j = 0; j < 28 - shift; j++)
		{
			shiftedBits[j + 28] = transformedBits[j + 28 + shift];
		}

		for (int j = 0; j < shift; j++)
		{
			shiftedBits[j + 56 - shift] = temp[j];
		}

		// apply the compress transform and get the round key
		for (int j = 0; j < 48; j++)
		{
			keyBits[j] = shiftedBits[gCompressTransform[j] - 1];
		}

		roundKeys[i] = fromBits(keyBits, 48);

		delete[] keyBits;
		delete[] temp;
		delete[] transformedBits;
		transformedBits = shiftedBits;
	}

	return roundKeys;
}

int * IPTransform(char *data, int dLen)
{
	int *dataBits = getBits(data, dLen);
	int *transformedBits = new int[dLen*BYTE_TO_BIT];

	for (int i = 0; i < dLen*BYTE_TO_BIT; i++)
	{
		transformedBits[i] = dataBits[gIPTransform[i] - 1];
	}

	return transformedBits;
}

char * encryptDes(char *src, int srcLen, char *key, int keyLen)
{
	//int srcLen = strnlen_s(src, MAX_LENGTH);
	//int keyLen = strnlen_s(key, MAX_LENGTH);
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
		char *leftPart;
		char *rightPart;

		for (int j = 0; j < BLOCK_LENGTH; j++)
		{
			blockData[i] = src[i*BLOCK_LENGTH + j];
		}

		int *blockBits = IPTransform(blockData, BLOCK_LENGTH);

		for (int j = 0; j < ROUND_NUM; j++)
		{
			char *roundkey = roundKeys[j];



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

	char *code = encryptDes(data, strlen(data), key, strlen(key));

	return 0;
}

