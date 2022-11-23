from db_manager import DBManager
from clients import Clients
from files import Files
from requests import Request
from responses import RESPONSES, Response
from server_exceptions import *
from global_consts import REQUEST_REGSTRATION_CODE, AES_KEY_SIZE
from base64 import b64encode as b

import asyncio
import logging
import os
from sqlite3 import Error as DBError


class Server:
    def __init__(self) -> None:
        self.db_connector = DBManager()
        self.clients = Clients(self.db_connector)
        self.files = Files(self.db_connector)
        self.aes_key = os.urandom(AES_KEY_SIZE)

    def __del__(self):
        self.db_connector.close()

    async def handle_client(self, reader, writer):
        try:
            address = writer.get_extra_info('peername')
            logging.info(f"Client connected from {address}")

        except (asyncio.streams.IncompleteReadError, ConnectionResetError):
            logging.info(f"Client {address} has disconnected")

        try:
            request_headers = Request(reader)
            header, payload = await request_headers.fetch()
            response = Response(self, header.client_id, reader, writer)

            # Update user's last seen
            if header.code != REQUEST_REGSTRATION_CODE:
                client = self.clients.update_client_last_seen(header.client_id)
                logging.debug(f"Got request {header.code} from {client.client_name}")

            response_func = RESPONSES[header.code]
            await response_func(response, payload)

            logging.info(f"Bye to {address}")

        except (NoSuchUser, NoSuchCode, InvalidSocket, NoSuchFile, DBError) as e:
            await response.error_response(str(e))


