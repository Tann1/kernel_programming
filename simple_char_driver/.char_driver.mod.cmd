cmd_/home/tann/kernel_programming/simple_char_driver/char_driver.mod := printf '%s\n'   char_driver.o | awk '!x[$$0]++ { print("/home/tann/kernel_programming/simple_char_driver/"$$0) }' > /home/tann/kernel_programming/simple_char_driver/char_driver.mod
