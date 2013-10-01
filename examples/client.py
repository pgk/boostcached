
from contextlib import contextmanager
import socket
import pickle

@contextmanager
def socket_create(*args, **kw):
    s = socket.create_connection(*args, **kw)
    yield s
    s.close()

with socket_create(("127.0.0.1", "8081")) as s:

	s.send("get foo\n")
	print s.recv(64)
	s.send("set python.tuple id2345d\n")
	print s.recv(256)
	s.send("get python.tuple\n")
	print s.recv(256)
