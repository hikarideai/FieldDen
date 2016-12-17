#include "Globals.hpp"
#include "App.hpp"
#include "Button.hpp"
#include "Slider.hpp"

#include <thread>

#define THREAD_DEBUG 0

#define _CRT_SECURE_NO_WARNINGS true
#define LANGUAGE_PACK_PATH "lang/russian.lang"
#define MOUSE_WHEEL_ZOOM 0.1

App::App() {
    settings.antialiasingLevel = 0;

    window.create(sf::VideoMode(width, height), "FieldDen", sf::Style::Default, settings);
    window.setFramerateLimit(60);

	sf::Image icon;
	icon.loadFromFile("icon.png");

	window.setIcon(256, 256, icon.getPixelsPtr());
		
	graph = new Plane();
	graph->clear();
	graph->setPosition(0, 0);
	graph->setSize(width, height);
	graph->setCentrePosition(width / 2, height / 2);

    if (!g_small_font.loadFromFile("Arial-Narrow.ttf"))
        std::cout << "ERROR: Cannot load font UbuntuMono.ttf\n";

	if (!g_font.loadFromFile("Arial.ttf"))
		std::cout << "ERROR: Cannot load font UbuntuMono.ttf\n";

	initRibbon();
    initGeneralTab();
	initColorTab();
	initEquationsTab();    

	graph->setTransparency(255);
}

void App::plot() {
reset:
	reset_plot_ = false;
	stop_plot_ = false;
	this->is_plotting = true;

	if (checkEquations())
		return;

	graph->clear();
    x.getVars() = x_eq_vars->get();
    y.getVars() = y_eq_vars->get();

	//std::cout << "X = " << x_value->getValue() << " Y = " << y_value->getValue() << std::endl;

    dt = dt_value->getValue();
    x.setVar("t", t_value->getValue());
	x.setVar("x", x_value->getValue());
	x.setVar("y", y_value->getValue());
    y.setVar("t", t_value->getValue());
	y.setVar("y", y_value->getValue());
	y.setVar("x", x_value->getValue());

	x.calc();
	y.calc();
	
	if (checkEquations())
		return;

	std::cout << "INFO: Plotting started\n";

	long long start_time = clock();

    double fx, fy;
	auto end = static_cast<long>(std::pow(10, iterations->getValue()));
	this->is_plotting = true;
    for (auto i = 0; i < end; i++) {
		if (reset_plot_) {
			graph->clear();
			goto reset;
		}
		if (stop_plot_)
			break;

    	fx = x.calc();
        fy = y.calc();

        graph->push(sf::Vector2f(fx, fy));

        x.setVar("t", x.getVal("t") + dt);
        y.setVar("t", y.getVal("t") + dt);
        x.setVar("x", fx);
        y.setVar("x", fx);
        x.setVar("y", fy);
        y.setVar("y", fy);
    }
	std::cout << "INFO: Plot is ready in " << float(clock()-start_time)/CLOCKS_PER_SEC << " sec\n";
	is_plotting = false;
	stop_plot_ = false;
}

void App::resetPlot()
{
	if (is_plotting) {
		reset_plot_ = true;
	graph->clear();
	}else {
		try {
			stopPlot();
			plotter = std::thread(&App::plot, this);
		}
		catch (...)
		{
			std::cout << "THREAD ERROR: startPlot() have been called, but was not stopped\n";
		}
	}
}

void App::stopPlot()
{
	if (!stop_plot_) {
		if (THREAD_DEBUG) std::cout << "THREAD INFO: Plot is beeing canceled\n";
		stop_plot_ = true;
		plotter.join();
		if (THREAD_DEBUG) std::cout << "THREAD INFO: Plot have been discarded\n";
		graph->clear();
	} else
	{
		if (is_plotting)
			std::cout << "THREAD WARNING: stopPlot() called, but won't stop plot(). However, it's running\n";
		else
			std::cout << "THREAD WARNING: stopPlot() called, but won't stop plot(), because it's not running\n";
	}
}

bool App::checkEquations()
{
	if (x.failed())
	{
		std::cout << "X EQUATION THROWN AN ERROR: " << x.what() << "\n";
		reset_plot_ = false; is_plotting = false;
		return true;
	}
	if (y.failed())
	{
		std::cout << "Y EQUATION THROWN AN ERROR: " << y.what() << "\n";
		reset_plot_ = false; is_plotting = false;
		return true;
	}
	return false;
}

