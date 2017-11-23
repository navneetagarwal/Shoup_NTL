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
	string type = "Enc";
	int k;
};

struct Add
{
	Enc * e1;
	Enc * e2;
	string type  ="Add";
};

ciphertext * negation_cipher(ciphertext * c)
{
	// ciphertext n;
	// n.X1 = c.X2;
	// n.Y1 = c.Y2;
	// n.X2 = c.X1;
	// n.Y2 = c.Y1;
	// return n;
	ciphertext * n = new ciphertext();
	n->X1 = c->X2;
	n->Y1 = c->Y2;
	n->X2 = c->X1;
	n->Y2 = c->Y1;
	return n;
}

Enc * negation(Enc * e)
{
	int k = e->k;
	if (k == 0)
	{
		// a1 = NULL, a2 = NULL, a3 = NULL, a4 = NULL
		Enc * n = new Enc();
		n->a1 = NULL;
		n->a2 = NULL;
		n->a3 = NULL;
		n->a4 = NULL;
		n->c = negation_cipher(e->c);
		return n;
	}
	Enc * a1_1 = negation((e->a1)->e1);
	Enc * a2_1 = negation((e->a2)->e1);	
	Enc * a3_1 = negation((e->a3)->e1);	
	Enc * a4_1 = negation((e->a4)->e1);	

	Enc * n = new Enc();
	Add * a = new Add();
	a->e1 = a1_1;
	a->e2 = (e->a1)->e2;
	n->a1 = a;

	Add * b = new Add();
	b->e1 = a2_1;
	b->e2 = (e->a2)->e2;
	n->a2 = b;

	Add * c = new Add();
	c->e1 = a3_1;
	c->e2 = (e->a3)->e2;
	n->a3 = c;

	Add * d = new Add();
	d->e1 = a4_1;
	d->e2 = (e->a4)->e2;
	n->a4 = d;

	n->k = k;
	return n;
}

Enc * OR(Enc *, Enc *);

ciphertext * encrypt(int num)
{
	ciphertext * c = new ciphertext;
	// Add code for encryption
	return c;
}


Enc * get_enc(int level, int num)
{
	if(level == 0)
	{
		Enc * n = new Enc();
		n->a1 = NULL;
		n->a2 = NULL;
		n->a3 = NULL;
		n->a4 = NULL;
		n->c = encrypt(num);
		return n;
	}
	Enc * a = new Enc();
	Enc * b = new Enc();
	if(num == 0)
	{
		a = get_enc(level - 1, 0);
		b = get_enc(level - 1, 0);
	}
	else
	{
		a = get_enc(level - 1, 0);
		b = get_enc(level - 1, 1);	
	}

	Enc * n = new Enc();
	n = OR(a, b);
	return n;
}

Enc * OR(Enc * x, Enc * y)
{
	int k = x->k;
	Enc * n = new Enc();
	n->k = k+1;
	n->a1 = new Add();
	n->a1->e1 = x;
	n->a1->e2 = get_enc(k, 0);
	n->a2 = new Add();
	n->a2->e1 = y;
	n->a2->e2 = get_enc(k, 0);
	n->a3 = new Add();
	n->a3->e1 = x;
	n->a3->e2 = y;
	n->a4 = new Add();
	n->a4->e1 = get_enc(k, 0);
	n->a4->e2 = get_enc(k, 1);
	return n;

}

int decode_enc(Enc *);

int decrypt(ciphertext * c)
{
	int val;
	// Add code for decryption
	return val;
}

int decode_add(Add * x)
{
	return (decode_enc(x->e1) + decode_enc(x->e2)) % 2;
}

int decode_enc(Enc * x)
{
	if(x->k == 0)
	{
		return decrypt(x->c);
	}
	else
	{
		int a1 = decode_add(x->a1);
		int a2 = decode_add(x->a2);
		int a3 = decode_add(x->a3);
		int a4 = decode_add(x->a4);

		if(a1 + a2 + a3 + a4 == 3)
			return 1;
		else
			return 0;
	}
}

Enc * randomize(ciphertext * c)
{
	Enc * n;
	// Perform randomization
	return  n;
}

Enc * randomize(Enc * x)
{
	int k = x->k;
	if(k == 0)
	{
		return randomize(x->c);
	}
	Enc * n = new Enc ();
	n->k = x->k;

	Enc * b11 = randomize(x->a1->e1);
	Enc * b12 = randomize(x->a1->e2);

	Enc * b21 = randomize(x->a2->e1);
	Enc * b22 = randomize(x->a2->e2);

	Enc * b31 = randomize(x->a3->e1);
	Enc * b32 = randomize(x->a3->e2);

	Enc * b41 = randomize(x->a4->e1);
	Enc * b42 = randomize(x->a4->e2);

	int r1 = rand() % 2;
	if(r1 == 0)
	{
		n->a1 = new Add();
		n->a1->e1 = b11;
		n->a1->e2 = b12;
	}
	else
	{
		n->a1 = new Add();
		n->a1->e1 = negation(b11);
		n->a1->e2 = negation(b12);
	}

	int r2 = rand() % 2;
	if(r2 == 0)
	{
		n->a2 = new Add();
		n->a2->e1 = b21;
		n->a2->e2 = b22;
	}
	else
	{
		n->a2 = new Add();
		n->a2->e1 = negation(b21);
		n->a2->e2 = negation(b22);
	}

	int r3 = rand() % 2;
	if(r3 == 0)
	{
		n->a3 = new Add();
		n->a3->e1 = b31;
		n->a3->e2 = b32;
	}
	else
	{
		n->a3 = new Add();
		n->a3->e1 = negation(b31);
		n->a3->e2 = negation(b32);
	}

	int r4 = rand() % 2;
	if(r4 == 0)
	{
		n->a4 = new Add();
		n->a4->e1 = b41;
		n->a4->e2 = b42;
	}
	else
	{
		n->a4 = new Add();
		n->a4->e1 = negation(b41);
		n->a4->e2 = negation(b42);
	}

	// Perform random permutation here
	return n;
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