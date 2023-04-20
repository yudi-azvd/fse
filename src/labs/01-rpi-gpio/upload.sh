final_dir="/fse/src/labs/01-rpi-gpio/"

files="*.c"
scp -P 13508 ${files} yudiazevedo@164.41.98.28:~${final_dir}
