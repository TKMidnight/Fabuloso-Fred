#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using namespace std;

#define NUM_CASILLAS 3
#define MAX_NIVELES 9
#define BUTTON_SIZE 80
#define BUTTON_SPACING 30
#define MATRIX_SIZE (NUM_CASILLAS * (BUTTON_SIZE + BUTTON_SPACING))

// Uso de enums, union y structs
enum Colores {
    ROJO,
    VERDE,
    AMARILLO,
    AZUL,
    CIAN,
    MAGENTA,
    BLANCO,
    GRIS_OSCURO,
    NARANJA,
    NEGRO
};

enum MenuOption {
    JUGAR,
    OPCIONES,
    AYUDA,
    RANKING,
    SALIR,
    MENU_OPTIONS_COUNT
};

enum OptionsMenu {
    SILENCIAR_MUSICA,
    SILENCIAR_EFECTOS,
    VOLVER,
    OPTIONS_MENU_COUNT
};

enum HelpMenu {
    VOLVER_DE_AYUDA,
    HELP_MENU_COUNT
};

union EstadoCasilla {
    bool vacia;
    bool seleccionada;
};

struct Casilla {
    int fila;
    int columna;
    Colores color;
};

struct Puntaje {
    char nombre[50];
    int puntaje;
};

// Uso de funciones

// Funcion para comparar puntajes
bool compararPuntajes(const Puntaje &a, const Puntaje &b) {
    return a.puntaje > b.puntaje;
}

// Funcion para obtener el color de Allegro correspondiente al enum de colores
ALLEGRO_COLOR obtenerColor(Colores color) {
    switch (color) {
        case ROJO: return al_map_rgb(255, 85, 85);
        case VERDE: return al_map_rgb(85, 255, 85);
        case AMARILLO: return al_map_rgb(255, 255, 85);
        case AZUL: return al_map_rgb(85, 85, 255);
        case CIAN: return al_map_rgb(85, 255, 255);
        case MAGENTA: return al_map_rgb(255, 85, 255);
        case BLANCO: return al_map_rgb(255, 255, 255);
        case GRIS_OSCURO: return al_map_rgb(85, 85, 85);
        case NARANJA: return al_map_rgb(255, 165, 0);
        case NEGRO: return al_map_rgb(0, 0, 0);
        default: return al_map_rgb(0, 0, 0);
    }
}

// Funcion para dibujar la matriz de colores en la pantalla
void dibujarMatriz(int filaCursor, int colCursor, Casilla* casillas, ALLEGRO_BITMAP* background, int nivel, int puntaje, const char* nombreUsuario, ALLEGRO_FONT* font, bool mostrarCursor = true, bool mostrarMensajeCorrecto = false, ALLEGRO_BITMAP* gif_frame = NULL) {
    al_draw_bitmap(background, 0, 0, 0);

    // Obtener el tamaño de la pantalla
    int display_width = al_get_display_width(al_get_current_display());
    int display_height = al_get_display_height(al_get_current_display());
    // Calcular la posicion inicial de la matriz
    int start_x = (display_width - MATRIX_SIZE) / 2 + BUTTON_SPACING / 2;
    int start_y = (display_height - MATRIX_SIZE) / 2 + BUTTON_SPACING / 2;

    // Dibujar informacion del nivel y puntaje
    if (font) {
        al_draw_textf(font, obtenerColor(BLANCO), display_width / 2, start_y - 90, ALLEGRO_ALIGN_CENTRE, "Jugador: %s", nombreUsuario);
        al_draw_textf(font, obtenerColor(BLANCO), display_width / 2, start_y - 60, ALLEGRO_ALIGN_CENTRE, "Nivel: %d", nivel);
        al_draw_textf(font, obtenerColor(BLANCO), display_width / 2, start_y - 30, ALLEGRO_ALIGN_CENTRE, "Puntaje: %d", puntaje);
        if (mostrarMensajeCorrecto) {
            al_draw_text(font, obtenerColor(VERDE), display_width / 2, display_height - 40, ALLEGRO_ALIGN_CENTRE, "¡Correcto!");
        }
    }

    // Dibujar cada casilla de la matriz
    for (int i = 0; i < NUM_CASILLAS; i++) {
        for (int j = 0; j < NUM_CASILLAS; j++) {
            int indice = i * NUM_CASILLAS + j;
            ALLEGRO_COLOR color = obtenerColor(casillas[indice].color);
            int x1 = start_x + j * (BUTTON_SIZE + BUTTON_SPACING);
            int y1 = start_y + i * (BUTTON_SIZE + BUTTON_SPACING);
            int x2 = x1 + BUTTON_SIZE;
            int y2 = y1 + BUTTON_SIZE;

            // Dibujar sombra
            al_draw_filled_rectangle(x1 + 5, y1 + 5, x2 + 5, y2 + 5, obtenerColor(GRIS_OSCURO));

            // Dibujar boton
            al_draw_filled_rectangle(x1, y1, x2, y2, color);

            // Dibujar borde
            al_draw_rectangle(x1, y1, x2, y2, obtenerColor(NEGRO), 4);

            // Dibujar el cursor si esta habilitado
            if (i == filaCursor && j == colCursor && mostrarCursor) {
                al_draw_rectangle(x1 - 4, y1 - 4, x2 + 4, y2 + 4, obtenerColor(AMARILLO), 4);
            }
        }
    }

    if (gif_frame) {
        al_draw_bitmap(gif_frame, 10, display_height - 110, 0);
    }

    al_flip_display();
}