void App::initGeneralTab() {
    general.init(sf::Vector2f(ribbon.pos().x+ribbon.size().x+8, 8));
    
    //Iterations number slider
    iterations = new Slider(Vector2d(0, 9), sf::Vector2f(0, 0), 200);
    iterations->setName(sf::String(L"Кол-во итераций (log)"));
	iterations->setValue(5);
    iterations->ifValueChanged([&]()
    {
		resetPlot();
    });

	//Transparency slider
	transparency_value = new Slider(Vector2d(0, 255), sf::Vector2f(0, iterations->pos().y + iterations->size().y + 4), 200);
	transparency_value->setName(sf::String(L"Прозрачность"));
	transparency_value->setValue(255);
	transparency_value->ifValueChanged([&]()
	{
		graph->setTransparency(transparency_value->getValue());
	});
	transparency_value->ifSliderMoved([&]()
	{
		graph->setTransparency(transparency_value->getValue());
	});

	//Zoom slider
	zoom_value = new Slider(Vector2d(0, 10), sf::Vector2f(0, transparency_value->pos().y + transparency_value->size().y + 4), 200);
	zoom_value->setName(sf::String(L"Увеличение (log)"));
	zoom_value->setValue(1);
	zoom_value->ifValueChanged([&]()
	{
		graph->setZoom(std::pow(10, zoom_value->getValue())/10);
	});
	zoom_value->ifSliderMoved([&]()
	{
		graph->setZoom(std::pow(10, zoom_value->getValue()) / 10);
	});

	center_view = new Button(g_font, sf::String(L"Выровнять по центру"), sf::Vector2f(0, zoom_value->pos().y+zoom_value->size().y+8));
	center_view->onClick([&]()
	{
		graph->realign();
	});

	//Link sliders
    general.add(iterations);
	general.add(transparency_value);
	general.add(zoom_value);
	general.add(center_view);

    general.hide();
}

void App::initColorTab()
{
	color_sel.init(sf::Vector2f(ribbon.pos().x + ribbon.size().x + 8, 8));

	color1 = new Text(g_font, sf::String(L"Начальный цвет"), 16);
	color2 = new Text(g_font, sf::String(L"Конечный цвет"), 16, sf::Vector2f(216, 0));

	//Color 1 HUE
	ch1 = new Slider(Vector2d(0, 360), sf::Vector2f(0, color1->pos().y + color1->size().y + 8));
	ch1->setName(sf::String(L"Оттенок"));
	ch1->ifValueChanged([&]()
	{
		graph->startColour().hue = int(ch1->getValue());
		graph->update();
	});
	ch1->ifSliderMoved([&]()
	{
		graph->startColour().hue = int(ch1->getValue());
		graph->update();
	});
	ch1->setValue(100);
	
	//Color 1 SATURATION
	cs1 = new Slider(Vector2d(0, 100), sf::Vector2f(0, ch1->pos().y + ch1->size().y + 4));
	cs1->setName(sf::String(L"Насыщенность"));
	cs1->ifValueChanged([&]()
	{
		graph->startColour().sat = cs1->getValue()/100;
		graph->update();
	});
	cs1->ifSliderMoved([&]()
	{
		graph->startColour().sat = cs1->getValue()/100;
		graph->update();
	});
	cs1->setValue(100);

	//Color 1 LUMINOSITY
	cl1 = new Slider(Vector2d(0, 100), sf::Vector2f(0, cs1->pos().y + cs1->size().y + 4));
	cl1->setName(sf::String(L"Светлота"));
	cl1->ifValueChanged([&]()
	{
		graph->startColour().lum = cl1->getValue()/100;
		graph->update();
	});
	cl1->ifSliderMoved([&]()
	{
		graph->startColour().lum = cl1->getValue()/100;
		graph->update();
	});
	cl1->setValue(50);

	//Color 2 HUE
	ch2 = new Slider(Vector2d(0, 360), sf::Vector2f(color2->pos().x, color2->pos().y + color2->size().y + 8));
	ch2->setName(sf::String(L"Оттенок"));
	ch2->ifValueChanged([&]()
	{
		graph->endColour().hue = int(ch2->getValue());
		graph->update();
	});
	ch2->ifSliderMoved([&]()
	{
		graph->endColour().hue = int(ch2->getValue());
		graph->update();
	});
	ch2->setValue(200);

	//Color 2 SATURATION
	cs2 = new Slider(Vector2d(0, 100), sf::Vector2f(color2->pos().x, ch2->pos().y + ch2->size().y + 4));
	cs2->setName(sf::String(L"Насыщенность"));
	cs2->ifValueChanged([&]()
	{
		graph->endColour().sat = cs2->getValue() / 100;
		graph->update();
	});
	cs2->ifSliderMoved([&]()
	{
		graph->endColour().sat = cs2->getValue() / 100;
		graph->update();
	});
	cs2->setValue(100);

	//Color 2 LUMINOSITY
	cl2 = new Slider(Vector2d(0, 100), sf::Vector2f(color2->pos().x, cs2->pos().y + cs2->size().y + 4));
	cl2->setName(sf::String(L"Светлота"));
	cl2->ifValueChanged([&]()
	{
		graph->endColour().lum = cl2->getValue()/100;
		graph->update();
	});
	cl2->ifSliderMoved([&]()
	{
		graph->endColour().lum = cl2->getValue()/100;
		graph->update();
	});
	cl2->setValue(50);

	color_sel.add(color1);
	color_sel.add(color2);
	color_sel.add(ch1);
	color_sel.add(cs1);
	color_sel.add(cl1);
	color_sel.add(ch2);
	color_sel.add(cs2);
	color_sel.add(cl2);

	color_sel.hide();
}


