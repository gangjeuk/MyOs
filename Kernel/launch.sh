make clean &&
make &&
sleep 1s &&
qemu-system-x86_64 -L . -m 64 -fda Disk.img  -M pc -s -enable-kvm 
