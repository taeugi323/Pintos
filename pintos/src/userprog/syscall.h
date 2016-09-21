#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

typedef int pid_t;

void syscall_init (void);

//////// syscall list
void sys_exit (int);
int sys_write (int, const void*, unsigned);
void sys_halt (void);
pid_t sys_exec (const char*);
int sys_wait (pid_t);

#endif /* userprog/syscall.h */
