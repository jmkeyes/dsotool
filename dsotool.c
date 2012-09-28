/* dsotool.c */

/*#define _POSIX_SOURCE*/
#define _GNU_SOURCE
#include <link.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <execinfo.h>

int dso_header_iterator(struct dl_phdr_info *info, size_t size, void *data) {
    if (strlen(info->dlpi_name) > 0) {
        printf("[%p:%lx] %s\n", (void *)info->dlpi_addr, (info->dlpi_addr + info->dlpi_phdr->p_memsz), info->dlpi_name);
    }
}

void signal_handler(int signum) {
    /* This is an awful hack. It will eat your pets. */
    struct sigcontext *p = (struct sigcontext *) (((char *) &p));
    static int pending = 0;
    void *frame[32];
    char **symbols;
    int total;

    if (pending++ > 0) {
        printf("Caught second signal: %d: %s\n", signum, strsignal(signum));
        _exit(1);
    }

    printf("----- Start of library backtrace -----\n");
    printf("Signal %d: %s\n", signum, strsignal(signum));

    printf("-- Mappings --\n");
    dl_iterate_phdr(&dso_header_iterator, NULL);

    printf("-- Registers --\n");
#if defined(__X86_64__)
    printf("[ r8=%#lx | r9=0x%#lx ]\n", p->r8, p->r9);
    printf("[ r10=%#lx | r11=0x%#lx ]\n", p->r10, p->r11);
    printf("[ r12=%#lx | r13=0x%#lx ]\n", p->r12, p->r13);
    printf("[ r14=%#lx | r15=0x%#lx ]\n", p->r14, p->r15);
    printf("[ rdi=%#lx | rsi=0x%#lx ]\n", p->rdi, p->rsi);
    printf("[ rbp=%#lx | rbx=0x%#lx ]\n", p->rbp, p->rbx);
    printf("[ rdx=%#lx | rax=0x%#lx ]\n", p->rdx, p->rax);
    printf("[ rcx=%#lx | rsp=0x%#lx ]\n", p->rcx, p->rsp);
    printf("[ rip=%#lx | ]\n", p->rip, " ");
#elif defined(__i686__)
    printf("[ eax=%#x' | ebx=%#x' ]\n", p->eax, p->ebx);
    printf("[ ecx=%#x' | edx=%#x' ]\n", p->eax, p->ebx);
    printf("[ esp=%#x' | ebp=%#x' ]\n", p->eax, p->ebx);
    printf("[ esi=%#x' | edi=%#x' ]\n", p->eax, p->ebx);
#endif

    printf("-- Backtrace --\n");
    total   = backtrace(frame, (sizeof(frame) / sizeof(void)));
    symbols = backtrace_symbols(frame, total);

    int i = 0;
    for (i = 2; i < total; i++) {
        printf("[sf:%d]: from %s\n", i - 2, symbols[i]);
    }
    free(symbols);

    printf("----- End of library backtrace -----\n");
    sync();
    _exit(0);
}

__attribute__ ((constructor))
void dsotool(void) {
  signal(SIGSEGV, &signal_handler);
  signal(SIGABRT, &signal_handler);
  signal(SIGFPE, &signal_handler);
  signal(SIGILL, &signal_handler);
}
