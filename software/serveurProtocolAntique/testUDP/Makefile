CC = gcc

SRC_TEST = testUDP.c
SRC_SERVEUR = serveur_carte.c
TARGET_TEST = testUDP 
TARGET_SERVEUR = serveur_carte

all: $(TARGET_TEST) $(TARGET_SERVEUR)

$(TARGET_TEST): $(SRC_TEST)
	$(CC) -Wall -Werror -Wextra -o $@ $<

$(TARGET_SERVEUR): $(SRC_SERVEUR)
	$(CC) -Wall -Werror -Wextra -o $@ $<


clean:
	rm -f $(TARGET_TEST) $(TARGET_SERVEUR)

