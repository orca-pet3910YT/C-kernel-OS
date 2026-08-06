#ifndef ERROR_H
#define ERROR_H
#define ESUCCESS 0 // Ok
#define EGENERIC 1 // Generic error
#define ENOFILE 2 // No such file or directory
#define EUNAVAIL 3 // Resource unavailable
#define ETOOBIG 4 // Returned data too large
#define ENOFND 5 // No such resource
#define EEOF 6 // Early EOF
#define EBADARG 7 // Bad argument
#define ENOSUP 8 // Not supported
#define EBAD 9 // Attempt to access a bad resource
__attribute__((unused)) static char *get_error(int code) {
	if (code < 0) code = -code;
	switch (code) {
		case 0:
			return "Ok";
		case EGENERIC:
			return "Generic error";
		case ENOFILE:
			return "No such file or directory";
		case EUNAVAIL:
			return "Resource unavailable";
		case ETOOBIG:
			return "Returned data too large";
		case ENOFND:
			return "No such resource";
		case EEOF:
			return "Early EOF";
		case EBADARG:
			return "Bad argument";
		case ENOSUP:
			return "Not supported";
		case EBAD:
			return "Attempt to access a bad resource";
		default:
			return "Unknown error";
	}
}
#endif
