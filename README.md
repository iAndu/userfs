# userfs

Project example: https://github.com/psankar/simplefs Start from latest commits upward

To test:

dd bs=1M count=100 if=dev/zero of=image
mkdir mount

make

sudo su #switch to root user

dmesg

insmod userfs.ko

dmesg

- Successfully registered userfs
mount -o loop -t userfs image mount

dmesg

- Successfully registered userfs
- userfs is successfully mounted on [/dev/loop1]

umount mount

rmmod userfs.ko

dmesg

- Sucessfully registered userfs
- userfs is successfully mounted on [/dev/loop1]
- Successfully unregistered userfs
