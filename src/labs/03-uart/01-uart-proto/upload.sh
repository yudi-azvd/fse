set -e

username="yudiazevedo"
remote_path="/home/${username}/01-uart-proto"
ipaddr1="164.41.98.16"
ipaddr2="164.41.98.28"
    
rsync \
    -rqptz \
    --progress --delete --delete-excluded \
    --exclude=uart \
    -e 'ssh -p 13508' \
    * \
    "${username}@${ipaddr1}:${remote_path}"

rsync \
    -rqptz \
    --progress --delete --delete-excluded \
    --exclude=uart \
    -e 'ssh -p 13508' \
    * \
    "${username}@${ipaddr2}:${remote_path}"
