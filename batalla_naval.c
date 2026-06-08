#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "oponente.h"

#define TAMANIO_TABLERO 10
#define CANTIDAD_BARCOS 5
#define LECTURA "r"
#define ESCRITURA "w"

const int COORDENADA_MINIMA = 1;

const char FORMATO_LECTURA[] = "%d;%d; %c;%d";
const int ARGUMENTO_BARCOS = 1;
const int ARGUMENTO_REPORTE = 2;

const int ERROR_LECTURA = 1;
const int ERROR_ESCRITURA = 2;
const int ERROR_ABRIR_ARCHIVO = 3;
const int ERROR_CANTIDAD_ARGUMENTOS = 4;
const int EXITO = 0;
const int INICIALIZACION_INVALIDA = -1;
const int ERROR_GENERAL = -1;
const int INICIALIZACION_VALIDA = 0;

const char AGUA = 'A';
const char TOCADO = 'T';
const char HUNDIDO = 'H';
const char BARCO = 'B';
const char MAPA_INCOGNITO = '.';

const int CANTIDAD_BARCOS_LARGO_2 = 1;
const int CANTIDAD_BARCOS_LARGO_3 = 2;
const int CANTIDAD_BARCOS_LARGO_4 = 1;
const int CANTIDAD_BARCOS_LARGO_5 = 1;
const int LARGO_BARCO_2 = 2;
const int LARGO_BARCO_3 = 3;
const int LARGO_BARCO_4 = 4;
const int LARGO_BARCO_5 = 5;
const int LARGO_MINIMO_BARCOS = 2;
const int LARGO_MAXIMO_BARCOS = 5;

const char NORTE = 'N';
const char SUR = 'S';
const char ESTE = 'E';
const char OESTE = 'O';

typedef struct juego
{
    char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO];
    char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO];
    barco_t barcos_jugador[CANTIDAD_BARCOS];
    int cantidad_barcos_leidos;

    int balas_aliadas_acertadas;
    int balas_aliadas_erradas;
    int balas_enemigas_acertadas;
    int balas_enemigas_erradas;
    int barcos_enemigos_hundidos;
    int barcos_aliados_sobrevivientes;
} juego_t;

/*
PRE: -
POS: Inicializa el juego, dejando los tableros con el mapa de incógnito, los barcos sin posiciones asignadas y las variables de conteo en 0 o cantidad inicial.
*/
void inicializar_juego(juego_t *juego)
{
    for (int i = INICIALIZACION_VALIDA; i < TAMANIO_TABLERO; i++)
    {
        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            juego->tablero_propio[i][j] = MAPA_INCOGNITO;
            juego->tablero_enemigo[i][j] = MAPA_INCOGNITO;
        }
    }
    for (int i = INICIALIZACION_VALIDA; i < CANTIDAD_BARCOS; i++)
    {
        juego->barcos_jugador[i].posiciones = NULL;
        juego->barcos_jugador[i].largo = INICIALIZACION_INVALIDA;
    }
    juego->cantidad_barcos_leidos = INICIALIZACION_VALIDA;
    juego->balas_aliadas_acertadas = INICIALIZACION_VALIDA;
    juego->balas_aliadas_erradas = INICIALIZACION_VALIDA;
    juego->balas_enemigas_acertadas = INICIALIZACION_VALIDA;
    juego->balas_enemigas_erradas = INICIALIZACION_VALIDA;
    juego->barcos_enemigos_hundidos = INICIALIZACION_VALIDA;
    juego->barcos_aliados_sobrevivientes = CANTIDAD_BARCOS;
}

/*
PRE: juego debe haber sido inicializado previamente.
POS: liber
*/
void liberar_barcos_memoria(juego_t *juego)
{
    for (int i = INICIALIZACION_VALIDA; i < juego->cantidad_barcos_leidos; i++)
    {
        if (juego->barcos_jugador[i].posiciones != NULL)
        {
            free(juego->barcos_jugador[i].posiciones);
            juego->barcos_jugador[i].posiciones = NULL;
        }
    }
}

