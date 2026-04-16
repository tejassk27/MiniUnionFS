#include "unionfs.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// 🔹 helper: parent path
static void get_parent_path(const char *path, char *parent) {
    strcpy(parent, path);
    char *last = strrchr(parent, '/');
    if (last && last != parent) *last = '\0';
    else strcpy(parent, "/");
}

// 🔹 copy file
int copy_file(const char *src, const char *dest)
{
    int in = open(src, O_RDONLY);
    if (in < 0) return -errno;

    int out = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        close(in);
        return -errno;
    }

    char buf[4096];
    ssize_t n;

    while ((n = read(in, buf, sizeof(buf))) > 0) {
        if (write(out, buf, n) != n) {
            close(in); close(out);
            return -EIO;
        }
    }

    close(in);
    close(out);
    return 0;
}

// 🔹 whiteout path
void get_whiteout_path(const char *path, char *whiteout_path)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char parent[PATH_MAX];
    get_parent_path(path, parent);

    const char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;

    snprintf(whiteout_path, PATH_MAX, "%s%s/.wh.%s",
             state->upper_dir, parent, filename);
    whiteout_path[PATH_MAX - 1] = '\0';
}

// 🔹 resolve path
int resolve_path(const char *path, char *resolved)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char upper[PATH_MAX];
    char lower[PATH_MAX];
    char whiteout[PATH_MAX];

    snprintf(upper, PATH_MAX, "%s%s", state->upper_dir, path);
    snprintf(lower, PATH_MAX, "%s%s", state->lower_dir, path);

    get_whiteout_path(path, whiteout);

    if (access(whiteout, F_OK) == 0)
        return -ENOENT;

    if (access(upper, F_OK) == 0) {
        strcpy(resolved, upper);
        return 0;
    }

    if (access(lower, F_OK) == 0) {
        strcpy(resolved, lower);
        return 0;
    }

    return -ENOENT;
}
