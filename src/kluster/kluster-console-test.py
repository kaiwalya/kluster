import zmq
import json

c = zmq.Context()
s = zmq.Socket(c, zmq.PUSH)

s.connect("tcp://127.0.0.1:4784");
s.send_string(json.dumps({"test": "test"}, indent='\t'))