void mostrar_juego(juego_t *juego)
{
    printf("\n"
           "    ┌────────────────────────────┐               ┌────────────────────────────┐\n"
           "    │    TU TABLERO (DEFENSA)    │               │  TABLERO ENEMIGO (ATAQUE)  │\n"
           "    └────────────────────────────┘               └────────────────────────────┘\n");

    printf("      1  2  3  4  5  6  7  8  9  10              1  2  3  4  5  6  7  8  9  10\n");
    printf("    ┌───────────────────────────────┐          ┌───────────────────────────────┐\n");

    for (int i = INICIALIZACION_VALIDA; i < TAMANIO_TABLERO; i++)
    {
        // --- MAPA JUGADORR ---
        printf("%2d  │ ", i + 1);
        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            printf("%c  ", juego->tablero_propio[i][j]);
        }
        printf("│      ");

        // --- MAPA ENEMIGO ---
        printf("%2d  │ ", i + 1);
        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            char celda = juego->tablero_enemigo[i][j];
            printf("%c  ", celda);
        }
        printf("│\n");
    }

    printf("    └───────────────────────────────┘          └───────────────────────────────┘\n\n");
}

bool validar_composicion_barcos(juego_t *juego)
{
    int cont_largo_2 = INICIALIZACION_VALIDA;
    int cont_largo_3 = INICIALIZACION_VALIDA;
    int cont_largo_4 = INICIALIZACION_VALIDA;
    int cont_largo_5 = INICIALIZACION_VALIDA;
    for (int i = INICIALIZACION_VALIDA; i < juego->cantidad_barcos_leidos; i++)
    {
        if (juego->barcos_jugador[i].largo == LARGO_BARCO_2){
            cont_largo_2++;
        }
        else if (juego->barcos_jugador[i].largo == LARGO_BARCO_3)
        {
            cont_largo_3++;
        }
        else if (juego->barcos_jugador[i].largo == LARGO_BARCO_4)
        {
            cont_largo_4++;
        }
        else if (juego->barcos_jugador[i].largo == LARGO_BARCO_5)
        {
            cont_largo_5++;
        }
    }
    return (cont_largo_2 == CANTIDAD_BARCOS_LARGO_2 && cont_largo_3 == CANTIDAD_BARCOS_LARGO_3 && cont_largo_4 == CANTIDAD_BARCOS_LARGO_4 && cont_largo_5 == CANTIDAD_BARCOS_LARGO_5);
}

int validar_largo_barco(int largo)
{
    return (largo >= LARGO_MINIMO_BARCOS && largo <= LARGO_MAXIMO_BARCOS);
}

int validar_cantidad_barcos_cargados(int cantidad)
{
    if (cantidad != CANTIDAD_BARCOS)
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}

int validar_direccion(char direccion)
{
    if ((direccion != NORTE) && (direccion != SUR) && (direccion != ESTE) && (direccion != OESTE))
    {
        return ERROR_LECTURA;
    }
    return EXITO;
}

int validar_datos_barco(int fila, int columna, char direccion, int largo, int cant_barcos)
{
    if (cant_barcos >= CANTIDAD_BARCOS)
    {
        printf("¡Error de lectura! Se han leído más barcos de los permitidos.\n");
        return ERROR_LECTURA;
    }
    if (fila < COORDENADA_MINIMA|| fila > TAMANIO_TABLERO || columna < COORDENADA_MINIMA || columna > TAMANIO_TABLERO)
    {
        printf("¡Error de lectura! Posición inicial fuera del mapa: %d;%d\n", fila, columna);
        return ERROR_LECTURA;
    }
    if (validar_direccion(direccion) != EXITO)
    {
        printf("¡Error de lectura! Dirección inválida: %c. \n Solo puede ser: -S-, -N-, -E-, -O-\n", direccion);
        return ERROR_LECTURA;
    }
    if (!validar_largo_barco(largo))
    {
        printf("¡Error de lectura! Largo de barco inválido: %d\n", largo);
        return ERROR_LECTURA;
    }

    return EXITO;
}

void cargar_direccion(char direccion, int *posicion_fila, int *posicion_columna)
{
    if (direccion == NORTE)
    {
        *posicion_fila = -1;
    }
    else if (direccion == SUR)
    {
        *posicion_fila = 1;
    }
    else if (direccion == ESTE)
    {
        *posicion_columna = 1;
    }
    else if (direccion == OESTE)
    {
        *posicion_columna = -1;
    }
}

bool validar_y_cargar_posiciones_barco(int fila_matriz, int columna_matriz, int posicion_fila, int posicion_columna, int largo_barco, coordenada_t *posiciones, bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    int fila_actual = INICIALIZACION_INVALIDA;
    int columna_actual = INICIALIZACION_INVALIDA;
    bool posicion_valida = true;

    for (int i = INICIALIZACION_VALIDA; i < largo_barco && posicion_valida; i++)
    {
        fila_actual = fila_matriz + (posicion_fila * i);
        columna_actual = columna_matriz + (posicion_columna * i);

        if (fila_actual < INICIALIZACION_VALIDA || fila_actual >= TAMANIO_TABLERO || columna_actual < INICIALIZACION_VALIDA || columna_actual >= TAMANIO_TABLERO || matriz_ocupacion[fila_actual][columna_actual])
        {
            posicion_valida = false;
        }
        else
        {
            posiciones[i].fila = fila_actual;
            posiciones[i].columna = columna_actual;
        }
    }
    return posicion_valida;
}

