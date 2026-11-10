#include "testUDP.h"


void messageUDP(char* hote, char* service, unsigned char* message, int taille) {
    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /* Creation de l'adresse de socket */
    memset(&precisions, 0, sizeof precisions);
    // precisions.ai_family = AF_UNSPEC;
    precisions.ai_family = AF_INET;
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



int main(int argc, char* argv[]) {
    char* hote = IP_LOOPBACK;
    char* service = PORT_SERVEUR;
    char message[MAX_UDP_MESSAGE];
    char chemin[MAX_UDP_MESSAGE-LONGUEUR_ENTETE];
    if (argc < 2) {
        printf("commandes disponibles : \n"
                "   ls <chemin> \n"
                "   read <chemin/fichier> \n"
                "   write <chemin/fichier> <ligne 1> ... <ligne n> \n");

    } else if (strcmp(argv[1], "ls") == 0) {
        if (argc > 2) {
            strcpy(chemin, argv[2]);
        } else {
            strcpy(chemin, "");
        }
        message[OPERATION] = REQUETE_LS;
        message[LONGUEUR] = strlen(chemin);
        message[NUMERO] = 0x00;
        for (int i=0 ; i<(int)message[LONGUEUR] ; i++) {
            message[i+LONGUEUR_ENTETE] = chemin[i];
        }
        messageUDP(hote, service, (unsigned char *)message, (int)message[LONGUEUR]+LONGUEUR_ENTETE);

    } else if (argc > 2 && strcmp(argv[1], "read") == 0) {
        strcpy(chemin, argv[2]);
        message[OPERATION] = REQUETE_READ;
        message[LONGUEUR] = strlen(chemin);
        message[NUMERO] = 0x00;
        for (int i=0 ; i<(int)message[LONGUEUR] ; i++) {
            message[i+LONGUEUR_ENTETE] = chemin[i];
        }
        messageUDP(hote, service, (unsigned char *)message, (int)message[LONGUEUR]+LONGUEUR_ENTETE);

    } else if (strcmp(argv[1], "write") == 0) {
        strcpy(chemin, argv[2]);
        message[OPERATION] = REQUETE_WRITE;
        message[LONGUEUR] = strlen(chemin);
        message[NUMERO] = 0x00;
        for (int i=0 ; i<(int)message[LONGUEUR] ; i++) {
            message[i+LONGUEUR_ENTETE] = chemin[i];
        }
        messageUDP(hote, service, (unsigned char *)message, (int)message[LONGUEUR]+LONGUEUR_ENTETE);

        char ligne[MAX_UDP_MESSAGE-LONGUEUR_ENTETE];
        for (int i=3 ; i<argc ; i++) {
            strcpy(ligne, argv[i]);
            message[OPERATION] = DONNEES_WRITE;
            message[LONGUEUR] = strlen(ligne);
            message[NUMERO] = 0x01 + (i-3);
            for (int i=0 ; i<(int)message[LONGUEUR] ; i++) {
                message[i+LONGUEUR_ENTETE] = ligne[i];
            }
            messageUDP(hote, service, (unsigned char *)message, (int)message[LONGUEUR]+LONGUEUR_ENTETE);
        }

        message[OPERATION] = DONNEES_WRITE;
        message[LONGUEUR] = 0x00;
        message[NUMERO] = 0x00;
        messageUDP(hote, service, (unsigned char *)message, (int)message[LONGUEUR]+LONGUEUR_ENTETE);

    } else {
        printf("commande invalide\n");
        printf("commandes disponibles : \n"
                "   ls <chemin> \n"
                "   read <chemin/fichier> \n"
                "   write <chemin/fichier> <ligne 1> ... <ligne n> \n");
    }
    
    return 0;
}