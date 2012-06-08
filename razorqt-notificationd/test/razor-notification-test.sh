#!/bin/sh
#
# reqrired: notify-send app installed (part of libnotify, or libnotify-tools)
#


# basic notifications
notify-send -u low -t 3000 --icon="document-open" "simple notification" "expires in 3s. No action there" &
notify-send -u normal -t 4000 --icon="document-close" "simple notification" "expires in 4s. No action there" &
notify-send -u critical -t 5000 --icon="application-exit" "simple notification" "expires in 5s. No action there" &
notify-send -u normal -t -1 --icon="go-next" "simple notification" "expires as server decies. No action there" &
notify-send -u normal -t 0 --icon="go-up" "simple notification" "expires never. No action there" &

