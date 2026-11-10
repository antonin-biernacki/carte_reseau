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
#include <dirent.h>

#define IP_CARTE            "10.0.0.2"
#define IP_LOOPBACK         "127.0.0.1"
#define PORT_CARTE          "5000"
#define PORT_SERVEUR        "4242"
#define MAX_UDP_MESSAGE     256


#define LONGUEUR_ENTETE     3
#define OPERATION           0
#define LONGUEUR            1
#define NUMERO              2

#define REQUETE_LS          0x01
#define REPONSE_LS          0x11

#define REQUETE_READ        0x02
#define REPONSE_READ        0x12

#define REQUETE_WRITE       0x03
#define DONNEES_WRITE       0x13
#define REPONSE_WRITE       0x23


void messageUDP(char* hote, char* service, unsigned char* message, int taille);
