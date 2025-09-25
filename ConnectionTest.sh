#!/usr/bin/env bash
set -euo pipefail

HOST="127.0.0.1"
PORT="6667"
PASS="123"
COUNT=10
BASE_NICK="testnick"
BASE_CHANNEL="#test"
KEEP_OPEN=60             # how long each client stays connected
MSG_INTERVAL=5           # seconds between PRIVMSG
MODE="partial"            # normal | halfopen | longnick | joinflood | privspam | invalid

send_slowly() {
    local cmd="$1"
    for (( j=0; j<${#cmd}; j++ )); do
        # Send one character at a time, no newline
        printf '%s' "${cmd:j:1}"
        sleep 0.1
    done
    # finally terminate with CRLF
    printf '\r\n'
}

for i in $(seq 1 "$COUNT"); do
    NICK="${BASE_NICK}${i}"
    USERNAME="${NICK}"
    CHANNEL="${BASE_CHANNEL}"

    echo "Starting $NICK in mode=$MODE"

    case "$MODE" in
    normal)
        (
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
          printf 'JOIN %s\r\n' "$CHANNEL"
          sleep 1
          start=$(date +%s)
          while [ $(( $(date +%s) - start )) -lt $KEEP_OPEN ]; do
              printf 'PRIVMSG %s :Hello from %s\r\n' "$CHANNEL" "$NICK"
              sleep "$MSG_INTERVAL"
          done
        ) | nc "$HOST" "$PORT" &
        ;;

    halfopen)
        # connects and authenticates, but never joins or sends PRIVMSG
        (
          printf 'CAP LS\r\n'
          sleep 1
          printf 'PASS %s\r\n' "$PASS"
          sleep 1
          printf 'NICK %s\r\n' "$NICK"
          sleep 1
          printf 'USER %s x 0 0\r\n' "$USERNAME"
          sleep 1
          printf 'CAP END\r\n'
          sleep "$KEEP_OPEN"
        ) | nc "$HOST" "$PORT" &
        ;;

    longnick)
        LONGNICK="${NICK}$(head -c 200 </dev/urandom | tr -dc 'a-zA-Z0-9')"
        (
          printf 'CAP LS\r\n'
          sleep 1
          printf 'PASS %s\r\n' "$PASS"
          sleep 1
          printf 'NICK %s\r\n' "$LONGNICK"
          sleep 1
          printf 'USER %s x 0 0\r\n' "$USERNAME"
          sleep 1
          printf 'CAP END\r\n'
          sleep "$KEEP_OPEN"
        ) | nc "$HOST" "$PORT" &
        ;;

    joinflood)
        (
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
          # join 200 channels
          for j in $(seq 1 200); do
              printf 'JOIN #%s%d\r\n' "$NICK" "$j"
              sleep 0.05
          done
          sleep "$KEEP_OPEN"
        ) | nc "$HOST" "$PORT" &
        ;;

    privspam)
        (
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
          printf 'JOIN %s\r\n' "$CHANNEL"
          sleep 1
          # flood PRIVMSGs
          for j in $(seq 1 1000); do
              printf 'PRIVMSG %s :spam-%d-from-%s\r\n' "$CHANNEL" "$j" "$NICK"
          done
          sleep "$KEEP_OPEN"
        ) | nc "$HOST" "$PORT" &
        ;;

    invalid)
        (
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
          # now send invalid stuff after handshake
          printf 'FOOBAR somebadcommand\r\n'
          printf 'NICK *&^%%$$$weird\r\n'
          sleep "$KEEP_OPEN"
        ) | nc "$HOST" "$PORT" &
        ;;

    partial)
        (
          send_slowly "CAP LS"
          sleep 1
          send_slowly "PASS $PASS"
          sleep 1
          send_slowly "NICK $NICK"
          sleep 1
          send_slowly "USER $USERNAME x 0 0"
          sleep 1
          send_slowly "CAP END"
          sleep 1
          send_slowly "JOIN $CHANNEL"
          sleep 1
          start=$(date +%s)
          while [ $(( $(date +%s) - start )) -lt $KEEP_OPEN ]; do
              send_slowly "PRIVMSG $CHANNEL :Hello (slow) from $NICK"
              sleep "$MSG_INTERVAL"
          done
        ) | nc "$HOST" "$PORT" &
        ;;
    esac

    sleep 0.2
done

wait
echo "All clients started in mode=$MODE."
