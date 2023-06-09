final_dir="/encoder/"

files="*"
# scp ${files} pi@rpi3-05.local:~${final_dir}
scp -r ${files} yudi@ratlas.local:~${final_dir}
