#!/bin/bash

# --- Налаштування ---
PORT=8000
HOST="127.0.0.1"
# Розмір повідомлення в байтах (100 kB = 100 * 1000, не 1024, для простоти)
SIZE=100000

# --- Крок 1: Генерація та надсилання потоку ---
echo "1. Generating a single ${SIZE}-byte string followed by a newline..."
echo "2. Sending stream to $HOST:$PORT..."

# Створюємо потік з $SIZE символів 'a', додаємо один '\n' в кінці,
# і все це передаємо напряму в nc.
# 'head -c' бере байти з /dev/zero (нульові байти)
# 'tr' замінює нульові байти на символ 'a'
# 'echo' додає фінальний перенос рядка
(head -c $SIZE /dev/zero | tr '\0' 'a'; echo) | nc -N "$HOST" "$PORT"

echo "3. Done. The stream has been sent."
