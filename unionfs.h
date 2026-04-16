#ifndef UNIONFS_H
#define UNIONFS_H

#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <limits.h>
#include <sys/stat.h>

// 🔹 Global state
struct unionfs_state {
    char *lower_dir;
    char *upper_dir;
};

#define UNIONFS_DATA ((struct unionfs_state *) fuse_get_context()->private_data)

// 🔹 logic.c
int resolve_path(const char *path, char *resolved_path);
int copy_file(const char *src, const char *dest);
void get_whiteout_path(const char *path, char *whiteout_path);

// 🔹 fuse_ops.c
int unionfs_getattr(const char *, struct stat *, struct fuse_file_info *);
int unionfs_readdir(const char *, void *, fuse_fill_dir_t, off_t,
                    struct fuse_file_info *, enum fuse_readdir_flags);
int unionfs_open(const char *, struct fuse_file_info *);
int unionfs_read(const char *, char *, size_t, off_t, struct fuse_file_info *);
int unionfs_write(const char *, const char *, size_t, off_t, struct fuse_file_info *);

// 🔹 file_ops.c
int unionfs_create(const char *, mode_t, struct fuse_file_info *);
int unionfs_unlink(const char *);
int unionfs_mkdir(const char *, mode_t);
int unionfs_rmdir(const char *);

#endif
