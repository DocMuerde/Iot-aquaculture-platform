#ifndef CONFIG_FIREBASE_H
#define CONFIG_FIREBASE_H

// ========================
// CONFIGURACIÓN FIREBASE 
// ========================

// Pega aquí tu API Key (desde Firebase Console)
#define FIREBASE_API_KEY "AIzaSyCsTgwJ7Z6GV54C6Sn7Af9NpjiGTSiAlO4"

// Pega aquí tu Project ID (desde Firebase Console)
#define FIREBASE_PROJECT_ID "datos-sensores-acuacol"

// URL de Firestore REST API 
#define FIRESTORE_URL "https://firestore.googleapis.com/v1/projects/" FIREBASE_PROJECT_ID "/databases/(default)/documents/sensores?key=" FIREBASE_API_KEY

#endif