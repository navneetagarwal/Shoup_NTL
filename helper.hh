#include <bits/stdc++.h>

using namespace std;
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

struct public_key
{
	int generator;
	int Y;
	int p;
	int sk;
} glob;


struct ciphertext
{
	int X1;
	int Y1;
	int X2;
	int Y2;
};

struct Add;
Add* copy(Add* x);

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
struct node
{
	string node_type; //  INPUT or OUTPUT or MID
	string type; // OR or NOT
	Enc * value;
	int layer;
	int input1;
	int input2;
	int num;
};

Enc* copy(Enc* x)
{
	Enc* res = new Enc;
	res->k = x->k;
	if(x->k == 0)
	{
		res->a1=NULL;
		res->a2=NULL;
		res->a3=NULL;
		res->a4=NULL;
		ciphertext * new_c = new ciphertext;
		new_c->X1 = (x->c)->X1;
		new_c->Y1 = (x->c)->Y1;
		new_c->X2 = (x->c)->X2;
		new_c->Y2 = (x->c)->Y2;
		res->c = new_c;
		return res;
	}
	else{
		res->a1 = copy(x->a1);
		res->a2 = copy(x->a2);
		res->a3 = copy(x->a3);
		res->a4 = copy(x->a4);
		res->c = NULL;
		return res;
	}
}

struct Add
{
	Enc * e1;
	Enc * e2;
	string type  ="Add";
};

Add* copy(Add* x)
{
	Add* res = new Add;
	res->e1 = copy(x->e1);
	res->e2 = copy(x->e2);
	return res;
}

// Performs negation of ciphertext

ciphertext * negation_cipher(ciphertext * c)
{
	ciphertext * n = new ciphertext();
	n->X1 = c->X2;
	n->Y1 = c->Y2;
	n->X2 = c->X1;
	n->Y2 = c->Y1;
	return n;
}

// Performs negation of Enc 

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

	Enc * n = copy(e);
	n->a1->e1 = a1_1;
	n->a2->e1 = a2_1;
	n->a3->e1 = a3_1;
	n->a4->e1 = a4_1;

	return n;
}


Enc * OR(Enc *, Enc *);


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

ciphertext * encrypt(int num)
{
	ciphertext * c = new ciphertext();
	/////////////////////////////////////////////////////////////
	// Add code for encryption
	// TODO
	if(num == 0)
		num = 1;
	else
		num = 4;
	int mod = (glob.p - 1) / 2;
	int r = rand()%mod;
	c->X1 = power(glob.generator, r, glob.p);
	c->Y1 = (num * power(glob.Y, r, glob.p)) % glob.p;

	if(num == 1)
		num = 4;
	else
		num = 1;
	r = rand() % mod;
	c->X2 = power(glob.generator, r, glob.p);
	c->Y2 = (num * power(glob.Y, r, glob.p)) % glob.p;
	/////////////////////////////////////////////////////////////

	return c;
}


// Get encryption of constants

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
		n->k = 0;
		return n;
	}
	Enc *a,*b;
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

	Enc * n = OR(a, b);
	n->k = level;
	return n;
}

// Perform OR operation as specified

Enc * OR(Enc * x, Enc * y)
{
	int k = x->k;
	Enc * n = new Enc();
	n->k = k+1;
	n->a1 = new Add();
	n->a1->e1 = copy(x);
	n->a1->e2 = get_enc(k, 0);
	n->a2 = new Add();
	n->a2->e1 = copy(y);
	n->a2->e2 = get_enc(k, 0);
	n->a3 = new Add();
	n->a3->e1 = copy(x);
	n->a3->e2 = copy(y);
	n->a4 = new Add();
	n->a4->e1 = get_enc(k, 0);
	n->a4->e2 = get_enc(k, 1);

	n->c = NULL;
	return n;
}

int decode_enc(Enc *, int , int);


int inverse(int n, int p)
{
	int mod = (p - 1)/2;
	return power(n, mod-1, p);
}

// Decrypt functions

int decrypt(ciphertext * c, int sk, int p)
{
	int val;
	/////////////////////////////////////////////////////////////
	// Add code for decryption
	// TODO
	int X = c->X1;
	X = power(X, sk, p);
	X = inverse(X, p);
	int Y = c->Y1;
	Y = (Y * X) % p;
	/////////////////////////////////////////////////////////////
	if(Y == 1)
		return 0;
	else
		return 1;
}

// Decode an add 

