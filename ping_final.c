#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
 
// Define the Packet Constants
// ping packet size
#define PING_PKT_S 64
   
// Automatic port number
#define PORT_NO 0
 
// Automatic port number
#define PING_SLEEP_RATE 1000000
 
// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1
 
// Define the Ping Loop
int pingloop=1;
 
 
// ping packet structure
struct ping_pkt
{
    struct icmphdr hdr;
    char msg[PING_PKT_S-sizeof(struct icmphdr)];
};
 
// Calculating the Check Sum
//This function calculates the 16-bit one's complement sum of the supplied buffer (ICMP) header

unsigned short checksum(void *b, int len)
{    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;
 
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
 
 
// Interrupt handler
void intHandler(int dummy)
{
    pingloop=0;
}
 
// Performs a DNS lookup
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con)
{
    struct hostent *host_entity;
    char *ip=(char*)malloc(NI_MAXHOST*sizeof(char));
    int i;
 
    if ((host_entity = gethostbyname(addr_host)) == NULL)
    {
        // No ip found for hostname
        return NULL;
    }
     
    //filling up address structure
    strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr));
 
    (*addr_con).sin_family = host_entity->h_addrtype;
    (*addr_con).sin_port = htons (PORT_NO);
    (*addr_con).sin_addr.s_addr  = *(long*)host_entity->h_addr;
 
    return ip;
     
}
 
// Resolves the reverse lookup of the hostname
char* reverse_dns_lookup(char *ip_addr)
{
    struct sockaddr_in temp_addr;    
    socklen_t len;
    char buf[NI_MAXHOST], *ret_buf;
 
    temp_addr.sin_family = AF_INET;
    temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
    len = sizeof(struct sockaddr_in);
 
    if (getnameinfo((struct sockaddr *) &temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD))
    {
        return NULL;
    }
    ret_buf = (char*)malloc((strlen(buf) +1)*sizeof(char) );
    strcpy(ret_buf, buf);
    return ret_buf;
}
 
// make a ping request
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *ping_dom, char *ping_ip, char *rev_host)
{
    int ttl_val=64, msg_count=0, i, addr_len, flag=1, msg_received_count=0;
     
    struct ping_pkt pckt;
    struct sockaddr_in r_addr;
    struct timespec time_start, time_end, tfs, tfe;
    long double Latency=0, total_msec=0;
    struct timeval tv_out;
    tv_out.tv_sec = RECV_TIMEOUT;
    tv_out.tv_usec = 0;
 
    clock_gettime(CLOCK_MONOTONIC, &tfs);
 
     

    // setting timeout of recv setting
    setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);
 
    // send icmp packet in an infinite loop
    while(pingloop)
    {
        // flag is whether packet was sent or not
        flag=1;
       
        //filling packet
        bzero(&pckt, sizeof(pckt));
         
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = getpid();
         
        for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
            pckt.msg[i] = i+'0';
         
        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = msg_count++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
 
 
        usleep(PING_SLEEP_RATE);
 
        //send packet
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        if ( sendto(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*) ping_addr, sizeof(*ping_addr)) <= 0)
        {
           
            flag=0;
        }
 
        //receive packet
        addr_len=sizeof(r_addr);
 
        if ( recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &addr_len) <= 0 && msg_count>1)
        {
                return 0;
        }
 
        else
        {
            clock_gettime(CLOCK_MONOTONIC, &time_end);
             
            double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
            Latency = (time_end.tv_sec- time_start.tv_sec) * 1000.0+ timeElapsed;
             return latency; 
            // if packet was not sent, don't receive
            if(flag)
            {
                if(!(pckt.hdr.type ==69 && pckt.hdr.code==0))
                {
                    return 0;
                }
                else
                {
                    return Latency;  
                    msg_received_count++;
                }
            }
        }    
    }
    clock_gettime(CLOCK_MONOTONIC, &tfe);
    double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec))/1000000.0;
     
    total_msec = (tfe.tv_sec-tfs.tv_sec)*1000.0+ timeElapsed;  

}