void asignar_barco(juego_t *juego, int largo_barco, coordenada_t *posiciones)
{
    barco_t *nuevo_barco = &(juego->barcos_jugador[juego->cantidad_barcos_leidos]);
    nuevo_barco->largo = largo_barco;
    nuevo_barco->posiciones = posiciones;
}

void dibujar_posiciones_barco(juego_t *juego, coordenada_t *posiciones, int largo_barco, bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = INICIALIZACION_VALIDA; i < largo_barco; i++)
    {
        matriz_ocupacion[posiciones[i].fila][posiciones[i].columna] = true;
        juego->tablero_propio[posiciones[i].fila][posiciones[i].columna] = BARCO;
    }
}

void inicializar_tablero_ocupacion(bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = INICIALIZACION_VALIDA; i < TAMANIO_TABLERO; i++)
    {
        for (int j = INICIALIZACION_VALIDA; j < TAMANIO_TABLERO; j++)
        {
            matriz_ocupacion[i][j] = false;
        }
    }
}


int cargar_barcos_propios(FILE *barcos_jugador, juego_t *juego)
{
    int fila_inicializacion = INICIALIZACION_INVALIDA;
    int columna_inicializacion = INICIALIZACION_INVALIDA;
    int largo_barco = INICIALIZACION_INVALIDA;
    char direccion;
    bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO];
    inicializar_tablero_ocupacion(matriz_ocupacion);

    bool hubo_error = false;
    int codigo_retorno = EXITO;

    while (!hubo_error && (fscanf(barcos_jugador, FORMATO_LECTURA, &fila_inicializacion, &columna_inicializacion, &direccion, &largo_barco)) != EOF)
    {
        if (validar_datos_barco(fila_inicializacion, columna_inicializacion, direccion, largo_barco, juego->cantidad_barcos_leidos)!= EXITO)
        {
            hubo_error = true;
            codigo_retorno = ERROR_LECTURA;
        }
        else
        {
            int posicion_fila = INICIALIZACION_VALIDA;
            int posicion_columna = INICIALIZACION_VALIDA;
            cargar_direccion(direccion, &posicion_fila, &posicion_columna);
            coordenada_t *posiciones = malloc(sizeof(coordenada_t) * (size_t)largo_barco);
            if (!posiciones)
            {
                hubo_error = true;
                codigo_retorno = ERROR_LECTURA;
            }
            else
            {
                int fila_matriz = fila_inicializacion - 1;
                int columna_matriz = columna_inicializacion - 1;

                if (!validar_y_cargar_posiciones_barco(fila_matriz, columna_matriz, posicion_fila, posicion_columna, largo_barco, posiciones, matriz_ocupacion))
                {
                    free(posiciones);
                    printf("¡Error de lectura! El barco ocupa posiciones inválidas o superpuestas.\n");
                    hubo_error = true;
                    codigo_retorno = ERROR_LECTURA;
                }
                else
                {
                    asignar_barco(juego, largo_barco, posiciones);
                    dibujar_posiciones_barco(juego, posiciones, largo_barco, matriz_ocupacion);
                    juego->cantidad_barcos_leidos++;
                }
            }
        }
    }
    fclose(barcos_jugador);

    if (!hubo_error)
    {
        if (validar_cantidad_barcos_cargados(juego->cantidad_barcos_leidos) != EXITO || !validar_composicion_barcos(juego))
        {
            printf("¡Error! La composición final de la flota no es la reglamentaria o faltan barcos.\n");
            codigo_retorno = ERROR_LECTURA;
        }
    }

    return codigo_retorno;
}

bool barco_fue_tocado(barco_t barco_actual, coordenada_t disparo)
{
    bool tocado = false;
    for (int i = INICIALIZACION_VALIDA; i < barco_actual.largo; i++)
    {
        if ((barco_actual.posiciones[i].fila == disparo.fila) && (barco_actual.posiciones[i].columna == disparo.columna))
        {
            tocado = true;
        }
    }
    return tocado;
}

bool enemigo_hunde_barco(char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO], barco_t *barco)
{
    bool hundido = true;
    for (int i = INICIALIZACION_VALIDA; i < barco->largo; i++)
    {
        if (tablero_propio[barco->posiciones[i].fila][barco->posiciones[i].columna] != TOCADO)
        {
            hundido = false;
        }
    }
    return hundido;
}

