SRC_DIR = src

all: kernel disk qemu

kernel:
	$(MAKE) -C $(SRC_DIR) kernel

disk:
	$(MAKE) -C $(SRC_DIR) disk

qemu:
	$(MAKE) -C $(SRC_DIR) qemu

qemu_native:
	$(MAKE) -C $(SRC_DIR) qemu_native

clean:
	$(MAKE) -C $(SRC_DIR) clean