/* 验证 .debug_line 行映射：cl /Fe:line_map_smoke.exe line_map_smoke.c ..\code\src\elf.c */
#include <stdio.h>
#include <string.h>
#include "../code/src/elf.h"

int main(int argc, char** argv)
{
    char err[256];
    OS_ElfFile* f;
    uint32_t addr = 0;
    const char* path = argc > 1 ? argv[1]
        : "D:/C_Code/g431kb_for_openscope_d/EWARM/g431rb_tst/Exe/g431rb_tst.out";
    const char* src = argc > 2 ? argv[2]
        : "D:/C_Code/g431kb_for_openscope_d/Core/Src/main.c";
    int line = argc > 3 ? atoi(argv[3]) : 100;
    int i, hits = 0;

    f = os_elf_open(path, err, sizeof(err));
    if (!f) {
        printf("open fail: %s\n", err);
        return 1;
    }
    printf("vars=%d line_rows=%d line_files=%d\n",
           os_elf_var_count(f), os_elf_line_count(f), os_elf_line_file_count(f));
    for (i = 0; i < os_elf_line_file_count(f) && i < 20; i++) {
        const char* fp = os_elf_line_file_at(f, i);
        if (fp && fp[0] && strstr(fp, "main.c"))
            printf("  file[%d]=%s\n", i, fp);
    }
    for (i = 1; i < os_elf_line_file_count(f); i++) {
        const char* fp = os_elf_line_file_at(f, i);
        if (fp && strstr(fp, "main.c")) hits++;
    }
    printf("main.c path entries: %d\n", hits);
    if (os_elf_addr_of_line(f, src, line, &addr))
        printf("MAP OK %s:%d -> 0x%08X\n", src, line, addr);
    else
        printf("MAP FAIL %s:%d\n", src, line);
    /* basename only */
    if (os_elf_addr_of_line(f, "main.c", line, &addr))
        printf("MAP basename OK main.c:%d -> 0x%08X\n", line, addr);
    else
        printf("MAP basename FAIL main.c:%d\n", line);
    os_elf_close(f);
    return 0;
}
