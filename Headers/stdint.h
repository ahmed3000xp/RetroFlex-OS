// Copyright (C) 2024 Ahmed
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MY_STDINT_H
#define MY_STDINT_H

#include <stdint.h>

#ifndef __int8_t_defined
typedef signed char int8_t;
typedef unsigned char uint8_t;
#define __int8_t_defined
#endif

#ifndef __int16_t_defined
typedef signed short int int16_t;
typedef unsigned short int uint16_t;
#define __int16_t_defined
#endif

#ifndef __int32_t_defined
typedef signed int int32_t;
typedef unsigned int uint32_t;
#define __int32_t_defined
#endif

#ifndef __int64_t_defined
typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;
#define __int64_t_defined
#endif

typedef uint8_t bool;

#define true 1
#define false 0

#define NULL ((void*)0)

#endif