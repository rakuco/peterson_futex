/*
 * Raphael Kubo da Costa - RA 072201
 *
 * MC514 - Lab2
 */

#ifndef __CLI_H
#define __CLI_H

#include <stdlib.h>

#define CLI_ARGC 2 /**< The number of arguments the command line must have */

/**
 * Parses the command line and returns the number of threads to be created.
 *
 * @retval 0 Invalid arguments were passed.
 * @retval N The number of threads that must be created.
 */
size_t cli_get_thread_count(int argc, char *argv[]);

/**
 * Displays a usage message.
 */
void   cli_show_usage(void);

#endif /* __CLI_H */
