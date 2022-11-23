from global_consts import DB_FILE_PATH
import sqlite3

class DBManager:
    def __init__(self):
        self._conn = sqlite3.connect(DB_FILE_PATH)
        self._conn.text_factory = bytes 
        self._cur = self._conn.cursor()

    def close(self):
        self._cur.close()
        self._conn.commit()
        self._conn.close() 

    def _create_if_not_exist(self, table_name, table_params):
        self._cur.execute(f"CREATE TABLE IF NOT EXISTS {table_name} ({', '.join(table_params)});")


