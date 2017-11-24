#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include "helper.hh"
using namespace std;


bool check_prime(int n)
{
	for (int i = 2; i <= sqrt(n) ; ++i)
	{
		/* code */
		if(n % i == 0)
			return false;
	}
	return n > 1;
}


// Encryption function
pair<int, int> encrypt(int plaintext, int p, int pk)
{
	int mod = (p - 1) / 2;
	int r = rand()%mod;
	// Generator
	int g = 4;
	pair<int, int> res;
	res.first = power(g, r, p);
	res.second = (plaintext * power(pk, r, p)) % p;
	return res;
}

int decrypt(int sk, pair<int, int> cipher, int p)
{
	int X = cipher.first;
	X = power(X, sk, p);
	X = inverse(X, p);
	int Y = cipher.second;
	Y = (Y * X) % p;
	return Y;
}



int main(int argc, char *argv[])
{
	srand(time(NULL));
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	// Initial Setups
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	
	// host resolution
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	// Socket stuff
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	
	bcopy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	
	/////////////////////////////////////////////////////////////////////////
	// Alice's job

	int p;
	printf("Enter a safe Prime as security parameter\n");
	cin>>p;
	if(!check_prime(p) || !check_prime((p-1)/2))
	{
		cout<<"Not a safe prime"<<endl;
		exit(0);
	}
	int mod = (p - 1) / 2;
	int sk = rand()%mod;
	int g = 4;
	int pk = power(g, sk, p);
	///////////////////////////////////////////////////////////////////////////
	// Send public key to Bob
	///////////////////////////////////////////////////////////////////////////
	string public_key = "";
	public_key += to_string(g) + " " + to_string(pk) + " " + to_string(p)  +  "!";
	n = write_to_socket(sockfd,public_key.c_str(),public_key.length());
	if (n < 0) 
		error("ERROR writing to socket");
	// Done Sending the generator and public key
	///////////////////////////////////////////////////////////////////////////


	cout<<"Secret Key : "<<sk<<" public Key : "<<pk<<endl;
	printf("Please enter the inputs to the circuit \n");
	string str;
	getline(cin, str);
	getline(cin, str);
	stringstream stream(str);
	vector<int> input_values;
	while(1) {
		int n;
		stream >> n;
		if(!stream)
			break;
		if(n == 0)
			input_values.push_back(1);
		else
			input_values.push_back(4);
	}
	// Print the values of the messages
	string to_send = "";
	for (int i = 0; i < input_values.size(); ++i)
	{
		pair<int, int> val = encrypt(input_values[i], p, pk);
		// cout<<"Message "<<input_values[i]<<" ";
		cout<<"Encrypt for message "<< i <<" : "<<val.first<<" "<<val.second<<endl;
		// cout<<"Decrypt : "<<decrypt(sk, val, p)<<endl;
		to_send += to_string(val.first) + " " + to_string(val.second) + " ";

		if 	(input_values[i] == 1)
			input_values[i] = 4;
		else
			input_values[i] = 1;

		val = encrypt(input_values[i], p, pk);
		to_send += to_string(val.first) + " " + to_string(val.second) + " ";
	}
	to_send += "!";
	n = write_to_socket(sockfd, to_send.c_str(), to_send.length());
	if (n < 0)
		error("ERROR writing to socket");



	//////////////////////////////////////////////////////////////////////////


	str += "!";
	n = write_to_socket(sockfd,str.c_str(),str.length());
	if (n < 0) 
		error("ERROR writing to socket");
	to_send = "";
	n = read_from_socket(sockfd,to_send);
	to_send = to_send.substr(0, to_send.length()-1);
	vector<int> v;
	stringstream stream_c(to_send);
	int num_layers;
	stream_c >> num_layers;
	int i = 0;
	while(true)
	{
		int n;
		if( i == 4 * (1 << (3*num_layers)))
			break;
		stream_c >> n;
		i++;
		v.push_back(n);
	}
	Enc * n1 = get_from_string_enc(v, num_layers, 0, 4*(1<<(3*num_layers)));
	cout<<"Final Output  = " << decode_enc(n1, sk, p)<<endl;
	if (n < 0) 
		error("ERROR reading from socket");
	close(sockfd);
	return 0;
}