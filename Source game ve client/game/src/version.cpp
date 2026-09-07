#include <stdio.h>

void WriteVersion()
{
#ifndef __WIN32__
	FILE* fp = fopen("VERSION.txt", "w");

	if (fp)
	{
		fprintf(fp, "__GAME_VERSION__: %s\n", __GAME_VERSION__);
		fprintf(fp, "%s@%s:%s\n", "Y2Fwb25l", __HOSTNAME__, __PWD__);
		fclose(fp);
	}
#endif
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
