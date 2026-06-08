#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "oponente.h"

#define TAMANIO_TABLERO 10
#define CANTIDAD_BARCOS 5

const char FORMATO_LECTURA[] = "%d;%d; %c;%d"; 
const char *LECTURA = "r";                      
const char *ESCRITURA = "w";                    
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
const char MAPA_INCOGNITO = ' ';

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

void inicializar_juego(juego_t *juego)
{
    for (int i = 0; i < TAMANIO_TABLERO; i++)
    {
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            juego->tablero_propio[i][j] = MAPA_INCOGNITO;
            juego->tablero_enemigo[i][j] = MAPA_INCOGNITO;
        }
    }
    juego->cantidad_barcos_leidos = INICIALIZACION_VALIDA;
    juego->balas_aliadas_acertadas = INICIALIZACION_VALIDA;
    juego->balas_aliadas_erradas = INICIALIZACION_VALIDA;
    juego->balas_enemigas_acertadas = INICIALIZACION_VALIDA;
    juego->balas_enemigas_erradas = INICIALIZACION_VALIDA;
    juego->barcos_enemigos_hundidos = INICIALIZACION_VALIDA;
    juego->barcos_aliados_sobrevivientes = CANTIDAD_BARCOS;
}

void liberar_barcos_memoria(juego_t *juego)
{
    for (int i = 0; i < juego->cantidad_barcos_leidos; i++)
    {
        if (juego->barcos_jugador[i].posiciones != NULL)
        {
            free(juego->barcos_jugador[i].posiciones);
            juego->barcos_jugador[i].posiciones = NULL;
        }
    }
}

void mostrar_interfaz(juego_t *juego)
{
    printf("\n==== TU TABLERO (DEFENSA) ====       ==== TABLERO ENEMIGO (ATAQUE) ====\n");
    printf("   1 2 3 4 5 6 7 8 9 10                 1 2 3 4 5 6 7 8 9 10\n");
    for (int i = 0; i < TAMANIO_TABLERO; i++)
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            printf("%c ", juego->tablero_propio[i][j]);
        }
        printf("             %2d ", i + 1);
        for (int j = 0; j < TAMANIO_TABLERO; j++)
        {
            printf("%c ", juego->tablero_enemigo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

bool validar_composicion_barcos(juego_t *juego)
{
    int cont_largo_2 = 0, cont_largo_3 = 0, cont_largo_4 = 0, cont_largo_5 = 0;
    for (int i = 0; i < CANTIDAD_BARCOS; i++)
    {
        if (juego->barcos_jugador[i].largo == 2)
            cont_largo_2++;
        else if (juego->barcos_jugador[i].largo == 3)
            cont_largo_3++;
        else if (juego->barcos_jugador[i].largo == 4)
            cont_largo_4++;
        else if (juego->barcos_jugador[i].largo == 5)
            cont_largo_5++;
    }
    return (cont_largo_2 == 1 && cont_largo_3 == 2 && cont_largo_4 == 1 && cont_largo_5 == 1);
}

int validar_largo_barco(int largo)
{
    return (largo >= 2 && largo <= 5);
}

int validar_largo_cantidad_barcos(int largo, int cantidad)
{
    if (largo == 2)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_2);
    else if (largo == 3)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_3);
    else if (largo == 4)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_4);
    else if (largo == 5)
        return (cantidad <= CANTIDAD_BARCOS_LARGO_5);
    return false;
}

