#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
using namespace std;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = write(s, buf+total, bytesleft);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

bool write_to_socket(int sockfd,const char* msg,int len){
	int n,i=0;
	do{	n = write(sockfd,msg+i,min(len-i,512));
		i+=n;
		// printf("left=%s\n",msg);	
	} while(i<len&&n>=0);
	return n>=0&&i==len;
}

//read long string from socket terminated by '!'
bool read_from_socket(int sockfd,string &s){
	char buffer[513];
	/* read message from client possibly spread over multiple blocks */
	bzero(buffer,513);
	vector<string>received;
	int n=0,len=0;
	while((n=read(sockfd,buffer,512))>0){
		received.push_back(buffer);
		// printf("kya machh raha hai? %s\n",buffer);
		len+=received.back().size();
		if(received.back().find('!')!=string::npos) break;
		bzero(buffer,512);
	}
	if (n < 0)		return false;
	/* linearize the received tokens into single token */
	s=string(len+1,0);
	n=0;
	for(auto r: received) {
		for(int i=0;i<r.size();i++)
			s[i+n]=r[i];
		n+=r.size();
	}
	// printf("%s\n",s.c_str());
	// s[n]=0;
	return true;
}



int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[513];
	if (argc < 3) {
	   fprintf(stderr,"usage %s hostname port\n", argv[0]);
	   exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		 (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	int num_inputs;
	string str;
	cin>>str;
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	int len = str.length();
	write_to_socket(sockfd, str.c_str(), len); 
	// n = write(sockfd,str.c_str(),strlen(str.c_str()));
	// if (n < 0) 
	// 	 error("ERROR writing to socket");
	bzero(buffer,513);
	n = read(sockfd,buffer,512);
	if (n < 0) 
		 error("ERROR reading from socket");
	printf("%s\n",buffer);
	close(sockfd);
	return 0;
}