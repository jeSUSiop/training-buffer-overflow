#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

/* ---------------------------------------------------------------------------
 * Task 1 flag keeper (lab infrastructure)
 *
 * When this helper is installed setuid-root by the sandbox provisioning, it
 * attaches the per-sandbox "flag-t1-..." group to the process as a
 * *supplementary* group and then drops ALL privileges back to the real user
 * before the shellcode runs. Because the group is supplementary (not the
 * effective gid), /bin/bash keeps it instead of stripping it, so it appears in
 * the output of `id` -- but only when `id` is launched through the shellcode.
 * Typing `id` in your own login shell does not show it (you are not a member
 * of the group), which is exactly the "capture-only" behaviour Task 1 needs.
 *
 * This function is a no-op on a normal (non-setuid) build, so it changes
 * nothing for Tasks 2-4, where this binary is never run privileged.
 * ------------------------------------------------------------------------- */
static void sl_attach_flag_group(void)
{
    /* Only act when launched setuid-root by an unprivileged user. */
    if (geteuid() != 0 || getuid() == 0)
        return;

    gid_t rgid = getgid();
    FILE *gf = fopen("/etc/sl-flag-gid", "r");
    if (gf) {
        long fgid = -1;
        if (fscanf(gf, "%ld", &fgid) == 1 && fgid >= 0) {
            gid_t list[2] = { rgid, (gid_t)fgid };
            setgroups(2, list);   /* needs root; adds the flag group */
        }
        fclose(gf);
    }

    /* Drop privileges permanently (gid before uid), keeping supplementary groups. */
    if (setgid(rgid) != 0) _exit(1);
    if (setuid(getuid()) != 0) _exit(1);
}

// Read the shellcode from a file, and then execute the code.
int main(int argc, char **argv)
{
    char code[500];
    FILE *fd;
#if __x86_64__
    const char *filename = "codefile_64";
#else
    const char *filename = "codefile_32";
#endif

    sl_attach_flag_group();

    fd = fopen(filename, "r");
    if (!fd){
       perror(filename); exit(1);
    }
    fread(code, sizeof(char), 500, fd);

    int (*func)() = (int(*)())code;
    func();
    return 1;
}
