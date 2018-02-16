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

#ifndef __EVENTS_H__
#define __EVENTS_H__

/********************** Events **********************/
struct event_group{
        TCB *task; /* task waiting for the event */
        struct event_group *next;
};

void rose_event_thread();

/* -------------- Application system calls ---------------- */
void init_event_group(struct event_group *p);
#if(CONFIG_EVENT_COUNT > 0)
struct event_group * create_event_group(void);
void delete_event_group(struct event_group *p);
#endif
int set_event_flag(unsigned int flag);
int clear_event_flag(unsigned int flag);
int wait_for_events(struct event_group *, int flag);
void notify_event(struct event_group *, unsigned int flag);
int wait_event(struct event_group *, int condition);
