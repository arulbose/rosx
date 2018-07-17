/* RosX RT-Kernel
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

#ifndef __RX_QUEUE_H__
#define __RX_QUEUE_H__

#define RX_Q_NAME_SIZE 8

/* Queue status at runtime */
#define RX_QUEUE_FULL (1 << 0)
#define RX_QUEUE_EMPTY ~RX_QUEUE_FULL

/* Queue properties */
#define RX_Q_CYCLIC_FULL (1 << 1)
#define RX_Q_STOP_FULL ~(RX_Q_CYCLIC_FULL)

#define RX_Q_BLOCK (1 << 3) 
#define RX_Q_NO_BLOCK ~(RX_Q_BLOCK)

#define RX_Q_PRIO (1 << 2)
#define RX_Q_PRIO_NONE ~(RX_Q_PRIO)

/* Supports muliple producers and single consumer */
struct msg_queue {
        char name[RX_Q_NAME_SIZE];
        char *start;
        char *head;
        char *tail;
        unsigned int flag;
        int size_of_block; /* in bytes */
        int num_of_blocks;
	RX_TASK *write_task; /* Task waiting to write */
	RX_TASK *read_task; /* Task waiting to read */
};

/* -------------- Application system calls ---------------- */
int rx_create_queue(struct msg_queue *mq, char *name, int size_of_block, int num_of_blocks, void *queue_start, unsigned int flag); /* flag 0: Q_STOP_FULL, Q_PRIO_NONE*/
int rx_delete_queue(struct msg_queue *mq);
int rx_read_from_queue(struct msg_queue *mq, char *msg_buffer, int size, int timeout);
int rx_write_to_queue(struct msg_queue *mq, const char *msg_buffer, int size, int timeout);

#endif
