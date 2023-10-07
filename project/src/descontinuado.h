/**
 * Direitos autorais (c) 2016 por Ludwig Grill (www.rotzbua.de)
 * Solução simples para contornar a obsolescência para o Arduino IDE
 * A IDE 1.6.8 usa o gcc 4.8, que não suporta o c++14 [[deprecated]]
 * Versões posteriores devem suportar o c++14, então use a sintaxe c++14
 */
#ifndef DEPRECATED_H
#define DEPRECATED_H

#ifdef __has_cpp_attribute
#if __has_cpp_attribute(deprecated)
#define DEPRECATED [[deprecated]]
#define DEPRECATED_MSG(msg) [[deprecated(msg)]]
#endif // __has_cpp_attribute(deprecated)
#else
#define DEPRECATED __attribute__((deprecated))
#define DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#endif // __has_cpp_attribute

#endif // DESCONTINUADO_H
