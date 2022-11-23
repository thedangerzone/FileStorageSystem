from global_consts import CLIENTS_TABLE
from server_exceptions import UserAlreadyExists, NoSuchUser
from cipher import AESCipher, RSACipher
from base64 import b64encode, b64decode

import datetime, uuid
import os
from dataclasses import dataclass


@dataclass
class Client:
    client_id: bytes
    client_name: str
    public_key: bytes
    aes_key: bytes
    last_seen: datetime


class Clients:
    _TABLE_NAME = "clients"
    _TABLE_PARAMS = ["ID VARCHARS(16) NOT NULL PRIMARY KEY",
                     "Name VARCHARS(255)",
                     "PublicKey VARCHARS(1280)",
                     "AesKey VARCHARS(128)",
                     "LastSeen TIMESTAMP"]

    def __init__(self, db_connector):
        self.db_connector = db_connector
        self.db_connector._create_if_not_exist(self._TABLE_NAME, self._TABLE_PARAMS) 

    def __iter__(self):
        clients = self.db_connector._cur.execute(f"SELECT ID, Name, PublicKey, AesKey, LastSeen FROM {self._TABLE_NAME};").fetchall()
        return iter([self.get_client_by_row(row) for row in clients])

    def add_client(self, client_name):
        if self.get_client_by_client_name(client_name):
            raise UserAlreadyExists
        
        client_id = uuid.uuid4().bytes
        aes_key = b64decode(b'fUoZdMWslGbBo2OeJWkzsg==') #os.urandom(16)
        public_key = b'\0' * 160 #temp value
        new_client = Client(client_id, client_name, public_key, aes_key, datetime.datetime.now())
        self.db_connector._cur.execute(f"INSERT INTO {self._TABLE_NAME} VALUES (?, ?, ?, ?, ?);", (new_client.client_id,
                                                                                new_client.client_name,
                                                                                new_client.public_key,
                                                                                new_client.aes_key, 
                                                                                new_client.last_seen))
        
        return new_client

    def get_client_by_client_name(self, client_name):
        result = self.db_connector._cur.execute(f"SELECT * from {self._TABLE_NAME} where Name = ?;",(client_name,)).fetchone()
        if result:
            return self.get_client_by_row(result)
        else:
            return None
    
    def get_client_by_client_id(self, client_id):
        result = self.db_connector._cur.execute(f"SELECT * from {self._TABLE_NAME} where ID = ?;",(client_id,)).fetchone()
        if result:
            return self.get_client_by_row(result)
        else:
            raise NoSuchUser
    
    def update_client_last_seen(self, client_id):
        client = self.get_client_by_client_id(client_id)
        current_timestamp = datetime.datetime.now()
        result = self.db_connector._cur.execute(f"UPDATE {self._TABLE_NAME} SET LastSeen = ? WHERE ID = ?;", (current_timestamp, client_id))
        client.last_seen = current_timestamp

        return client

    def get_client_by_row(self, row):
        client_id, client_name, public_key, aes_key, last_seen = row
        return Client(client_id, client_name, public_key, aes_key, last_seen)
    
    def get_aes_key(self, client_id, aes_key, public_key):
        self.db_connector._cur.execute(f"UPDATE {self._TABLE_NAME} SET PublicKey = ? WHERE ID = ?;",
                                       (public_key, client_id))

        cipher = RSACipher(public_key)
        encrypted_aes = cipher.encrypt(aes_key)
        return encrypted_aes

    def decrypt(self, cipher_data, client_id):
        client = self.get_client_by_client_id(client_id)
        cipher = AESCipher(client.aes_key)
        decrypted = cipher.decrypt(cipher_data)
        return decrypted
