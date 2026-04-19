# 🚀 Mini-UnionFS (FUSE-Based Layered File System)

## 📌 Overview

Mini-UnionFS is a userspace filesystem built using **FUSE (Filesystem in Userspace)**.
It merges two directories into a single unified view, mimicking how container systems like Docker manage layered storage.

---

## 🎯 Features

* ✅ **Layer Stacking**

  * Combines `lower_dir` (read-only) and `upper_dir` (read-write)
  * Unified view via mount point

* ✅ **Copy-on-Write (CoW)**

  * Modifying a lower-layer file creates a copy in upper layer

* ✅ **Whiteout Deletion**

  * Deleting lower-layer file creates `.wh.<filename>` in upper layer

* ✅ **POSIX Operations Supported**

  * `getattr`, `readdir`, `read`, `write`, `create`, `unlink`, `mkdir`, `rmdir`

---

## 🏗️ Project Structure

```
mini-unionfs/
│
├── main.c
├── fuse_ops.c
├── logic.c
├── file_ops.c
├── unionfs.h
├── Makefile
├── test_unionfs.sh
└── README.md
```

---

## ⚙️ Requirements

* Linux (Ubuntu 20.04 / 22.04 recommended)
* FUSE3 installed

### Install FUSE3:

```bash
sudo apt update
sudo apt install libfuse3-dev fuse3
```

---

## 🛠️ Build Instructions

```bash
make
```

This generates:

```
mini_unionfs
```

---

## ▶️ Run Instructions

```bash
./mini_unionfs <lower_dir> <upper_dir> <mountpoint>
```

### Example:

```bash
mkdir lower upper mnt
echo "hello from lower" > lower/file.txt

./mini_unionfs lower upper mnt
```

---

## 🧪 Testing

Run the provided test script:

```bash
chmod +x test_unionfs.sh
./test_unionfs.sh
```

### Tests Included:

* ✔ Layer visibility
* ✔ Copy-on-Write behavior
* ✔ Whiteout deletion

---

## 🔍 How It Works

### 📂 Layer Precedence

```
upper_dir > lower_dir
```

If file exists in both → upper version is used.

---

### ✏️ Copy-on-Write

When modifying a lower file:

1. Copy file → upper_dir
2. Modify copy
3. Keep lower unchanged

---

### ❌ Whiteout (Deletion)

Deleting file creates:

```
upper_dir/.wh.filename
```

This hides the file from lower layer.

---

## 👥 Team Contribution

| Member   | Responsibility                                     |
| -------- | -------------------------------------------------- |
| Person 1 | FUSE Interface (`main.c`, `fuse_ops.c`)            |
| Person 2 | Core Logic (`logic.c`)                             |
| Person 3 | File Ops + Build (`file_ops.c`, Makefile, testing) |

---

## ⚠️ Limitations

* No support for symbolic links
* No caching mechanism
* Limited directory size handling (fixed buffer)

---

## 📄 Design Document

Refer to:

```
Mini_UnionFS_Design_Document.pdf
```

Includes:

* Architecture diagram
* Data structures
* Edge case handling

---

## 🎓 Learning Outcomes

* Understanding of FUSE filesystem design
* Implementation of Copy-on-Write
* Handling layered storage systems
* Real-world concept behind Docker storage

---

## 🧑‍💻 Authors

* Team of 3 (Mini-UnionFS Project)

---

## 📌 Notes

* Always unmount after use:

```bash
fusermount -u mnt
```

---

## ⭐ Final Status

✔ Fully functional
✔ Passes all test cases
✔ Ready for submission