// Funcion para dibujar el menú principal
void draw_menu(ALLEGRO_FONT* font, int selected_option, ALLEGRO_BITMAP* background, ALLEGRO_BITMAP* logo, ALLEGRO_BITMAP* gif_frame = NULL) {
    al_draw_bitmap(background, 0, 0, 0);

    int logo_width = al_get_bitmap_width(logo);
    int logo_height = al_get_bitmap_height(logo);
    al_draw_bitmap(logo, (640 - logo_width) / 2, 50, 0);

    // Opciones del menu
    const char* menu_options[MENU_OPTIONS_COUNT] = {
        "JUGAR",
        "OPCIONES",
        "AYUDA",
        "RANKING",
        "SALIR"
    };

    // Dibujar cada opción del menu
    for (int i = 0; i < MENU_OPTIONS_COUNT; ++i) {
        ALLEGRO_COLOR color = (i == selected_option) ? obtenerColor(CIAN) : obtenerColor(GRIS_OSCURO); // Cian para la opción seleccionada
        al_draw_text(font, color, 320, (240) + i * 40, ALLEGRO_ALIGN_CENTRE, menu_options[i]);
    }

    // Dibujar la imagen adicional si esta disponible
    if (gif_frame) {
        al_draw_bitmap(gif_frame, 10, 380, 0);
    }

    al_flip_display();
}

// Funcion para dibujar el menú de opciones
void draw_options_menu(ALLEGRO_FONT* font, int selected_option, ALLEGRO_BITMAP* background, bool mute_music, bool mute_effects) {
    al_draw_bitmap(background, 0, 0, 0);

    // Opciones del menu de opciones
    const char* options_menu[OPTIONS_MENU_COUNT] = {
        "Silenciar Música: ",
        "Silenciar Efectos: ",
        "Volver"
    };

    // Convertir los estados de silencio a cadenas de caracteres
    string mute_music_str = mute_music ? "Si" : "No";
    string mute_effects_str = mute_effects ? "Si" : "No";

    // Dibujar cada opción del menú de opciones
    for (int i = 0; i < OPTIONS_MENU_COUNT; ++i) {
        ALLEGRO_COLOR color = (i == selected_option) ? obtenerColor(CIAN) : obtenerColor(GRIS_OSCURO);
        if (i == SILENCIAR_MUSICA) {
            al_draw_textf(font, color, 320, (240) + i * 40, ALLEGRO_ALIGN_CENTRE, "%s%s", options_menu[i], mute_music_str.c_str());
        } else if (i == SILENCIAR_EFECTOS) {
            al_draw_textf(font, color, 320, (240) + i * 40, ALLEGRO_ALIGN_CENTRE, "%s%s", options_menu[i], mute_effects_str.c_str());
        } else {
            al_draw_text(font, color, 320, (240) + i * 40, ALLEGRO_ALIGN_CENTRE, options_menu[i]);
        }
    }

    al_flip_display();
}

