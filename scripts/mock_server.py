import socket
import sys
import time

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
server_address = ('localhost', 12121)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
# Listen for incoming connections
sock.listen(1)

def recv_file(connection):
    with open('../assets/received_file', 'wb') as f:
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
def firmware_upgrade(connection):
    print 'HOLO'
    data = connection.recv(35)
    print >>sys.stderr, 'received "%s"' % data
    if data:
        data = '\x63\x00\x00\x00{"command_id":1,"satellite_id":555555,"station_id":999999,"payload":"{\\"current_version\\":11235}"}'
        connection.sendall(data)
    recv_file(connection)
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
        data = '\x7A\x00\x00\x00{"command_id":2,"satellite_id":555555,"station_id":999999,"payload":"{\\"slice_quantity\\":3,\\"slice_size_bytes\\":256}"}'
        connection.sendall(data)
    send_file(connection)
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