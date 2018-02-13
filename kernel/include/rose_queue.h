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

#ifndef __QUEUE_H__
#define __QUEUE_H__

#define Q_NAME_SIZE 8

/* Queue status at runtime */
#define QUEUE_FULL (1 << 0)
#define QUEUE_EMPTY ~QUEUE_FULL

/* Queue properties */
#define Q_CYCLIC_FULL (1 << 1)
#define Q_STOP_FULL ~(Q_CYCLIC_FULL)

#define Q_BLOCK (1 << 3) 
#define Q_NO_BLOCK ~(Q_BLOCK)

#define Q_PRIO (1 << 2)
#define Q_PRIO_NONE ~(Q_PRIO)

/* Supports muliple producers and single consumer */
struct queue {
        char name[Q_NAME_SIZE];
        char *start;
        char *head;
        char *tail;
        unsigned int flag;
        int size_of_block; /* in bytes */
        int num_of_blocks;
	TCB *task;
};

/* -------------- Application system calls ---------------- */
#if(CONFIG_QUEUE_COUNT > 0)
struct queue *create_queue(char *name, int size_of_block, int num_of_blocks, unsigned int flag); /* flag 0: Q_STOP_FULL, Q_PRIO_NONE*/
void delete_queue(struct queue *p);
#endif
int read_from_queue(struct queue *p, char *n, int size, int timeout);
int write_to_queue(struct queue *p, char *n, int size);

#endif
