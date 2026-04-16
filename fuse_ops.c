#include "unionfs.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

// 🔹 getattr
int unionfs_getattr(const char *path, struct stat *stbuf,
                    struct fuse_file_info *fi)
{
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    char resolved[PATH_MAX];
    if (resolve_path(path, resolved) != 0)
        return -ENOENT;

    if (lstat(resolved, stbuf) == -1)
        return -errno;

    return 0;
}

// 🔹 readdir
int unionfs_readdir(const char *path, void *buf,
                    fuse_fill_dir_t filler, off_t offset,
                    struct fuse_file_info *fi,
                    enum fuse_readdir_flags flags)
{
    (void) offset;
    (void) fi;
    (void) flags;

    struct unionfs_state *state = UNIONFS_DATA;

    char upper_path[PATH_MAX];
    char lower_path[PATH_MAX];

    snprintf(upper_path, PATH_MAX, "%s%s", state->upper_dir, path);
    snprintf(lower_path, PATH_MAX, "%s%s", state->lower_dir, path);

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    char seen[1024][256];
    int seen_count = 0;

    DIR *dp;
    struct dirent *de;

    // Upper layer
    dp = opendir(upper_path);
    if (dp) {
        while ((de = readdir(dp))) {
            if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
                continue;

            if (strncmp(de->d_name, ".wh.", 4) == 0)
                continue;

            filler(buf, de->d_name, NULL, 0, 0);
            strcpy(seen[seen_count++], de->d_name);
        }
        closedir(dp);
    }

    // Lower layer
    dp = opendir(lower_path);
    if (dp) {
        while ((de = readdir(dp))) {

            if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
                continue;

            int found = 0;
            for (int i = 0; i < seen_count; i++) {
                if (!strcmp(seen[i], de->d_name)) {
                    found = 1;
                    break;
                }
            }
            if (found) continue;

            char temp_path[PATH_MAX];
            snprintf(temp_path, PATH_MAX, "%s/%s", path, de->d_name);

            char whiteout[PATH_MAX];
            get_whiteout_path(temp_path, whiteout);

            if (access(whiteout, F_OK) == 0)
                continue;

            filler(buf, de->d_name, NULL, 0, 0);
        }
        closedir(dp);
    }

    return 0;
}

// 🔹 open
int unionfs_open(const char *path, struct fuse_file_info *fi)
{
    struct unionfs_state *state = UNIONFS_DATA;

    char upper[PATH_MAX];
    char lower[PATH_MAX];

    snprintf(upper, PATH_MAX, "%s%s", state->upper_dir, path);
    snprintf(lower, PATH_MAX, "%s%s", state->lower_dir, path);

    // Copy-on-Write
    if ((fi->flags & O_WRONLY) || (fi->flags & O_RDWR)) {
        if (access(upper, F_OK) != 0 && access(lower, F_OK) == 0) {
            if (copy_file(lower, upper) != 0)
                return -EIO;
        }
    }

    char resolved[PATH_MAX];
    if (resolve_path(path, resolved) != 0)
        return -ENOENT;

    int fd = open(resolved, fi->flags);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

// 🔹 read
int unionfs_read(const char *path, char *buf, size_t size,
                 off_t offset, struct fuse_file_info *fi)
{
    int fd = fi->fh;
    if (fd < 0) return -EBADF;

    int res = pread(fd, buf, size, offset);
    if (res == -1) return -errno;

    return res;
}

// 🔹 write
int unionfs_write(const char *path, const char *buf, size_t size,
                  off_t offset, struct fuse_file_info *fi)
{
    int fd = fi->fh;
    if (fd < 0) return -EBADF;

    int res = pwrite(fd, buf, size, offset);
    if (res == -1) return -errno;

    return res;
}
