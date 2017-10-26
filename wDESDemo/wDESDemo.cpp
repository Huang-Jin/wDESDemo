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

#define ENCODE 0,16,1       
#define DECODE 15,-1,-1     

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

int gExtentedTransform[48] =
{
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

int gSBox[8][64]=
{
	{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8, 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0, 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },
	{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5, 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15, 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1, 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7, 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },
	{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15, 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 19, 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4, 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },
	{ 2, 12, 4, 1, 7, 10, 11, 6, 5, 8, 3, 15, 13, 0, 14, 9, 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 13, 3, 9, 8, 6, 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14, 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },
	{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11, 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8, 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6, 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },
	{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1, 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6, 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2, 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },
	{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7, 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2, 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8, 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
};

int gPBox[32] = 
{
	16, 7, 20, 21, 29, 12, 28, 17, 
	1, 15, 23, 26, 5, 18, 31, 10, 
	2, 8, 24, 14, 32, 27, 3, 9, 
	19, 13, 30, 6, 22, 11, 4, 25
};

int gIPTranspose[64] =
{
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30, 
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

// get bits array(0,1) from a char array
char *getBits(char *data, int dataLen)
{
	if (data == NULL)
	{
		return NULL;
	}

	//int dataLen = strnlen_s(data, MAX_LENGTH);
	int bitsLen = dataLen * BYTE_TO_BIT;
	char *bits = new char[bitsLen];

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
char *fromBits(char * bits, int len)
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

// copy bits from src to a new array with length len.
char *copyBits(char *src, int len)
{
	char* cpBits = new char[len];

	try
	{
		for (int i = 0; i < len; i++)
		{
			cpBits[i] = src[i];
		}
	}
	catch (int)
	{
		delete[] cpBits;
		return NULL;
	}

	return cpBits;
}

// merge left and right into a whole part
char *mergeBits(char *left, int leftLen, char *right, int rightLen)
{
	char *merged = new char[leftLen + rightLen];

	for (int i = 0; i < leftLen; i++)
	{
		merged[i] = left[i];
	}
	for (int i = 0; i < rightLen; i++)
	{
		merged[leftLen + i] = left[i];
	}

	return merged;
}

// xor in bits
char *xorBits(char *bits1, char *bits2, int len)
{
	char *xoredBits = new char[len];

	for (int i = 0; i < len; i++)
	{
		xoredBits[i] = bits1[i] ^ bits2[i];
	}

	return xoredBits;
}

char ** getRoundKeys(char *key, int roundNum)
{
	char **roundKeys = new char*[roundNum];
	//for (int i = 0; i < roundNum; i++)
	//{
	//	roundKeys[i] = new char[XOR_KEY_LENGTH];
	//}

	char *blockBits = getBits(key, KEY_LENGTH);
	char *transformedBits = new char[56];
	for (int i = 0; i < 56; i++)
	{
		//int temp = gKeyTransorm[i] - 1;
		//transformedBits[temp - temp/8] = blockBits[i];
		transformedBits[i] = blockBits[gKeyTransorm[i] - 1];
	}
	delete[] blockBits;

	for (int i = 0; i < roundNum; i++)
	{
		char *shiftedBits = new char[56];
		int shift = gKeyCircleShift[i];
		char *temp = new char[shift];
		char *keyBits = new char[48];
		
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

		roundKeys[i] = keyBits;

		//delete[] keyBits;
		delete[] temp;
		delete[] transformedBits;
		transformedBits = shiftedBits;
	}

	return roundKeys;
}

char * IPTransform(char *data, int dLen)
{
	char *dataBits = getBits(data, dLen);
	char *transformedBits = new char[dLen*BYTE_TO_BIT];

	for (int i = 0; i < dLen*BYTE_TO_BIT; i++)
	{
		transformedBits[i] = dataBits[gIPTransform[i] - 1];
	}

	return transformedBits;
}

char * ExtentedTransform(char *bits, int transformLen)
{
	char *transformedBits = new char[transformLen];

	for (int i = 0; i < transformLen; i++)
	{
		transformedBits[i] = bits[gExtentedTransform[i] - 1];
	}

	return transformedBits;
}

char * SBoxTransform(char *bits)
{
	char *transfomedBits = new char[32];

	// eight boxes
	for (int i = 0; i < 8; i++)
	{
		int row = (bits[i * 6] << 1) & bits[i * 6 + 5];
		int rank = (bits[i * 6 + 1] << 3) | (bits[i * 6 + 2] << 2) | (bits[i * 6 + 3] << 1) | bits[i * 6 + 4];

		int temp = gSBox[i][row * 16 + rank];

		for (int j = 0; j < 4; j++)
		{
			transfomedBits[i * 4 + j] = (temp >> (4 - j - 1)) & 1;
		}
	}

	return transfomedBits;
}

char * PBoxTransform(char *bits)
{
	char *transfomedBits = new char[32];

	for (int i = 0; i < 32; i++)
	{
		transfomedBits[i] = bits[gPBox[i] - 1];
	}

	return transfomedBits;
}

char * IPTranspose(char *bits)
{
	char *transfomedBits = new char[32];

	for (int i = 0; i < 32; i++)
	{
		transfomedBits[i] = bits[gPBox[i] - 1];
	}

	return transfomedBits;
}

// encrypt 0,16,1
// decrypt 15,-1,-1
char * Des(char *src, int srcLen, char *key, int keyLen, int start, int end, int step)
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
	
	char *data = NULL;
	// 
	if (srcLen % BLOCK_LENGTH != 0)
	{
		encryptedDataLen = (srcLen / BLOCK_LENGTH + 1)*BLOCK_LENGTH;
		data = new char[encryptedDataLen];
		for (int i = 0; i < srcLen; i++)
		{
			data[i] = src[i];
		}

		for (int i = 0; i < encryptedDataLen - srcLen; i++)
		{
			data[srcLen + i] = 0;
		}
	}
	else
	{
		encryptedDataLen = srcLen;
		data = new char[encryptedDataLen];
		for (int i = 0; i < encryptedDataLen; i++)
		{
			data[i] = src[i];
		}
	}
	char * encryptedData = new char[encryptedDataLen];

	char **roundKeys = getRoundKeys(key, ROUND_NUM);
	if (roundKeys == NULL)
	{
		return NULL;
	}

	// Compute the number of block
	encryptBlockNum = encryptedDataLen / BLOCK_LENGTH;

	for (int i = 0; i < encryptBlockNum; i++)
	{
		char *blockData = new char[BLOCK_LENGTH];
		
		char *leftPart;
		char *rightPart;

		
		for (int j = 0; j < BLOCK_LENGTH; j++)
		{
			blockData[j] = src[i*BLOCK_LENGTH + j];
		}

		char *blockBits = IPTransform(blockData, BLOCK_LENGTH);

		for (int j = start; j != end; j += step)
		{
			char *roundkey = roundKeys[j];
			char *extendTransformed = new char[48];

			leftPart = copyBits(blockBits, 32); // with length of 32
			rightPart = copyBits(blockBits + 32, 32);
			extendTransformed = ExtentedTransform(rightPart, 48);

			char *xoredBits = xorBits(roundkey, extendTransformed, 48);
			char *sBoxTransformed = SBoxTransform(xoredBits);
			char *pBoxTransformed = PBoxTransform(sBoxTransformed);
			char *finalxoredBits = xorBits(pBoxTransformed, leftPart, 32);
			
			delete[] blockBits;
			blockBits = mergeBits(rightPart, 32, finalxoredBits, 32);
			delete[] extendTransformed;
			delete[] finalxoredBits;
		}

		char *blockResult = IPTranspose(blockBits);
		char *blockBytes = fromBits(blockResult, 64);

		for (int j = 0; j < BLOCK_LENGTH; j++)
		{
			encryptedData[i*BLOCK_LENGTH + j] = blockBytes[j];
		}

		delete[] blockBytes;
		delete[] blockBits;
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
	char *data = "This is a test.";
	char *key = "12345678";

	bool encrypt = 0;
	char *result = NULL;
	FILE *file = NULL;
	
	if (encrypt)
	{
		result = Des(data, strlen(data), key, strlen(key), ENCODE);
		for (int i = 0; i < 16; i++)
		{
			printf("%02x", 0xff & result[i]);
		}
		fopen_s(&file, "code.txt", "wb");
		fwrite(result, sizeof(char), 16, file);
	}
	else
	{
		char *code = new char[17];
		fopen_s(&file, "code.txt", "rb");
		fread_s(code, 16, 1, 16, file);
		code[16] = 0;
		result = Des(code, strlen(code), key, strlen(key), DECODE);
		printf(result);
	}

	delete[] result;
	system("pause");
	return 0;
}

