# Roadmap

## v2.1.0 – Completato (2026-03-13)

- [x] Slave: encoder volume/balance, LED ring WS2812B, relay via IPC (CMD_POLL_INPUTS, SET_LED_RING, RELAY_SET)
- [x] Master: USE_SLAVE_PERIPHERALS, storage_init prima di audio_init
- [x] Docs: MASTER_SLAVE_ARCHITECTURE.md, SCHEMATIC_DSP_CONNECTIONS.html

## v2.0.x – Completato (2026-03)

- [x] Architettura Dual-ESP32 (Master + Slave)
- [x] I2S condiviso, UART IPC, storage delegation
- [x] Display LVGL 800×480 (UEDX + GT911), tab Home/Discovery/Assegnazione/Luci/DSP/Neon
- [x] Sistema audio adattivo multi-SR, SRC, protezione, delay buffer
- [x] RS-485 discovery reale, protocollo CQ260D (SET_PARAM, gain, delay, preset)
- [x] AutoTune locale e remoto (smartphone), mappa venue 2D, WLED client

## v2.2.0 – Possibili evoluzioni

- [ ] SRC polyphase FIR (sostituzione interpolazione lineare)
- [ ] Metering LUFS, analisi THD+N
- [ ] Calibrazione touch GT911 da UI
- [ ] OTA e backup/restore configurazioni

## v3.0.0 – Lungo termine

- [ ] Rete multi-room con sincronizzazione clock
- [ ] Dashboard cloud (monitoring remoto)
- [ ] App mobile oltre Flutter (es. React Native / PWA)
