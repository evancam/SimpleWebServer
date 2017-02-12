# CSC 361 Assignment 1 #

Evan Cameron, V00780688, Lab B02


# Design #

The server initially starts up and binds a socket. The input is then checked for whether or not it should quit,
or continue waiting until a request is received from a client. Once a request is received, the request is then 
checked for its content to determine the server response. If the request is a valid 200 request, then the file 
associated with the request is also returned to the client. If it is not a valid request, a response is sent to 
the client telling them this. Then the server returns to checking for responses or quit inputs. 


# Structure #

The purpose of this assignment was to build a simple web server. It functions as follows:

1. Starts up the server based on parameters passed , ie ./sws <port> <directory>
2. Enters the main loop
3. Uses the select() function to  take socket and stdin inputs
4. If q is the input, quit the server
5. If there is input from a client to the socket, receive the data and handle it using handleRequest()
6. Continue looping

handleRequest()

1. Parse the request into pieces for it to be verified
2. Verify the request using verifyRequest() and send back an error code
3. If it is a 200 OK code, send the file to the client
4. If it is a 40x code, send back the relevant information to the client
5. Log the request on the server, including date, time and result

When a client tries to get a file, the path of the file will be relative based on where the server is located.



# Setup #

to run the server, run the makefile, it will set file permissions and build the sws binary.
after the make file has been run, type in ./sws <port> <directory> to run the server.
The server can also be run using make run to default to port 8080 and directory www.


# Testing #

Testing can be done by sending something similiar to the following command:

echo -e -n "GET / HTTP/1.1\r\n\r\n" | nc -u -s 192.168.1.100 10.10.1.100 8080 &

where 8080 is the port of the server, 192.168.1.100 is the source IP, and 10.10.1.100 is the destination IP

Testing can also be run using the testing file test_sws.sh

notes:

if q is input at the same time as a request is sent to the socket, it is likely the server will close
before a response is set. This has not been tested, but the q input is checked before the request is parsed.

