# Roadmap

## v2.1.0 – Sistema Audio Adattivo + Display (Current)

- [x] Correzione bug critico `ms_to_samples()` DSP
- [x] Sistema audio adattivo multi-sample-rate (`audio_config`)
- [x] Sample Rate Converter 44.1k/48k/96k → 48kHz
- [x] Audio protection pipeline (DC block + HPF + limiter + soft clip)
- [x] Delay buffer software in PSRAM
- [x] Configurazione display UEDX80480050E-WB-A (pinout completo)
- [x] LovyanGFX driver per pannello RGB 800×480 + GT911

## v2.2.0 – UI LVGL + Autotune (Q2 2025)

- [ ] UI LVGL 8.3 completa su display 800×480
  - [ ] Screen principale: VU meter + FFT
  - [ ] Screen DSP: EQ grafico + preset
  - [ ] Screen venue map: delay per posizione cassa
  - [ ] Screen DMX: controllo luci
- [ ] Autotune delay automatico via microfono di riferimento
- [ ] Calibrazione touch GT911
- [ ] Web dashboard aggiornata con SRC status

## v2.3.0 – Qualità Audio Pro (Q3 2025)

- [ ] SRC polyphase FIR (sostituzione linear interpolation)
- [ ] Dithering TPDF per riduzione rumore quantizzazione
- [ ] Metering LUFS (loudness standard broadcasting)
- [ ] Analisi THD+N

## v3.0.0 – Multi-room + Cloud (Q4 2025)

- [ ] Rete multi-room con sincronizzazione clock
- [ ] Dashboard cloud (monitoring remoto)
- [ ] OTA automatico
- [ ] Backup/restore configurazioni su cloud
- [ ] App mobile (React Native)
