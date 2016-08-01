#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    /*
    struct thread *t_current = thread_current();
    printf("current thread is %s\n",t_current->name);
    printf("syscall num : %d\n",*(int*)(f->esp));
    //printf("vaddr is %x\n",t_current->pagedir);
    hex_dump(f->esp, f->esp, 30, true);
    thread_exit ();
    */
    void *current_sp = f->esp;
    if(!is_user_vaddr(current_sp))
        exit(-1);
    else
    {
        if(*(int*)current_sp == SYS_EXIT){
            exit(*(int*)(current_sp+4));
        }

        else if(*(int*)current_sp == SYS_WRITE){
            write( *(int*)(current_sp+4), current_sp+8, *(unsigned*)(current_sp+12) );
        }

    }
}


void exit (int status)
{
    struct thread *t_current = thread_current();
    if(status == -1){
        printf("%s: exit(%d)\n",t_current->name, status);
        thread_exit();  
    }
    return ;

}

/////// need to edit! it's right that f->esp points current stack pointer! Keep going!
int write (int fd, const void *buffer, unsigned size)
{
    if(fd == 1){
        putbuf(buffer, size);
    }
    return size;
}

/*
pid_t exec (const char *file)
{
}

int wait (pid_t pid)
{
}
*/
