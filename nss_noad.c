#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <nss.h>
#include <stdio.h>
#include <stdlib.h>

#define NOAD_ALLOW_FILE "/tmp/noad.txt"

#ifndef DEBUG
#define DEBUG 0
#endif
#define LOGFILE "/tmp/noad.log"

enum nss_status _nss_noad_gethostbyname2_r(const char *name, int af, struct
		hostent *result, char *buffer, size_t buflen, int *errnop, int
		*h_errnop)
{
    enum nss_status status = NSS_STATUS_UNAVAIL;

    if (af == AF_UNSPEC)
        af = AF_INET;

    if (af != AF_INET) {
        *errnop = EINVAL;
        *h_errnop = NO_RECOVERY;
        goto finish;
    }

    if (DEBUG) {
	    FILE *logfile;

	    logfile = fopen(LOGFILE, "a+");
	    if (logfile) {
		    fprintf(logfile, "noad_ghbn2: %s\n", name);
		    fflush(logfile);
		    fclose(logfile);
	    }
    }

    *((char**) buffer) = 0;
    result->h_name = buffer;
    result->h_aliases = (char**)buffer;
    result->h_aliases[1] = NULL;
    result->h_addrtype = af;
    result->h_length = sizeof(struct in_addr);
    result->h_addr_list = (char**)(buffer);

    status = NSS_STATUS_SUCCESS;
finish:

    return status;
}

enum nss_status _nss_noad_gethostbyname_r(const char *name, struct hostent
		*result, char *buffer, size_t buflen, int *errnop, int
		*h_errnop)
{
    return _nss_noad_gethostbyname2_r(name, AF_UNSPEC, result, buffer, buflen,
		    errnop, h_errnop);
}

enum nss_status _nss_noad_gethostbyaddr_r( const void* addr, int len, int af,
		struct hostent *result, char *buffer, size_t buflen, int
		*errnop, int *h_errnop)
{
	return NSS_STATUS_UNAVAIL;
}