void App::initEquationsTab() {
	x.ignore({ "x", "y", "t" });
	y.ignore({ "x", "y", "t" });

    equations.init(sf::Vector2f(ribbon.pos().x + ribbon.size().x + 8, 8));

	time_lab = new Text(g_font, sf::String(L"Время"), 16, sf::Vector2f(200+16, 0));

	//t value slider
	t_value = new Slider(Vector2d(-10, 10), sf::Vector2f(time_lab->pos().x, time_lab->pos().y+time_lab->size().y+12), 200);
	t_value->setName("t");
	t_value->ifValueChanged([&]()
	{
		resetPlot();
	});

	//dt value slider
	dt_value = new Slider(Vector2d(0, 1), sf::Vector2f(time_lab->pos().x, t_value->pos().y + t_value->size().y + 4), 200);
	dt_value->setName("dt");
	dt_value->setValue(0.005);
	dt_value->ifValueChanged([&]()
	{
		resetPlot();
	});

	eqat_lab = new Text(g_font, sf::String(L"Функции"), 16);
	
	//Xinit
	x_value = new Slider(Vector2d(-10, 10), sf::Vector2f(0, eqat_lab->pos().y + eqat_lab->size().y + 8), 200);
	x_value->setName("x");
	x_value->setValue(0);
	x_value->ifValueChanged([&]()
	{
		resetPlot();
	});

	//X expression
	x_eq = new TextBox(g_font, sf::Vector2f(0, x_value->pos().y + x_value->size().y + 6));
	x_eq->ifTextChanged([&]()
	{
		stopPlot();
		x.parse(x_eq->content());
		x_eq_vars->genFrom(getXVals());
		if (!x_eq_vars->get().size() && !y_eq_vars->get().size())
		{
			param_lab->hide();
		} else
		{
			param_lab->show();
		}
		resetPlot();
	});

	//YInit
	y_value = new Slider(Vector2d(-10, 10), sf::Vector2f(0, x_eq->pos().y + x_eq->size().y + 8), 200);
	y_value->setName("y");
	y_value->setValue(0);
	y_value->ifValueChanged([&]()
	{
		resetPlot();
	});
	
	//Y expression
	y_eq = new TextBox(g_font, sf::Vector2f(0, y_value->pos().y + y_value->size().y + 6));
	y_eq->ifTextChanged([&]()
	{
		stopPlot();
		y.parse(y_eq->content());
		y_eq_vars->genFrom(getYVals());
		if (!x_eq_vars->get().size() && !y_eq_vars->get().size())
		{
			param_lab->hide();
		}
		else
		{
			param_lab->show();
		}
		resetPlot();
	});

	param_lab = new Text(g_font, sf::String(L"Параметры"), 16, sf::Vector2f(0, y_eq->pos().y + y_eq->size().y + 16));
	param_lab->hide();

    x_eq_vars = new ValueTable(sf::Vector2f(0, param_lab->pos().y));
    y_eq_vars = new ValueTable(sf::Vector2f(116, param_lab->pos().y ));

	Slider samp(Vector2d(-10, 10), sf::Vector2f(), 100);
    samp.ifValueChanged([&]()
    {
		resetPlot();
    });
	
	samp.ifSliderMoved([&]()
	{
		resetPlot();
	});

    x_eq_vars->setSample(samp);
    y_eq_vars->setSample(samp);
    
	equations.add(t_value);
	equations.add(dt_value);
    equations.add(x_eq);
    equations.add(y_eq);
    equations.add(x_eq_vars);
    equations.add(y_eq_vars);    
	equations.add(x_value);
	equations.add(y_value);
	equations.add(time_lab);
	equations.add(eqat_lab);
	equations.add(param_lab);

    equations.hide();
}

