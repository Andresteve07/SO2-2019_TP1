import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the port
server_address = ('localhost', 12121)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
while True:
    print >>sys.stderr, '\nwaiting to receive message'
    data, address = sock.recvfrom(4096)
    
    print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
    print >>sys.stderr, data
    
    if data:
        data = '\xD3\x00\x00\x00{"command_id":3,"satellite_id":555555,"station_id":999999,"payload_size":99,"error":null,"payload":"{\\"cpu_usage\\":73.68,\\"firmware_version\\":12413,\\"mem_usage\\":45.71,\\"satellite_id\\":555555,\\"uptime\\":98.89}"}'
        sent = sock.sendto(data, address)
        print >>sys.stderr, 'sent %s bytes back to %s' % (sent, address)