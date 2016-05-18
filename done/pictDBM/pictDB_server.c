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
#define MAX_QUERY_PARAM 5

static int s_sig_received = 0;
static const struct mg_serve_http_opts s_http_server_opts = {
        .document_root = ".",
        .enable_directory_listing = "yes"
};

/********************************************************************//**
 * Splits query_string in parts.
 ********************************************************************** */
static void split(char* result[], char* tmp, const char* src, const char* delim, size_t len) {

    if (result == NULL || tmp == NULL || src == NULL || delim == NULL) {
        return;
    }

    strncpy(tmp, src, len);
    size_t param_id = 0;
    result[param_id] = strtok(tmp, delim);

    while (result[param_id] != NULL && param_id < MAX_QUERY_PARAM) {
        result[param_id] = strtok(NULL, delim);
        ++param_id;
    }

    result[param_id] = NULL;
}

/********************************************************************//**
 * Handles list route.
 ********************************************************************** */
static void handle_list(struct mg_connection *nc, struct http_message *hm)
{
    const char* resp = do_list(nc->mgr->user_data, JSON);
    // TODO : leak

    mg_printf(nc, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n"
                      "Content-Type: application/json\r\n\r\n%s",
              (int) strlen(resp), resp);
    nc->flags |= MG_F_SEND_AND_CLOSE;

}

/********************************************************************//**
 * Handles termination signals by storing stopping state.
 ********************************************************************** */
static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);
    s_sig_received = sig_num;
}

/********************************************************************//**
 * Handles events received by the server.
 ********************************************************************** */
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

    M_REQUIRE_VALID_FILENAME(argv[1]);

    const char *db_filename = argv[1];
    struct pictdb_file myfile;
    int status = do_open(db_filename, "rb", &myfile);

    if (status == 0) {
        print_header(&myfile.header);

        struct mg_mgr mgr;
        struct mg_connection *nc;

        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);

        mg_mgr_init(&mgr, &myfile);
        nc = mg_bind(&mgr, PORT, ev_handler);
        if (nc == NULL) {
            return -1;
        }

        mg_set_protocol_http_websocket(nc);

        // TODO : why not https://github.com/cesanta/mongoose/blob/76364af243530f3ca52cac78c869a66d58f20ace/docs/c-api/http.h/mg_register_http_endpoint.md ?
        while (!s_sig_received) {
            mg_mgr_poll(&mgr, 1000);
        }

        mg_mgr_free(&mgr);
    }

    do_close(&myfile);
    return 0;
}