void App::initRibbon() {
    tab_gen = new Tab(sf::String(L"Основные"), 24, sf::Vector2f(32, 32));
    tab_gen->onActivation([&]() {
        general.show();
        return false;
    });
    tab_gen->onDeactivation([&]() {
        general.hide();
        return true;
    });
    
	tab_col = new Tab(sf::String(L"Цвет"), 24, sf::Vector2f(32, 32));
	tab_col->onActivation([&]() {
		color_sel.show();
		return false;
	});
	tab_col->onDeactivation([&]() {
		color_sel.hide();
		return true;
	});

    tab_eq = new Tab(sf::String(L"Вывод"), 24, sf::Vector2f(32, 32));
    tab_eq->onActivation([&]() {
        equations.show();
        return false;
    });
    tab_eq->onDeactivation([&]() {
        equations.hide();
        return false;
    });

    ribbon.init(sf::Vector2f(8, 8), ORDER_VERTICALY);
    ribbon.add(tab_gen);
	ribbon.add(tab_col);
    ribbon.add(tab_eq);
}

void App::initialize() {
    settings.antialiasingLevel = 0;

    window.create(sf::VideoMode(width, height), "FieldDen", sf::Style::Default, settings);
	window.setFramerateLimit(60);

    initGeneralTab();
    initEquationsTab();
	initColorTab();
	initRibbon();
	
	graph->clear();
	graph->setPosition(0, 0);
	graph->setSize(width, height);
	graph->setCentrePosition(width / 2, height / 2);
	/*
    test.init(sf::Vector2f(50, 50));
    test.add("a", new Slider(sf::Vector2f(50, 50), Vector2d(-10, 10), 100, new TextBox(g_font, sf::Vector2f(0, 0))), g_font);
    test.add("b", new Slider(sf::Vector2f(50, 50), Vector2d(-10, 10), 100, new TextBox(g_font, sf::Vector2f(0, 0))), g_font);*/

    //Drawing
    
}

void App::run() {
    loop();
	stopPlot();
}

void App::loop() {
	bool lap = false, rap = false, uap = false, dap = false;

    sf::Event event;
    while (window.isOpen()) {

        while (window.pollEvent(event)) {
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (event.mouseWheelScroll.delta > 0)
				{
					zoom_value->setValue(zoom_value->getValue() + MOUSE_WHEEL_ZOOM);
				}
				else if (event.mouseWheelScroll.delta < 0)
				{
					zoom_value->setValue(zoom_value->getValue() - MOUSE_WHEEL_ZOOM);
				}
			}
            else if (event.type == sf::Event::KeyPressed)
            {
				if (event.key.code == sf::Keyboard::Left)
				{
					lap = true;
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					rap = true;
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					uap = true;
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					dap = true;
				}
            	else if (event.key.code == sf::Keyboard::Key::F5)
				{
					resetPlot();
				}
				else if (event.key.code == sf::Keyboard::Key::F11)
				{
					time_t now = time(0);
					tm *gmtm = localtime(&now);

					std::string path = "screenshots\\" + std::to_string(1 + gmtm->tm_mon) + '-'
						+ std::to_string(gmtm->tm_mday) + '-'
						+ std::to_string(1900 + gmtm->tm_year) + ' '
						+ std::to_string(gmtm->tm_hour) + '-'
						+ std::to_string(gmtm->tm_min) + '-'
						+ std::to_string(gmtm->tm_sec) + ".png";

					//TODO: Use boost to create screenshots directory
					graph->saveAsImage("screenshots\\"+std::to_string(1+gmtm->tm_mon)+'-'
						+ std::to_string(gmtm->tm_mday)+'-'
						+ std::to_string(1900+gmtm->tm_year)+' '
						+ std::to_string(gmtm->tm_hour) + '-'
						+ std::to_string(gmtm->tm_min) + '-'
						+ std::to_string(gmtm->tm_sec) + ".png");
					std::cout << "Screenhot saved as " << path << std::endl;
				}
            }
			else if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Left)
				{
					lap = false;
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					rap = false;
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					uap = false;
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					dap = false;
				}
			}
            else if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0.0, 0.0, (float)event.size.width, (float)event.size.height)));
				graph->setSize((float)event.size.width, (float)event.size.height);
				graph->setCentrePosition((float)event.size.width / 2, (float)event.size.height / 2);
            }
		
            ribbon.check(event, window);
            general.check(event, window);
			color_sel.check(event, window);
            equations.check(event, window);
            test.check(event, window);
        }

		if (lap)
		{
			graph->shiftX(10);
		}
		if (rap)
		{
			graph->shiftX(-10);
		}
		if (uap)
		{
			graph->shiftY(10);
		}
		if (dap)
		{
			graph->shiftY(-10);
		}

        //plot();

        window.clear(sf::Color::White);

		graph->draw(window);

        window.draw(ribbon);
        window.draw(general);
		window.draw(color_sel);
        window.draw(equations);

        window.display();
    }
}