cmd_/home/tann/kernel_programming/simple_hello/modules.order := {   echo /home/tann/kernel_programming/simple_hello/hello_world.ko; :; } | awk '!x[$$0]++' - > /home/tann/kernel_programming/simple_hello/modules.order