#!/bin/sh
echo "Kiem tra cac lenh built-in"
pwd
help

echo "Kiem tra Alias"
alias ll=ls
ll

echo "Kiem tra quan ly file"
mkdir test_dir
cd test_dir
touch test_file.txt
write test_file.txt "Hello from tinyShell script!"
echo "Noi dung file test_file.txt:"
read test_file.txt
ls
cd ..

echo "Kiem tra lenh ngoai"
date
uname -a

echo "Kiem tra lich su tien trinh"
history

echo "Hoan thanh khao sat"
