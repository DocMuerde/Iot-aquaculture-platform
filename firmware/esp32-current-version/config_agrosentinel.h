#ifndef CONFIG_AGROSENTINEL_H
#define CONFIG_AGROSENTINEL_H

// ============================================================
// AGROSENTINEL — FIRMWARE CONFIGURATION
// ============================================================
//
// Drop-in parallel channel alongside Firebase + MQTT.
// Zero interference with existing logic.
//
// DEPLOYMENT IDENTITY  ← edit these two lines per installation
// ============================================================

// Region prefix: "ES" (Spain/ETSIA) | "CO" (Colombia)
#define DEVICE_REGION     "ES"

// Deployment site: 4-char max, uppercase, no spaces
// ETSIA Sevilla = "ETSA"  |  Colombia pilot = "CLMB"
#define DEPLOYMENT_SITE   "ETSA"

// ============================================================
// CANONICAL DEVICE ID  ← THIS MUST MATCH backend device_registry
// ============================================================
// Format: INSTITUTION_LOCATION_NODEID (exact match required)
// If this does not match the registry, the backend will reject
// all payloads with HTTP 403 (ghost node / UNKNOWN source).
//
// Valid IDs (from device_registry.py DEVICE_REGISTRY):
//   ETSIA_SEVILLA_NODO_00   ← Nodo 0, ETSIA Sevilla
//   ETSIA_SEVILLA_NODO_01   ← Nodo 1, ETSIA Sevilla
//   ETSIA_SEVILLA_NODO_03   ← Nodo 3, ETSIA Sevilla (full sensors)
//   SIM_LOCAL_NODO_01       ← Simulator (TEST only)
//
// ⚠️  The auto-generated ID "ES-ETSA-X" is NOT in the registry.
//     Always set DEVICE_CANONICAL_ID to the exact registry key.
#define DEVICE_CANONICAL_ID   "ETSIA_SEVILLA_NODO_01"

// device_id assembled at runtime in Send_AgroSentinel.ino:
//   Uses DEVICE_CANONICAL_ID directly — no auto-generation.
//   ES-ETSA-X format kept only in _as_build_human_label() for logs.

// ============================================================
// NETWORK
// ============================================================

// Target backend — change to cloud URL before ETSIA deployment
// LOCAL:  "https://agrosentinel-1.onrender.com/ingest_aquacol"
// CLOUD:  "https://agrosentinel.yourdomain.com/ingest_aquacol"
#define AGROSENTINEL_URL      "https://agrosentinel-1.onrender.com/ingest_aquacol"

// HTTP timeout (ms). Keep short — SIM900/WiFi can hang.
// 8 s is safe; lower to 5 s on flaky links.
#define AGROSENTINEL_TIMEOUT  8000

// ============================================================
// RETRY / SD FALLBACK
// ============================================================

// How many failed sends before writing frame to SD queue.
// 3 is a good balance: fast recovery, not hammering the server.
#define AGROSENTINEL_MAX_RETRIES   3

// SD file paths (8.3 compliant for FAT16/FAT32)
#define AGROSENTINEL_SD_QUEUE  "/agro_q.jl"    // JSONL queue
#define AGROSENTINEL_SD_ACK    "/agro_ack.log"  // sent log

// Max frames to replay from SD per cycle.
// Prevents blocking loop() for too long during burst resend.
#define AGROSENTINEL_SD_BURST_LIMIT  10

// ============================================================
// FEATURE FLAGS
// ============================================================

#define AGROSENTINEL_ENABLED  true

// Set to true only during integration testing.
// In production, verbose serial output wastes ~20 ms per frame.
#define AGROSENTINEL_DEBUG    false

#endif  // CONFIG_AGROSENTINEL_H