void modificar_tablero_propio_hundido(char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO], barco_t *barco)
{
    for (int i = INICIALIZACION_VALIDA; i < barco->largo; i++)
    {
        tablero_propio[barco->posiciones[i].fila][barco->posiciones[i].columna] = HUNDIDO;
    }
}

void consecuencia_barco_hundido(int *barcos_aliados_sobrevivientes, int *balas_enemigas_acertadas)
{
    printf("¡El enemigo ha hundido uno de tus barcos! Te quedan %d barcos aliados sobrevivientes.\n", *barcos_aliados_sobrevivientes - 1);
    (*barcos_aliados_sobrevivientes)--;
}

void consecuencia_barco_tocado()
{
    printf("¡El enemigo ha tocado uno de tus barcos!\n");
}

void analizar_disparo_enemigo(juego_t *juego, coordenada_t disparo)
{
    if (juego->tablero_propio[disparo.fila][disparo.columna] == BARCO)
    {
        juego->tablero_propio[disparo.fila][disparo.columna] = TOCADO;
        juego->balas_enemigas_acertadas++;

        for (int i = INICIALIZACION_VALIDA; i < CANTIDAD_BARCOS; i++)
        {
            barco_t *barco_actual = &(juego->barcos_jugador[i]);
            if (barco_fue_tocado(*barco_actual, disparo))
            {
                if (enemigo_hunde_barco(juego->tablero_propio, barco_actual))
                {
                    modificar_tablero_propio_hundido(juego->tablero_propio, barco_actual);
                    consecuencia_barco_hundido(&(juego->barcos_aliados_sobrevivientes), &(juego->balas_enemigas_acertadas));
                }
                else
                {
                    consecuencia_barco_tocado();
                }
            }
        }
    }
    else if (juego->tablero_propio[disparo.fila][disparo.columna] == MAPA_INCOGNITO)
    {
        juego->tablero_propio[disparo.fila][disparo.columna] = AGUA;
        juego->balas_enemigas_erradas++;
        printf("¡El enemigo disparó al agua!\n");
    }
}

void escribir_reporte(FILE *reporte_archivo, juego_t *juego)
{
    fprintf(reporte_archivo, "Balas aliadas acertadas: %d\n", juego->balas_aliadas_acertadas);
    fprintf(reporte_archivo, "Balas aliadas erradas: %d\n", juego->balas_aliadas_erradas);
    fprintf(reporte_archivo, "Balas enemigas acertadas: %d\n", juego->balas_enemigas_acertadas);
    fprintf(reporte_archivo, "Balas enemigas erradas: %d\n", juego->balas_enemigas_erradas);
    fprintf(reporte_archivo, "Barcos enemigos hundidos: %d\n", juego->barcos_enemigos_hundidos);
    fprintf(reporte_archivo, "Barcos aliados sobrevivientes: %d\n", juego->barcos_aliados_sobrevivientes);
}

int verificar_cant_argumentos(int argc)
{
    if (argc != 3)
    {
        printf("Error de cantidad de argumentos. Se DEBE ingresar: <programa_ejecutable> <archivo_barcos> <archivo_reporte>\n");
        return ERROR_CANTIDAD_ARGUMENTOS;
    }
    return EXITO;
}

bool disparo_valido(int fila, int columna, char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    if (fila < COORDENADA_MINIMA || fila > TAMANIO_TABLERO || columna < COORDENADA_MINIMA || columna > TAMANIO_TABLERO)
    {
        return false;
    }
    if (tablero_enemigo[fila - 1][columna - 1] != MAPA_INCOGNITO)
    {
        return false;
    }
    return true;
}

void consecuencia_resultado_disparo_jugador(char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO], int *balas_acertadas, int *balas_erradas, int *barcos_hundidos, char resultado, coordenada_t disparo)
{
    printf("Resultado de tu disparo: ");
    if (resultado == AGUA)
    {
        printf("LE DISTE AL AGUA EN: %d;%d!\n", disparo.fila + 1, disparo.columna + 1);
        (*balas_erradas)++;
        tablero_enemigo[disparo.fila][disparo.columna] = AGUA;
    }
    else if (resultado == TOCADO)
    {
        printf("TOCASTE UN BARCO EN: %d;%d!\n", disparo.fila + 1, disparo.columna + 1);
        (*balas_acertadas)++;
        tablero_enemigo[disparo.fila][disparo.columna] = TOCADO;
    }
    else if (resultado == HUNDIDO)
    {
        printf("HUNDISTE UN BARCO EN: %d;%d!\n", disparo.fila + 1, disparo.columna + 1);
        (*balas_acertadas)++;
        (*barcos_hundidos)++;
        tablero_enemigo[disparo.fila][disparo.columna] = HUNDIDO;
    }
}

