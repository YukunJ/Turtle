echo "====##Build the webbench if have not yet##===="

make

echo "====##Run the http server in the background at default Port 20080 and mute its stdout and stderr##===="

cd ../build
./http_server & s> /dev/null 2>&1 

echo "====##Wait 1 second to make sure the http server is up##====="

sleep 1

echo "====##Start the webbench stress testing##====="

cd ../webbench
./webbench -c $1 -t $2 http://127.0.0.1:20080/

echo "====##Find the process ID of the background-running http server and kill it##===="

ps -A | grep http_server | grep -v grep | awk '{print $1}' | xargs kill

echo "====##Clean workspace##===="

make clean