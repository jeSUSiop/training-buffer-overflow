#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

/* ---------------------------------------------------------------------------
 * Task 1 flag keeper (lab infrastructure)
 *
 * The per-sandbox Task 1 flag is the name of a group whose sole member is the
 * dedicated account "flagkeeper1" (it is flagkeeper1's primary group). When
 * this helper is installed setuid-root by the sandbox provisioning, it loads
 * flagkeeper1's groups into the process as *supplementary* groups and then
 * drops ALL privileges back to the real user before the shellcode runs.
 *
 * Because the flag group ends up supplementary (not the effective gid),
 * /bin/bash keeps it instead of stripping it, so it shows up in `id` -- but
 * only when `id` is launched through the shellcode. Typing `id` in your own
 * login shell does not show it (you are not a member of the group), which is
 * exactly the "capture-only" behaviour Task 1 needs.
 *
 * This function is a no-op on a normal (non-setuid) build, so it changes
 * nothing for Tasks 2-4, where this binary is never run privileged.
 * ------------------------------------------------------------------------- */
#define FLAG_KEEPER_USER "flagkeeper1"

static void sl_attach_flag_group(void)
{
    /* Only act when launched setuid-root by an unprivileged user. */
    if (geteuid() != 0 || getuid() == 0)
        return;

    /* Load flagkeeper1's groups (its primary group is the per-sandbox flag),
     * keeping the real user's own gid in the list. Needs root (CAP_SETGID). */
    initgroups(FLAG_KEEPER_USER, getgid());

    /* Drop privileges permanently (gid before uid); supplementary groups stay. */
    if (setgid(getgid()) != 0) _exit(1);
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
