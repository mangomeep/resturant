# resturant
an http server written in C for fun

## installation
either download this repository as a .zip or use git to clone this repository. cd into rhe folder, use make to compile the server, then `./server` to start it up.
```
git clone https://github.com/mangomeep/resturant.git
cd resturant
make
./server
```

## connecting
the server should bind to localhost on the port you specified when you start the server.
connect to `localhost:xxxx` or `127.0.0.1:xxxx` with a browser, where xxxx is the port you specified.

## customising
you can add .html and .css files to the site folder, and as long as they're linked properly, they will be available.
if you want to have other devices connect to the server, change landing\_address on line 35 of server.c to your computer's internal network IP address (probably something like `192.168.1.x`). you'll then have to use `192.168.1.x:xxxx` to connect to the server instead of `localhost:xxxx`
