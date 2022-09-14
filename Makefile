obj-m += rootkit.o
rootkit-objs = main.o functions/kill.o functions/protectKing.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
