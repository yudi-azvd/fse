set -e

files="*"
final_dir="~/fse/03-uart/00-loopback/"
scp -q -r ${files} yudi@ratlas.local:${final_dir}
