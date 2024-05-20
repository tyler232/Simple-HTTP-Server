# Simple-HTTP-Server

check firewall status:

sudo systemctl status firewalld

firewall allow:

sudo firewall-cmd --zone=public --add-port=8080/tcp --permanent
sudo firewall-cmd --reload