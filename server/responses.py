from global_consts import *
from server_exceptions import *
from crc import crc
from base64 import b64encode, b64decode

import struct
import logging
from ctypes import create_string_buffer
from asyncio.streams import StreamReader, StreamWriter
import os

RESPONSES = {}


def request_code(code):
    def deco(inner):
        RESPONSES[code] = inner
        inner.CODE = code
        return inner

    return deco


class Response(object):
    def __init__(self, server, client_id, reader: StreamReader, writer: StreamWriter) -> None:
        self._server = server
        self.request_client_id = client_id
        self.reader = reader
        self.writer = writer

    async def write(self, payload):
        self.writer.write(payload)
        await self.writer.drain()

    async def headers_response(self, payload_size, code):
        await self.write(struct.pack(RESPONSE_HEADER_FMT,
                                     SERVER_VERSION,
                                     code,
                                     payload_size))

    async def success_response(self, payload, code: int):
        await self.headers_response(len(payload), code)
        await self.write(payload)

    async def error_response(self, error_message=None, code=SERVER_ERROR_CODE):
        logging.error(error_message)
        await self.headers_response(0, code)

    @request_code(REQUEST_REGSTRATION_CODE)
    async def handle_registration(self, request_payload):
        client_name = create_string_buffer(request_payload.name).value.decode()
        try:
            client = self._server.clients.add_client(client_name)
            payload = struct.pack(REGSTRATION_SUCCESS_FMT, client.client_id)

            await self.success_response(payload, REGSTRATION_SUCCESS_CODE)

        except UserAlreadyExists:
            await self.error_response(f"Got register request for an existing client name - '{client_name}'", REGSTRATION_FAILURE_CODE)

    @request_code(REQUEST_AES_KEY_CODE)
    async def handle_public_key(self, request_payload):
        try:
            client = self._server.clients.get_client_by_client_id(self.request_client_id)

            public_key = request_payload.public_key
            crypted_key = self._server.clients.get_aes_key(client.client_id, client.aes_key, public_key)
            payload = struct.pack(RESPONSE_AES_KEY_FMT, client.client_id, crypted_key)

            await self.success_response(payload, RESPONSE_AES_KEY_CODE)

        except NoSuchUser:
            await self.error_response(f"Got request for non existing client")


    @request_code(REQUEST_SEND_FILE_CODE)
    async def handle_send_file(self, request_payload):
        content = await self.reader.readexactly(request_payload.content_size)
        if len(content) != request_payload.content_size:
            await self.error_response(f"content size received - {len(content)} "
                                      f"is different from content_size in header - {request_payload.content_size}")
            return

        plain_content = self._server.clients.decrypt(content, self.request_client_id)
        file_crc = crc(plain_content)

        dir_path = f"clients\\{b64encode(self.request_client_id).decode('utf-8')}"
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)

        file_name = request_payload.file_name.decode('utf-8').rstrip('\x00')
        file_path = f"{dir_path}\\{file_name}"
        with open(file_path, 'w') as new_file:
            new_file.write(b64encode(content).decode("utf-8"))

        file = self._server.files.add_file(self.request_client_id,
                                           request_payload.file_name,
                                           file_path,
                                           False)
        payload = struct.pack(RESPONSE_FILE_FMT, request_payload.client_id,
                              request_payload.content_size, file.file_name, file_crc)

        print("file crc", file_crc)
        print("client id", b64encode(request_payload.client_id))
        await self.success_response(payload, RESPONSE_FILE_CODE)

    @request_code(REQUEST_CRC_VALID_CODE)
    async def handle_crc_valid(self, request_payload):
        try:
            client = self._server.clients.get_client_by_client_id(request_payload.client_id)
            self._server.files.validate_file(client.client_id, request_payload.file_name)

        except NoSuchUser:
            await self.error_response(f"Got request for non existing client")

        except NoSuchFile:
            await self.error_response(f"Got request for non existing file")

    @request_code(REQUEST_CRC_INVALID_CODE)
    async def handle_crc_invalid(self, request_payload):
        try:
            self._server.files.delete_file(request_payload.client_id, request_payload.file_name)

        except NoSuchUser:
            await self.error_response(f"Got request for non existing client")

        except NoSuchFile:
            await self.error_response(f"Got request for non existing file")

    @request_code(REQUEST_CRC_INVALID_DONE_CODE)
    async def handle_crc_invalid_done(self, request_payload):
        self.handle_crc_valid(request_payload)


