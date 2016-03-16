/**
 * @file db_create.c
 * @brief pictDB library: do_create implementation.
 *
 * @author Mia Primorac
 * @date 2 Nov 2015
 */

#include "pictDB.h"

#include <string.h> // for strncpy

/********************************************************************//**
 * Creates the database called db_filename. Writes the header and the
 * preallocated empty metadata array to database file.
 */
/* **********************************************************************
 * TODO: ADD THE PROTOTYPE OF do_create HERE.
 * **********************************************************************
 */
{
    // Sets the DB header name
    strncpy(db_file.header.db_name, CAT_TXT,  MAX_DB_NAME);
    db_file.header.db_name[MAX_DB_NAME] = '\0';

    /* **********************************************************************
     * TODO: WRITE YOUR CODE HERE
     * **********************************************************************
     */
}
