/****************************************************
App.hpp - App class file

hikarideai 
****************************************************/


#pragma once
#include "SFML/Graphics.hpp"
#include "Panel.hpp"
#include "TabHolder.hpp"
#include "ValueTable.hpp"
#include "Equation.hpp"
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include "Plane.hpp"

class App {
public:
    App();
    void run();
    void loop();

    void setIterations(long n) { iteraions = n; }
    void setT(double n) { t = n; }
    void setDT(double n) { dt = n; }
    void setX(std::string e) {
        x.parse(e);
    }
    void setY(std::string e) {
        y.parse(e);
    }

    std::map<std::string, double> getXVars() {
        return x.getVars();
    }
    std::map<std::string, double> getYVars() {
        return y.getVars();
    }
    std::deque<std::string>& getXVals() {
        return x.getVals();
    }
    std::deque<std::string>& getYVals() {
        return y.getVals();
    }

	~App()
    {
		delete iterations;
		delete t_value;
		delete dt_value;
		delete x_eq;
		delete y_eq;
		delete x_eq_vars;
		delete y_eq_vars;
		delete tab_gen;
		delete tab_eq;
    }

private:
    void initialize();
    void initGeneralTab();
	void initColorTab();
    void initEquationsTab();
    void initRibbon();

    sf::ContextSettings settings;
    sf::RenderWindow window;

    //General tab
    Panel general;
	Slider *iterations;
	Slider *transparency_value, *zoom_value;
    
	//Color tab
	Panel color_sel;
	Text *color1, *color2;
	Slider *ch1, *cs1, *cl1;
	Slider *ch2, *cs2, *cl2;

    //Equations
    Panel equations;
	Text *param_lab, *time_lab, *eqat_lab;
	Slider *t_value, *dt_value, *x_value, *y_value;
    TextBox *x_eq, *y_eq;
    ValueTable *x_eq_vars, *y_eq_vars;

    //Ribbon
    TabHolder ribbon;
    Tab *tab_gen, *tab_eq, *tab_col;


    ValueTable test;

	std::mutex safe_calc;
	double x0, y0;
	Equation x, y;
    
	long iteraions;
    double t, dt;
    bool changing = false;
    size_t dir = 0;
    double width = 640, height = 480;
    Plane *graph;
	std::recursive_mutex plot_mute;
	bool stop_plotting = true, is_plotting = false;
	void plot(); void startPlot(); void stopPlot();

	std::thread plotter;
};
