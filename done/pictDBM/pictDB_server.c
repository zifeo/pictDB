/**
 * @file pictDB_server.c
 * @brief pictDB Server: web server for pictDB core commands.
 *
 * Picture Database Management Server
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 7 May 2016
 */

#include <vips/vips.h>
#include "libmongoose/mongoose.h"
#include "pictDB.h"

#define ROUTE_LIST "/pictDB/list"
#define ROUTE_READ "/pictDB/read"
#define ROUTE_INSERT "/pictDB/insert"
#define ROUTE_DELETE "/pictDB/delete"

#define PORT "8000"
#define MAX_QUERY_PARAM 5

#define ARG_DELIM "&="
#define ARG_RES "res"
#define ARG_PICT_ID "pict_id"
#define ARGNAME_MAX 16

#define MAX_SPLIT_LEN (MAX_PIC_ID + 1) * MAX_QUERY_PARAM

static int s_sig_received = 0;
static const struct mg_serve_http_opts s_http_server_opts = {
    .document_root = "."
};

/********************************************************************//**
 * Splits query_string in parts.
 ********************************************************************** */
static void split(char* result[], char* tmp, const char* src, const char* delim, size_t len)
{

    if (result == NULL || tmp == NULL || src == NULL || delim == NULL) {
        return;
    }

    strncpy(tmp, src, len > MAX_SPLIT_LEN ? MAX_SPLIT_LEN : len);
    tmp[MAX_SPLIT_LEN - 1] = '\0';

    size_t param_id = 0;
    result[param_id] = strtok(tmp, delim);

    while (result[param_id] != NULL && param_id + 1 < MAX_QUERY_PARAM) {
        ++param_id;
        result[param_id] = strtok(NULL, delim);
    }

    result[param_id] = NULL;
}

/********************************************************************//**
 * Handles error with corresponding content.
 ********************************************************************** */
static void mg_error(struct mg_connection* nc, int error)
{
    if (nc == NULL || error < 0 || error >= ERROR_COUNT) {
        return;
    }

    // TODO : content type ?
    mg_printf(nc, "HTTP/1.1 500 %s\r\n"
              "Content-Length: 0\r\n\r\n",
              ERROR_MESSAGES[error]);
    nc->flags |= MG_F_SEND_AND_CLOSE;

}

/********************************************************************//**
 * Handles list route.
 ********************************************************************** */
static void handle_list_call(struct mg_connection *nc, struct http_message *hm)
{
    char* resp = do_list(nc->mgr->user_data, JSON);

    mg_printf(nc, "HTTP/1.1 200 OK\r\n"
              "Content-Length: %d\r\n"
              "Content-Type: application/json\r\n\r\n%s",
              (int) strlen(resp), resp);
    nc->flags |= MG_F_SEND_AND_CLOSE;

    free(resp);
    resp = NULL;
}

/********************************************************************//**
 * Handles read route.
 ********************************************************************** */
static void handle_read_call(struct mg_connection *nc, struct http_message *hm)
{


    char* params[MAX_QUERY_PARAM] = {};
    char tmp[MAX_SPLIT_LEN] = "";

    split(params, tmp, hm->query_string.p, ARG_DELIM, hm->query_string.len);

    char *pict_id = NULL;
    int resolution_parsed = -1;

    for (size_t i = 0; i + 1 < MAX_QUERY_PARAM; i += 2) {

        if (params[i] != NULL && params[i + 1] != NULL) {

            if (!strncmp(params[i], ARG_RES, ARGNAME_MAX)) {
                resolution_parsed = resolution_atoi(params[i + 1]);
            } else if (!strncmp(params[i], ARG_PICT_ID, ARGNAME_MAX)) {
                pict_id = params[i + 1];
            }
        }
    }

    if (resolution_parsed == -1 || pict_id == NULL) {
        mg_error(nc, ERR_INVALID_ARGUMENT);
        return;
    }

    uint32_t resolution = (uint32_t) resolution_parsed;
    char *image_buffer = NULL;
    uint32_t image_size = 0;

    int status = do_read(pict_id, resolution, &image_buffer, &image_size, nc->mgr->user_data);

    if (status != 0) {
        mg_error(nc, status);
        return;
    }

    assert(image_buffer != NULL);

    // TODO : content type/length order
    mg_send(nc, image_buffer, image_size);
    mg_printf(nc, "HTTP/1.1 200 OK\r\n"
              "Content-Length: %d\r\n"
              "Content-Type: image/jpeg",
              image_size);
    nc->flags |= MG_F_SEND_AND_CLOSE;

    free(image_buffer);
    image_buffer = NULL;

}

/********************************************************************//**
 * Handles insert route.
 ********************************************************************** */
static void handle_insert_call(struct mg_connection *nc, struct http_message *hm)
{
    char varname[FILENAME_MAX];
    char filename[FILENAME_MAX];
    const char *data;
    size_t data_len = 0;

    if (mg_parse_multipart(hm->body.p, hm->body.len, varname, sizeof(varname), filename, sizeof(filename), &data,
                           &data_len) <= 0) {

        mg_error(nc, ERR_INVALID_ARGUMENT);
        return;
    }

    int status = do_insert(data, data_len, filename, nc->mgr->user_data);
    if (status != 0) {
        mg_error(nc, status);
        return;
    }

    mg_printf(nc, "HTTP/1.1 302 Found\r\n"
              "Location: http://localhost:%s/index.html",
              PORT);
    nc->flags |= MG_F_SEND_AND_CLOSE;

}

/********************************************************************//**
 * Handles delete route.
 ********************************************************************** */
static void handle_delete_call(struct mg_connection *nc, struct http_message *hm)
{

    char* params[MAX_QUERY_PARAM];
    char tmp[(MAX_PIC_ID + 1) * MAX_QUERY_PARAM] = "";

    split(params, tmp, hm->query_string.p, ARG_DELIM, hm->query_string.len);

    char *pict_id = NULL;

    for (size_t i = 0; i + 1 < MAX_QUERY_PARAM; i += 2) {
        if (params[i] != NULL && params[i + 1] != NULL && !strncmp(params[i], ARG_PICT_ID, ARGNAME_MAX)) {
            pict_id = params[i + 1];
        }
    }

    if (pict_id == NULL) {
        mg_error(nc, ERR_INVALID_ARGUMENT);
        return;
    }

    int status = do_delete(pict_id, nc->mgr->user_data);
    if (status != 0) {
        mg_error(nc, status);
        return;
    }

    mg_printf(nc, "HTTP/1.1 302 Found\r\n"
              "Location: http://localhost:%s/index.html",
              PORT);
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
            handle_list_call(nc, hm);
        } else if (mg_vcmp(&hm->uri, ROUTE_READ) == 0) {
            handle_read_call(nc, hm);
        } else if (mg_vcmp(&hm->uri, ROUTE_INSERT) == 0) {
            // TODO : post route ?
            // strcmp(request_info->request_method, "GET")
            handle_insert_call(nc, hm);
        } else if (mg_vcmp(&hm->uri, ROUTE_DELETE) == 0) {
            handle_delete_call(nc, hm);
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
    if (VIPS_INIT(argv[0])) {
        vips_error_exit("unable to start VIPS");
    }

    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_VALID_FILENAME(argv[1]);

    const char *db_filename = argv[1];
    struct pictdb_file myfile;
    int status = do_open(db_filename, "rb+", &myfile);

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

        while (!s_sig_received) {
            mg_mgr_poll(&mgr, 1000);
        }

        mg_mgr_free(&mgr);
    }

    do_close(&myfile);
    vips_shutdown();

    return 0;
}
