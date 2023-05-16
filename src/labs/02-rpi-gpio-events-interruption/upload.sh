final_dir="/02-rpi-gpio-events-interruption"

files="*.c"
scp -P 13508 ${files} pi@rpi3-08:~${final_dir}
