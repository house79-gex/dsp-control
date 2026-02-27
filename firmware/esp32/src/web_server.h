#pragma once

// ——— API pubblica server HTTP REST ———
void web_server_init();
void web_server_handle();  // No-op per ESPAsyncWebServer (gestione asincrona)
