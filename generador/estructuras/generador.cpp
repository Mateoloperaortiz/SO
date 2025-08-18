#include "generador.h"
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <random>    // Generadores aleatorios modernos
#include <vector>
#include <algorithm> // Para find_if
#include <unordered_map>
#include <string>
#include <cstdio>    // std::sscanf

// --- Bases de datos para generación realista ---

// Nombres femeninos comunes en Colombia
const std::vector<std::string> nombresFemeninos = {
    "María", "Luisa", "Carmen", "Ana", "Sofía", "Isabel", "Laura", "Andrea", "Paula", "Valentina",
    "Camila", "Daniela", "Carolina", "Fernanda", "Gabriela", "Patricia", "Claudia", "Diana", "Lucía", "Ximena"
};

// Nombres masculinos comunes en Colombia
const std::vector<std::string> nombresMasculinos = {
    "Juan", "Carlos", "José", "James", "Andrés", "Miguel", "Luis", "Pedro", "Alejandro", "Ricardo",
    "Felipe", "David", "Jorge", "Santiago", "Daniel", "Fernando", "Diego", "Rafael", "Martín", "Óscar",
    "Edison", "Sofia","Camila","Juana","Ana","Laura","Karla","Andrea","Daniela","Alejandra","Martina",
    "Nelly","María","Nestor","Trinidad","Fernanda", "Carolina", "Lina", "Gertridis"
};

// Apellidos comunes en Colombia
const std::vector<std::string> apellidos = {
    "Gómez", "Rodríguez", "Martínez", "López", "García", "Pérez", "González", "Sánchez", "Ramírez", "Torres",
    "Díaz", "Vargas", "Castro", "Ruiz", "Álvarez", "Romero", "Suárez", "Rojas", "Moreno", "Muñoz", "Valencia",
};

// Principales ciudades colombianas
const std::vector<std::string> ciudadesColombia = {
    "Bogotá", "Medellín", "Cali", "Barranquilla", "Cartagena", "Bucaramanga", "Pereira", "Santa Marta", "Cúcuta", "Ibagué",
    "Manizales", "Pasto", "Neiva", "Villavicencio", "Armenia", "Sincelejo", "Valledupar", "Montería", "Popayán", "Tunja"
};

// Implementación de funciones generadoras

std::string generarFechaNacimiento() {
    // Genera día aleatorio (1-28 para simplificar)
    int dia = 1 + rand() % 28;
    // Mes aleatorio (1-12)
    int mes = 1 + rand() % 12;
    // Año entre 1960-2010
    int anio = 1960 + rand() % 50;
    
    // Convierte a string en formato DD/MM/AAAA
    return std::to_string(dia) + "/" + std::to_string(mes) + "/" + std::to_string(anio);
}

std::string generarID() {
    static long contador = 1000000000; // ID inicial
    return std::to_string(contador++); // Incrementa después de usar
}

