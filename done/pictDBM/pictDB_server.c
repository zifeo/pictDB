/**
 * @file pictDB_server.c
 * @brief pictDB Server: web server for pictDB core commands.
 *
 * Picture Database Management Server
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 7 May 2016
 */

#include "libmongoose/mongoose.h"
#include "pictDB.h"

#define PORT "8000"

#define ROUTE_LIST "/pictDB/list"

// TODO : better than global ?
static struct pictdb_file myfile;
static int s_sig_received = 0;
static struct mg_serve_http_opts s_http_server_opts;

static void handle_list(struct mg_connection *nc, struct http_message *hm)
{
    const char* rep = do_list(&myfile, JSON);
    // TODO : leak/error ?

    // TODO : get parameter ?

    // TODO : better routines ?
    mg_printf(nc, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n"
              "Content-Type: application/json\r\n\r\n%s",
              (int) strlen(rep), rep);

}

static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);
    s_sig_received = sig_num;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    struct http_message *hm = (struct http_message *) ev_data;

    switch (ev) {
    case MG_EV_HTTP_REQUEST:
        if (mg_vcmp(&hm->uri, ROUTE_LIST) == 0) {
            handle_list(nc, hm);
        } else {
            mg_serve_http(nc, hm, s_http_server_opts);
        }
        break;
    default:
        break;
    }
}

/********************************************************************//**
 * MAIN
 ********************************************************************** */
int main(int argc, char *argv[])
{

    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (strlen(argv[1]) == 0 || strlen(argv[1]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    const char *db_filename = argv[1];
    int status = do_open(db_filename, "rb", &myfile);

    if (status == 0) {
        print_header(&myfile.header);

        struct mg_mgr mgr;
        struct mg_connection *nc;

        // TODO : sigint ?
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);

        mg_mgr_init(&mgr, NULL);
        nc = mg_bind(&mgr, PORT, ev_handler);
        if (nc == NULL) {
            // TODO : mongoose which error
            return ERR_IO;
        }

        mg_set_protocol_http_websocket(nc);
        s_http_server_opts.document_root = ".";
        s_http_server_opts.enable_directory_listing = "yes";

        while (!s_sig_received) {
            mg_mgr_poll(&mgr, 1000);
        }

        mg_mgr_free(&mgr);
    }

    do_close(&myfile);
    return 0;
}
