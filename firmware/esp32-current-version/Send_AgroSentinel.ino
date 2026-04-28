// ============================================================
// Send_AgroSentinel.ino — Parallel telemetry channel
// ============================================================
//
// Sends the same sensor readings already consumed by
// Send_Firebase() and Send_MQTT() to the AgroSentinel backend.
//
// CONTRACT:
//   • Reads global sensor vars (Tw1, PH, O2, EC, Ta1, Ha1,
//     Ta2, Ha2, level_sup, level_inf, timestamp, config.id)
//   • Reads config constants from config_agrosentinel.h
//   • NEVER modifies any variable used by Firebase or MQTT
//   • Self-contained failure path: SD queue + bounded burst
//
// INTEGRATION POINT (Acuacol_definitivo_V8.ino, loop()):
//
//   Send_MQTT();          // untouched
//   Send_Firebase();      // untouched
//   Send_AgroSentinel();  // ← add this line, same block
//
// ============================================================

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config_agrosentinel.h"

// ── Internal state (no external linkage needed) ─────────────
static struct {
  bool     last_ok        = false;
  int      retries        = 0;
  uint32_t last_send_ms   = 0;
  bool     sd_active      = false;
} _as_state;

// ── Forward declarations ─────────────────────────────────────
static bool   _as_http_post(const String& json);
static void   _as_sd_save(const String& json);
static void   _as_sd_replay();
static String _as_build_device_id();
static String _as_build_payload();

// ============================================================
// PUBLIC ENTRY POINT
// Called once per data-send cycle alongside Firebase + MQTT.
// ============================================================
void Send_AgroSentinel() {

  if (!AGROSENTINEL_ENABLED) return;

  String json = _as_build_payload();

  if (_as_http_post(json)) {
    // ── Success ─────────────────────────────────────────────
    _as_state.last_ok      = true;
    _as_state.retries      = 0;
    _as_state.sd_active    = false;
    _as_state.last_send_ms = millis();

    #if AGROSENTINEL_DEBUG
      Serial.println("[AS] ✓ sent OK");
    #endif

    // Replay pending SD frames now that connectivity is good
    if (SD.exists(AGROSENTINEL_SD_QUEUE)) {
      _as_sd_replay();
    }

  } else {
    // ── Failure ─────────────────────────────────────────────
    _as_state.last_ok = false;
    _as_state.retries++;

    #if AGROSENTINEL_DEBUG
      Serial.print("[AS] ✗ fail #"); Serial.println(_as_state.retries);
    #endif

    if (_as_state.retries >= AGROSENTINEL_MAX_RETRIES) {
      _as_sd_save(json);
      _as_state.sd_active = true;
      _as_state.retries   = 0;
    }
  }
}

// ============================================================
// BUILD DEVICE ID
// Format: <REGION>-<SITE>-<config.id>
// Examples: ES-ETSA-1   CO-CLMB-7
//
// ────────────────────────────────────────────────────────────
// Returns the CANONICAL device_id that matches backend device_registry.
// DEVICE_CANONICAL_ID is defined in config_agrosentinel.h and MUST be
// one of the IDs in DEVICE_REGISTRY (Python backend).
// Using the auto-generated ES-ETSA-X format causes HTTP 403 (ghost node).
// ────────────────────────────────────────────────────────────
static String _as_build_device_id() {
  return String(DEVICE_CANONICAL_ID);  // e.g. "ETSIA_SEVILLA_NODO_01"
}

// Human-readable label for serial debug only (NOT sent to backend).
static String _as_build_human_label() {
  char buf[24];
  snprintf(buf, sizeof(buf), "%s-%s-%d",
           DEVICE_REGION, DEPLOYMENT_SITE, config.id);
  return String(buf);
}

// ============================================================
// BUILD JSON PAYLOAD  (DATA CONTRACT v1.0)
//
// Reuses the same global sensor vars read by buildFirestoreJSON().
// No duplicate sensor reads. No extra I2C/bus traffic.
// ============================================================
static String _as_build_payload() {

  // StaticJsonDocument fits on stack; no heap allocation.
  // Size tuned to contract: ~450 bytes serialised → 512 safe.
  StaticJsonDocument<512> doc;

  doc["device_id"]   = _as_build_device_id();
  doc["timestamp"]   = (uint32_t)timestamp;
  doc["sequence_id"] = (uint64_t)generate_sequence_id();

  JsonObject pl = doc.createNestedObject("payload");
  pl["water_temperature_c"]      = round2(Tw1);
  pl["water_temperature_2_c"]    = round2(Tw2);
  pl["ph"]                       = round2(PH);
  pl["dissolved_oxygen"]         = round2(O2);
  pl["electrical_conductivity"]  = isnan(EC) ? 0.0f : round2(EC);
  pl["air_temperature_c"]        = round2(Ta1);
  pl["air_humidity_pct"]         = round2(Ha1);
  pl["air_temperature_2_c"]      = round2(Ta2);
  pl["air_humidity_2_pct"]       = round2(Ha2);
  pl["water_level_high"]         = (level_sup == 1);
  pl["water_level_low"]          = (level_inf == 1);

  JsonObject meta = doc.createNestedObject("meta");
  meta["source"]       = "esp32_wifi";
  meta["retry_count"]  = _as_state.retries;

  String out;
  out.reserve(460);
  serializeJson(doc, out);
  return out;
}

