cmd_/home/tann/kernel_programming/simple_char_driver/Module.symvers := sed 's/ko$$/o/' /home/tann/kernel_programming/simple_char_driver/modules.order | scripts/mod/modpost -m -a  -o /home/tann/kernel_programming/simple_char_driver/Module.symvers -e -i Module.symvers   -T -