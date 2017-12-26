#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

/* Function creates, configures and returns an inode for the asked file (or)
   directory (differentiated by the mode param), under the directory specified
   by the dir param on the device dev, managed by the superblock sb param)
*/
struct inode* userfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, dev_t dev) {
	// Create a new inode to the superblock
	struct inode *inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);

		// Set the acces time, modified time and creation time metadata
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;

		switch (mode & S_IFMT) {
			case S_IFDIR:
				inc_nlink(inode);
				break;
			case S_IFREG:
			case S_IFLNK:
			default:
				printk(KERN_ERR "userfs can create meaningful inode for only root directory at the moment\n");
				return NULL;
				break;
		}
	}

	return inode;
}

// Function fills and validates the super_block structure of the filesystem
int userfs_fill_super(struct super_block *sb, void *data, int silent) {
	struct inode *inode;

	// Magic number that identifies our filesystem (some random number)
	sb->s_magic = 0x10032013;

	inode = userfs_get_inode(sb, NULL, S_IFDIR, 0);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		return -ENOMEM;
	}

	return 0;
}

// Function called at mounting the filesystem. Returns the root of the filesystem (like / for linux filesystem)
static struct dentry* userfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data) {
	// ret represents the root of our filesystem
	struct dentry *ret;

	ret = mount_bdev(fs_type, flags, dev_name, data, userfs_fill_super);

	if (unlikely(IS_ERR(ret))) {
		printk(KERN_ERR "Error mounting userfs\n");
	} else {
		printk(KERN_INFO "userfs is succesfully mounted on [%s]\n", dev_name);
	}

	return ret;
}

struct file_system_type userfs_fs_type = {
	// Owner is used to prevent kernel from unloading the module until the filesystem is disassembled
	.owner = THIS_MODULE,
	// Name of the filesystem
	.name = "userfs",
	// Function to be called when mounting the filesystem
	.mount = userfs_mount,
	// Function to be called when disassemblying the filesystem (provided by linux kernel)
	.kill_sb = kill_block_super,
	// Specify that filesystem requires a disk for operation
	.fs_flags = FS_REQUIRES_DEV
};

// Function called when loading the module
static int __init userfs_init(void) {
	int ret;

	// Register the filesystem so kernel recognize it
	ret = register_filesystem(&userfs_fs_type);
	
	// Likely function instructs the kernel that branch success is more probably for helping the CPU predict the branch success rate.
	if (likely(ret == 0)) {
		printk(KERN_INFO "Successfully registered userfs\n");
	} else {
		printk(KERN_ERR "Failed to register userfs. Error:[%d]", ret);
	}

	return ret;
}

// Function called when unloading the module
static void __exit userfs_exit(void) {
	int ret;

	ret = unregister_filesystem(&userfs_fs_type);

	if (likely(ret == 0)) {
		printk(KERN_INFO "Successfully unregistered userfs\n");
	} else {
		printk(KERN_ERR "Failed to unregister userfs. Error:[%d]", ret);
	}
}

// Specify the functions called at loading/unloading the module
module_init(userfs_init);
module_exit(userfs_exit);

// Set license to "GPL" to benefit of functions available for GPL modules.
MODULE_LICENSE("GPL");
