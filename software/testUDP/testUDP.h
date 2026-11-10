#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define IP_CARTE            "10.0.0.2"
#define PORT                "53"
#define TAILLE              256
#define MAX_UDP_MESSAGE     256


int initialisationSocketUDP(char* service);
void messageUDP(char* hote, char* service, unsigned char* message, int taille);
int boucleServeurUDP(int s, int (*traitement)(unsigned char* , int/*, struct sockaddr* , int*/));
int votreFonctionUDP (unsigned char* , int/*, struct sockaddr* , int*/);
