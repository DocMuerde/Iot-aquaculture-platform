// ========================================
// SEQUENCE ID GENERATOR (BOOT-BASED)
// ========================================
//
// Genera IDs únicos por dispositivo para deduplicación
// Formato: device_id + boot_counter + incremento local
//
// Garantiza que cada frame tiene ID único incluso tras reboot
//

#include <EEPROM.h>

// EEPROM addresses (reservar espacio)
#define EEPROM_BOOT_COUNTER_ADDR 0      // 4 bytes (uint32_t)
#define EEPROM_SIZE 512

volatile uint32_t boot_counter = 0;
volatile uint64_t frame_counter = 0;

// ========================================
// Inicializar sequence ID system
// ========================================
void init_sequence_id_system() {
  // Leer boot counter de EEPROM
  EEPROM.begin(EEPROM_SIZE);
  boot_counter = read_boot_counter();
  
  // Incrementar boot counter (cada reboot)
  boot_counter++;
  write_boot_counter(boot_counter);
  
  // Reset frame counter
  frame_counter = 0;
  
  Serial.print("Sequence ID System Initialized: boot_counter=");
  Serial.println(boot_counter);
}

// ========================================
// Leer boot counter de EEPROM
// ========================================
uint32_t read_boot_counter() {
  uint32_t value = 0;
  for (int i = 0; i < 4; i++) {
    value |= ((uint32_t)EEPROM.read(EEPROM_BOOT_COUNTER_ADDR + i)) << (i * 8);
  }
  return value;
}

// ========================================
// Escribir boot counter a EEPROM
// ========================================
void write_boot_counter(uint32_t value) {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(EEPROM_BOOT_COUNTER_ADDR + i, (value >> (i * 8)) & 0xFF);
  }
  EEPROM.commit();
}

// ========================================
// Generar sequence ID único
// ========================================
// Formato: device_id (16 bits) | boot_counter (24 bits) | frame_counter (24 bits)
// Resultado: uint64_t único y ordenable
uint64_t generate_sequence_id() {
  frame_counter++;
  
  uint64_t seq_id = 0;
  seq_id |= ((uint64_t)config.id & 0xFFFF) << 48;           // device_id (16 bits)
  seq_id |= ((uint64_t)boot_counter & 0xFFFFFF) << 24;     // boot_counter (24 bits)
  seq_id |= ((uint64_t)frame_counter & 0xFFFFFF);           // frame_counter (24 bits)
  
  return seq_id;
}

// ========================================
// Reset frame counter (por si es necesario)
// ========================================
void reset_frame_counter() {
  frame_counter = 0;
}

// ========================================
// Debug: Print sequence ID info
// ========================================
void print_sequence_id_debug(uint64_t seq_id) {
  if (!AGROSENTINEL_DEBUG) return;
  
  uint16_t dev_id = (seq_id >> 48) & 0xFFFF;
  uint32_t boot_cnt = (seq_id >> 24) & 0xFFFFFF;
  uint32_t frame_cnt = seq_id & 0xFFFFFF;
  
  Serial.print("Sequence ID: ");
  Serial.print(dev_id); Serial.print("|");
  Serial.print(boot_cnt); Serial.print("|");
  Serial.println(frame_cnt);
}
