class InvalidSocket(Exception):
    def __str__(self):
        return f"Got an invalid request socket"

class NoSuchCode(Exception):
    def __str__(self):
        return f"Got request with invalid code"

class UserAlreadyExists(Exception):
    def __str__(self):
        return f"Cannot register user which already exists"

class NoSuchUser(Exception):
    def __str__(self):
        return f"Got request from unregistered client"

class NoSuchFile(Exception):
    def __str__(self):
        return f"Got request to unregistered file"
