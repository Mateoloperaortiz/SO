#include "generador.h"
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <random>    // Generadores aleatorios modernos
#include <vector>
#include <algorithm> // Para find_if
#include <unordered_map>
#include <cstdio>    // sscanf

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
    // Decide aleatoriamente si es hombre o mujer
    bool esHombre = rand() % 2;
    
    // Selecciona nombre según género
    std::string nombre = esHombre ? 
        nombresMasculinos[rand() % nombresMasculinos.size()] :
        nombresFemeninos[rand() % nombresFemeninos.size()];
    
    // Combina dos apellidos aleatorios
    std::string apellido = apellidos[rand() % apellidos.size()] + " " + 
                           apellidos[rand() % apellidos.size()];
    
    // Genera identificadores únicos
    std::string id = generarID();
    // Ciudad aleatoria de Colombia
    std::string ciudad = ciudadesColombia[rand() % ciudadesColombia.size()];
    // Fecha aleatoria
    std::string fecha = generarFechaNacimiento();
    
    // --- Generación de datos económicos realistas ---
    // Ingresos entre 10 millones y 500 millones COP
    double ingresos = randomDouble(10000000, 500000000);
    // Patrimonio entre 0 y 2 mil millones COP
    double patrimonio = randomDouble(0, 2000000000);
    // Deudas hasta el 70% del patrimonio
    double deudas = randomDouble(0, patrimonio * 0.7);
    // 70% probabilidad de ser declarante si gana > 50 millones
    bool declarante = (ingresos > 50000000) && (rand() % 100 > 30);
    
    // Construye y retorna objeto Persona
    return Persona(nombre, apellido, id, ciudad, fecha, ingresos, patrimonio, deudas, declarante);
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
        if (persona.getId() == id) {
            return &persona; // Retorna dirección si encuentra coincidencia
        }
    }
    return nullptr; // Retorna nulo si no encuentra
}

// --- Utilidades y análisis ---

// Calcula edad a partir de una fecha en formato DD/MM/AAAA (retorna -1 si no puede parsear)
int calcularEdad(const std::string& fechaNacimiento) {
    int d = 0, m = 0, y = 0;
    if (std::sscanf(fechaNacimiento.c_str(), "%d/%d/%d", &d, &m, &y) != 3) {
        return -1;
    }
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int year = now->tm_year + 1900;
    int month = now->tm_mon + 1;
    int day = now->tm_mday;
    int age = year - y;
    if (month < m || (month == m && day < d)) {
        age--;
    }
    return age;
}

// Determina grupo tributario por los dos últimos dígitos del documento ('A','B','C', o 'X' si inválido)
char grupoCalendarioPorID(const std::string& id) {
    if (id.empty()) return 'X';
    std::string tail = id.size() >= 2 ? id.substr(id.size() - 2) : (std::string("0") + id);
    int last2 = 0;
    try {
        last2 = std::stoi(tail);
    } catch (...) {
        return 'X';
    }
    if (last2 >= 0 && last2 <= 39) return 'A';
    if (last2 <= 79) return 'B';
    if (last2 <= 99) return 'C';
    return 'X';
}

// Persona más longeva (global)
const Persona* personaMasLongeva(const std::vector<Persona>& personas) {
    const Persona* mejor = nullptr;
    int mejorEdad = -1;
    for (const auto& p : personas) {
        int edad = calcularEdad(p.getFechaNacimiento());
        if (edad >= 0 && (mejor == nullptr || edad > mejorEdad)) {
            mejor = &p;
            mejorEdad = edad;
        }
    }
    return mejor;
}

// Persona más longeva por ciudad
std::unordered_map<std::string, const Persona*> personaMasLongevaPorCiudad(const std::vector<Persona>& personas) {
    std::unordered_map<std::string, const Persona*> res;
    std::unordered_map<std::string, int> mejorEdad;
    for (const auto& p : personas) {
        int edad = calcularEdad(p.getFechaNacimiento());
        if (edad < 0) continue;
        const std::string& ciudad = p.getCiudadNacimiento();
        auto it = mejorEdad.find(ciudad);
        if (it == mejorEdad.end() || edad > it->second) {
            mejorEdad[ciudad] = edad;
            res[ciudad] = &p;
        }
    }
    return res;
}

// Mayor patrimonio (global)
const Persona* mayorPatrimonioGlobal(const std::vector<Persona>& personas) {
    const Persona* mejor = nullptr;
    double mejorVal = -1.0;
    for (const auto& p : personas) {
        double v = p.getPatrimonio();
        if (mejor == nullptr || v > mejorVal) {
            mejor = &p;
            mejorVal = v;
        }
    }
    return mejor;
}

// Mayor patrimonio por ciudad
std::unordered_map<std::string, const Persona*> mayorPatrimonioPorCiudad(const std::vector<Persona>& personas) {
    std::unordered_map<std::string, const Persona*> res;
    std::unordered_map<std::string, double> mejorVal;
    for (const auto& p : personas) {
        const std::string& ciudad = p.getCiudadNacimiento();
        double v = p.getPatrimonio();
        auto it = mejorVal.find(ciudad);
        if (it == mejorVal.end() || v > it->second) {
            mejorVal[ciudad] = v;
            res[ciudad] = &p;
        }
    }
    return res;
}

// Mayor patrimonio por grupo tributario (A/B/C)
std::unordered_map<char, const Persona*> mayorPatrimonioPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, const Persona*> res;
    std::unordered_map<char, double> mejorVal;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.getId());
        if (g != 'A' && g != 'B' && g != 'C') continue;
        double v = p.getPatrimonio();
        auto it = mejorVal.find(g);
        if (it == mejorVal.end() || v > it->second) {
            mejorVal[g] = v;
            res[g] = &p;
        }
    }
    return res;
}

// Conteo de personas por grupo tributario (A/B/C)
std::unordered_map<char, size_t> conteoPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, size_t> cnt;
    cnt['A'] = 0; cnt['B'] = 0; cnt['C'] = 0;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'A' || g == 'B' || g == 'C') {
            cnt[g]++;
        }
    }
    return cnt;
}

// Conteo de declarantes por grupo tributario (A/B/C)
std::unordered_map<char, size_t> conteoDeclarantesPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, size_t> cnt;
    cnt['A'] = 0; cnt['B'] = 0; cnt['C'] = 0;
    for (const auto& p : personas) {
        if (!p.getDeclaranteRenta()) continue;
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'A' || g == 'B' || g == 'C') {
            cnt[g]++;
        }
    }
    return cnt;
}

// Listado de declarantes por grupo tributario (A/B/C)
std::unordered_map<char, std::vector<const Persona*>> listarDeclarantesPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, std::vector<const Persona*>> res;
    res['A'] = {}; res['B'] = {}; res['C'] = {};
    for (const auto& p : personas) {
        if (!p.getDeclaranteRenta()) continue;
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'A' || g == 'B' || g == 'C') {
            res[g].push_back(&p);
        }
    }
    return res;
}

// Conteo de IDs inválidos según la asignación de grupo (devuelve cantidad)
size_t conteoInvalidosGrupo(const std::vector<Persona>& personas) {
    size_t invalidos = 0;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.getId());
        if (g != 'A' && g != 'B' && g != 'C') {
            invalidos++;
        }
    }
    return invalidos;
}
