#!/bin/bash

echo 'this is a test'
wget -q -O - https://dl-ssl.google.com/linux/linux_signing_key.pub | sudo apt-key add -
sudo sh -c 'echo "deb http://dl.google.com/linux/chrome/deb/ stable main" >> /etc/apt/sources.list.d/google-chrome.list'
sudo apt-get update
sudo apt-get install -y google-chrome-stable

sudo apt-get install -y vsftpd
sudo apt-get install -y openssh-server
sudo apt-get install -y xdotool
sudo apt-get install =y unclutter