int validar_cantidad_barcos_cargados(int cantidad)
{
    if (cantidad == 0 || cantidad > CANTIDAD_BARCOS)
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

int validacion_largo_barco_cantidad(int largo_barco, FILE *barcos, int *cantidad_largo_2, int *cantidad_largo_3, int *cantidad_largo_4, int *cantidad_largo_5)
{
    if (largo_barco == 2)
    {
        (*cantidad_largo_2)++;
        if (!validar_largo_cantidad_barcos(largo_barco, *cantidad_largo_2))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 2 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    else if (largo_barco == 3)
    {
        (*cantidad_largo_3)++;
        if (!validar_largo_cantidad_barcos(largo_barco, *cantidad_largo_3))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 3 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    else if (largo_barco == 4)
    {
        (*cantidad_largo_4)++;
        if (!validar_largo_cantidad_barcos(largo_barco, *cantidad_largo_4))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 4 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    else if (largo_barco == 5)
    {
        (*cantidad_largo_5)++;
        if (!validar_largo_cantidad_barcos(largo_barco, *cantidad_largo_5))
        {
            fclose(barcos);
            printf("¡Error de lectura! Hay más barcos de largo 5 de los permitidos.\n");
            return ERROR_LECTURA;
        }
    }
    return EXITO;
}

int validar_datos_barco(int fila, int columna, char direccion, int largo, int cant_barcos, FILE *barcos_jugador, int *cant_largo_2, int *cant_largo_3, int *cant_largo_4, int *cant_largo_5)
{
    //cantidad de barcos no puede ser mayor a la permitida
    if (cant_barcos >= CANTIDAD_BARCOS)
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Se han leído más barcos de los permitidos.\n");
        return ERROR_LECTURA;
    }
    //dentro del tablero
    if (fila < 1 || fila > TAMANIO_TABLERO || columna < 1 || columna > TAMANIO_TABLERO)
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Posición inicial fuera del mapa: %d;%d\n", fila, columna);
        return ERROR_LECTURA;
    }
    //dirección válida
    if (validar_direccion(direccion) != EXITO)
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Dirección inválida: %c. \n Solo puede ser: -S-, -N-, -E-, -O-\n", direccion);
        return ERROR_LECTURA;
    }
    //largo válido del barco
    if (!validar_largo_barco(largo))
    {
        fclose(barcos_jugador);
        printf("¡Error de lectura! Largo de barco inválido: %d\n", largo);
        return ERROR_LECTURA;
    }
    //cantidad de barcos por largo no puede ser mayor a la permitida
    if (validacion_largo_barco_cantidad(largo, barcos_jugador, cant_largo_2, cant_largo_3, cant_largo_4, cant_largo_5) != EXITO)
    {
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

    for (int i = 0; i < largo_barco && posicion_valida; i++)
    {
        fila_actual = fila_matriz + (posicion_fila * i);
        columna_actual = columna_matriz + (posicion_columna * i);

        if (fila_actual < 0 || fila_actual >= TAMANIO_TABLERO || columna_actual < 0 || columna_actual >= TAMANIO_TABLERO || matriz_ocupacion[fila_actual][columna_actual])
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
    for (int i = 0; i < largo_barco; i++)
    {
        matriz_ocupacion[posiciones[i].fila][posiciones[i].columna] = true;
        juego->tablero_propio[posiciones[i].fila][posiciones[i].columna] = BARCO;
    }
}

void inicializar_tablero_ocupacion(bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO])
{
    for (int i = 0; i < TAMANIO_TABLERO; i++)
    {
        for (int j = 0; j < TAMANIO_TABLERO; j++)
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
    char direccion = '\0';
    int cantidad_largo_2 = 0;
    int cantidad_largo_3 = 0;
    int cantidad_largo_4 = 0;
    int cantidad_largo_5 = 0;
    bool matriz_ocupacion[TAMANIO_TABLERO][TAMANIO_TABLERO];
    inicializar_tablero_ocupacion(matriz_ocupacion);

    bool hubo_error = false;
    int codigo_retorno = EXITO;

    while (!hubo_error && (fscanf(barcos_jugador, FORMATO_LECTURA, &fila_inicializacion, &columna_inicializacion, &direccion, &largo_barco)) != EOF)
    {
        if (validar_datos_barco(fila_inicializacion, columna_inicializacion, direccion, largo_barco, juego->cantidad_barcos_leidos, barcos_jugador, &cantidad_largo_2, &cantidad_largo_3, &cantidad_largo_4, &cantidad_largo_5) != EXITO)
        {
            hubo_error = true;
            codigo_retorno = ERROR_LECTURA;
        }
        else
        {
            int posicion_fila = 0;
            int posicion_columna = 0;

            cargar_direccion(direccion, &posicion_fila, &posicion_columna);
            coordenada_t *posiciones = malloc(sizeof(coordenada_t) * (size_t)largo_barco);
            if (!posiciones)
            {
                fclose(barcos_jugador);
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
                    fclose(barcos_jugador);
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

    return codigo_retorno;
}

/*-----------------------------------------------------------------*/
bool barco_fue_tocado(barco_t barco_actual, coordenada_t disparo)
{
    bool tocado = false;
    for (int i = 0; i < barco_actual.largo; i++)
    {
        if ((barco_actual.posiciones[i].fila == disparo.fila) && (barco_actual.posiciones[i].columna == disparo.columna))
        {
            tocado = true;
        }
    }
    return tocado;
}

/*
PRE: el juego debe estar inicializado y el disparo debe ser una coordenada válida dentro del tablero.
POS: se verifica si el disparo enemigo ha hundido un barco aliado. Para esto, se verifica si todos los segmentos del barco han sido tocados. Si el largo completo del barco es todo "Tocado" ('T'), quiere deci que el barco ha sido hundido y se devuelve true. Si al menos un segmento del barco no ha sido tocado, se devuelve false.
*/
bool enemigo_hunde_barco(char tablero_propio[TAMANIO_TABLERO][TAMANIO_TABLERO], barco_t *barco)
{
    bool hundido = true;
    for (int i = 0; i < barco->largo; i++)
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
    for (int i = 0; i < barco->largo; i++)
    {
        tablero_propio[barco->posiciones[i].fila][barco->posiciones[i].columna] = HUNDIDO;
    }
}

void consecuencia_barco_hundido(int *barcos_aliados_sobrevivientes, int *balas_enemigas_acertadas)
{
    printf("¡El enemigo ha hundido uno de tus barcos! Te quedan %d barcos aliados sobrevivientes.\n", *barcos_aliados_sobrevivientes - 1);
    (*barcos_aliados_sobrevivientes)--;
    (*balas_enemigas_acertadas)++;
}

void consecuencia_barco_tocado()
{
    printf("¡El enemigo ha tocado uno de tus barcos!\n");
}

/*
PRE: el juego debe estar inicializado y el disparo debe ser una coordenada válida dentro del tablero.
POS: actualiza el tablero propio del juego según el resultado del disparo enemigo. Si el disparo impacta en un barco, se marca como tocado ('T') y se verifica si el barco ha sido hundido. Si el barco es hundido, se actualiza el tablero para reflejarlo y se notifica al jugador. Si el disparo impacta en agua, se marca como agua ('A') y se notifica al jugador. En ambos casos, se actualizan las estadísticas de balas acertadas o erradas del enemigo.
*/
void verificar_disparo_enemigo(juego_t *juego, coordenada_t disparo)
{
    if (juego->tablero_propio[disparo.fila][disparo.columna] == BARCO)
    {
        juego->tablero_propio[disparo.fila][disparo.columna] = TOCADO;
        juego->balas_enemigas_acertadas++;

        for (int i = 0; i < CANTIDAD_BARCOS; i++)
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
    else
    {
        juego->balas_enemigas_erradas++;
    }
}

int escribir_reporte(const char *archivo_reporte, juego_t *juego)
{
    FILE *reporte_archivo = fopen(archivo_reporte, ESCRITURA);
    if (!reporte_archivo)
    {
        return ERROR_ESCRITURA;
    }
    if (fprintf(reporte_archivo, "Balas aliadas acertadas: %d\n", juego->balas_aliadas_acertadas) < INICIALIZACION_VALIDA ||
        fprintf(reporte_archivo, "Balas aliadas erradas: %d\n", juego->balas_aliadas_erradas) < INICIALIZACION_VALIDA ||
        fprintf(reporte_archivo, "Balas enemigas acertadas: %d\n", juego->balas_enemigas_acertadas) < INICIALIZACION_VALIDA ||
        fprintf(reporte_archivo, "Balas enemigas erradas: %d\n", juego->balas_enemigas_erradas) < INICIALIZACION_VALIDA ||
        fprintf(reporte_archivo, "Barcos enemigos hundidos: %d\n", juego->barcos_enemigos_hundidos) < INICIALIZACION_VALIDA ||
        fprintf(reporte_archivo, "Barcos aliados sobrevivientes: %d\n", juego->barcos_aliados_sobrevivientes) < INICIALIZACION_VALIDA)
    {
        return ERROR_ESCRITURA;
    }
    if (fclose(reporte_archivo) != 0)
    {
        return ERROR_ESCRITURA;
    }
    return EXITO;
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

bool disparo_valido(int fila, int columna)
{
    return (fila >= 1 && fila <= TAMANIO_TABLERO && columna >= 1 && columna <= TAMANIO_TABLERO);
}


void consecuencia_resultado_disparo_jugador(char tablero_enemigo[TAMANIO_TABLERO][TAMANIO_TABLERO], int *balas_acertadas, int *balas_erradas, int *barcos_hundidos, char resultado, coordenada_t disparo)
{
    printf("Resultado de tu disparo: ");
    if (resultado == AGUA)
    {
        printf("¡Tu disparo al agua! No has acertado ningún barco enemigo.\n");
        (*balas_erradas)++;
        tablero_enemigo[disparo.fila][disparo.columna] = AGUA;
    }
    else if (resultado == TOCADO)
    {
        printf("¡Has tocado un barco enemigo en la posición %d;%d!\n", disparo.fila + 1, disparo.columna + 1);
        (*balas_acertadas)++;
        tablero_enemigo[disparo.fila][disparo.columna] = TOCADO;
    }
    else if (resultado == HUNDIDO)
    {
        printf("¡Has hundido un barco enemigo en la posición %d;%d!\n", disparo.fila + 1, disparo.columna + 1);
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

void obtener_disparo_valido(int *fila, int *columna)
{
    do
    {
        obtener_disparo_usuario(fila, columna);
        if(!disparo_valido(*fila, *columna))
        {
            printf("Disparo inválido. Por favor, ingrese una fila y columna entre 1 y %d\n", TAMANIO_TABLERO);
        }
    } while (!disparo_valido(*fila, *columna));
}

void empezar_turnos_batalla(oponente_t *oponente, juego_t *juego)
{
    int fila_ingresada = 0;
    int columna_ingresada = 0;
    printf("Empieza la batalla!\n");
    
    while (juego->barcos_aliados_sobrevivientes > INICIALIZACION_VALIDA && juego->barcos_enemigos_hundidos < CANTIDAD_BARCOS)
    {
        mostrar_interfaz(juego);

        obtener_disparo_valido(&fila_ingresada, &columna_ingresada);

        coordenada_t disparo_jugador;
        disparo_jugador.fila = fila_ingresada - 1;
        disparo_jugador.columna = columna_ingresada - 1;

        char resultado_jugador = oponente_recibe_disparo(oponente, disparo_jugador);
        consecuencia_resultado_disparo_jugador(juego->tablero_enemigo, &(juego->balas_aliadas_acertadas), &(juego->balas_aliadas_erradas), &(juego->barcos_enemigos_hundidos), resultado_jugador, disparo_jugador);
        
        if(juego->barcos_enemigos_hundidos < CANTIDAD_BARCOS)
        {
            coordenada_t disp_enemigo = oponente_realiza_disparo(oponente);
            printf("El oponente dispara: %d;%d\n", disp_enemigo.fila + 1, disp_enemigo.columna + 1);
            verificar_disparo_enemigo(juego, disp_enemigo);
        }
    }
}

void mostrar_mensaje_final(juego_t juego)
{
    if (juego.barcos_enemigos_hundidos == CANTIDAD_BARCOS)
    {
        printf("¡Felicidades Homero! ¡Lograste vencer a la computadora de manera justa!\n");
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
    empezar_turnos_batalla(oponente, &juego);
    mostrar_mensaje_final(juego);

    int estado_reporte = escribir_reporte(argv[ARGUMENTO_REPORTE], &juego);
    oponente_destruir(oponente);
    liberar_barcos_memoria(&juego);

    if (estado_reporte != EXITO)
    {
        printf("Error de escritura.\n");
        return estado_reporte;
    }

    return EXITO;
}