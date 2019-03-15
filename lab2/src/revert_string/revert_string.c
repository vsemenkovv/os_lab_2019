#include "revert_string.h"
#include  <string.h>
#include ‘<stdlib.h>’

void RevertString(char *str)
{

	int N = strlen(str) - 1;
	char *buf =  malloc(sizeof(char) * (strlen(str)+1));
	strcpy(buf, str);
	for (int i = 0; i < N+1; i++) {
		strncpy((buf + i), (str + N - i), 1);
	}
	strcpy(str,buf);
}

