## Simple project to learn about socket programming in c.
  
## How WoL Works
Target computer must be configured to accept WoL packets
Magic packet is sent over the network to the target's MAC address
Network card receives packet even when computer is off/sleeping
Computer powers on automatically

## The Magic Packet
The magic packet is a special network frame containing:

6 bytes of 0xFF (255 in decimal): FF FF FF FF FF FF
Target MAC address repeated 16 times
Optional password (4 or 6 bytes)

## How I want this to work
The idea is to make a server that listen for incomming tcp connections on a specific port, for example using ``` netcat 127.0.0.1 8080 ```.
The client is then prompted to enter a password, if correct the server create a client connexion to send a Magic Packet to the machine with
the coressponding MAC @. This enables all machines/servers on the same datalink layer (Layer 2) to remain in sleep mode or idle state while 
maintaining network connectivity. The network interface card (NIC) typically draws only 1-5 watts of standby power to keep listening for 
magic packets, providing significant energy savings compared to keeping systems fully powered.
