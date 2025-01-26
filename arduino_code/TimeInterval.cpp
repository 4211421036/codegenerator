/**
 * @file TimeInterval.cpp
 * @author ESDeveloperBR (esdeveloperbr@gmail.com)
 * @brief This time control library is based on the MILLIS() and MICROS() procedures, with the main objective of replacing the DELAY() command, thus avoiding the programming LOOP paralyzation.
 * @brief Esta biblioteca de controle de tempo é baseada nos procedimentos MILLIS() e MICROS(), com o objetivo principal de substituir o comando DELAY(), evitando assim a paralisação do LOOP de programação.
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TimeInterval.h"

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<< Retorna "true" quando o tempo em milissegundos tiver terminado >>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Returns “true” when the time in milliseconds passed by parameter (millisTime) reaches the end;
 * @brief [PORTUGUÊS BR]
 * @brief Retorna “verdadeiro” quando o tempo em milissegundos passado por parâmetro (millisTime) chegar ao final;
 * 
 * @param millisTime Time in milliseconds of the interval. [Português BR] Tempo em milissegundos do intervalo.
 * @return boolean 
 * 
 * @note if(intervalTest.intervalMillis(2000)) { Serial.println("It's not stopping the main loop."); }
 * 
 */
boolean TimeInterval::intervalMillis(long millisTime){
    long tempMillisAtual = millis();
    if( (tempMillisAtual - _start_Millis) >= millisTime ){
        _start_Millis = millis();
        return true;
    }else{
        return false;
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<< Reiniciar o contador de milissegundos associado ao método intervalMillis() >>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Reset millisecond counter associated with intervalMillis() method.
 * @brief [Português BR]
 * @brief Reiniciar o contador de milissegundos associado ao método intervalMillis().
 * 
 * @note if(intervalTest.intervalMillis(2000)) Serial.println("It's not stopping!");
 * @note intervalTest.resetMillis(); // Reset the intervalMillis counter.
 */
void TimeInterval::resetMillis(){
    _start_Millis = millis();
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<< Retorna "true" quando o tempo em microssegundos tiver terminado >>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Returns “true” when the time in microseconds passed by parameter (microsTime) reaches the end;
 * @brief [Português BR]
 * @brief Retorna “verdadeiro” quando o tempo em microssegundos passado por parâmetro (microsTime) chegar ao final;
 * 
 * @param microsTime Time in microseconds of the interval. [Português BR] Tempo em microsegundos do intervalo.
 * @return boolean 
 * 
 * @note if(intervalTest.intervalMicros(2000000)) { Serial.println("It's not stopping."); }
 * 
 */
boolean TimeInterval::intervalMicros(long microsTime){
    long tempMicrosAtual = micros();

    if( (tempMicrosAtual - _start_Micros) >= microsTime ){
        _start_Micros = micros();
        return true;
    }else{
        return false;
    }
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<< Reiniciar o contador de microssegundos associado ao método intervalMicros() >>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Reset the microseconds counter associated with the intervalMicros() method.
 * @brief [Português BR]
 * @brief Reiniciar o contador de microssegundos associado ao método intervalMicros().
 * 
 * @note if(intervalTest.intervalMicros(2000000)) { Serial.println("It's not stopping."); }
 * @note intervalTest.resetMicros(); // Reset the intervalMicros counter.
 */
void TimeInterval::resetMicros(){
    _start_Micros = micros();
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<< Inicializa um cronômetro em Milissegundos. >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Initializes the stopwatch in milliseconds.
 * @brief [Português BR]
 * @brief Inicializa um cronômetro em Milissegundos.
 * 
 * @note See example program "TimeInterval-stopwatchStartMillis.ino"
 * @note [Português BR]
 * @note Veja o programa exemplo "TimeInterval-stopwatchStartMillis.ino"
 * 
*/
void TimeInterval::stopwatchStartMillis(){
    _start_Millis = millis();
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<<<<<< Parar cronômetro - Retornar o valor obtido em milissegundos. >>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Stop stopwatch - Return the obtained value in milliseconds.
 * @brief [Português BR]
 * @brief Parar cronômetro - Retornar o valor obtido em milissegundos.
 * 
 * @note See example program "TimeInterval-stopwatchStartMillis.ino"
 * @note [Português BR]
 * @note Veja o programa exemplo "TimeInterval-stopwatchStartMillis.ino"
*/
long TimeInterval::stopwatchStopMillis(){
    return  millis() - _start_Millis; 
}

// <<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<< Inicializa um cronômetro em Microssegundos >>>>>>>>>>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Initializes the stopwatch in microseconds
 * @brief [Português BR]
 * @brief Inicializa um cronômetro em Microssegundos.
 * 
 * @note See example program "TimeInterval-stopwatchStartMicros.ino"
 * @note [Português BR]
 * @note Veja o programa exemplo "TimeInterval-stopwatchStartMicros.ino"
*/
void TimeInterval::stopwatchStartMicros(){
    _start_Micros = micros();
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// <<<<<<<<<<<<<< Parar cronômetro - Retornar o valor obtido em microssegundos >>>>>>>
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/**
 * @brief Stop stopwatch - Return the value obtained in microseconds.
 * @brief [Português BR]
 * @brief Parar cronômetro - Retornar o valor obtido em microssegundos.
 * 
 * @note See example program "TimeInterval-stopwatchStartMicros.ino"
 * @note [Português BR]
 * @note Veja o programa exemplo "TimeInterval-stopwatchStartMicros.ino"
*/
long TimeInterval::stopwatchStopMicros(){
    return  micros() - _start_Micros; 
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
