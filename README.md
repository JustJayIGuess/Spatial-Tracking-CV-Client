# Spatial-Tracking-Client
 Client-side code for spatial tracking system
# Basic Usage
 First, run `./Server` (built from EchoTestServer; just a test server to echo incoming data) and input the number of clients that will be connecting.
 The server will now listen for clients requesting to connect, and will switch to data receiving mode once enough clients connect.
 To run a client, run `./CVTracking -h <horizontal FOV> -v <vertical FOV> -t <brightness threshold>` on a machine connected to the same local network as the server.
 If all works correctly, the server will detect the client's broadcast and respond with its ip to initiate direct communication. The client will begin sending data when a valid tracking contour is detected through the camera.
