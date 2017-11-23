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
using namespace std;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

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

bool write_to_socket(int sockfd,const char* msg,int len){
	int n,i=0;
	do{ n = write(sockfd,msg+i,min(len-i,512));
		i+=n;
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
	if (n < 0)      return false;
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

// Power function
int power(int x, int y, int p)
{
    int res = 1;      // Initialize result
 
    x = x % p;  // Update x if it is more than or 
                // equal to p
    while (y > 0)
    {
        // If y is odd, multiply x with result
        if (y & 1)
            res = (res*x) % p;
        // y must be even now
        y = y>>1; // y = y/2
        x = (x*x) % p;  
    }
    return res;
}

// Encryption function
pair<int, int> encrypt(int plaintext, int p, int pk)
{
	int mod = (p - 1) / 2;
	int r = rand()%mod;
	cout<<"Randomness: " <<r<<endl;
	// Generator
	int g = 4;
	pair<int, int> res;
	res.first = power(g, r, p);
	res.second = (plaintext * power(pk, r, p)) % p;
	return res;
}
int inverse(int n, int p)
{
	int mod = (p - 1)/2;
	return power(n, mod-1, p);
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
	public_key += to_string(g) + " " + to_string(pk) + " " + to_string(p) + "!";
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
		cout<<"Encrypt : "<<val.first<<" "<<val.second<<endl;
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
	string s;
	n = read_from_socket(sockfd,s);
	if (n < 0) 
		error("ERROR reading from socket");
	cout<<s<<endl;
	close(sockfd);
	return 0;
}