// ============================================================
// HTTP POST
// Independent session from Firebase (separate HTTPClient).
// Returns true on HTTP 2xx.
// ============================================================
static bool _as_http_post(const String& json) {

  if (WiFi.status() != WL_CONNECTED) {
    #if AGROSENTINEL_DEBUG
      Serial.println("[AS] no WiFi");
    #endif
    return false;
  }

  HTTPClient http;
  http.setTimeout(AGROSENTINEL_TIMEOUT);
  http.begin(AGROSENTINEL_URL);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(json);

  #if AGROSENTINEL_DEBUG
    Serial.print("[AS] HTTP "); Serial.println(code);
    // Only read response body in debug mode (saves ~200 bytes heap)
    Serial.println(http.getString());
  #endif

  http.end();

  return (code >= 200 && code < 300);
}

// ============================================================
// SD FALLBACK — SAVE
// One JSON object per line (JSONL).
// ============================================================
static void _as_sd_save(const String& json) {
  File f = SD.open(AGROSENTINEL_SD_QUEUE, FILE_APPEND);
  if (!f) {
    Serial.println("[AS] SD open fail");
    return;
  }
  f.println(json);
  f.close();

  #if AGROSENTINEL_DEBUG
    Serial.println("[AS] frame queued to SD");
  #endif
}

// ============================================================
// SD FALLBACK — REPLAY
//
// Reads at most AGROSENTINEL_SD_BURST_LIMIT frames per call
// to avoid blocking loop() during a large SD backlog.
//
// On any send failure it stops immediately — connectivity
// is gone, don't exhaust the file handle for nothing.
//
// When the full file is replayed, it is deleted.
// Partial success: keeps the file (framework will retry on
// next successful normal send).
// ============================================================
static void _as_sd_replay() {

  File f = SD.open(AGROSENTINEL_SD_QUEUE);
  if (!f) return;

  #if AGROSENTINEL_DEBUG
    Serial.println("[AS] SD replay start");
  #endif

  String line;
  line.reserve(480);

  int sent   = 0;
  int failed = 0;

  while (f.available() && sent < AGROSENTINEL_SD_BURST_LIMIT) {
    char c = f.read();
    if (c == '\n') {
      if (line.length() > 0) {
        if (_as_http_post(line)) {
          sent++;
        } else {
          failed++;
          break;  // stop on first failure
        }
        line = "";
        delay(50);  // brief pause between burst frames
      }
    } else {
      line += c;
    }
  }

  bool eof = !f.available();
  f.close();

  #if AGROSENTINEL_DEBUG
    Serial.print("[AS] SD replayed "); Serial.print(sent);
    Serial.print(" failed "); Serial.println(failed);
  #endif

  // Only delete if we reached EOF with zero failures
  if (eof && failed == 0) {
    SD.remove(AGROSENTINEL_SD_QUEUE);
    _as_state.sd_active = false;
    #if AGROSENTINEL_DEBUG
      Serial.println("[AS] SD queue cleared");
    #endif
  }
}

// ============================================================
// DEBUG DUMP (call from Serial monitor if needed)
// ============================================================
void agrosentinel_debug_state() {
  Serial.println("\n── AgroSentinel Channel ──");
  Serial.print("device_id  : "); Serial.println(_as_build_device_id());
  Serial.print("human_label: "); Serial.println(_as_build_human_label());
  Serial.print("last_ok   : "); Serial.println(_as_state.last_ok);
  Serial.print("retries   : "); Serial.println(_as_state.retries);
  Serial.print("sd_active : "); Serial.println(_as_state.sd_active);
  Serial.print("boot#     : "); Serial.println(boot_counter);
  Serial.print("frame#    : "); Serial.println(frame_counter);
  Serial.println("──────────────────────────\n");
}
