#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    
    struct thread *t_current = thread_current();
    /*
    printf("current thread is %s\n",t_current->name);
    printf("syscall num : %d\n",*(int*)(f->esp));
    //printf("vaddr is %x\n",t_current->pagedir);
    hex_dump(f->esp, f->esp, 30, true);
    thread_exit ();
    */
    void *current_sp = f->esp;

    if(!is_user_vaddr(current_sp) || pagedir_get_page(t_current->pagedir, current_sp) == NULL){
        f->eax = -1; 
        sys_exit(-1);
    }
    else
    {
        if(*(int*)current_sp == SYS_EXIT){
            f->eax = *(int*)(current_sp+4);
            sys_exit(*(int*)(current_sp+4));
        }

        else if(*(int*)current_sp == SYS_WRITE){
            //printf("fd : %d, buf pos: %x, size : %d\n", *(int*)(current_sp+4), current_sp+8, *(unsigned*)(current_sp+12) );
            //hex_dump(*(int*)(current_sp+8), *(int*)(current_sp+8), 30, true);
            //hex_dump(0xbfffff68, 0xbfffff68, 30, true);

            ////// For here, watch out second parameter! 
            ////// Second param is not the address(0xbfffff30), but the value(0xbfffff68).
            sys_write( *(int*)(current_sp+4), *(int*)(current_sp+8), *(unsigned*)(current_sp+12) );

        }

    }
}


void sys_exit (int status)
{
    struct thread *t_current = thread_current();

    printf("%s: exit(%d)\n",t_current->name, status);
    thread_exit();
    return ;

}

/////// need to edit! it's right that f->esp points current stack pointer! Keep going!
int sys_write (int fd, const void *buffer, unsigned size)
{
    if(fd == 1){
        putbuf(buffer, size);
    }
    return size;
}

/*
pid_t sys_exec (const char *file)
{
}

int sys_wait (pid_t pid)
{
}
*/
