#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 1338
#define BROAD_PORT 1339
#define MAXLINE 64
#define RESPONSE_MESSAGE "STIPRSPN"	// Spatial Tracking IP ReSPoNse
#define REQUEST_MESSAGE "STIPRQST"	// Spatial Tracking IP ReQueST
#define OK_MESSAGE "STDATAOK"		// Spatial Tracking DATA OK

int main() {
	int expectedclients;
	int datasockfd;
	int dscsockfd; // DiSCovery SOCKet File Descriptor
	char buffer[MAXLINE];
	const char* responsemessage = RESPONSE_MESSAGE;
	const char* requestmessage = REQUEST_MESSAGE;
	const char* okmessage = OK_MESSAGE;
	struct sockaddr_in dataservaddr, dscservaddr, cliaddr;

	// Data Socket fd
	if ((datasockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Data socket couldn\'t be created.");
		exit(EXIT_FAILURE);
	}

	// Discovery Socker fd
	if ((dscsockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Discovery socket couldn't be created.");
		exit(EXIT_FAILURE);
	}

	memset(&dataservaddr, 0, sizeof(dataservaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	memset(&dscservaddr, 0, sizeof(dscservaddr));

	// Server info
	dataservaddr.sin_family = AF_INET;
	dataservaddr.sin_addr.s_addr = INADDR_ANY;
	dataservaddr.sin_port = htons(PORT);
	
	// Discovery info
	dscservaddr.sin_family = AF_INET;
	dscservaddr.sin_addr.s_addr = INADDR_ANY;
	dscservaddr.sin_port = htons(BROAD_PORT);

	// Bind
	if (bind(datasockfd, (const struct sockaddr*)&dataservaddr, sizeof(dataservaddr)) < 0) {
		perror("Data bind failed.");
		exit(EXIT_FAILURE);
	}
	if (bind(dscsockfd, (const struct sockaddr*)&dscservaddr, sizeof(dscservaddr)) < 0) {
		perror("Discovert bind failed.");
		exit(EXIT_FAILURE);
	}

	// Get expected number of clients
	std::string numclientsraw;
	std::cout << "How many clients must join before the server starts?" << std::endl;
	std::getline(std::cin, numclientsraw);
	expectedclients = stoi(numclientsraw);

	socklen_t len;
	int n;

	// Listen for clients asking via broadcast for server IP
	// TODO: Make this into an update() loop that is called at a fixed frequency, poll 
	//  both datasock and dscsock each update to allow clients to connect and disconnect 
	//  at any time over dscsock. Clients broadcast over BROAD_PORT to join and leave 
	//  through direct message, still over BROAD_PORT through dscsock. Maybe rename 
	//  BROAD_PORT to DISC_PORT and PORT to DATA_PORT to better communicate this.
	bool connecting = true;
	int numconnected = 0;
	while (connecting) {
		len = sizeof(cliaddr);
		printf("Awaiting requests...\n");	
		n = recvfrom(dscsockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		buffer[n] = '\0';
		if (strcmp(buffer, requestmessage) == 0) {
			printf("\tClient requested IP: %s\n", buffer);
			sendto(dscsockfd, (const char*)responsemessage, strlen(responsemessage), MSG_CONFIRM, (const struct sockaddr*)&cliaddr, len);
			char addrstr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(cliaddr.sin_addr), addrstr, INET_ADDRSTRLEN);
			std::cout << "\tResponse message sent back to " << addrstr << std::endl;
			numconnected++;
			if (numconnected == expectedclients) {
				connecting = false;
			}
		}
	}

	while (true) {
		len = sizeof(cliaddr);
		printf("Awaiting data...\n");
		n = recvfrom(datasockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);
		buffer[n] = '\0';
		printf("\tReceived data: %s\n", buffer);
		sendto(datasockfd, (const char*)okmessage, strlen(okmessage), MSG_CONFIRM, (const struct sockaddr*) &cliaddr, len);
		printf("\tSent OK response.\n");
	}
	return 0;
}
