#include "ulib.h"

int wrap(const char *from,const char head,const char tail,char *to)
{
	if(from == NULL || head == '\0' || tail == '\0')
		return FALSE;
	int from_str_length = strlen(from) * sizeof(char);
	//sprintf(to, "%c%s%c", head, from, tail);
	memset(to, head, 1);
	strncpy(to + sizeof(head), from, from_str_length);
	memset(to + from_str_length + sizeof(tail), tail, 1);
	return TRUE;
}

int unwrap(char *str,char *head,char *tail)
{
	if (str == NULL || head == NULL || tail == NULL) 
		return FALSE;
	int tail_position = (strlen(str) - 1) * sizeof(char);
	*head = *str;
	*tail = *(str + tail_position);
	memset(str + tail_position, 0, 1);
	memmove(str, str + 1,tail_position);
	return TRUE;
}
