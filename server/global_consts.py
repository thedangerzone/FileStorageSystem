# Sizes(in bytes)
CLIENT_ID_SIZE      = 16
CLIENT_NAME_SIZE    = 255
FILE_NAME_SIZE      = 255
PUBLIC_KEY_SIZE     = 160
AES_KEY_SIZE        = 128

# Request codes
REQUEST_REGSTRATION_CODE      = 1100
REQUEST_AES_KEY_CODE          = 1101
REQUEST_SEND_FILE_CODE        = 1103
REQUEST_CRC_VALID_CODE        = 1104
REQUEST_CRC_INVALID_CODE      = 1105
REQUEST_CRC_INVALID_DONE_CODE = 1106

# Response codes
REGSTRATION_SUCCESS_CODE      = 2100
REGSTRATION_FAILURE_CODE      = 2101
RESPONSE_AES_KEY_CODE         = 2102
RESPONSE_FILE_CODE            = 2103
SERVER_ERROR_CODE             = 2104

# Request formats
REQUEST_HEADER_FMT              = f"""\
                                client_id {CLIENT_ID_SIZE}s
                                version B
                                code H
                                payload_size I"""

REQUEST_REGSTRATION_FMT         = f"""\
                                name {CLIENT_NAME_SIZE}s"""

REQUEST_AES_KEY_FMT             = f"""\
                                name {CLIENT_NAME_SIZE}s
                                public_key {PUBLIC_KEY_SIZE}s"""

REQUEST_SEND_FILE_CODE_FMT      = f"""\
                                client_id {CLIENT_ID_SIZE}s
                                content_size I
                                file_name {FILE_NAME_SIZE}s"""

REQUEST_CRC_VALID_FMT           = f"""\
                                client_id {CLIENT_ID_SIZE}s
                                file_name {FILE_NAME_SIZE}s"""

REQUEST_CRC_INVALID_FMT ,  REQUEST_CRC_INVALID_DONE_FMT = REQUEST_CRC_VALID_FMT, REQUEST_CRC_VALID_FMT

# Response formats

RESPONSE_HEADER_FMT         = f"<BHI"
REGSTRATION_SUCCESS_FMT     = f"<{CLIENT_ID_SIZE}s"
RESPONSE_AES_KEY_FMT        = f"<{CLIENT_ID_SIZE}s{AES_KEY_SIZE}s"
RESPONSE_FILE_FMT           = f"<{CLIENT_ID_SIZE}sI{FILE_NAME_SIZE}sI"

# Request payload
REQUEST_PAYLOAD = {
    REQUEST_REGSTRATION_CODE      : REQUEST_REGSTRATION_FMT,
    REQUEST_AES_KEY_CODE          : REQUEST_AES_KEY_FMT,
    REQUEST_SEND_FILE_CODE        : REQUEST_SEND_FILE_CODE_FMT,
    REQUEST_CRC_VALID_CODE        : REQUEST_CRC_VALID_FMT,
    REQUEST_CRC_INVALID_CODE      : REQUEST_CRC_INVALID_FMT,
    REQUEST_CRC_INVALID_DONE_CODE : REQUEST_CRC_INVALID_DONE_FMT
}

# Server info
SERVER_VERSION  = 3
PORT_INFO_PATH  = "port.info"
DEFAULT_PORT_VALUE = 1234

DB_FILE_PATH    = "server.db"
SERVER_IP_ADDR  = "0.0.0.0"
CLIENTS_TABLE   = "clients"
FILES_TABLE  = "files"