// Funcion para dibujar el menu de ayuda
void draw_help_menu(ALLEGRO_FONT* font, ALLEGRO_FONT* small_font, ALLEGRO_BITMAP* background) {
    al_draw_bitmap(background, 0, 0, 0);

    // Texto de ayuda
    const char* help_text[] = {
        "INSTRUCCIONES:",
        "1. Observa la secuencia de colores mostrada.",
        "2. Usa las teclas de flecha para mover el cursor.",
        "3. Presiona ENTER para seleccionar la casilla.",
        "4. Repite la secuencia en el mismo orden.",
        "",
        "Presiona ENTER para volver."
    };

    // Dibujar cada línea del texto de ayuda
    int y_offset = 100;
    for (int i = 0; i < 7; ++i) {
        al_draw_text(small_font, obtenerColor(BLANCO), 320, y_offset, ALLEGRO_ALIGN_CENTRE, help_text[i]);
        y_offset += 30; // Ajustar el espaciado entre líneas para acomodar el texto
    }

    al_flip_display();
}

// Funcion para dibujar el menu de ranking
void draw_ranking_menu(ALLEGRO_FONT* font, ALLEGRO_FONT* small_font, ALLEGRO_BITMAP* background, const vector<Puntaje>& rankings) {
    al_draw_bitmap(background, 0, 0, 0);

    al_draw_text(font, obtenerColor(BLANCO), 320, 50, ALLEGRO_ALIGN_CENTRE, "RANKING");

    int y_offset = 100;
    for (size_t i = 0; i < rankings.size() && i < 10; ++i) {
        al_draw_textf(small_font, obtenerColor(BLANCO), 320, y_offset, ALLEGRO_ALIGN_CENTRE, "%d. %s - %d", (int)i + 1, rankings[i].nombre, rankings[i].puntaje);
        y_offset += 30;
    }

    // Instruccion para volver al menu principal
    al_draw_text(small_font, obtenerColor(BLANCO), 320, y_offset + 20, ALLEGRO_ALIGN_CENTRE, "Presiona ENTER para volver");

    al_flip_display();
}

// Funcion para generar una secuencia aleatoria de longitud dada
int* generarSecuencia(int longitud) {
    int* secuencia = new int[longitud];
    for (int i = 0; i < longitud; i++) {
        secuencia[i] = rand() % (NUM_CASILLAS * NUM_CASILLAS);
    }
    return secuencia;
}

// Funcion para obtener la tecla presionada
int obtenerTecla(ALLEGRO_EVENT_QUEUE* event_queue) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue, &ev);

    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        return ev.keyboard.keycode;
    }

    return 0;
}

