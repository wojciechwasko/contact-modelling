obj-m += skin_test_device.o
skin_test_device-objs := main.o init.o

# skin-config erroneously adds -D_FORTIFY_SOURCE=0 to cflags. two times.
# Hence let's remove it and add it at the end.
EXTRA_CFLAGS += $(shell skin-config --module-cflags | sed -e"s/-D_FORTIFY_SOURCE=[0-9]//g") -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
KBUILD_EXTRA_SYMBOLS := $(shell skin-config --module-symbols)

.PHONY: all
all:
	@$(MAKE) --no-print-directory -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)\
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(KBUILD_EXTRA_SYMBOLS)" modules
main.o: main.c init.h
init.o: init.c init.h
.PHONY: clean
clean:
	-@$(MAKE) --no-print-directory -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
