#include <HTTPClient.h>
#include "config_firebase.h"

// ===================================
// ENVÍO DE DATOS A FIREBASE (FIRESTORE REST API)
// ===================================

void Send_Firebase() {
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Firebase: Sin WiFi, guardando en SD...");
    Store_Firebase_SD(); // Guarda para reenviar después
    return;
  }

  HTTPClient http;
  http.begin(FIRESTORE_URL);
  http.addHeader("Content-Type", "application/json");
  
  // Construir JSON en formato Firestore
  String jsonPayload = buildFirestoreJSON();
  
  Serial.println("Enviando a Firebase...");
  Serial.println(jsonPayload);
  
  int httpResponseCode = http.POST(jsonPayload);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Firebase OK: ");
    Serial.println(httpResponseCode);
    
    // Si se envió correctamente, intentar reenviar datos pendientes
    if (SD.exists("/firebase_pending.txt")) {
      Resend_Firebase_Pending();
    }
    
  } else {
    Serial.print("Firebase ERROR: ");
    Serial.println(httpResponseCode);
    Store_Firebase_SD(); // Guardar para reintentar después
  }
  
  http.end();
}

// ======================================
// CONSTRUIR JSON EN FORMATO FIRESTORE
// ======================================

String buildFirestoreJSON() {
  String json = "{\"fields\": {";
  
  // ID de instalación
  json += "\"ft_ins_inst\": {\"integerValue\": \"" + String(config.id) + "\"},";
  
  // Timestamp
  json += "\"ft_ins_timestamp\": {\"integerValue\": \"" + String(timestamp) + "\"},";
  
  // Temperaturas del agua
  json += "\"ft_ins_t1\": {\"doubleValue\": " + String(round2(Tw1), 2) + "},";
  json += "\"ft_ins_t2\": {\"doubleValue\": " + String(round2(Tw2), 2) + "},";
  
  // pH
  json += "\"ft_ins_ph\": {\"doubleValue\": " + String(round2(PH), 2) + "},";
  
  // Oxígeno
  json += "\"ft_ins_o2\": {\"doubleValue\": " + String(round2(O2), 2) + "},";
  
  // Conductividad
  if (isnan(EC)) {
    json += "\"ft_ins_ec\": {\"nullValue\": null},";
  } else {
    json += "\"ft_ins_ec\": {\"doubleValue\": " + String(round2(EC), 2) + "},";
  }
  
  // Ambiente 1
  json += "\"ft_ins_ta1\": {\"doubleValue\": " + String(round2(Ta1), 2) + "},";
  json += "\"ft_ins_ha1\": {\"doubleValue\": " + String(round2(Ha1), 2) + "},";
  
  // Ambiente 2
  json += "\"ft_ins_ta2\": {\"doubleValue\": " + String(round2(Ta2), 2) + "},";
  json += "\"ft_ins_ha2\": {\"doubleValue\": " + String(round2(Ha2), 2) + "},";
  
  // Niveles
  json += "\"ft_ins_ns\": {\"integerValue\": \"" + String(level_sup) + "\"},";
  json += "\"ft_ins_ni\": {\"integerValue\": \"" + String(level_inf) + "\"},";
  
  // Fecha y hora legibles
  json += "\"date\": {\"stringValue\": \"" + DATE + "\"},";
  json += "\"time\": {\"stringValue\": \"" + TIME + "\"}";
  
  json += "}}";
  
  return json;
}

// ============================================
// GUARDAR EN SD SI FALLA FIREBASE
// ============================================

void Store_Firebase_SD() {
  String data = String(timestamp) + "," + 
                String(round2(Tw1), 2) + "," + 
                String(round2(Tw2), 2) + "," + 
                String(round2(PH), 2) + "," + 
                String(round2(O2), 2) + "," + 
                String(round2(EC), 2) + "," + 
                String(round2(Ta1), 2) + "," + 
                String(round2(Ha1), 2) + "," + 
                String(round2(Ta2), 2) + "," + 
                String(round2(Ha2), 2) + "," + 
                String(level_sup) + "," + 
                String(level_inf);
  
  appendFile(SD, "/firebase_pending.txt", data);
  Serial.println("Datos guardados en SD para Firebase");
}

// ============================================
// REENVIAR DATOS PENDIENTES DE SD
// ============================================

void Resend_Firebase_Pending() {
  File file = SD.open("/firebase_pending.txt");
  if (!file) return;
  
  Serial.println("Reenviando datos pendientes a Firebase...");
  
  String line = "";
  bool allSent = true;
  
  while (file.available()) {
    char c = file.read();
    if (c == '\n') {
      if (line.length() > 0) {
        if (!Resend_Single_Firebase(line)) {
          allSent = false;
          break; // Si falla uno, dejamos los demás para después
        }
        delay(100); // Pequeña pausa entre envíos
      }
      line = "";
    } else {
      line += c;
    }
  }
  
  file.close();
  
  if (allSent) {
    SD.remove("/firebase_pending.txt");
    Serial.println("Todos los datos pendientes enviados a Firebase");
  }
}

// ============================================
// REENVIAR UN DATO INDIVIDUAL
// ============================================

bool Resend_Single_Firebase(String csvData) {
  // Parsear CSV: timestamp,tw1,tw2,ph,o2,ec,ta1,ha1,ta2,ha2,ns,ni
  int indices[12];
  int idx = 0;
  for (int i = 0; i < csvData.length() && idx < 12; i++) {
    if (csvData[i] == ',') {
      indices[idx++] = i;
    }
  }
  
  if (idx < 11) return false; // Datos incompletos
  
  // Construir JSON (simplificado, usa los valores parseados)
  HTTPClient http;
  http.begin(FIRESTORE_URL);
  http.addHeader("Content-Type", "application/json");
  
  String json = "{\"fields\": {";
  json += "\"ft_ins_timestamp\": {\"integerValue\": \"" + csvData.substring(0, indices[0]) + "\"},";
  json += "\"ft_ins_t1\": {\"doubleValue\": " + csvData.substring(indices[0] + 1, indices[1]) + "},";
  json += "\"ft_ins_t2\": {\"doubleValue\": " + csvData.substring(indices[1] + 1, indices[2]) + "},";
  json += "\"ft_ins_ph\": {\"doubleValue\": " + csvData.substring(indices[2] + 1, indices[3]) + "},";
  json += "\"ft_ins_o2\": {\"doubleValue\": " + csvData.substring(indices[3] + 1, indices[4]) + "},";
  json += "\"ft_ins_ec\": {\"doubleValue\": " + csvData.substring(indices[4] + 1, indices[5]) + "},";
  json += "\"ft_ins_ta1\": {\"doubleValue\": " + csvData.substring(indices[5] + 1, indices[6]) + "},";
  json += "\"ft_ins_ha1\": {\"doubleValue\": " + csvData.substring(indices[6] + 1, indices[7]) + "},";
  json += "\"ft_ins_ta2\": {\"doubleValue\": " + csvData.substring(indices[7] + 1, indices[8]) + "},";
  json += "\"ft_ins_ha2\": {\"doubleValue\": " + csvData.substring(indices[8] + 1, indices[9]) + "},";
  json += "\"ft_ins_ns\": {\"integerValue\": \"" + csvData.substring(indices[9] + 1, indices[10]) + "\"},";
  json += "\"ft_ins_ni\": {\"integerValue\": \"" + csvData.substring(indices[10] + 1) + "\"}";
  json += "}}";
  
  int httpCode = http.POST(json);
  http.end();
  
  return (httpCode == 200);
}