void obtener_disparo_usuario(int *fila, int *columna)
{
    printf("Ingrese su disparo (FILA;COLUMNA): ");
    scanf(" %d;%d", fila, columna);
}

void obtener_disparo_valido(coordenada_t *disparo, char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    do
    {
        obtener_disparo_usuario(&disparo->fila, &disparo->columna);
        if (!disparo_valido(disparo->fila, disparo->columna, tablero_enemigo))
        {
            printf("Disparo inválido. Por favor, ingrese una fila y columna entre 1 y %d\n. Disparos repetidos no se admiten", TAMANIO_TABLERO);
        }
    } while (!disparo_valido(disparo->fila, disparo->columna, tablero_enemigo));
    disparo->fila = disparo->fila - 1;
    disparo->columna = disparo->columna - 1;
}

void empezar_turnos_batalla(oponente_t *oponente, juego_t *juego)
{
    coordenada_t disparo_jugador;
    printf("Empieza la batalla naval...!\n");
    while (juego->barcos_aliados_sobrevivientes > INICIALIZACION_VALIDA && juego->barcos_enemigos_hundidos < CANTIDAD_BARCOS)
    {
        obtener_disparo_valido(&disparo_jugador, juego->tablero_enemigo);
        system("clear");

        char resultado_jugador = oponente_recibe_disparo(oponente, disparo_jugador);
        consecuencia_resultado_disparo_jugador(juego->tablero_enemigo, &(juego->balas_aliadas_acertadas), &(juego->balas_aliadas_erradas), &(juego->barcos_enemigos_hundidos), resultado_jugador, disparo_jugador);

        if (juego->barcos_enemigos_hundidos < CANTIDAD_BARCOS)
        {
            coordenada_t disp_enemigo = oponente_realiza_disparo(oponente);
            printf("El oponente dispara: %d;%d\n", disp_enemigo.fila + 1, disp_enemigo.columna + 1);
            analizar_disparo_enemigo(juego, disp_enemigo);
        }

        mostrar_juego(juego);
    }
}

void mostrar_mensaje_final(juego_t juego)
{
    if (juego.barcos_enemigos_hundidos == CANTIDAD_BARCOS)
    {
        printf("GANASTE! hundiste todos los barcos enemigos\n");
    }
    else
    {
        printf("Has sido derrotado. La MAQUINA hundió toda tu flota.\n");
    }
}

void mostrar_mensaje_error(int estado_carga)
{
    if (estado_carga == ERROR_ABRIR_ARCHIVO)
    {
        printf("Error de abrir el archivo.\n");
    }
    else
    {
        printf("Error de lectura.\n");
    }
}

int main(int argc, char *argv[])
{
    int estado_carga = INICIALIZACION_INVALIDA;
    estado_carga = verificar_cant_argumentos(argc);
    if (estado_carga != EXITO)
    {
        return estado_carga;
    }

    juego_t juego;
    inicializar_juego(&juego);
    FILE *barcos_jugador = fopen(argv[ARGUMENTO_BARCOS], LECTURA);
    if (!barcos_jugador)
    {
        printf("¡Error de abrir el archivo de barcos!\n");
        return ERROR_ABRIR_ARCHIVO;
    }
    estado_carga = cargar_barcos_propios(barcos_jugador, &juego);

    if (estado_carga != EXITO)
    {
        mostrar_mensaje_error(estado_carga);
        liberar_barcos_memoria(&juego);
        return estado_carga;
    }

    oponente_t *oponente = oponente_crear(juego.barcos_jugador);
    if (!oponente)
    {
        liberar_barcos_memoria(&juego);
        return ERROR_LECTURA;
    }
    mostrar_juego(&juego);
    empezar_turnos_batalla(oponente, &juego);
    mostrar_mensaje_final(juego);
    FILE *reporte_archivo = fopen(argv[ARGUMENTO_REPORTE], ESCRITURA);
    if (!reporte_archivo)
    {
        return ERROR_ESCRITURA;
    }

    escribir_reporte(reporte_archivo, &juego);
    oponente_destruir(oponente);
    liberar_barcos_memoria(&juego);
    printf("Reporte escrito correctamente.\n");
    fclose(reporte_archivo);

    return EXITO;
}