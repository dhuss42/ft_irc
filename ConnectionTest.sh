#!/usr/bin/env bash
set -euo pipefail

HOST="127.0.0.1"
PORT="6667"
PASS="123"
COUNT=3
BASE_NICK="testnick"
CHANNEL="#test"
KEEP_OPEN=60    # how long to keep each client connected (seconds)

for i in $(seq 1 "$COUNT"); do
    NICK="${BASE_NICK}${i}"
    USERNAME="${NICK}"

    echo "Starting connection #$i with nick: $NICK"

    (
      # registration sequence
      printf 'CAP LS\r\n'
      sleep 1
      printf 'PASS %s\r\n' "$PASS"
      sleep 1
      printf 'NICK %s\r\n' "$NICK"
      sleep 1
      printf 'USER %s x 0 0\r\n' "$USERNAME"
      sleep 1
      printf 'CAP END\r\n'
      sleep 1

      # join channel
      printf 'JOIN %s\r\n' "$CHANNEL"
      sleep 1

      # send messages every 5 seconds until time runs out
      start=$(date +%s)
      while [ $(( $(date +%s) - start )) -lt $KEEP_OPEN ]; do
          printf 'PRIVMSG %s :Hello from %s\r\n' "$CHANNEL" "$NICK"
          sleep 5
      done
    ) | nc "$HOST" "$PORT" &

    # slight stagger between new clients
    sleep 0.2
done

wait
echo "All clients started, each sending PRIVMSG to $CHANNEL every 5s for ${KEEP_OPEN}s."
