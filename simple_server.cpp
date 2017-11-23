/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

struct node
{
	string node_type; //  INPUT or OUTPUT or MID
	string type; // OR or NOT
	int value;
	int layer;
	int input1;
	int input2;
	int num;
};
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
		// cout<<n<<endl;
		received.push_back(buffer);
		// printf("kya machh raha hai? %s\n",buffer);
		len+=received.back().size();
		if(received.back().find('!')!=string::npos) break;
		bzero(buffer,512);
	}
	if (n < 0)		return false;
	/* linearize the received tokens into single token */
	s=string(len,0);
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
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[513];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, 
		(struct sockaddr *) &cli_addr, 
		&clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");
	bzero(buffer,513);
	// n = read(newsockfd,buffer,1023);
	// int size_recv , total_size= 0;
	// int CHUNK_SIZE = 512;
 //    char chunk[CHUNK_SIZE];
 //    //loop
 //    while(1)
 //    {
 //    	printf("In\n");
 //        memset(chunk ,0 , CHUNK_SIZE);  //clear the variable
 //        if((size_recv =  recv(newsockfd , chunk , CHUNK_SIZE , 0) ) < 0)
 //        {
 //            break;
 //        }
 //        else
 //        {
 //            total_size += size_recv;
 //            printf("%s" , chunk);
 //        }
 //        printf("%d\n", size_recv);
 //        if(size_recv != CHUNK_SIZE)
 //        	break;
 //    }
    // printf("Done\n");
    string s = "";
	read_from_socket(newsockfd, s);
	cout<<s;

	// string str = "";
	// cout<<"Here is the value:" + str + "\n"<<endl;
	n = write(newsockfd,"I got your message",18);
	if (n < 0) error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	return 0; 
}