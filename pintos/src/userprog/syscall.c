#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

struct list dead_list;

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

    if(!is_user_vaddr(current_sp) || pagedir_get_page(t_current->pagedir, current_sp) == NULL) {
        f->eax = -1; 
        sys_exit(-1);
    }
    else
    {
        if(*(int*)current_sp == SYS_EXIT) {
            f->eax = *(int*)(current_sp+4);
            sys_exit(*(int*)(current_sp+4));
        }

        else if(*(int*)current_sp == SYS_WRITE) {
            //printf("fd : %d, buf pos: %x, size : %d\n", *(int*)(current_sp+4), current_sp+8, *(unsigned*)(current_sp+12) );
            //hex_dump(*(int*)(current_sp+8), *(int*)(current_sp+8), 30, true);
            //hex_dump(0xbfffff68, 0xbfffff68, 30, true);

            ////// For here, watch out second parameter! 
            ////// Second param is not the address(0xbfffff30), but the value(0xbfffff68).
            sys_write( *(int*)(current_sp+4), *(int*)(current_sp+8), *(unsigned*)(current_sp+12) );

        }

        else if(*(int*)current_sp == SYS_HALT) {
            sys_halt();
        }

        else if(*(int*)current_sp == SYS_EXEC) {
            f->eax = sys_exec(*(int*)(current_sp+4));
        }

        else if(*(int*)current_sp == SYS_WAIT) {
            f->eax = sys_wait(*(int*)(current_sp+4));
        }


    }
}


void sys_exit (int status)
{
    struct thread *t_current = thread_current();
    struct dead_thread *temp;
    struct list_elem *e;
    char command_name[32]={0,};
    char *token, *save_ptr; 

    memcpy(command_name, t_current->name, 32);
    token = strtok_r(command_name, " ", &save_ptr );
    printf("%s: exit(%d)\n",token, status);
    t_current->exit_status = status;

    thread_exit();
    /*
    for (e = list_begin (&dead_list); e != list_end (&dead_list);
            e = list_next (e)) {

        temp = list_entry(e, struct dead_thread, deadelem);
        printf("[syscall_sysexit] name : %s, tid : %d\n", temp->name, temp->tid);
    }*/

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

void sys_halt(void)
{
    shutdown_power_off();
}

// In here, process_execute() will be used.
// But, it returns tid_t variable.
// tid_t is originally for kernel threads, but we will use tid_t as pid_t.
// It's all right because PINTOS uses single thread system.

pid_t sys_exec (const char *cmd_line)
{
    pid_t child_pid;
    
    child_pid = process_execute(cmd_line);
    
    return child_pid;
}

int sys_wait (pid_t pid)
{
    //printf("[sys_wait] current_thread's tid : %d\n",thread_current()->tid);
    return process_wait(pid);
}

