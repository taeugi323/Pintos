#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

//////// syscall list
void exit (int);
int write (int, const void*, unsigned);

#endif /* userprog/syscall.h */
