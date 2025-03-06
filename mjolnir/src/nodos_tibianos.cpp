//
// Created by mike on 11/04/21.
//

#include <mjolnir/include/nodos_tibianos.h>


nodo_template_matching::nodo_template_matching(cv::Point c, int r) : nodo(c, r)
{
    mcolor = cv::Scalar(255, 0, 0);
    sid = "template_matching" + sid;
}

void nodo_template_matching::suscribir_fuente(nodo *fuente) {
    if (fuente != nullptr)
    {
        std::cout << "suscribiendo " << sid << " a " << fuente->sid << ". " << fuente->sid << " -> " << sid << std::endl;
        proveedores.push_back(fuente);
        fuente->suscriptores.push_back(this);
        msrc = fuente->mmat;
        // procuramos solo inicializar esta matriz con el tamaño correcto una vez, para ahorrar tiempo
        // por ende en alguna de las dos suscripciones se va a hacer esto
        if(plantilla.dims > 0) {
            const int result_cols =  msrc.cols - plantilla.cols + 1;
            const int result_rows = msrc.rows - plantilla.rows + 1;
            floating_result = cv::Mat(result_cols, result_rows, CV_32FC1);
        }
    }
}

void nodo_template_matching::suscribir_plantilla(nodo *fuente) {
    if (fuente != nullptr) {
        std::cout << "suscribiendo " << sid << " a " << fuente->sid << ". " << fuente->sid << " -> " << sid << std::endl;
        proveedores.push_back(fuente);
        fuente->suscriptores.push_back(this);
        cv::cvtColor(fuente->mmat, plantilla, cv::COLOR_BGR2GRAY);
        // procuramos solo inicializar esta matriz con el tamaño correcto una vez, para ahorrar tiempo
        // por ende en alguna de las dos suscripciones se va a hacer esto
        if(msrc.dims > 0) {
            const int result_cols =  msrc.cols - plantilla.cols + 1;
            const int result_rows = msrc.rows - plantilla.rows + 1;
            floating_result = cv::Mat(result_cols, result_rows, CV_32FC1);
        }
    }
}

void nodo_template_matching::procesar() {
    if (msrc.dims > 0 && plantilla.dims > 0)
    {

        cv::cvtColor(msrc, gris, cv::COLOR_BGR2GRAY);
        // /2
        const cv::Rect ROI = cv::Rect(cv::Point(status_col_offset, 0), cv::Point(gris.cols, gris.rows/2));
        cv::Mat columna_estatus = gris(ROI);
        cv::matchTemplate(columna_estatus, plantilla, floating_result, cv::TM_CCOEFF_NORMED);

        cv::normalize( floating_result, floating_result, 0, 1, cv::NORM_MINMAX, -1);
        double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
        cv::minMaxLoc(floating_result, &minVal, &maxVal, &minLoc, &maxLoc);
        maxLoc.x += status_col_offset;
        mmat = msrc(cv::Rect(maxLoc, cv::Point( maxLoc.x + plantilla.cols , maxLoc.y + plantilla.rows)));
        for(auto& suscr : suscriptores)
        {
            suscr->msrc = mmat;
        }
    }
}

nodo_porcentaje_vida_mana::nodo_porcentaje_vida_mana(cv::Point c, int r) : nodo(c, r) {
    b_mostrar = true;
}

void nodo_porcentaje_vida_mana::procesar() {
    if (!msrc.empty()) {
        roja = msrc(cv::Rect(cv::Point(16,3), sz_barra));
        azul = msrc(cv::Rect(cv::Point(16,16), sz_barra));
        for(int i=0; i<90; ++i) {
            cout << "roja[" << i << "]=" << roja.at(5,i);
        }
    }
}

void nodo_porcentaje_vida_mana::mostrar() {
    if(b_mostrar) {
        if(!azul.empty()) {
            cv::namedWindow("mana");
            cv::imshow("mana",azul);
        }
        if(!roja.empty()) {
            cv::namedWindow("vida");
            cv::imshow("vida",roja);
        }
    }
}

