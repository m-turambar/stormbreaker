//
// Created by mike on 11/04/21.
//

#ifndef STORMBREAKER_NODOS_TIBIANOS_H
#define STORMBREAKER_NODOS_TIBIANOS_H

#include "nodo.h"
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

struct nodo_template_matching : nodo
{
    nodo_template_matching(cv::Point c, int r);
    void procesar() override;
    void suscribir_fuente(nodo* fuente);
    void suscribir_plantilla(nodo* plantilla);
    cv::Mat plantilla;
    cv::Mat gris;
    cv::Mat floating_result;
private:
    const int status_col_offset{1700};
    void suscribir_a(nodo *src) override {}
};

struct nodo_porcentaje_vida_mana : nodo
{
    nodo_porcentaje_vida_mana(cv::Point c, int r);
    void procesar() override;
    void mostrar() override;
    cv::Mat roja;
    cv::Mat azul;
    float vida=100.00;
    float mana=100.00;
    const cv::Size sz_barra = {90, 10};
    float calcular_vida();
    float calcular_mana();
};

















#endif //STORMBREAKER_NODOS_TIBIANOS_H