double randomDouble(double min, double max) {
    // Generador moderno Mersenne Twister
    static std::mt19937 generator(time(nullptr));
    // Distribución uniforme en rango [min, max]
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

Persona generarPersona() {
    Persona p; // Crea una instancia de la estructura Persona
    
    // Decide aleatoriamente si es hombre o mujer
    bool esHombre = rand() % 2;
    
    // Selecciona nombre según género
    p.nombre = esHombre ? 
        nombresMasculinos[rand() % nombresMasculinos.size()] :
        nombresFemeninos[rand() % nombresFemeninos.size()];
    
    // Combina dos apellidos aleatorios
    p.apellido = apellidos[rand() % apellidos.size()] + " " + 
                 apellidos[rand() % apellidos.size()];
    
    // Genera identificadores únicos
    p.id = generarID();
    // Ciudad aleatoria de Colombia
    p.ciudadNacimiento = ciudadesColombia[rand() % ciudadesColombia.size()];
    // Fecha aleatoria
    p.fechaNacimiento = generarFechaNacimiento();
    
    // --- Generación de datos económicos realistas ---
    // Ingresos entre 10 millones y 500 millones COP
    p.ingresosAnuales = randomDouble(10000000, 500000000);
    // Patrimonio entre 0 y 2 mil millones COP
    p.patrimonio = randomDouble(0, 2000000000);
    // Deudas hasta el 70% del patrimonio
    p.deudas = randomDouble(0, p.patrimonio * 0.7);
    // 70% probabilidad de ser declarante si gana > 50 millones
    p.declaranteRenta = (p.ingresosAnuales > 50000000) && (rand() % 100 > 30);
    
    return p; // Retorna la estructura completa
}

std::vector<Persona> generarColeccion(int n) {
    std::vector<Persona> personas;
    // Reserva espacio para n personas (optimización)
    personas.reserve(n);
    
    // Genera n personas y las añade al vector
    for (int i = 0; i < n; ++i) {
        personas.push_back(generarPersona());
    }
    
    return personas;
}

const Persona* buscarPorID(const std::vector<Persona>& personas, const std::string& id) {
    // Búsqueda lineal por ID (solución simple para colecciones medianas)
    for (const auto& persona : personas) {
        if (persona.id == id) { // Acceso directo al campo id
            return &persona; // Retorna dirección si encuentra coincidencia
        }
    }
    return nullptr; // Retorna nulo si no encuentra
}

// --- Utilidades y análisis ---

int calcularEdad(const std::string& fechaNacimiento) {
    int d = 0, m = 0, y = 0;
    if (std::sscanf(fechaNacimiento.c_str(), "%d/%d/%d", &d, &m, &y) != 3) {
        return -1;
    }
    if (y <= 0 || m < 1 || m > 12 || d < 1 || d > 31) {
        return -1;
    }
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int age = (now->tm_year + 1900) - y;
    if ((now->tm_mon + 1) < m || ((now->tm_mon + 1) == m && now->tm_mday < d)) {
        age--;
    }
    return age;
}

char grupoCalendarioPorID(const std::string& id) {
    if (id.size() < 2) return 'X';
    char c1 = id[id.size() - 2];
    char c2 = id[id.size() - 1];
    if (c1 < '0' || c1 > '9' || c2 < '0' || c2 > '9') return 'X';
    int ultDos = (c1 - '0') * 10 + (c2 - '0');
    if (ultDos >= 0 && ultDos <= 39) return 'A';
    if (ultDos >= 40 && ultDos <= 79) return 'B';
    if (ultDos >= 80 && ultDos <= 99) return 'C';
    return 'X';
}

const Persona* personaMasLongeva(const std::vector<Persona>& personas) {
    const Persona* mejor = nullptr;
    int mejorEdad = -1;
    for (const auto& p : personas) {
        int edad = calcularEdad(p.fechaNacimiento);
        if (edad > mejorEdad) {
            mejorEdad = edad;
            mejor = &p;
        }
    }
    return mejor;
}

std::unordered_map<std::string, const Persona*> personaMasLongevaPorCiudad(const std::vector<Persona>& personas) {
    std::unordered_map<std::string, const Persona*> res;
    std::unordered_map<std::string, int> edadMax;
    for (const auto& p : personas) {
        int edad = calcularEdad(p.fechaNacimiento);
        const auto& c = p.ciudadNacimiento;
        auto it = edadMax.find(c);
        if (it == edadMax.end() || edad > it->second) {
            edadMax[c] = edad;
            res[c] = &p;
        }
    }
    return res;
}

const Persona* mayorPatrimonioGlobal(const std::vector<Persona>& personas) {
    const Persona* mejor = nullptr;
    double maxPat = -1.0;
    for (const auto& p : personas) {
        if (!mejor || p.patrimonio > maxPat) {
            maxPat = p.patrimonio;
            mejor = &p;
        }
    }
    return mejor;
}

std::unordered_map<std::string, const Persona*> mayorPatrimonioPorCiudad(const std::vector<Persona>& personas) {
    std::unordered_map<std::string, const Persona*> res;
    std::unordered_map<std::string, double> maxPat;
    for (const auto& p : personas) {
        const auto& c = p.ciudadNacimiento;
        auto it = maxPat.find(c);
        if (it == maxPat.end() || p.patrimonio > it->second) {
            maxPat[c] = p.patrimonio;
            res[c] = &p;
        }
    }
    return res;
}

std::unordered_map<char, const Persona*> mayorPatrimonioPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, const Persona*> res;
    std::unordered_map<char, double> maxPat;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.id);
        if (g == 'X') continue;
        auto it = maxPat.find(g);
        if (it == maxPat.end() || p.patrimonio > it->second) {
            maxPat[g] = p.patrimonio;
            res[g] = &p;
        }
    }
    return res;
}

std::unordered_map<char, size_t> conteoPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, size_t> cnt;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.id);
        if (g == 'X') continue;
        cnt[g]++;
    }
    return cnt;
}

std::unordered_map<char, size_t> conteoDeclarantesPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, size_t> cnt;
    for (const auto& p : personas) {
        if (!p.declaranteRenta) continue;
        char g = grupoCalendarioPorID(p.id);
        if (g == 'X') continue;
        cnt[g]++;
    }
    return cnt;
}

std::unordered_map<char, std::vector<const Persona*>> listarDeclarantesPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, std::vector<const Persona*>> listas;
    for (const auto& p : personas) {
        if (!p.declaranteRenta) continue;
        char g = grupoCalendarioPorID(p.id);
        if (g == 'X') continue;
        listas[g].push_back(&p);
    }
    return listas;
}

size_t conteoInvalidosGrupo(const std::vector<Persona>& personas) {
    size_t c = 0;
    for (const auto& p : personas) {
        if (grupoCalendarioPorID(p.id) == 'X') c++;
    }
    return c;
}
