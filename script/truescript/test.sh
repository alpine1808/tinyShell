#!/home/manh/DistroboxData/tinyShell/tinyShell/build/tinyshell
pwd
sleep 2
echo "Hello World"
sleep 2
cd ..
sleep 2
pwd
sleep 2
ls
sleep 2
mkdir test
sleep 2
cd test
pwd
sleep 2
touch hoho.txt
ls
sleep 2
write hoho.txt "This is a test file."
sleep 2
read hoho.txt
sleep 2
rm hoho.txt
ls
sleep 2
cd ..
rm test
ls
sleep 2
gnome-text-editor haha.txt &
myList
sleep 5
export PIDS=$(myList | grep gnome-text-editor | grep -v "Terminated" | awk '{print $1}')
stop $PIDS
sleep 1
resume $PIDS
sleep 1
kill $PIDS
unset PIDS
gnome-text-editor haha.txt &
gnome-text-editor hihi.txt &
sleep 2
export PIDS=$(myList | grep gnome-text-editor | grep -v "Terminated" | awk '{print $1}')
stop $PIDS
sleep 1
resume $PIDS
sleep 1
kill $PIDS
unset PIDS
sleep 2
gnome-text-editor haha.txt
path add /home/manh/DistroboxData/tinyShell/tinyShell/script/externapp
cd ..
cd ..
pwd
sleep 1
path 
sleep 1
printNum &
sleep 10
export PIDS=$(myList | grep printNum | awk '{print $1}')
kill $PIDS
unset PIDS
path remove /home/manh/DistroboxData/tinyShell/tinyShell/script/externapp
path
sleep 1
printNum &
sleep 1
exit