// Funcion para obtener la secuencia ingresada por el usuario
int* obtenerSecuenciaUsuario(int longitud, Casilla* casillas, ALLEGRO_BITMAP* background, int nivel, int puntaje, const char* nombreUsuario, ALLEGRO_FONT* font, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_SAMPLE** color_sounds, bool mute_effects, ALLEGRO_BITMAP* gif_frame = NULL) {
    int* secuenciaUsuario = new int[longitud];

    int filaCursor = 0, colCursor = 0;
    int contador = 0;

    // Dibujar la matriz sin el cursor
    dibujarMatriz(-1, -1, casillas, background, nivel, puntaje, nombreUsuario, font, false, false, gif_frame);

    // Capturar la secuencia del usuario
    while (contador < longitud) {
        int tecla = obtenerTecla(event_queue);
        switch (tecla) {
            case ALLEGRO_KEY_UP:
                if (filaCursor > 0) filaCursor--;
                break;
            case ALLEGRO_KEY_DOWN:
                if (filaCursor < NUM_CASILLAS - 1) filaCursor++;
                break;
            case ALLEGRO_KEY_LEFT:
                if (colCursor > 0) colCursor--;
                break;
            case ALLEGRO_KEY_RIGHT:
                if (colCursor < NUM_CASILLAS - 1) colCursor++;
                break;
            case ALLEGRO_KEY_ENTER:
                secuenciaUsuario[contador] = filaCursor * NUM_CASILLAS + colCursor;
                contador++;
                if (!mute_effects) al_play_sample(color_sounds[casillas[filaCursor * NUM_CASILLAS + colCursor].color], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                dibujarMatriz(filaCursor, colCursor, casillas, background, nivel, puntaje, nombreUsuario, font, false, false, gif_frame);
                break;
        }
        dibujarMatriz(filaCursor, colCursor, casillas, background, nivel, puntaje, nombreUsuario, font, true, false, gif_frame);
    }
    return secuenciaUsuario;
}

// Funcion para comparar las secuencias generada y la ingresada por el usuario
bool compararSecuencias(int* secuencia, int* secuenciaUsuario, int longitud) {
    for (int i = 0; i < longitud; i++) {
        if (secuencia[i] != secuenciaUsuario[i]) {
            return false;
        }
    }
    return true;
}

// Funcion para guardar el puntaje en un archivo de texto
void guardarPuntaje(const char* nombre, int puntaje) {
    FILE* archivo = fopen("puntajes.txt", "a");
    if (archivo) {
        fprintf(archivo, "%s: %d\n", nombre, puntaje);
        fclose(archivo);
        cout << "Puntaje guardado correctamente." << endl;
    } else {
        cerr << "No se pudo abrir el archivo para escribir los puntajes." << endl;
    }
}

// Funcion para cargar los puntajes desde un archivo de texto
vector<Puntaje> cargarPuntajes() {
    vector<Puntaje> rankings;
    FILE* archivo = fopen("puntajes.txt", "r");
    if (archivo) {
        char linea[100];
        while (fgets(linea, sizeof(linea), archivo)) {
            Puntaje p;
            sscanf(linea, "%[^:]: %d", p.nombre, &p.puntaje);
            rankings.push_back(p);
        }
        fclose(archivo);
        sort(rankings.begin(), rankings.end(), compararPuntajes);
    }
    return rankings;
}

// Funcion para obtener el nombre del usuario
string obtenerNombreUsuario(ALLEGRO_FONT* font, ALLEGRO_BITMAP* background, ALLEGRO_EVENT_QUEUE* event_queue) {
    string nombreUsuario = "";
    ALLEGRO_EVENT ev;

    // Capturar el nombre del usuario
    while (true) {
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (ev.keyboard.unichar == '\r') {
                break;
            } else if (ev.keyboard.unichar == '\b') {
                if (!nombreUsuario.empty()) {
                    nombreUsuario.pop_back();
                }
            } else if (ev.keyboard.unichar >= 32 && ev.keyboard.unichar <= 126) {
                nombreUsuario += ev.keyboard.unichar;
            }
        }

        al_draw_bitmap(background, 0, 0, 0);
        al_draw_text(font, obtenerColor(BLANCO), 320, 200, ALLEGRO_ALIGN_CENTRE, "Introduce tu nombre:");
        al_draw_text(font, obtenerColor(BLANCO), 320, 240, ALLEGRO_ALIGN_CENTRE, nombreUsuario.c_str());
        al_flip_display();
    }

    return nombreUsuario;
}

