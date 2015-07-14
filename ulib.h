#ifndef __ulib_H__
	#define __ulib_H__
	/*
	 * ulib.h
	 * This file is part of dp2pcsc
	 *
	 * Copyright (C) 2015 - Muromi Uhikari <chendianbuji@gmail.com>
	 *
	 * dp2pcsc is free software; you can redistribute it and/or
	 * modify it under the terms of the GNU Lesser General Public
	 * License as published by the Free Software Foundation; either
	 * version 2.1 of the License, or (at your option) any later version.
	 *
	 * dp2pcsc is distributed in the hope that it will be useful,
	 * but WITHOUT ANY WARRANTY; without even the implied warranty of
	 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	 * Lesser General Public License for more details.
	 *
	 * You should have received a copy of the GNU Lesser General Public License
	 * along with dp2pcsc. If not, see <http://www.gnu.org/licenses/>.
	 */
	
	#include <stdlib.h>
	#include <string.h>

	
	#define malloc_safe(pointer, size) \
		malloc(size);\
		memset(pointer, 0, size)
	
	#define malloc_string_safe(pointer, size)\
		malloc(size + 1 * sizeof(char));\
		memset(pointer, 0, size + 1 * sizeof(char))
	
	#define free_safe(pointer) \
		if (pointer != NULL) free(pointer);\
		pointer = NULL;
	
	#ifndef TRUE
	#define TRUE 0
	#endif
	
	#ifndef	FALSE
	#define FALSE 1
	#endif
	
	#ifndef ERROR
	#define ERROR -1
	#endif
//	#define ONINIT 0
//	#define ONRUN 1
//	#define ONDESTROY 2
	
	int wrap(const char *from, const char tail,char *to);
	int un_wrap(char *str, char *tail);
	int compare_wrap(const char *str,char tail);
#endif /* __ulib_H__ */ 
