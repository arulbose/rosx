/* Rose RT-Kernel
 * Copyright (C) 2016 Arul Bose<bose.arul@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __BYTEPOOL_H__
#define __BYTEPOOL_H__

/* Internal funtions */
unsigned char *__bytepool_start; /* Address must be 32 bit aligned */
void bytepool_init();

/* -------------- Application system calls ---------------- */
void *salloc(int size); /* Simple time sensitive allocation from the bytepool; size is number of bytes */
void sfree(void *);

#endif

