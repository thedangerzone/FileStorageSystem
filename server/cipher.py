from base64 import b64encode, b64decode
import hashlib
from Crypto.Util.Padding import pad, unpad
from Crypto import Random
from Crypto.Cipher import AES, PKCS1_v1_5, PKCS1_OAEP
from Crypto.PublicKey import RSA


class AESCipher:

    def __init__(self, key):
        self.bs = AES.block_size
        self.key = key
        self.iv = b'\0'*16 #Default zero based bytes[16]

    def encrypt(self, raw):
        raw = pad(raw, self.bs)
        print("padded info before encrypt", raw)
        cipher = AES.new(self.key, AES.MODE_CBC, self.iv)
        return b64encode(cipher.encrypt(raw))

    def decrypt(self, cipher_text):
        cipher = AES.new(self.key, AES.MODE_CBC, self.iv)
        decoded_cipher = b64decode(cipher_text)
        text = cipher.decrypt(decoded_cipher)
        return unpad(text, self.bs)


class RSACipher:

    def __init__(self, key):
        self.publicKey = RSA.import_key(key)
        self.cipher = PKCS1_OAEP.new(self.publicKey)

    def encrypt(self, raw):
        return self.cipher.encrypt(raw)
