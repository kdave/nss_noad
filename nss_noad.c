#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <nss.h>
#include <stdio.h>
#include <stdlib.h>

#define NOAD_FILE "/tmp/noad.txt"
#define NAMEBUF 4096

#ifndef DEBUG
#define DEBUG 0
#endif
#define LOGFILE "/tmp/noad.log"

#define HERE	\
	do {	\
	if (logfile) {		\
		fprintf(logfile, "HERE: line %d\n", __LINE__);		\
		fflush(logfile);		\
	} } while(0)

enum nss_status _nss_noad_gethostbyname2_r(const char *name, int af, struct
		hostent *result, char *buffer, size_t buflen, int *errnop, int
		*h_errnop)
{
	enum nss_status status = NSS_STATUS_UNAVAIL;
	FILE *db;
	FILE *logfile = NULL;
	char buf[NAMEBUF];
	int line = 1;
	int found = 0;

	if (af == AF_UNSPEC)
		af = AF_INET;

	if (af != AF_INET) {
		*errnop = EINVAL;
		*h_errnop = NO_RECOVERY;
		goto finish;
	}

	if (DEBUG) {
		logfile = fopen(LOGFILE, "a+");
	}

	if (logfile) {
		fprintf(logfile, "noad_ghbn2: %s\n", name);
		fflush(logfile);
	}

	db = fopen(NOAD_FILE, "r");
	if (!db) {
		*errnop = EINVAL;
		*h_errnop = NO_RECOVERY;
		goto finish;
	}
	if (logfile) {
		fprintf(logfile, "open db\n");
		fflush(logfile);
	}
	while (fgets(buf, sizeof(buf), db)) {
		int len = strlen(buf);

		if (!*buf)
			continue;

		if (buf[len - 1] == '\n')
			buf[len - 1] = 0;
		if (logfile) {
			fprintf(logfile, "try: %s\n", buf);
			fflush(logfile);
		}
		if (strncmp(buf, name, sizeof(buf)) == 0) {
			if (logfile)
				fprintf(logfile, "hit, line %d\n", line);
			found = 1;
			break;
		}
		line++;
	}
	if (logfile) {
		fprintf(logfile, "close db\n");
		fflush(logfile);
	}
	fclose(db);

	if (found) {
		in_addr_t localhost = htonl(INADDR_LOOPBACK);

		result->h_name = name;
		result->h_aliases[0] = name;
		result->h_addrtype = af;
		result->h_length = sizeof(struct in_addr);
		localhost = htonl(INADDR_LOOPBACK);
		memcpy(&result->h_addr_list[0], &localhost, sizeof(localhost));
		result->h_addr_list[1] = NULL;
		if (logfile) {
			fprintf(logfile, "addr: %u\n", localhost);
			fflush(logfile);
			fprintf(logfile, "addr: %u\n", (unsigned)(result->h_addr_list[0]));
			fflush(logfile);
		}

		status = NSS_STATUS_SUCCESS;
	}

finish:
	if (logfile)
		fclose(logfile);
	return status;
}

enum nss_status _nss_noad_gethostbyname_r(const char *name, struct hostent
					  *result, char *buffer,
					  size_t buflen, int *errnop, int
					  *h_errnop)
{
	return _nss_noad_gethostbyname2_r(name, AF_UNSPEC, result, buffer,
					  buflen, errnop, h_errnop);
}

enum nss_status _nss_noad_gethostbyaddr_r(const void *addr, int len,
					  int af, struct hostent *result,
					  char *buffer, size_t buflen, int
					  *errnop, int *h_errnop)
{
	return NSS_STATUS_UNAVAIL;
}
