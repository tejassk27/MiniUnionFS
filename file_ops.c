#include "unionfs.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// 🔹 create
int unionfs_create(const char *path, mode_t mode,
                   struct fuse_file_info *fi)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char upper[PATH_MAX];
    snprintf(upper, PATH_MAX, "%s%s", state->upper_dir, path);

    char whiteout[PATH_MAX];
    get_whiteout_path(path, whiteout);

    if (access(whiteout, F_OK) == 0)
        unlink(whiteout);

    int fd = open(upper, fi->flags | O_CREAT, mode);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

// 🔹 unlink
int unionfs_unlink(const char *path)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char upper[PATH_MAX];
    char lower[PATH_MAX];
    char whiteout[PATH_MAX];

    snprintf(upper, PATH_MAX, "%s%s", state->upper_dir, path);
    snprintf(lower, PATH_MAX, "%s%s", state->lower_dir, path);

    get_whiteout_path(path, whiteout);

    if (access(upper, F_OK) == 0)
        unlink(upper);

    if (access(lower, F_OK) == 0) {
        int fd = open(whiteout, O_CREAT | O_WRONLY, 0644);
        if (fd == -1)
            return -errno;
        close(fd);
    }

    return 0;
}

// 🔹 mkdir
int unionfs_mkdir(const char *path, mode_t mode)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char upper[PATH_MAX];
    snprintf(upper, PATH_MAX, "%s%s", state->upper_dir, path);

    if (mkdir(upper, mode) == -1)
        return -errno;

    return 0;
}

// 🔹 rmdir
int unionfs_rmdir(const char *path)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char upper[PATH_MAX];
    snprintf(upper, PATH_MAX, "%s%s", state->upper_dir, path);

    if (rmdir(upper) == -1)
        return -errno;

    return 0;
}
