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
#include "helper.hh"
using namespace std;



int main(int argc, char *argv[])
{

	// Take graph as input, numbering from 1
	vector<node> edges;
	//Number of layers
	int num_layers;
	cin>>num_layers;

	// printf("NUMBER OF LAYERS DONE\n");
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
	// printf("INPUT NODES DONE\n");
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
	// printf("MIDDLE NODES DONE\n");

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

	printf("Circuit Taken as Input\n");
	printf("--------------------------------------\n");




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
	glob.generator = g;
	int pk;
	stream_pk >> pk;
	glob.Y = pk;
	int p;
	stream_pk >> p;
	glob.p = p;

	cout<<"Received from Alice - Generator : " << g<<" public Key : "<<pk<<" Prime : "<<p << endl;

	vector<Enc*> received;
	string s = "";
	n = read_from_socket(newsockfd, s);
	if (n < 0)
		error("ERROR reading from socket");
	s = s.substr(0, s.length()-1);
	stringstream stream_ciphers(s);
	for (int i = 0; i < inp; ++i)
	{
		ciphertext * c = new ciphertext();
		stream_ciphers >> c->X1;
		stream_ciphers >> c->Y1;
		cout<<"Ciphertext for input "<<i<<" received as (" <<c->X1 << ", " << c->Y1<<"), ("; 
		stream_ciphers >> c->X2;
		stream_ciphers >> c->Y2;
		cout<<c->X2 << ", " << c->Y2<<") "<<endl; 
		Enc * a = new Enc();
		a->a1 = NULL;
		a->a2 = NULL;
		a->a3 = NULL;
		a->a4 = NULL;
		a->c = c;
		a->k = 0;
		received.push_back(a);
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
	for (int i = 0; i < edges.size(); ++i)
	{
		if (edges[i].node_type == "INPUT")
		{
			edges[i].value = received[i];
		}
		else if (edges[i].node_type == "MID"  || edges[i].node_type == "OUTPUT")
		{
			if (edges[i].type == "OR")
			{
				edges[i].value = OR(edges[edges[i].input1].value, edges[edges[i].input2].value);
			}
			else if (edges[i].type == "NOT")
			{
				edges[i].value = negation(edges[edges[i].input1].value);
			}
		}
	}
	string to_send = "";
	to_send += to_string(num_layers) + " ";
	for (int i = edges.size()-1; i >= 0; i--)
	{
		if(edges[i].node_type == "OUTPUT")
		{
			print(edges[i].value, to_send);
		}
	}
	to_send += "!";
	// cout<<"Here is the value:" + str + "\n"<<endl;
	n = write_to_socket(newsockfd,to_send.c_str(),to_send.length());
	if (n < 0) error("ERROR writing to socket");
	close(newsockfd);
	close(sockfd);
	return 0; 
}