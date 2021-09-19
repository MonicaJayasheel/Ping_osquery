# Ping_osquery

Part1: Steps to perfom

1. Install OsQuery following the steps provided in https://osquery.io/ for you operating system.
2. 





Part 2: Explaining the Ping.c code 
Creating the PING



Working Mechanism

The Ping program sends a small packet of information containing an ICMP ECHO_REQUEST to a specified computer, which then sends an ECHO_REPLY packet in return.
The packet has a TTL (time-to-live) value determining max number of router hops.
If the packet does not reach, then the sender is noted back with the error.

Implementation

The steps followed by a simple ping program are:
1.	Take a hostname as input

2.	Do a DNS lookup
    DNS lookup can be done using gethostbyname(). The gethostbyname() function converts a normal human readable website and returns a structure of type hostent which contains IP     address in form of binary dot notation and also address type.

3.	Reverse DNS lookup is performed using getnameinfo(), and it converts dot notation IP address to hostname.

4.	Open a Raw socket using SOCK_RAW with protocol as IPPROTO_ICMP.
    Note: raw socket requires superuser rights so you have to run this code using sudo

5.	When crtl + C is pressed, ping gives a report. 
    This interrupt is caught by an interrupt handler which just sets our pinging looping condition to false.

6.	Here comes the main ping sending loop.
    We have to:
      1.	Set the ttl option to a value in the socket
          TTL value is set to limit the number of hops a packet can make.
      2.	Set the timeout of the recv function
          If timeout is not set, recv will wait forever, halting the loop.
      3.	Fill up the icmp packet
            As follows:
          1.	Set packet header type to ICMP_ECHO.
          2.	Set id to pid of process
          3.	Fill msg part randomly.
          4.	Calculate checksum and fill it in checksum field.
          5.	Send the packet
          6.	Wait for it to be received.

Part2: Methods used and Implementation 
