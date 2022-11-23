from utils import CStruct
from global_consts import *
from server_exceptions import NoSuchCode, InvalidSocket

import struct
import logging

from asyncio.streams import StreamReader


class Request:
    def __init__(self, reader: StreamReader):
        self.reader = reader

    async def fetch(self):
        header_cs = CStruct("RequestHeader", REQUEST_HEADER_FMT)
        received_bytes = await self.reader.readexactly(header_cs.struct.size)

        if len(received_bytes) != header_cs.struct.size:
            raise InvalidSocket()

        header = header_cs.object_from_bytes(received_bytes)

        if header.code not in REQUEST_PAYLOAD:
            raise NoSuchCode()

        if header.payload_size and header.code not in REQUEST_PAYLOAD:
            raise InvalidSocket()

        elif header.payload_size and header.code in REQUEST_PAYLOAD:
            payload_cs = CStruct("RequestPayload", REQUEST_PAYLOAD[header.code])
            received_bytes = await self.reader.readexactly(payload_cs.struct.size)

            if len(received_bytes) != payload_cs.struct.size:
                raise InvalidSocket()

            payload = payload_cs.object_from_bytes(received_bytes)
            return header, payload

        return header, None
