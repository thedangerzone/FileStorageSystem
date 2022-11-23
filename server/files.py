from global_consts import FILES_TABLE
from server_exceptions import NoSuchFile
from random import randint
from dataclasses import dataclass
from base64 import b64encode, b64decode
import os


@dataclass
class File:
    client_id: bytes
    file_name: str
    file_path: str
    verified: bool

class Files:
    _TABLE_NAME = FILES_TABLE
    _TABLE_PARAMS = ["ID VARCHARS(16) NOT NULL",
                     "FileName VARCHARS(255) NOT NULL",
                     "FilePath VARCHARS(255)",
                     "Verified INT1",
                     "PRIMARY KEY(ID, FileName)"]

    def __init__(self, db_connector):
        self.db_connector = db_connector
        self.db_connector._create_if_not_exist(self._TABLE_NAME, self._TABLE_PARAMS) 

    def __iter__(self):
        files = self.db_connector._cur.execute(f"SELECT ID, FileName, FilePath, Verified FROM {self._TABLE_NAME};").fetchall()
        return iter([self.get_file_by_row(row) for row in files])

    def add_file(self, client_id, file_name, file_path, verified):
        new_file = File(client_id, file_name, file_path, verified)
        self.db_connector._cur.execute(f"INSERT INTO {self._TABLE_NAME} VALUES (?, ?, ?, ?);", (new_file.client_id,
                                                                                                new_file.file_name,
                                                                                                new_file.file_path,
                                                                                                new_file.verified))
        self.db_connector._conn.commit()
        
        return new_file

    def delete_file(self, client_id, file_name):
        self.db_connector._cur.execute(f"DELETE FROM {self._TABLE_NAME} WHERE ID = ? AND FileName = ?;", (client_id, file_name))
        self.db_connector._conn.commit()

        dir_path = f"clients\\{b64encode(client_id).decode('utf-8')}"
        file_name = file_name.decode('utf-8').rstrip('\x00')
        file_path = f"{dir_path}\\{file_name}"
        if os.path.exists(file_path):
            os.remove(file_path)
        else:
            raise NoSuchFile
    
    def validate_file(self, client_id, file_name):
        self.db_connector._cur.execute(f"UPDATE {self._TABLE_NAME} SET Verified = ? WHERE ID = ?, FileName = ?;", (True, client_id,file_name))
        self.db_connector._conn.commit()
    
    def get_file_by_row(self, row):
        client_id, file_name, file_path, verified = row
        return File(client_id, file_name, file_path, verified)