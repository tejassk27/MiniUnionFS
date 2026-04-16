#include "unionfs.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static struct fuse_operations unionfs_oper = {
    .getattr = unionfs_getattr,
    .readdir = unionfs_readdir,
    .open    = unionfs_open,
    .read    = unionfs_read,
    .write   = unionfs_write,
    .create  = unionfs_create,
    .unlink  = unionfs_unlink,
    .mkdir   = unionfs_mkdir,
    .rmdir   = unionfs_rmdir,
};

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <lower_dir> <upper_dir> <mountpoint>\n", argv[0]);
        return 1;
    }

    struct unionfs_state *state = malloc(sizeof(struct unionfs_state));
    state->lower_dir = realpath(argv[1], NULL);
    state->upper_dir = realpath(argv[2], NULL);

    char *fuse_argv[argc];
    fuse_argv[0] = argv[0];
    fuse_argv[1] = argv[3];

    for (int i = 4; i < argc; i++)
        fuse_argv[i - 2] = argv[i];

    return fuse_main(argc - 2, fuse_argv, &unionfs_oper, state);
}