// Funcion para jugar un nivel
void jugarNivel(int nivel, Casilla* casillas, ALLEGRO_FONT* font, ALLEGRO_BITMAP* background, ALLEGRO_EVENT_QUEUE* event_queue, int& puntaje, const char* nombreUsuario, ALLEGRO_SAMPLE** color_sounds, bool mute_effects, ALLEGRO_BITMAP* gif_frame = NULL) {
    srand(time(0));
    int* secuencia = generarSecuencia(nivel);

    cout << "Nivel " << nivel << "!" << endl;

    al_rest(2.0);

    // Mostrar la secuencia al usuario
    for (int i = 0; i < nivel; i++) {
        int casilla = secuencia[i];
        int fila = casilla / NUM_CASILLAS;
        int col = casilla % NUM_CASILLAS;

        if (!mute_effects) al_play_sample(color_sounds[casillas[casilla].color], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        dibujarMatriz(fila, col, casillas, background, nivel, puntaje, nombreUsuario, font, true, false, gif_frame);
        al_rest(1.0);
    }

    dibujarMatriz(0, 0, casillas, background, nivel, puntaje, nombreUsuario, font, true, false, gif_frame);
    int* secuenciaUsuario = obtenerSecuenciaUsuario(nivel, casillas, background, nivel, puntaje, nombreUsuario, font, event_queue, color_sounds, mute_effects, gif_frame);

    // Verificar si la secuencia ingresada por el usuario es correcta
    if (compararSecuencias(secuencia, secuenciaUsuario, nivel)) {
        cout << "¡Correcto!" << endl;
        puntaje += 100; 
        dibujarMatriz(0, 0, casillas, background, nivel, puntaje, nombreUsuario, font, true, true, gif_frame);
        al_rest(1.0); 
    } else {
        cout << "Incorrecto. La secuencia correcta era: ";
        for (int i = 0; i < nivel; i++) {
            cout << secuencia[i] << " ";
        }
        cout << "\n";
        delete[] secuencia;
        delete[] secuenciaUsuario;
        throw runtime_error("Juego Terminado");
    }

    delete[] secuencia;
    delete[] secuenciaUsuario;
}

// Funcion para ajustar el volumen de la musica
void ajustarVolumenMusica(ALLEGRO_SAMPLE_INSTANCE* music_instance, float volumen) {
    al_set_sample_instance_gain(music_instance, volumen);
}

// el main
int main() {
    // Inicializar Allegro y sus addons
    if (!al_init()) {
        fprintf(stderr, "Failed to initialize Allegro.\n");
        return -1;
    }

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Failed to initialize primitives addon.\n");
        return -1;
    }

    if (!al_init_font_addon()) {
        fprintf(stderr, "Failed to initialize font addon.\n");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        fprintf(stderr, "Failed to initialize ttf addon.\n");
        return -1;
    }

    if (!al_init_image_addon()) {
        fprintf(stderr, "Failed to initialize image addon.\n");
        return -1;
    }

    if (!al_install_audio()) {
        fprintf(stderr, "Failed to initialize audio addon.\n");
        return -1;
    }

    if (!al_init_acodec_addon()) {
        fprintf(stderr, "Failed to initialize audio codec addon.\n");
        return -1;
    }

    if (!al_reserve_samples(12)) {  // Incrementar el numero de muestras reservadas
        fprintf(stderr, "Failed to reserve samples.\n");
        return -1;
    }

    if (!al_install_keyboard()) {
        fprintf(stderr, "Failed to install keyboard.\n");
        return -1;
    }

    // Crear la ventana de visualizacion
    ALLEGRO_DISPLAY* display = al_create_display(640, 480);
    if (!display) {
        fprintf(stderr, "Failed to create display.\n");
        return -1;
    }

    // Cargar fuentes
    ALLEGRO_FONT* font = al_load_ttf_font("PressStart2P-Regular.ttf", 24, 0);
    if (!font) {
        fprintf(stderr, "Could not load 'PressStart2P-Regular.ttf'.\n");
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_FONT* small_font = al_load_ttf_font("PressStart2P-Regular.ttf", 18, 0);
    if (!small_font) {
        fprintf(stderr, "Could not load 'PressStart2P-Regular.ttf'.\n");
        al_destroy_display(display);
        al_destroy_font(font);
        return -1;
    }

    // Cargar imagenes
    ALLEGRO_BITMAP* background = al_load_bitmap("background.png");
    if (!background) {
        fprintf(stderr, "Failed to load background image.\n");
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_BITMAP* logo = al_load_bitmap("logo.png");
    if (!logo) {
        fprintf(stderr, "Failed to load logo image.\n");
        al_destroy_bitmap(background);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_BITMAP* gif_frame = al_load_bitmap("bonzi.png"); // Cargar la imagen de Bonzi
    if (!gif_frame) {
        fprintf(stderr, "Failed to load bonzi image.\n");
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    // Cargar musica y sonidos
    ALLEGRO_SAMPLE* music = al_load_sample("music.ogg");
    if (!music) {
        fprintf(stderr, "Failed to load music.\n");
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_bitmap(gif_frame);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_SAMPLE* menu_sound = al_load_sample("menu_sound.ogg");
    if (!menu_sound) {
        fprintf(stderr, "Failed to load menu sound.\n");
        al_destroy_sample(music);
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_bitmap(gif_frame);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_SAMPLE* enter_sound = al_load_sample("enter_sound.ogg");
    if (!enter_sound) {
        fprintf(stderr, "Failed to load enter sound.\n");
        al_destroy_sample(menu_sound);
        al_destroy_sample(music);
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_bitmap(gif_frame);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    // Cargar sonidos de colores
    ALLEGRO_SAMPLE* color_sounds[9];
    color_sounds[ROJO] = al_load_sample("red.ogg");
    color_sounds[VERDE] = al_load_sample("green.ogg");
    color_sounds[AMARILLO] = al_load_sample("yellow.ogg");
    color_sounds[AZUL] = al_load_sample("blue.ogg");
    color_sounds[CIAN] = al_load_sample("cyan.ogg");
    color_sounds[MAGENTA] = al_load_sample("magenta.ogg");
    color_sounds[BLANCO] = al_load_sample("white.ogg");
    color_sounds[GRIS_OSCURO] = al_load_sample("gray.ogg");
    color_sounds[NARANJA] = al_load_sample("orange.ogg");

    // Verificar que todos los sonidos se cargaron correctamente
    for (int i = 0; i < 9; i++) {
        if (!color_sounds[i]) {
            fprintf(stderr, "Failed to load color sound %d.\n", i);
            for (int j = 0; j < i; j++) {
                al_destroy_sample(color_sounds[j]);
            }
            al_destroy_sample(enter_sound);
            al_destroy_sample(menu_sound);
            al_destroy_sample(music);
            al_destroy_bitmap(logo);
            al_destroy_bitmap(background);
            al_destroy_bitmap(gif_frame);
            al_destroy_font(font);
            al_destroy_font(small_font);
            al_destroy_display(display);
            return -1;
        }
    }

    // Crear una instancia de la musica
    ALLEGRO_SAMPLE_INSTANCE* music_instance = al_create_sample_instance(music);
    if (!music_instance) {
        fprintf(stderr, "Failed to create music instance.\n");
        for (int i = 0; i < 9; i++) {
            al_destroy_sample(color_sounds[i]);
        }
        al_destroy_sample(enter_sound);
        al_destroy_sample(menu_sound);
        al_destroy_sample(music);
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_bitmap(gif_frame);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    // Adjuntar la instancia de música al mezclador por defecto y establecer el modo de reproducción en bucle
    al_attach_sample_instance_to_mixer(music_instance, al_get_default_mixer());
    al_set_sample_instance_playmode(music_instance, ALLEGRO_PLAYMODE_LOOP);

    if (!al_play_sample_instance(music_instance)) {
        fprintf(stderr, "Failed to play music.\n");
        al_destroy_sample_instance(music_instance);
        for (int i = 0; i < 9; i++) {
            al_destroy_sample(color_sounds[i]);
        }
        al_destroy_sample(enter_sound);
        al_destroy_sample(menu_sound);
        al_destroy_sample(music);
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_bitmap(gif_frame);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    // Crear cola de eventos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) {
        fprintf(stderr, "Failed to create event_queue.\n");
        al_destroy_sample_instance(music_instance);
        for (int i = 0; i < 9; i++) {
            al_destroy_sample(color_sounds[i]);
        }
        al_destroy_sample(enter_sound);
        al_destroy_sample(menu_sound);
        al_destroy_sample(music);
        al_destroy_bitmap(logo);
        al_destroy_bitmap(background);
        al_destroy_bitmap(gif_frame);
        al_destroy_font(font);
        al_destroy_font(small_font);
        al_destroy_display(display);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    int selected_option = JUGAR;
    int selected_option_menu = SILENCIAR_MUSICA;
    int selected_option_help = VOLVER_DE_AYUDA;
    bool do_exit = false;
    string nombreUsuario;
    bool nombre_obtenido = false;
    bool in_options_menu = false;
    bool in_help_menu = false;
    bool in_ranking_menu = false;
    bool mute_music = false;
    bool mute_effects = false;
    bool show_gif = false;

    // Aqui es para obtener el nombre del usuario antes de mostrar el menu
    nombreUsuario = obtenerNombreUsuario(font, background, event_queue);
    if (nombreUsuario == "bonzi") {
        show_gif = true;
    }
    nombre_obtenido = true;

    // Cargar los puntajes desde el archivo
    vector<Puntaje> rankings = cargarPuntajes();

    // Bucle principal del juego
    while (!do_exit) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (in_options_menu) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        selected_option_menu = (selected_option_menu > 0) ? selected_option_menu - 1 : OPTIONS_MENU_COUNT - 1;
                        if (!mute_effects) al_play_sample(menu_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        break;
                    case ALLEGRO_KEY_DOWN:
                        selected_option_menu = (selected_option_menu + 1) % OPTIONS_MENU_COUNT;
                        if (!mute_effects) al_play_sample(menu_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if (!mute_effects) al_play_sample(enter_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        if (selected_option_menu == VOLVER) {
                            in_options_menu = false;
                        } else if (selected_option_menu == SILENCIAR_MUSICA) {
                            mute_music = !mute_music;
                            if (mute_music) {
                                al_stop_sample_instance(music_instance);
                            } else {
                                al_play_sample_instance(music_instance);
                            }
                        } else if (selected_option_menu == SILENCIAR_EFECTOS) {
                            mute_effects = !mute_effects;
                        }
                        break;
                }
                draw_options_menu(font, selected_option_menu, background, mute_music, mute_effects);
            } else if (in_help_menu) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (!mute_effects) al_play_sample(enter_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    in_help_menu = false;
                }
                draw_help_menu(font, small_font, background);
            } else if (in_ranking_menu) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (!mute_effects) al_play_sample(enter_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    in_ranking_menu = false;
                }
                draw_ranking_menu(font, small_font, background, rankings);
            } else {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        selected_option = (selected_option > 0) ? selected_option - 1 : MENU_OPTIONS_COUNT - 1;
                        if (!mute_effects) al_play_sample(menu_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        break;
                    case ALLEGRO_KEY_DOWN:
                        selected_option = (selected_option + 1) % MENU_OPTIONS_COUNT;
                        if (!mute_effects) al_play_sample(menu_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if (!mute_effects) al_play_sample(enter_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        if (selected_option == SALIR) {
                            do_exit = true;
                        } else if (selected_option == JUGAR) {
                
                            ajustarVolumenMusica(music_instance, 0.3);

                            // Definir las casillas de la matriz
                            Casilla casillas[NUM_CASILLAS * NUM_CASILLAS] = {
                                {0, 0, ROJO}, {0, 1, VERDE}, {0, 2, AMARILLO},
                                {1, 0, AZUL}, {1, 1, CIAN}, {1, 2, MAGENTA},
                                {2, 0, BLANCO}, {2, 1, GRIS_OSCURO}, {2, 2, NARANJA}
                            };

                            // Inicializar nivel y puntaje
                            int nivel = 1;
                            int puntaje = 0;
                            try {
                                while (nivel <= MAX_NIVELES) {
                                    jugarNivel(nivel, casillas, font, background, event_queue, puntaje, nombreUsuario.c_str(), color_sounds, mute_effects, show_gif ? gif_frame : NULL);
                                    nivel++;
                                }
                                cout << "¡Felicidades, " << nombreUsuario << "! Has completado todos los niveles." << endl;
                            } catch (const runtime_error& e) {
                                cout << "Juego Terminado. Nivel alcanzado: " << nivel - 1 << endl;
                            }

                            // Guardar el puntaje del usuario
                            guardarPuntaje(nombreUsuario.c_str(), puntaje);
                            nombre_obtenido = false; // Reset para permitir jugar de nuevo con otro usuario

                            // Cargar los puntajes actualizados
                            rankings = cargarPuntajes();

                            // Restaurar el volumen de la musica después de jugar
                            ajustarVolumenMusica(music_instance, 1.0);
                        } else if (selected_option == OPCIONES) {
                            in_options_menu = true;
                        } else if (selected_option == AYUDA) {
                            in_help_menu = true;
                        } else if (selected_option == RANKING) {
                            in_ranking_menu = true;
                        }
                        break;
                }
                draw_menu(font, selected_option, background, logo, show_gif ? gif_frame : NULL);
            }
        }
    }

    // Liberar recursos y destruir instancias
    al_destroy_event_queue(event_queue);
    al_destroy_sample_instance(music_instance);
    for (int i = 0; i < 9; i++) {
        al_destroy_sample(color_sounds[i]);
    }
    al_destroy_sample(enter_sound);
    al_destroy_sample(menu_sound);
    al_destroy_sample(music);
    al_destroy_bitmap(logo);
    al_destroy_bitmap(background);
    al_destroy_bitmap(gif_frame);
    al_destroy_font(font);
    al_destroy_font(small_font);
    al_destroy_display(display);

    return 0;
}
