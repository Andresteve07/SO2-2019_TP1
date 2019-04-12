import socket
import sys
import time
import json

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
server_address = ('localhost', 12121)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
# Listen for incoming connections
sock.listen(1)

def recv_file(connection):
    with open('recv_update', 'wb') as f:
        print 'file opened'
        while True:
            print('receiving data...')
            data = connection.recv(1024)
            print('data=%s', (data))
            if not data:
                break
            # write data to a file
            f.write(data)

def recv_data(connection):
# Receive the data in small chunks and retransmit it
    while True:
        data = connection.recv(16)
        print >>sys.stderr, 'received "%s"' % data
        if data:
            print >>sys.stderr, 'sending data back to the client'
            data = '\xC6\x00\x00\x00{"command_id":3,"satellite_id":555555,"station_id":999999,"payload_size":99,"payload":"{\\"cpu_usage\\":73.68,\\"firmware_version\\":12413,\\"mem_usage\\":45.71,\\"satellite_id\\":555555,\\"uptime\\":98.89}"}'
            connection.sendall(data)
        else:
            print >>sys.stderr, 'no more data from', client_address
            break
def telemetry_mock(connection):
    while True:
        data = connection.recv(16)
        print >>sys.stderr, 'received "%s"' % data
        if data:
            data = '\xC6\x00\x00\x00{"command_id":3,"satellite_id":555555,"station_id":999999,"payload_size":99,"payload":"{\\"cpu_usage\\":73.68,\\"firmware_version\\":12413,\\"mem_usage\\":45.71,\\"satellite_id\\":555555,\\"uptime\\":98.89}"}'
            connection.sendall(data)
def recv_file(socketinst, filename, file_size):
    recv_bytes = 0
    with open(filename, 'wb') as f:
        print ('file opened %s',filename)
        print ('Trying to read %i',file_size)
        while True:
            #print('receiving data...')
            data = socketinst.recv(32)
            #print('recved=%i data=%s',len(data), (data))
            recv_bytes+=len(data)
            #print('bytesread: %i -- partial count %i',len(data), recv_bytes)
            # write data to a file
            f.write(data)
            if recv_bytes >= file_size:
                break
        print("FINAL COUNT: %i",recv_bytes)
        f.close()

def firmware_upgrade(connection):
    data = connection.recv(504)
    string_data = data[4:].decode('ASCII')
    parsed_json = json.loads(string_data)
    print >>sys.stderr, 'received "%s"' % data
    if data:
        data = '\x6F\x00\x00\x00{"command_id":1,"satellite_id":555555,"station_id":999999,"error":null,"payload":"{\\"current_version\\":10502}"}'
        connection.sendall(data)
    request_string = parsed_json['payload']
    print(request_string)
    parsed_payload = json.loads(request_string)
    print(parsed_payload['file_size_bytes'])
    file_size = parsed_payload['file_size_bytes']
    print("FILE SIZE: ", file_size)
    recv_file(connection, "recv_update_file",file_size)

def send_file(connection):
    filename='../assets/file_server' #In the same folder or path is this file running must the file you want to tranfser to be
    #$ echo -n -e '\xff' >> file_server #This adds an EOF to the end of the file.
    #$ dd if=/dev/urandom of=file_server count=32 bs=1024
    
    for x in range(0, 3):
        f = open(filename,'rb')
        l = f.read(1024)
        while (l):
            connection.send(l)
            l = f.read(1024)
        print('Sent ',x)
        f.close()
        
def image_scanner(connection):
    data = connection.recv(1024)
    print >>sys.stderr, 'received "%s"' % data
    if data:
        #data = '\x7A\x00\x00\x00{"command_id":2,"satellite_id":555555,"station_id":999999,"payload":"{\\"slice_quantity\\":3,\\"slices_dataset\\":[{\\"name\\":\\"mock_slice_0.jpg\\",\\"size\\":79717},{\\"name\\":\\"mock_slice_0.jpg\\",\\"size\\":178845},{\\"name\\":\\"mock_slice_0.jpg\\",\\"size\\":200802}]}"}'
        data = '\xE6\x00\x00\x00{"command_id":2,"satellite_id":555555,"station_id":999999,"error":null,"payload":"{\\"slice_quantity\\":2,\\"slices_dataset\\":[{\\"name\\":\\"mock_slice_0.jpg\\",\\"size\\":79717},{\\"name\\":\\"mock_slice_1.jpg\\",\\"size\\":178845}]}"}'
        connection.sendall(data)
    for x in range(4, 6):
        time.sleep(1)
        filename = "scann_"+str(x)+".jpg"
        print "Sending " + filename
        f = open(filename,'rb')
        l = f.read(1024)
        while (l):
            connection.send(l)
            l = f.read(1024)
        print('Sent ',x)
        f.close()

def timeout_test(connection):
    print 'Waiting Start'
    time.sleep(20)
    print 'Waiting Finished'
    
while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()
    try:
        print >>sys.stderr, 'connection from', client_address
        
        #data = connection.recv(128)
        #print >>sys.stderr, 'received "%s"' % data
        #recv_file(connection)

        firmware_upgrade(connection)

        #image_scanner(connection)

        #timeout_test(connection)
            
    finally:
        # Clean up the connection
        connection.close()


#{"cpu_usage":73.68,"firmware_version":12413,"mem_usage":45.71,"satellite_id":555555,"uptime":98.89}
#{"command_id":3,"satellite_id":555555,"station_id":999999,"payload_size":99,"payload":"{\"cpu_usage\":73.68,\"firmware_version\":12413,\"mem_usage\":45.71,\"satellite_id\":555555,\"uptime\":98.89}"}