obj-m := userfs.o
userfs-objs := user.o
ccfalgs-y := -DUSERFS_DEBUG

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean
