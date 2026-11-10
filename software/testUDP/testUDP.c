#include "testUDP.h"


int initialisationSocketUDP(char* service) {
    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions, 0, sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_DGRAM;
    precisions.ai_flags = AI_PASSIVE;
    statut = getaddrinfo(NULL, service, &precisions, &origine);
    if(statut < 0) { 
        perror("initialisationSocketUDP.getaddrinfo");
        exit(EXIT_FAILURE); 
    }
    struct addrinfo* p;
    for(p=origine, resultat=origine ; p!=NULL ; p=p->ai_next)
    if(p->ai_family == AF_INET6) {
        resultat = p;
        break;
    }

    /* Creation d'une socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if(s < 0) {
        perror("initialisationSocketUDP.socket");
        exit(EXIT_FAILURE);
    }

    /* Options utiles */
    int vrai = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &vrai, sizeof(vrai)) < 0) {
        perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
        exit(EXIT_FAILURE);
    }

    /* Specification de l'adresse de la socket */
    statut = bind(s, resultat->ai_addr, resultat->ai_addrlen);
    if(statut < 0) {
        freeaddrinfo(origine);
        shutdown(s, SHUT_RDWR);
        perror("initialisationServeurUDP.bind");
        return -1;
    }

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    return s;
}



int boucleServeurUDP(int s, int (*traitement)(unsigned char* , int/*, struct sockaddr* , int*/)) {
    while(1) {
        struct sockaddr_storage adresse;
        socklen_t taille = sizeof(adresse);
        unsigned char message[MAX_UDP_MESSAGE];
        printf("En attente de requêtes\n");
        int nboctets = recvfrom(s, message, MAX_UDP_MESSAGE, 0, (struct sockaddr *)&adresse, &taille);
        if(nboctets < 0) return -1;
        if(traitement(message, nboctets/*, (struct sockaddr *)&adresse, taille*/) < 0) break;
    }
    return 0;
}



int votreFonctionUDP (unsigned char* message, int nboctets/*, struct sockaddr* adresse, int taille*/) {
    printf("requête reçue en cours de traitement\n");
    uint8_t octet3 = message[3];
    if (octet3 && 0x80) {
        int i, j, l;
		i = 12;
		printf("Noms de domaine de la question : \n		");
		while(message[i] != 0x00) {
			l = message[i];
			i++;
			for(j=i ; j<(i+l+1) ; j++) {
				printf("%c", message[j]);
			}
			if (message[i+l+1] != 0x00) {
				printf(".");
			}
			i = i + l;
		}
		printf("\n");
    }
    messageUDP(IP_CARTE, PORT, message, nboctets);
    printf("requête traitée\n");
    return 0;
}



void messageUDP(char* hote, char* service, unsigned char* message, int taille) {
    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /* Creation de l'adresse de socket */
    memset(&precisions, 0, sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_DGRAM;
    statut = getaddrinfo(hote, service, &precisions, &origine);
    if(statut < 0) { 
        perror("messageUDPgenerique.getaddrinfo");
        exit(EXIT_FAILURE);
    }
    struct addrinfo* p;
    for(p=origine, resultat=origine ; p!=NULL ; p=p->ai_next)
    if(p->ai_family == AF_INET6) { 
        resultat = p;
        break;
    }

    /* Creation d'une socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if(s < 0) {
        perror("messageUDPgenerique.socket");
        exit(EXIT_FAILURE);
    }

    /* Option sur la socket */
    int vrai = 1;
    if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &vrai, sizeof(vrai)) < 0) {
        perror("initialisationServeurUDPgenerique.setsockopt (BROADCAST)");
        exit(EXIT_FAILURE);
    }

    /* Envoi du message */
    int nboctets = sendto(s, message, taille, 0, resultat->ai_addr, resultat->ai_addrlen);
    if(nboctets < 0) { 
        perror("messageUDPgenerique.sento");
        exit(EXIT_FAILURE);
    }

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    /* Fermeture de la socket d'envoi */
    shutdown(s, SHUT_RDWR);
}



int main(void) {
    int s = initialisationSocketUDP(PORT);
    if(s < 0) { 
        fprintf(stderr, "Port non utilisable\n");
        exit(EXIT_FAILURE);
    }
    boucleServeurUDP(s, votreFonctionUDP);
    shutdown(s, SHUT_RDWR);
    return 0;
}