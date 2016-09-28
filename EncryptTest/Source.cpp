#include<stdio.h>

#include<string.h>

#pragma warning(disable:4996)


void swap(unsigned char *a, unsigned char *b)
{
	unsigned char tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}


void re_S(unsigned char *S)
{
	unsigned int i;
	for (i = 0; i < 256; i++)
	{
		S[i] = (unsigned char)i;
	}
}


void re_T(char *T, char *key)
{
	int i;
	int keyLen;
	keyLen = strlen(key);
	for (i = 0; i < 256; i++)
	{
		T[i] = key[i%keyLen];
	}
}

void re_Sbox(unsigned char *S, char *T)
{
	int i, j = 0;
	for (i = 0; i < 256; i++)
	{
		j = (j + S[i] + T[i]) % 256;
		swap(&S[i], &S[j]);
	}
}


void re_RC4(unsigned char *S, char *key)
{
	char T[256] = { 0 };
	re_S(S);
	re_T(T, key);
	re_Sbox(S, T);
}

void RC4(char *inBuf, char *outBuf, int offset, int bufLen, char *key)
{
	unsigned char S[256] = { 0 };
	unsigned char readbuf[1];

	int i, j, t;
	int z;
	re_RC4(S, key);

	i = j = 0;
	z = 0;
	while (z < offset)
	{
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		swap(&S[i], &S[j]);
		z++;
	}
	z = 0;
	while (z < bufLen)
	{
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		swap(&S[i], &S[j]);
		t = (S[i] + (S[j] % 256)) % 256;
		readbuf[0] = inBuf[z];
		readbuf[0] = readbuf[0] ^ S[t];
		outBuf[z] = readbuf[0];
		z++;
	}
}


int main()
{
	char buffer[1024] = { 0 };
	int readLen;
	FILE *pf = fopen("data.txt", "r");
	if (pf == NULL)
	{
		printf("file open error\n");
		return -1;
	}

	int offset = 0;
	while (feof(pf) == 0)
	{
		readLen = fread(buffer + offset, 1, 128, pf);
		offset += 128;
		printf("%d\n", readLen);
	}
	fclose(pf);
	

	//test the RC4 encrypt algorithm
	char *key = "123";
	int len = strlen(buffer);
	readLen = 128;
	offset = 0;

	pf = fopen("encrypt.txt", "w");
	if (pf == NULL)
	{
		printf("file open error");
		return -1;
	}
	

	while (offset < len)
	{
		readLen = (readLen < (len - offset) ? readLen : (len - offset));
		RC4(buffer + offset, buffer + offset, offset, readLen, key);
		offset += readLen;
	}

	fwrite(buffer, 1, len, pf);
	fclose(pf);

	//decrypt the file
	pf = fopen("decrypt.txt", "w");
	if (pf == NULL)
	{
		printf("file open error\n");
		return -1;
	}
	offset = 0;

	while (offset < len)
	{
		readLen = (readLen < (len - offset) ? readLen : (len - offset));
		RC4(buffer + offset, buffer + offset, offset, readLen, key);
		offset += readLen;
	}
	printf("%s\n", buffer);
	fwrite(buffer, 1, len, pf);
	fclose(pf);
	

	return 0;
}