int decode_add(Add * x, int sk, int p)
{
	return (decode_enc(x->e1, sk, p) + decode_enc(x->e2, sk, p)) % 2;
}

// Decode an Enc
int decode_enc(Enc * x, int sk, int p)
{
	if(x->k == 0)
	{
		return decrypt(x->c, sk, p);
	}
	else
	{
		int a1 = decode_add(x->a1, sk, p);
		int a2 = decode_add(x->a2, sk, p);
		int a3 = decode_add(x->a3, sk, p);
		int a4 = decode_add(x->a4, sk, p);

		if(a1 + a2 + a3 + a4 == 3)
			return 1;
		else
			return 0;
	}
}

// Randomize El Gamal

Enc * randomize(ciphertext * c)
{
	Enc * n;
	/////////////////////////////////////////////////////////////
	// Perform Randomization
	// TODO
	ciphertext * d = new ciphertext();
	int mod = (glob.p - 1) / 2;
	int r = rand() % mod;
	d->X1 = (c->X1 * power(glob.generator, r, glob.p))%glob.p;
	d->Y1 = (c->Y1 * power(glob.Y, r, glob.p))%glob.p;

	r = rand() % mod;
	d->X2 = (c->X2 * power(glob.generator, r, glob.p))%glob.p;
	d->Y2 = (c->Y2 * power(glob.Y, r, glob.p))%glob.p;
	/////////////////////////////////////////////////////////////
	n->a1 = NULL;
	n->a2 = NULL;
	n->a3 = NULL;
	n->a4 = NULL;

	n->c = d;
	n->k = 0;
	return  n;
}

void permute(Enc* x)
{
	vector<Add*> v;
	v.push_back(x->a1);
	v.push_back(x->a2);
	v.push_back(x->a3);
	v.push_back(x->a4);
	random_shuffle(v.begin(), v.end());

	x->a1 = v[0];
	x->a2 = v[1];
	x->a3 = v[2];
	x->a4 = v[3];
}

// Randomize Enc
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
	n->c = NULL;
	/////////////////////////////////////////////////////////////
	// Perform random permutation here
	// TODO
	permute(n);
	/////////////////////////////////////////////////////////////
	return n;
}

void print(Add *, string &);

void print(Enc* x, string &prev)
{
	if(x->k==0)
	{
		ciphertext* new_c = x->c;
		prev+= " " + to_string(new_c->X1) + " " + to_string(new_c->Y1) + 
		" " + to_string(new_c->X2) + " " +  to_string(new_c->Y2) + " ";
		return;
	}
	else{
		print(x->a1,prev);
		print(x->a2,prev);
		print(x->a3,prev);
		print(x->a4,prev);
	}
}

void print(Add* x, string &prev)
{
	print(x->e1,prev);
	print(x->e2,prev);
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
	s=string(len,0);
	n=0;
	for(auto r: received) {
		for(int i=0;i<r.size();i++)
			s[i+n]=r[i];
		n+=r.size();
	}
	return true;
}


Add* get_from_string_add(vector<int> &v, int k, int start, int end);

Enc* get_from_string_enc(vector<int> &v, int k, int start, int end)
{
	if(k==0)
	{
		ciphertext* new_c = new ciphertext;
		new_c->X1 = v[start];
		new_c->Y1 = v[start + 1];
		new_c->X2 = v[start + 2];
		new_c->Y2 = v[start + 3];
		Enc* res = new Enc;
		res->a1 = NULL;
		res->a2 = NULL;
		res->a3 = NULL;
		res->a4 = NULL;
		res->k = k;
		res->c = new_c;
		return res;
	}
	else{
		Enc* res = new Enc;
		int interval = (end-start)/4;
		res->a1 = get_from_string_add(v,k,start, start + interval);
		res->a2 = get_from_string_add(v,k,start + interval, start + 2*interval);
		res->a3 = get_from_string_add(v,k,start + 2*interval, start + 3*interval);
		res->a4 = get_from_string_add(v,k,start + 3*interval, start + 4*interval);
		res->c = NULL;
		res->k = k;
		return res;
	}
}

Add* get_from_string_add(vector<int> &v, int k, int start, int end)
{
	Add* res = new Add;
	int interval = (end-start)/2;
	res->e1 = get_from_string_enc(v,k-1,start, start + interval);
	res->e2 = get_from_string_enc(v,k-1,start + interval, start + 2*interval);
	return res;
}
