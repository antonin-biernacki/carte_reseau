#include "serveur_carte.h"


int initialisationSocketUDP(char* service) {
    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions, 0, sizeof precisions);
    // precisions.ai_family = AF_UNSPEC;
    precisions.ai_family = AF_INET;
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
        int nboctets = recvfrom(s, message, MAX_UDP_MESSAGE, 0, (struct sockaddr *)&adresse, &taille);
        if(nboctets < 0) return -1;
        if(traitement(message, nboctets/*, (struct sockaddr *)&adresse, taille*/) < 0) break;
    }
    return 0;
}





int traitementMessageUDP (unsigned char* message, int nboctets/*, struct sockaddr* adresse, int taille*/) {
    uint8_t operation = message[OPERATION];
    uint8_t longueur = message[LONGUEUR];

    if ((nboctets != (longueur + LONGUEUR_ENTETE)) || (longueur > (MAX_UDP_MESSAGE - LONGUEUR_ENTETE))) {

        printf("nboctets : %d\nlongueur : %d\n", nboctets, longueur);
        printf("Erreur message mal formé ou mal transmit\n");

    } else if (operation == REPONSE_LS) {

        printf("res ls\n");

    } else if (operation == REPONSE_READ) {

        printf("res read\n");

    } else if (operation == REPONSE_WRITE) {

        printf("res write\n");

    } else {
        printf("Message non reconnu \n\n");
    }
    return 0;
}





int main(void) {
    int s = initialisationSocketUDP(PORT_CARTE);
    if(s < 0) { 
        fprintf(stderr, "Port non utilisable\n");
        exit(EXIT_FAILURE);
    }
    boucleServeurUDP(s, traitementMessageUDP);
    shutdown(s, SHUT_RDWR);
    return 0;
}