# Ping_osquery

Part1: Steps to perfom

1. Install OsQuery following the steps provided in https://osquery.io/ for you operating system.
2.  Perform Ad-hoc Security Check
     To launch osquery execute sudo osqueryi --config_path /etc/osquery/osquery.conf --verbose  (assing osqueryi and osqueryd the verbose option lets you see any errors or warnings that might indicate issues with osquery)
3. To Check who else is loged into the system execute select * from logged_in_users ;
4. List of loaded kernel modules execute select name, used_by, status from kernel_modules where status="Live" ;
5. To find Backdoors on the server is to run a query that lists all the listening ports. execute: select * from listening_ports ;
6. To look at file activity on the server execute: select target_path, action, uid from file_events ;
7. Navigate to the spec source folder and place the Ping.table
8. Place the Ping.c file under osquery/tables/utility/time.cpp
9. ./build/[darwin|linux]/osquery/osqueryi 
10. Run SELECT Latency FROM ping 

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

 
Part3: Security Threats and prevention

ICMP resides in the Layer3 i.e the Network layer and this is prone to the Smurf attack, i.e A Smurf Attack is a DDoS form that makes computer networks inoperable by exploiting IP (Internet Protocol) and ICMP (Internet Control Message Protocols) vulnerabilities. 
To avoid being the victim, you should have a prevention strategy based on traffic network monitoring that can detect any oddments – like packet volume, behaviour and signature. This could help you stop a Smurf Attack before it even begins you can use a DNS based cloud service and have secure certificate exchange. 
