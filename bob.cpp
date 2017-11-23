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

struct ciphertext
{
	int X1;
	int Y1;
	int X2;
	int Y2;
};

struct Add;

struct Enc
{
	Add * a1;
	Add * a2;
	Add * a3;
	Add * a4;
	ciphertext * c;
	int k;
};

struct Add
{
	Enc * e1;
	Enc * e2;
	int k;
};

ciphertext negate(ciphertext c)
{
	ciphertext n;
	n.X1 = c.X2;
	n.Y1 = c.Y2;
	n.X2 = c.X1;
	n.Y2 = c.Y1;
	return n;
}

Enc negate(Enc e, int k)
{

	*((e.a1)->e1)
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
	return true;
}


int main(int argc, char *argv[])
{

	// Take graph as input, numbering from 1
	vector<node> edges;
	//Number of layers
	int num_layers;
	cin>>num_layers;

	printf("NUMBER OF LAYERS DONE\n");
	// Number of input nodes
	int inp;
	cin>>inp;
	int num = 0;
	for (int i = 0; i < inp; ++i)
	{
		node n;
		n.node_type = "INPUT";
		n.layer = 1;
		n.num = num++;
		edges.push_back(n);
	}
	printf("INPUT NODES DONE\n");
	// Number of other nodes excluding output nodes
	int remaining;
	cin>>remaining;

	for (int i = 0; i < remaining; ++i)
	{
		node n;
		n.node_type = "MID";
		cin>>n.layer>>n.type;
		if(n.type == "OR")
		{
			cin>>n.input1>>n.input2;
		}
		else if (n.type == "NOT")
		{
			cin>>n.input1;
		}
		else
		{
			cout<<"ERROR"<<endl;
			exit(0);
		}
		n.num = num++;
		edges.push_back(n);
	}
	printf("MIDDLE NODES DONE\n");

	// Number of output nodes
	int out;
	cin>>out;
	for (int i = 0; i < out; ++i)
	{
		node n;
		n.node_type = "OUTPUT";
		cin>>n.layer>>n.type;
		if(n.type == "OR")
		{
			cin>>n.input1>>n.input2;
		}
		else if (n.type == "NOT")
		{
			cin>>n.input1;
		}
		n.num = num++;
		edges.push_back(n);
	}

	printf("INPUT TAKEN\n");




	int sockfd, newsockfd, portno;
	socklen_t clilen;
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


	// Read data from here

	////////////////////////////////////////////////////////////////////
	// Code to read in the public key and the generator
	////////////////////////////////////////////////////////////////////
	string g_pk;
	n = read_from_socket(newsockfd, g_pk);
	g_pk = g_pk.substr(0, g_pk.length()-1);
	stringstream stream_pk(g_pk);
	int g;
	stream_pk >> g;
	int pk;
	stream_pk >> pk;
	int p;
	stream_pk >> p;
	cout<<"Generator : " << g<<" public Key : "<<pk<<" Prime : "<<p << endl;

	vector<ciphertext> received(inp);
	string s = "";
	n = read_from_socket(newsockfd, s);
	if (n < 0)
		error("ERROR reading from socket");
	s = s.substr(0, s.length()-1);
	stringstream stream_ciphers(s);
	for (int i = 0; i < inp; ++i)
	{
		stream_ciphers >> received[i].X1;
		stream_ciphers >> received[i].Y1;
		cout<<received[i].X1 << " " << received[i].Y1<<endl; 
		stream_ciphers >> received[i].X2;
		stream_ciphers >> received[i].Y2;
		cout<<received[i].X2 << " " << received[i].Y2<<endl; 
	}



	// string str;
	// n = read_from_socket(newsockfd,str);

	// if (n < 0) error("ERROR reading from socket");
	// str = str.substr(0, str.length()-1);
	// stringstream stream(str);
	// vector<int> input_values;
	// while(1) {
	// 	int n;
	// 	stream >> n;
	// 	if(!stream)
	// 		break;
	// 	input_values.push_back(n);
	// }
	// for (int i = 0; i < edges.size(); ++i)
	// {
	// 	if (edges[i].node_type == "INPUT")
	// 	{
	// 		edges[i].value = input_values[i];
	// 	}
	// 	else if (edges[i].node_type == "MID"  || edges[i].node_type == "OUTPUT")
	// 	{
	// 		if (edges[i].type == "OR")
	// 		{
	// 			edges[i].value = (edges[edges[i].input1].value | edges[edges[i].input2].value);
	// 		}
	// 		else if (edges[i].type == "NOT")
	// 		{
	// 			edges[i].value = 1 - edges[edges[i].input1].value;
	// 		}
	// 	}
	// }
	// for (int i = edges.size()-1; i >= 0; i--)
	// {
	// 	if(edges[i].node_type == "OUTPUT")
	// 	{
	// 		cout<<edges[i].value<<endl;
	// 	}
	// }
	// cout<<"Here is the value:" + str + "\n"<<endl;
	n = write_to_socket(newsockfd,"I got your message!",18);
	if (n < 0) error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	return 0; 
}