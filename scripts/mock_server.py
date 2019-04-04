import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
server_address = ('localhost', 12121)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()
    try:
        print >>sys.stderr, 'connection from', client_address

        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(16)
            print >>sys.stderr, 'received "%s"' % data
            if data:
                print >>sys.stderr, 'sending data back to the client'
                data = '\xBC\x00\x00\x00{"command_id":3,"satellite_id":555555,"station_id":999999,"payload_size":99,"payload":"{"cpu_usage":73.68,"firmware_version":12413,"mem_usage":45.71,"satellite_id":555555,"uptime":98.89}"}'
                connection.sendall(data)
            else:
                print >>sys.stderr, 'no more data from', client_address
                break
            
    finally:
        # Clean up the connection
        connection.close()

#{"cpu_usage":73.68,"firmware_version":12413,"mem_usage":45.71,"satellite_id":555555,"uptime":98.89}
#{"command_id":3,"satellite_id":555555,"station_id":999999,"payload_size":94,"payload":"{"cpu_usage":73.68,"firmware_version":12413,"mem_usage":45.71,"satellite_id":555555,"uptime":98.89}"}