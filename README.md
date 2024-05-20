# Simple-HTTP-Server

## Overview

This is a simple HTTP server implemented in C, designed to handle HTTP GET requests for files or directories. The server uses epoll for handling multiple simultaneous connections efficiently. It serves files as downloadable content and provides a basic directory listing when a directory is requested.


## Dependencies

Linux Operating System with support for POSIX-compliant system calls, some recommendations are:
 - Ubuntu 20.04 and later
 - Fedora 32 and later
 - Debian 10 and later

GNU Compiler Collection (GCC) for Compilation
 - GCC 9.3.0 and later


## Set up

First you need to clone this repository and make executable
```bash
git clone https://github.com/tyler232/Simple-HTTP-Server.git
cd Simple-HTTP-Server
make
```

Executable will be created in your ~/server_bin, you need to export the path to use it directly
```bash
# Add this to your .bashrc
export PATH="$HOME/server_bin:$PATH"
```

If you are running a firewall on your server, you need to allow traffic on the port your HTTP server will use (default is 8080).
```bash
# Check firewall status
sudo systemctl status firewalld

# let firewall allow traffic on 8080
sudo firewall-cmd --zone=public --add-port=8080/tcp --permanent
sudo firewall-cmd --reload
```

## Usage

```bash
server  # host current directory on port 8080

server 8000 # host on port 8000, make sure firewall allows

server --directory dir/my_dir # host directory with path dir/my_dir on port 8080

server 8000 --directory dir/my_dir # host directory with path dir/my_dir on port 8000
```


## Contributions

Contributions are welcome! Please fork the repository and submit pull requests.


## Contact

For any questions or issues, please open an issue on this repo.