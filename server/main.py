import os
import sys
import asyncio
import logging

from network_manager import Server
from global_consts import PORT_INFO_PATH, SERVER_IP_ADDR, DEFAULT_PORT_VALUE

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")


def get_port_from_file():
    if os.path.exists(PORT_INFO_PATH) == False: #if the port doesnt exist
        logging.warning(f"port.info file was not found in path: {PORT_INFO_PATH}")
        return DEFAULT_PORT_VALUE

    with open(PORT_INFO_PATH, "rb") as fp:
        return int(fp.read())


async def main():
    server = Server()
    server_port = get_port_from_file()

    try:
        server_loop = await asyncio.start_server(
            server.handle_client, SERVER_IP_ADDR, server_port)

    except OSError:
        logging.error(f"Cannot start server on port {server_port}")
        sys.exit(0)

    print("Starting server on:", server_loop.sockets[0].getsockname())

    async with server_loop:
        await server_loop.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
