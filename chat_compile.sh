#!/bin/sh

rm client
rm server
gcc chat_client.c -o client
gcc chat_server.c -o server
./server
