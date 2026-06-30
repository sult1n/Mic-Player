#!/usr/bin/env bash

# --- БЛОК ОЧИСТКИ СТАРЫХ МОДУЛЕЙ ---
echo "Очистка старых виртуальных устройств..."

# Функция для безопасного удаления модулей по маске (на случай, если их несколько)
unload_by_grep() {
    pactl list modules short | grep "$1" | awk '{print $1}' | while read -r module_id; do
        if [ ! -z "$module_id" ]; then
            pactl unload-module "$module_id"
            echo "Удален модуль (ID: $module_id) по маске: $1"
        fi
    done
}

# Удаляем в обратном порядке: Loopback -> Mic -> Sink
unload_by_grep "source_name=Virtual-mic-for-mic-player"
unload_by_grep "sink=Virtual-sink-for-mic-player"
unload_by_grep "sink_name=Virtual-sink-for-mic-player"

echo "Очистка завершена. Создание новых устройств..."
echo "----------------------------------------"

# --- БЛОК СОЗДАНИЯ НОВЫХ МОДУЛЕЙ ---

# 1. Автоматически находим имя реального микрофона по умолчанию
REAL_MIC=$(pactl get-default-source)
if [ -z "$REAL_MIC" ]; then
    echo "Ошибка: Реальный микрофон не найден!"
    exit 1
fi

echo "Используется реальный микрофон: $REAL_MIC"

# 2. Создаем виртуальный Sink
SINK_ID=$(pactl load-module module-null-sink \
    sink_name=Virtual-sink-for-mic-player \
    sink_properties=device.description=Virtual_Sink_for_Mic_Player)

# 3. Перенаправляем звук
LOOP_ID=$(pactl load-module module-loopback \
    source="$REAL_MIC" \
    sink=Virtual-sink-for-mic-player \
    latency_msec=1)

# 4. Создаем виртуальный микрофон
MIC_ID=$(pactl load-module module-remap-source \
    master=Virtual-sink-for-mic-player.monitor \
    source_name=Virtual-mic-for-mic-player \
    source_properties=device.description=Virtual_Mic_for_Mic_Player)
    
echo "----------------------------------------"
echo "Успешно создано!"
echo "ID новых модулей: Sink=$SINK_ID, Loopback=$LOOP_ID, Mic=$MIC_ID"
