#include "generador.h"
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <random>    // std::mt19937, std::uniform_real_distribution
#include <vector>
#include <algorithm> // std::find_if
#include <unordered_map>
#include <string>
#include <cstdio>    // std::sscanf

// Bases de datos para generación realista

// Nombres femeninos comunes en Colombia
const std::vector<std::string> nombresFemeninos = {
    "María", "Luisa", "Carmen", "Ana", "Sofía", "Isabel", "Laura", "Andrea", "Paula", "Valentina",
    "Camila", "Daniela", "Carolina", "Fernanda", "Gabriela", "Patricia", "Claudia", "Diana", "Lucía", "Ximena"
};

// Nombres masculinos comunes en Colombia
const std::vector<std::string> nombresMasculinos = {
    "Juan", "Carlos", "José", "James", "Andrés", "Miguel", "Luis", "Pedro", "Alejandro", "Ricardo",
    "Felipe", "David", "Jorge", "Santiago", "Daniel", "Fernando", "Diego", "Rafael", "Martín", "Óscar",
    "Edison", "Nestor", "Gertridis"
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

/**
 * Implementación de generarFechaNacimiento.
 * 
 * POR QUÉ: Simular fechas de nacimiento realistas.
 * CÓMO: Día (1-28), mes (1-12), año (1960-2009).
 * PARA QUÉ: Atributo fechaNacimiento de Persona.
 */
std::string generarFechaNacimiento() {
    int dia = 1 + rand() % 28;       // Día: 1 a 28 (evita problemas con meses)
    int mes = 1 + rand() % 12;        // Mes: 1 a 12
    int anio = 1960 + rand() % 50;    // Año: 1960 a 2009
    return std::to_string(dia) + "/" + std::to_string(mes) + "/" + std::to_string(anio);
}

/**
 * Implementación de generarID.
 * 
 * POR QUÉ: Generar identificadores únicos y secuenciales.
 * CÓMO: Contador estático que inicia en 1000000000 y se incrementa.
 * PARA QUÉ: Simular números de cédula.
 */
std::string generarID() {
    static long contador = 1000000000; // Inicia en 1,000,000,000
    return std::to_string(contador++); // Convierte a string e incrementa
}

/**
 * Implementación de randomDouble.
 * 
 * POR QUÉ: Generar números decimales aleatorios en un rango.
 * CÓMO: Mersenne Twister (mejor que rand()) y distribución uniforme.
 * PARA QUÉ: Valores de ingresos, patrimonio, etc.
 */
double randomDouble(double min, double max) {
    static std::mt19937 generator(time(nullptr)); // Semilla basada en tiempo
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

/**
 * Implementación de generarPersona.
 * 
 * POR QUÉ: Crear una persona con datos aleatorios.
 * CÓMO: Seleccionando aleatoriamente de las bases de datos y generando números.
 * PARA QUÉ: Generar datos de prueba.
 */
Persona generarPersona() {
    // Decide si es hombre o mujer
    bool esHombre = rand() % 2;
    
    // Selecciona nombre según género
    std::string nombre = esHombre ? 
        nombresMasculinos[rand() % nombresMasculinos.size()] :
        nombresFemeninos[rand() % nombresFemeninos.size()];
    
    // Construye apellido compuesto (dos apellidos aleatorios)
    std::string apellido = apellidos[rand() % apellidos.size()];
    apellido += " ";
    apellido += apellidos[rand() % apellidos.size()];
    
    // Genera los demás atributos
    std::string id = generarID();
    std::string ciudad = ciudadesColombia[rand() % ciudadesColombia.size()];
    std::string fecha = generarFechaNacimiento();
    
    // Genera datos financieros realistas
    double ingresos = randomDouble(10000000, 500000000);   // 10M a 500M COP
    double patrimonio = randomDouble(0, 2000000000);       // 0 a 2,000M COP
    double deudas = randomDouble(0, patrimonio * 0.7);     // Deudas hasta el 70% del patrimonio
    bool declarante = (ingresos > 50000000) && (rand() % 100 > 30); // Probabilidad 70% si ingresos > 50M
    
    return Persona(nombre, apellido, id, ciudad, fecha, ingresos, patrimonio, deudas, declarante);
}

/**
 * Implementación de generarColeccion.
 * 
 * POR QUÉ: Generar un conjunto de n personas.
 * CÓMO: Reservando espacio y agregando n personas generadas.
 * PARA QUÉ: Crear datasets para pruebas.
 */
std::vector<Persona> generarColeccion(int n) {
    std::vector<Persona> personas;
    personas.reserve(n); // Reserva espacio para n personas (eficiencia)
    
    for (int i = 0; i < n; ++i) {
        personas.push_back(generarPersona());
    }
    
    return personas;
}

/**
 * Implementación de buscarPorID.
 * 
 * POR QUÉ: Encontrar una persona por su ID en una colección.
 * CÓMO: Usando un algoritmo de búsqueda secuencial (lineal).
 * PARA QUÉ: Para operaciones de búsqueda en la aplicación.
 */
const Persona* buscarPorID(const std::vector<Persona>& personas, const std::string& id) {
    // Usa find_if con una lambda para buscar por ID
    auto it = std::find_if(personas.begin(), personas.end(),
        [&id](const Persona& p) { return p.getId() == id; });
    
    if (it != personas.end()) {
        return &(*it); // Devuelve puntero a la persona encontrada
    } else {
        return nullptr; // No encontrado
    }
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
        int edad = calcularEdad(p.getFechaNacimiento());
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
        int edad = calcularEdad(p.getFechaNacimiento());
        const auto& c = p.getCiudadNacimiento();
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
        if (!mejor || p.getPatrimonio() > maxPat) {
            maxPat = p.getPatrimonio();
            mejor = &p;
        }
    }
    return mejor;
}

std::unordered_map<std::string, const Persona*> mayorPatrimonioPorCiudad(const std::vector<Persona>& personas) {
    std::unordered_map<std::string, const Persona*> res;
    std::unordered_map<std::string, double> maxPat;
    for (const auto& p : personas) {
        const auto& c = p.getCiudadNacimiento();
        auto it = maxPat.find(c);
        if (it == maxPat.end() || p.getPatrimonio() > it->second) {
            maxPat[c] = p.getPatrimonio();
            res[c] = &p;
        }
    }
    return res;
}

std::unordered_map<char, const Persona*> mayorPatrimonioPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, const Persona*> res;
    std::unordered_map<char, double> maxPat;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'X') continue;
        auto it = maxPat.find(g);
        if (it == maxPat.end() || p.getPatrimonio() > it->second) {
            maxPat[g] = p.getPatrimonio();
            res[g] = &p;
        }
    }
    return res;
}

std::unordered_map<char, size_t> conteoPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, size_t> cnt;
    for (const auto& p : personas) {
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'X') continue;
        cnt[g]++;
    }
    return cnt;
}

std::unordered_map<char, size_t> conteoDeclarantesPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, size_t> cnt;
    for (const auto& p : personas) {
        if (!p.getDeclaranteRenta()) continue;
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'X') continue;
        cnt[g]++;
    }
    return cnt;
}

std::unordered_map<char, std::vector<const Persona*>> listarDeclarantesPorGrupo(const std::vector<Persona>& personas) {
    std::unordered_map<char, std::vector<const Persona*>> listas;
    for (const auto& p : personas) {
        if (!p.getDeclaranteRenta()) continue;
        char g = grupoCalendarioPorID(p.getId());
        if (g == 'X') continue;
        listas[g].push_back(&p);
    }
    return listas;
}

size_t conteoInvalidosGrupo(const std::vector<Persona>& personas) {
    size_t c = 0;
    for (const auto& p : personas) {
        if (grupoCalendarioPorID(p.getId()) == 'X') c++;
    }
    return c;
}