#include "Globals.hpp"
#include "App.hpp"
#include "Button.hpp"
#include "Slider.hpp"

#include <thread>
#define _CRT_SECURE_NO_WARNINGS true

App::App() {
    settings.antialiasingLevel = 0;

    window.create(sf::VideoMode(width, height), "Universal 2D Plotter", sf::Style::Default, settings);
    window.setFramerateLimit(60);

	graph = new Plane();
	graph->clear();
	graph->setPosition(0, 0);
	graph->setSize(width, height);
	graph->setCentrePosition(width / 2, height / 2);

    if (!g_font.loadFromFile("Inconsolata-Regular.ttf"))
        std::cout << "ERROR: Cannot load font UbuntuMono.ttf\n";

    initGeneralTab();
    initEquationsTab();
    initRibbon();

	graph->setTransparency(255);
}

void App::plot() {
	stop_plotting = false; is_plotting = true;
    std::cout << "Vertex thread> Checking plot's requirements\n";
	if (x.failed())
	{
		std::cout << "Vertex thread> x equation error: " << x.what() << "\n";
		stop_plotting = false; is_plotting = false;
		return;
	}
	if (y.failed())
	{
		std::cout << "Vertex thread> y equation error: " << y.what() << "\n";
		stop_plotting = false; is_plotting = false;
		return;
	}

	graph->clear();
    x.getVars() = x_eq_vars->get();
    y.getVars() = y_eq_vars->get();

    dt = dt_value->getValue();
    x.setVar("t", t_value->getValue());
    y.setVar("t", t_value->getValue());
	x.setVar("x", x_value->getValue());
	y.setVar("y", y_value->getValue());
	y.setVar("x", x_value->getValue());
	x.setVar("y", y_value->getValue());


	x.calc();
	y.calc();
	std::cout << "Vertex thread> Computing plot\n";
	if (x.failed())
	{
		std::cout << "Vertex thread> x equation error: " << x.what() << "\n";
		stop_plotting = false; is_plotting = false;
		return;
	}
	if (y.failed())
	{
		std::cout << "Vertex thread> y equation error: " << y.what() << "\n";
		stop_plotting = false; is_plotting = false;
		return;
	}

	long long start_time = clock();

    double fx, fy;
	auto end = static_cast<long>(std::pow(10, iterations->getValue()));
    for (auto i = 0; i < end; i++) {
		if (stop_plotting) {
			stop_plotting = false;
			is_plotting = false;
			return;
		}

		std::lock_guard<std::mutex> safe_execution(safe_calc);

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
	std::cout << "Vertex thread> Ready in " << float(clock()-start_time)/CLOCKS_PER_SEC << " sec\n";
}

void App::startPlot()
{
	try {
		plotter = std::thread(&App::plot, this);
	} catch(...)
	{
		std::cout << "startPlot has been called, but thread wasn't stopped\n";
	}
}

void App::stopPlot()
{
	if (!stop_plotting) {
		std::cout << "Vertex thread> Canceling plot\n";
		stop_plotting = true;
		plotter.join();
		std::cout << "Vertex thread> Plot discarded\n";
		graph->clear();
	} else
	{
		if (is_plotting)
			std::cout << "Vertex thread> stopPlot() called, but won't stop plot(). However, it's running\n";
		else
			std::cout << "Vertex thread> stopPlot() called, but won't stop plot(), because it's not running\n";
	}
}

void App::initGeneralTab() {
    general.init(sf::Vector2f(64, 8));
    
    //Iterations number slider
    iterations = new Slider(Vector2d(0, 9), sf::Vector2f(0, 0), 200);
    iterations->setName("Iterations(log)");
	iterations->setValue(5);
    iterations->ifValueChanged([&]()
    {
		stopPlot(); startPlot();
    });

	//Transparency slider
	transparency_value = new Slider(Vector2d(0, 255), sf::Vector2f(0, iterations->pos().y + iterations->size().y + 4), 200);
	transparency_value->setName("Transparency");
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
	zoom_value->setName("Zoom(log)");
	zoom_value->setValue(1);
	zoom_value->ifValueChanged([&]()
	{
		graph->setZoom(std::pow(10, zoom_value->getValue())/10);
	});
	zoom_value->ifSliderMoved([&]()
	{
		graph->setZoom(std::pow(10, zoom_value->getValue()) / 10);
	});
	

	//Link sliders
    general.add(iterations);
	general.add(transparency_value);
	general.add(zoom_value);

    general.hide();
}

void App::initEquationsTab() {
	x.ignore({ "x", "y", "t" });
	y.ignore({ "x", "y", "t" });

    equations.init(sf::Vector2f(64, 8));

	time_lab = new Text(g_font, "Time", 16);

	//t value slider
	t_value = new Slider(Vector2d(-10, 10), sf::Vector2f(0, time_lab->pos().y+time_lab->size().y+12), 200);
	t_value->setName("t");
	t_value->ifValueChanged([&]()
	{
		stopPlot(); startPlot();
	});

	//dt value slider
	dt_value = new Slider(Vector2d(0, 1), sf::Vector2f(0, t_value->pos().y + t_value->size().y + 4), 200);
	dt_value->setName("dt");
	dt_value->setValue(0.005);
	dt_value->ifValueChanged([&]()
	{
		stopPlot(); startPlot();
	});

	eqat_lab = new Text(g_font, "Equations", 16, sf::Vector2f(0, dt_value->pos().y + dt_value->size().y + 16));
	
	//Xinit
	x_value = new Slider(Vector2d(-10, 10), sf::Vector2f(0, eqat_lab->pos().y + eqat_lab->size().y + 8), 200);
	x_value->setName("x");
	x_value->setValue(0);
	x_value->ifValueChanged([&]()
	{
		stopPlot();
		startPlot();
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
		startPlot();
	});

	//YInit
	y_value = new Slider(Vector2d(-10, 10), sf::Vector2f(0, x_eq->pos().y + x_eq->size().y + 8), 200);
	y_value->setName("y");
	y_value->setValue(0);
	y_value->ifValueChanged([&]()
	{
		stopPlot();
		startPlot();
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
		startPlot();
	});

	param_lab = new Text(g_font, "Parameters", 16, sf::Vector2f(0, y_eq->pos().y + y_eq->size().y + 16));
	param_lab->hide();

    x_eq_vars = new ValueTable(sf::Vector2f(0, param_lab->pos().y));
    y_eq_vars = new ValueTable(sf::Vector2f(116, param_lab->pos().y ));

	Slider samp(Vector2d(-10, 10), sf::Vector2f(), 100);
    samp.ifValueChanged([&]()
    {
		stopPlot(); startPlot();
    });
	/*
	samp.ifSliderMoved([&]()
	{
		stopPlot(); startPlot();
	});*/

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
    tab_gen = new Tab("General", 24, sf::Vector2f(32, 32));
    tab_gen->onActivation([&]() {
        general.show();
        return false;
    });
    tab_gen->onDeactivation([&]() {
        general.hide();
        return true;
    });
    
    
    tab_eq = new Tab("Output", 24, sf::Vector2f(32, 32));
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
    ribbon.add(tab_eq);
}

void App::initialize() {
    settings.antialiasingLevel = 0;

    window.create(sf::VideoMode(width, height), "FieldDen", sf::Style::Default, settings);
	window.setFramerateLimit(60);

    initGeneralTab();
    initEquationsTab();
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
            if (event.type == sf::Event::KeyPressed)
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
					stopPlot();
					startPlot();
				}
				else if (event.key.code == sf::Keyboard::Key::F11)
				{
					time_t now = time(0);
					tm *gmtm = localtime(&now);

					//TODO: Use boost to create screenshots directory
					graph->saveAsImage("screenshots\\"+std::to_string(1+gmtm->tm_mon)+'-'
						+ std::to_string(gmtm->tm_mday)+'-'
						+ std::to_string(1900+gmtm->tm_year)+' '
						+ std::to_string(gmtm->tm_hour) + '-'
						+ std::to_string(gmtm->tm_min) + '-'
						+ std::to_string(gmtm->tm_sec) + ".png");
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
            }
		
            ribbon.check(event, window);
            general.check(event, window);
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
        window.draw(equations);

        window.display();
    }
}