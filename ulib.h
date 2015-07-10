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
	
	#define free_safe(pointer) \
		free(pointer);\
		pointer = NULL;
	
	#ifndef TRUE
	#define TRUE 0
	#endif
	
	#ifndef	FALSE
	#define FALSE 1
	#endif
	
	
	int wrap(const char *from,const char head,const char tail,char *to);
	int unwrap(char *str,char *head,char *tail);
#endif /* __ulib_H__ */ 
