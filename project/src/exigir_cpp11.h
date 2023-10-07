/**
 * Direitos autorais (c) 2016 por Ludwig Grill (www.rotzbua.de)
 * Lança um erro se o C++11 não for suportado
 **/
#ifndef REQUIRE_CPP11_H
#define REQUIRE_CPP11_H

#if __cplusplus < 201103L
#error "Esta biblioteca requer pelo menos um compilador compatível com C++11. Verifique se o suporte ao C++11 está ativado nas opções do compilador ou, se estiver usando a IDE do Arduino, atualize para a versão >=1.6.6."
#endif

#endif // REQUIRE_CPP11_H
